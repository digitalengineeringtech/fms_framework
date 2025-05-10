
// Constants for commands
#define CMD_PRICE_CHANGE 0
#define CMD_CLEAR_SHIFT  1
#define CMD_READ_SHIFT   2
#define CMD_STOP_PUMP    3
#define CMD_REALTIME     4

// Buffer for receiving data from dispenser
#define MAX_BUFFER_SIZE 20
byte inputBuffer[MAX_BUFFER_SIZE];
int bufferIndex = 0;

// Command processing variables
byte dispenserAddress = 1;
unsigned long priceValue = 1234;
int currentCommand = -1;
unsigned long commandStartTime = 0;
bool waitingForResponse = false;
int expectedResponseLength = 0;

void setup() {
  // Initialize serial communication with computer
  Serial.begin(115200);
  Serial1.begin(2400, SERIAL_8E1,16,17); 

  Serial.println(F("Fuel Dispenser Control"));
  Serial.println(F("Commands:"));
  Serial.println(F("  PRICE,address,value   - Change price # price set" ));
  Serial.println(F("  CLEAR,address         - Clear shift total"));
  Serial.println(F("  READ,address          - Read shift total"));
  Serial.println(F("  STOP,address          - Stop pump"));
  Serial.println(F("  REALTIME,address      - Request real-time data # fuelled amount"));
  Serial.println();
}

void loop() {
  // Check for commands from Serial Monitor
  if (Serial.available()) {
    processSerialCommand();
  }
  
  // Check for response from dispenser if we're waiting for one
  if (waitingForResponse) {
    checkDispenserResponse();
    
    // Check for timeout (3 seconds)
    if (millis() - commandStartTime > 3000) {
      Serial.println(F("Response timeout"));
      waitingForResponse = false;
    }
  }
}

void processSerialCommand() {
  // Read command from Serial Monitor
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  // Parse command
  int firstComma = command.indexOf(',');
  if (firstComma == -1) {
    Serial.println(F("Invalid command format. Use: COMMAND,ADDRESS[,DATA]"));
    return;
  }
  
  String cmdType = command.substring(0, firstComma);
  cmdType.toUpperCase();
  
  // Extract address
  int secondComma = command.indexOf(',', firstComma + 1);
  String addressStr;
  
  if (secondComma == -1) {
    addressStr = command.substring(firstComma + 1);
  } else {
    addressStr = command.substring(firstComma + 1, secondComma);
  }
  
  dispenserAddress = addressStr.toInt();
  
  // Process command type
  if (cmdType == "PRICE") {
    if (secondComma == -1) {
      priceValue = 1234; // Default value
    } else {
      priceValue = command.substring(secondComma + 1).toInt();
    }
    sendCommand(CMD_PRICE_CHANGE);
  } 
  else if (cmdType == "CLEAR") {
    sendCommand(CMD_CLEAR_SHIFT);
  } 
  else if (cmdType == "READ") {
    sendCommand(CMD_READ_SHIFT);
  } 
  else if (cmdType == "STOP") {
    sendCommand(CMD_STOP_PUMP);
  } 
  else if (cmdType == "REALTIME") {
    sendCommand(CMD_REALTIME);
  } 
  else {
    Serial.println(F("Unknown command. Valid commands: PRICE, CLEAR, READ, STOP, REALTIME"));
  }
}

void sendCommand(int command) {
  // Store current command
  currentCommand = command;
  
  // Step 1: Send address byte with mark parity
  Serial1.end();
  Serial1.begin(2400, SERIAL_8O1,16,17); // 8 data bits, mark parity, 1 stop bit
  Serial1.write(dispenserAddress);
  delay(10); // Short delay to ensure byte is sent
  
  // Step 2: Send command with space parity
  Serial1.end();
  Serial1.begin(2400, SERIAL_8E1,16,17); // 8 data bits, space parity, 1 stop bit
  
  // Prepare command bytes based on command type
  byte outBytes[7]; // Max size needed
  int outLength = 0;
  byte checksum = 0;
  
  switch (command) {
    case CMD_PRICE_CHANGE:
      outBytes[0] = 0x07;
      outBytes[1] = 0x80;
      outBytes[2] = (priceValue >> 24) & 0xFF;
      outBytes[3] = (priceValue >> 16) & 0xFF;
      outBytes[4] = (priceValue >> 8) & 0xFF;
      outBytes[5] = priceValue & 0xFF;
      outLength = 6;
      break;
      
    case CMD_CLEAR_SHIFT:
      outBytes[0] = 0x03;
      outBytes[1] = 0x04;
      outLength = 2;
      break;
      
    case CMD_READ_SHIFT:
      outBytes[0] = 0x03;
      outBytes[1] = 0x8D;
      outLength = 2;
      break;
      
    case CMD_STOP_PUMP:
      outBytes[0] = 0x03;
      outBytes[1] = 0x10;
      outLength = 2;
      break;
      
    case CMD_REALTIME:
      outBytes[0] = 0x03;
      outBytes[1] = 0x8F;
      outLength = 2;
      break;
  }
  
  // Calculate checksum
  unsigned int sum = dispenserAddress;
  for (int i = 0; i < outLength; i++) {
    sum += outBytes[i];
  }
  checksum = (0x10000 - sum) & 0xFF;
  outBytes[outLength] = checksum;
  outLength++;
  
  // Send command bytes
  for (int i = 0; i < outLength; i++) {
    Serial1.write(outBytes[i]);
  }
  
  // Debug output
  Serial.print(F("TXD: "));
  Serial.print(dispenserAddress, HEX);
  Serial.print(F(" "));
  for (int i = 0; i < outLength; i++) {
    if (outBytes[i] < 0x10) Serial.print(F("0"));
    Serial.print(outBytes[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();
  
  // Set up for response
  Serial.print(F("RXD: "));
  bufferIndex = 0;
  waitingForResponse = true;
  commandStartTime = millis();
  
  // Set expected response length based on command
  switch (command) {
    case CMD_PRICE_CHANGE:
    case CMD_CLEAR_SHIFT:
    case CMD_STOP_PUMP:
      expectedResponseLength = 3;
      break;
    case CMD_READ_SHIFT:
      expectedResponseLength = 15;
      break;
    case CMD_REALTIME:
      expectedResponseLength = 13;
      break;
  }
}

void checkDispenserResponse() {
  // Check if data is available from dispenser
  while (Serial1.available() && bufferIndex < MAX_BUFFER_SIZE) {
    inputBuffer[bufferIndex] = Serial1.read();
    bufferIndex++;
    
    // If we've received the expected number of bytes, process the response
    if (bufferIndex >= expectedResponseLength) {
      processResponse();
      waitingForResponse = false;
      break;
    }
  }
}

void processResponse() {
  // Print raw response bytes
  for (int i = 0; i < bufferIndex; i++) {
    if (inputBuffer[i] < 0x10) Serial.print(F("0"));
    Serial.print(inputBuffer[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();
  
  // Process response based on command type
  switch (currentCommand) {
    case CMD_PRICE_CHANGE:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          Serial.println(F("Price change: ACK (Success)"));
        } else {
          Serial.println(F("Price change: NACK (Failed)"));
        }
      }
      break;
      
    case CMD_CLEAR_SHIFT:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          Serial.println(F("Clear shift total: ACK (Success)"));
        } else {
          Serial.println(F("Clear shift total: NACK (Failed)"));
        }
      }
      break;
      
    case CMD_READ_SHIFT:
      if (bufferIndex >= 15) {
        // Extract volume and amount from response
        unsigned long volume = 0;
        unsigned long amount = 0;
        
        // Volume is in bytes 2-5
        volume = ((unsigned long)inputBuffer[2] << 24) |
                 ((unsigned long)inputBuffer[3] << 16) |
                 ((unsigned long)inputBuffer[4] << 8) |
                 inputBuffer[5];
                 
        // Amount is in bytes 6-9
        amount = ((unsigned long)inputBuffer[6] << 24) |
                 ((unsigned long)inputBuffer[7] << 16) |
                 ((unsigned long)inputBuffer[8] << 8) |
                 inputBuffer[9];
                 
        Serial.print(F("Shift Total - Volume: "));
        Serial.print(volume);
        Serial.print(F(", Amount: "));
        Serial.println(amount);
      }
      break;
      
    case CMD_STOP_PUMP:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          Serial.println(F("Stop pump: ACK (Success)"));
        } else {
          Serial.println(F("Stop pump: NACK (Failed)"));
        }
      }
      break;
      
    case CMD_REALTIME:
      if (bufferIndex >= 13) {
        // Extract status, volume and amount from response
        byte status = inputBuffer[2];
        unsigned long volume = 0;
        unsigned long amount = 0;
        
        // Volume is in bytes 3-6
        volume = ((unsigned long)inputBuffer[3] << 24) |
                 ((unsigned long)inputBuffer[4] << 16) |
                 ((unsigned long)inputBuffer[5] << 8) |
                 inputBuffer[6];
                 
        // Amount is in bytes 7-10
        amount = ((unsigned long)inputBuffer[7] << 24) |
                 ((unsigned long)inputBuffer[8] << 16) |
                 ((unsigned long)inputBuffer[9] << 8) |
                 inputBuffer[10];
                 
        Serial.print(F("Real-time data - Status: "));
        Serial.print(status, HEX);
        Serial.print(F(", Volume: "));
        Serial.print(volume);
        Serial.print(F(", Amount: "));
        Serial.println(amount);
      }
      break;
  }
}
