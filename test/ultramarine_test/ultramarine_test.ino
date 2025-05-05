/*
 * Fuel Dispenser Protocol Analyzer for ESP32
 * 
 * This utility helps analyze and debug the communication protocol
 * between the ESP32 and the fuel dispenser.
 * 
 * Commands (via Serial Monitor):
 * - START: Start monitoring
 * - STOP: Stop monitoring
 * - CLEAR: Clear logs
 * - MARK: Switch to MARK parity
 * - SPACE: Switch to SPACE parity
 * - SEND <hex data>: Send hex data (e.g., SEND 01 03 8F 6D)
 * - CHANNEL <0-3>: Select multiplexer channel (e.g., CHANNEL 2)
 * - TEST: Test RS232 connection on current channel
 * - LOOPBACK: Test RS232 in loopback mode on current channel
 * - U913: Test U913 device on specified channel (e.g., U913 2)
 * - RS485: Test RS485 Modbus communication on current channel
 * - RS485READ <id> <reg>: Read Modbus register (e.g., RS485READ 1 40001)
 * - RS485WRITE <id> <reg> <value>: Write Modbus register (e.g., RS485WRITE 1 40001 100)
 */

#include <HardwareSerial.h>
#include <ModbusMaster.h>

// Simple circular buffer implementation
#define LOG_BUFFER_SIZE 100

// Multiplexer pins
#define MUX_S0 25
#define MUX_S1 26
#define MUX_EN 27

// RS485 control pin
#define RS485_DE_RE 22  // Data Enable and Receiver Enable pin for RS485

// Serial communication settings
#define DISPENSER_SERIAL Serial2
#define DISPENSER_RX_PIN 16  // UART RX pin via MUX
#define DISPENSER_TX_PIN 17  // Direct TX to device opto-LED
#define BAUD_RATE 2400
#define MODBUS_BAUD_RATE 9600  // Standard baud rate for Modbus

// Buffer for storing communication logs
String communicationLog[LOG_BUFFER_SIZE];
int logHead = 0;
int logCount = 0;

// Monitoring state
bool monitoringActive = false;
bool markParity = true; // Start with mark parity
uint8_t currentChannel = 0; // Default channel

// Create ModbusMaster instance
ModbusMaster modbus;

// Pre-transmission callback function for RS485
void preTransmission() {
  digitalWrite(RS485_DE_RE, HIGH); // Enable transmission
  delay(1); // Small delay to allow the driver to switch
}

// Post-transmission callback function for RS485
void postTransmission() {
  delay(1); // Small delay to allow the last byte to be sent
  digitalWrite(RS485_DE_RE, LOW); // Enable reception
}

void setup() {
  // Initialize serial for debugging and commands
  Serial.begin(115200);
  Serial.println("\nFuel Dispenser Protocol Analyzer with RS232/RS485 Support");
  Serial.println("=========================================================");
  
  // Initialize multiplexer pins
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_EN, OUTPUT);
  
  // Initialize RS485 control pin
  pinMode(RS485_DE_RE, OUTPUT);
  digitalWrite(RS485_DE_RE, LOW); // Default to receive mode
  
  // Select default channel
  selectMuxChannel(currentChannel);
  
  // Initialize dispenser serial with mark parity (odd parity)
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  // Add initial log entry
  logMessage("Protocol Analyzer started. Waiting for communication...");
  
  // Print help
  printHelp();
}

void loop() {
  // Check for serial commands
  checkSerialCommands();
  
  // Monitor serial communication if active
  if (monitoringActive) {
    monitorSerialCommunication();
  }
}

void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_EN, LOW);
  delay(2);  // Let MUX stabilize
  
  currentChannel = channel;
  logMessage("Selected MUX channel " + String(channel));
}

void printHelp() {
  Serial.println("\nAvailable Commands:");
  Serial.println("------------------");
  Serial.println("START - Start monitoring");
  Serial.println("STOP - Stop monitoring");
  Serial.println("CLEAR - Clear logs");
  Serial.println("MARK - Switch to MARK parity");
  Serial.println("SPACE - Switch to SPACE parity");
  Serial.println("SEND <hex data> - Send hex data (e.g., SEND 01 03 8F 6D)");
  Serial.println("CHANNEL <0-3> - Select multiplexer channel (e.g., CHANNEL 2)");
  Serial.println("TEST - Test RS232 connection on current channel");
  Serial.println("LOOPBACK - Test RS232 in loopback mode on current channel");
  Serial.println("U913 [channel] - Test U913 device (optionally on specified channel)");
  Serial.println("RS485 - Simple RS485 Modbus test on current channel");
  Serial.println("RS485READ <id> <reg> - Read Modbus register (e.g., RS485READ 1 40001)");
  Serial.println("RS485WRITE <id> <reg> <value> - Write Modbus register (e.g., RS485WRITE 1 40001 100)");
  Serial.println("HELP - Show this help message");
  Serial.println("STATUS - Show current status");
  Serial.println("LOGS - Show all stored logs");
  Serial.println();
}

void checkSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    // Store original command for parsing
    String originalCommand = command;
    
    // Convert to uppercase for standard commands
    command.toUpperCase();
    
    if (command == "START") {
      monitoringActive = true;
      logMessage("Monitoring started");
      Serial.println("Monitoring started");
    } 
    else if (command == "STOP") {
      monitoringActive = false;
      logMessage("Monitoring stopped");
      Serial.println("Monitoring stopped");
    } 
    else if (command == "CLEAR") {
      logHead = 0;
      logCount = 0;
      logMessage("Log cleared");
      Serial.println("Log cleared");
    } 
    else if (command == "MARK") {
      markParity = true;
      DISPENSER_SERIAL.end();
      DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
      logMessage("Switched to MARK parity");
      Serial.println("Switched to MARK parity");
    } 
    else if (command == "SPACE") {
      markParity = false;
      DISPENSER_SERIAL.end();
      DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
      logMessage("Switched to SPACE parity");
      Serial.println("Switched to SPACE parity");
    } 
    else if (command.startsWith("SEND ")) {
      String hexData = command.substring(5);
      sendHexData(hexData);
      Serial.println("Data sent: " + hexData);
    }
    else if (command.startsWith("CHANNEL ")) {
      int channel = command.substring(8).toInt();
      if (channel >= 0 && channel <= 3) {
        selectMuxChannel(channel);
        Serial.println("Selected channel: " + String(channel));
      } else {
        Serial.println("Error: Channel must be between 0 and 3");
      }
    }
    else if (command == "TEST") {
      testRS232Connection();
    }
    else if (command == "LOOPBACK") {
      testRS232Loopback();
    }
    else if (command.startsWith("U913")) {
      // Check if a channel is specified
      if (command.length() > 4) {
        int channel = command.substring(5).toInt();
        if (channel >= 0 && channel <= 3) {
          selectMuxChannel(channel);
          Serial.println("Selected channel " + String(channel) + " for U913 test");
        } else {
          Serial.println("Invalid channel. Using current channel " + String(currentChannel));
        }
      }
      testU913Device();
    }
    else if (command == "RS485") {
      testRS485Simple();
    }
    else if (command.startsWith("RS485READ ")) {
      String params = command.substring(10);
      int spaceIndex = params.indexOf(' ');
      if (spaceIndex == -1) {
        Serial.println("Error: Missing parameters. Format: RS485READ <slave_id> <register>");
        return;
      }
      
      int slaveId = params.substring(0, spaceIndex).toInt();
      uint16_t reg = params.substring(spaceIndex + 1).toInt();
      
      readModbusRegister(slaveId, reg);
    }
    else if (command.startsWith("RS485WRITE ")) {
      String params = command.substring(11);
      int spaceIndex1 = params.indexOf(' ');
      if (spaceIndex1 == -1) {
        Serial.println("Error: Missing parameters. Format: RS485WRITE <slave_id> <register> <value>");
        return;
      }
      
      int spaceIndex2 = params.indexOf(' ', spaceIndex1 + 1);
      if (spaceIndex2 == -1) {
        Serial.println("Error: Missing value parameter. Format: RS485WRITE <slave_id> <register> <value>");
        return;
      }
      
      int slaveId = params.substring(0, spaceIndex1).toInt();
      uint16_t reg = params.substring(spaceIndex1 + 1, spaceIndex2).toInt();
      uint16_t value = params.substring(spaceIndex2 + 1).toInt();
      
      writeModbusRegister(slaveId, reg, value);
    }
    else if (command == "HELP") {
      printHelp();
    } 
    else if (command == "STATUS") {
      printStatus();
    }
    else if (command == "LOGS") {
      printAllLogs();
    }
    else {
      Serial.println("Unknown command. Type HELP for available commands.");
    }
  }
}

void testRS485Simple() {
  Serial.println("\n=== Simple RS485 Modbus Test on Channel " + String(currentChannel) + " ===");
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  monitoringActive = false;
  
  // Configure serial for Modbus
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(MODBUS_BAUD_RATE, SERIAL_8N1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Test parameters
  int slaveId = 1;  // Default slave ID
  uint16_t startReg = 40001;  // Default register to read
  
  Serial.println("Testing Modbus communication with device ID " + String(slaveId));
  Serial.println("Reading holding register " + String(startReg));
  
  // Initialize ModbusMaster
  modbus.begin(slaveId, DISPENSER_SERIAL);
  
  // Set callbacks for RS485 direction control
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
  
  // Adjust register number (convert from 4xxxx format to 0-based)
  uint16_t modbusReg = startReg - 40001;
  
  // Read holding register
  Serial.println("Sending Modbus read request...");
  uint8_t result = modbus.readHoldingRegisters(modbusReg, 1);
  
  if (result == modbus.ku8MBSuccess) {
    uint16_t value = modbus.getResponseBuffer(0);
    Serial.println("\n✓ SUCCESS! Communication with Modbus device established.");
    Serial.println("Register " + String(startReg) + " value: " + String(value));
    logMessage("RS485 Test on CH" + String(currentChannel) + ": SUCCESS - Register " + 
               String(startReg) + " = " + String(value));
    
    // Try writing to the register
    Serial.println("\nTrying to write value " + String(value + 1) + " to register " + String(startReg));
    result = modbus.writeSingleRegister(modbusReg, value + 1);
    
    if (result == modbus.ku8MBSuccess) {
      Serial.println("✓ Write successful!");
      
      // Read back to verify
      delay(100);
      result = modbus.readHoldingRegisters(modbusReg, 1);
      if (result == modbus.ku8MBSuccess) {
        uint16_t newValue = modbus.getResponseBuffer(0);
        Serial.println("Register " + String(startReg) + " new value: " + String(newValue));
      }
    } else {
      Serial.println("✗ Write failed. Error: " + getModbusError(result));
      Serial.println("This is normal if the device is read-only or the register is protected.");
    }
  } else {
    Serial.println("\n✗ FAILED! Could not communicate with Modbus device.");
    Serial.println("Error: " + getModbusError(result));
    Serial.println("\nTroubleshooting tips:");
    Serial.println("1. Check that the device is powered on");
    Serial.println("2. Verify the device ID (default is 1)");
    Serial.println("3. Check wiring: A+ to A+, B- to B-");
    Serial.println("4. Try a different baud rate (common rates: 9600, 19200, 38400)");
    Serial.println("5. Ensure the RS485 adapter is working properly");
    Serial.println("6. Try a different register address");
    
    logMessage("RS485 Test on CH" + String(currentChannel) + ": FAILED - " + getModbusError(result));
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, markParity ? SERIAL_8O1 : SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  Serial.println("\nRS485 test completed. Previous settings restored.");
}

void readModbusRegister(int slaveId, uint16_t reg) {
  Serial.println("\n=== Reading Modbus Register on Channel " + String(currentChannel) + " ===");
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  monitoringActive = false;
  
  // Configure serial for Modbus
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(MODBUS_BAUD_RATE, SERIAL_8N1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  Serial.println("Reading from device ID " + String(slaveId) + ", register " + String(reg));
  
  // Initialize ModbusMaster
  modbus.begin(slaveId, DISPENSER_SERIAL);
  
  // Set callbacks for RS485 direction control
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
  
  // Determine register type and adjust address
  uint8_t function;
  uint16_t modbusReg;
  
  if (reg >= 1 && reg <= 9999) {
    // Coil (0xxxx)
    function = 1; // Read Coils
    modbusReg = reg - 1;
  } else if (reg >= 10001 && reg <= 19999) {
    // Discrete Input (1xxxx)
    function = 2; // Read Discrete Inputs
    modbusReg = reg - 10001;
  } else if (reg >= 30001 && reg <= 39999) {
    // Input Register (3xxxx)
    function = 4; // Read Input Registers
    modbusReg = reg - 30001;
  } else if (reg >= 40001 && reg <= 49999) {
    // Holding Register (4xxxx)
    function = 3; // Read Holding Registers
    modbusReg = reg - 40001;
  } else {
    Serial.println("Invalid register address. Use format:");
    Serial.println("1-9999: Coils");
    Serial.println("10001-19999: Discrete Inputs");
    Serial.println("30001-39999: Input Registers");
    Serial.println("40001-49999: Holding Registers");
    
    // Restore previous settings
    monitoringActive = wasMonitoring;
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, markParity ? SERIAL_8O1 : SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
    return;
  }
  
  // Execute appropriate function
  uint8_t result;
  switch (function) {
    case 1:
      Serial.println("Reading coil...");
      result = modbus.readCoils(modbusReg, 1);
      break;
    case 2:
      Serial.println("Reading discrete input...");
      result = modbus.readDiscreteInputs(modbusReg, 1);
      break;
    case 3:
      Serial.println("Reading holding register...");
      result = modbus.readHoldingRegisters(modbusReg, 1);
      break;
    case 4:
      Serial.println("Reading input register...");
      result = modbus.readInputRegisters(modbusReg, 1);
      break;
  }
  
  if (result == modbus.ku8MBSuccess) {
    uint16_t value = modbus.getResponseBuffer(0);
    
    Serial.println("\n✓ SUCCESS! Read successful.");
    
    if (function == 1 || function == 2) {
      // For coils and discrete inputs (bit values)
      Serial.println("Value: " + String(value & 0x01));
      logMessage("Modbus Read on CH" + String(currentChannel) + ": SUCCESS - Register " + 
                 String(reg) + " = " + String(value & 0x01) + " (bit)");
    } else {
      // For registers (16-bit values)
      Serial.println("Decimal: " + String(value));
      Serial.println("Hex: 0x" + String(value, HEX));
      Serial.println("Binary: " + String(value, BIN));
      logMessage("Modbus Read on CH" + String(currentChannel) + ": SUCCESS - Register " + 
                 String(reg) + " = " + String(value));
    }
  } else {
    Serial.println("\n✗ FAILED! Could not read register.");
    Serial.println("Error: " + getModbusError(result));
    logMessage("Modbus Read on CH" + String(currentChannel) + ": FAILED - " + getModbusError(result));
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, markParity ? SERIAL_8O1 : SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  Serial.println("\nModbus read completed. Previous settings restored.");
}

void writeModbusRegister(int slaveId, uint16_t reg, uint16_t value) {
  Serial.println("\n=== Writing Modbus Register on Channel " + String(currentChannel) + " ===");
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  monitoringActive = false;
  
  // Configure serial for Modbus
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(MODBUS_BAUD_RATE, SERIAL_8N1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  Serial.println("Writing to device ID " + String(slaveId) + ", register " + String(reg) + ", value " + String(value));
  
  // Initialize ModbusMaster
  modbus.begin(slaveId, DISPENSER_SERIAL);
  
  // Set callbacks for RS485 direction control
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
  
  // Determine register type and adjust address
  uint8_t function;
  uint16_t modbusReg;
  
  if (reg >= 1 && reg <= 9999) {
    // Coil (0xxxx)
    function = 5; // Write Single Coil
    modbusReg = reg - 1;
  } else if (reg >= 40001 && reg <= 49999) {
    // Holding Register (4xxxx)
    function = 6; // Write Single Register
    modbusReg = reg - 40001;
  } else {
    Serial.println("Invalid register address for writing. Use format:");
    Serial.println("1-9999: Coils");
    Serial.println("40001-49999: Holding Registers");
    
    // Restore previous settings
    monitoringActive = wasMonitoring;
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, markParity ? SERIAL_8O1 : SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
    return;
  }
  
  // Execute appropriate function
  uint8_t result;
  switch (function) {
    case 5:
      Serial.println("Writing to coil...");
      result = modbus.writeSingleCoil(modbusReg, value ? 0xFF00 : 0x0000);
      break;
    case 6:
      Serial.println("Writing to holding register...");
      result = modbus.writeSingleRegister(modbusReg, value);
      break;
  }
  
  if (result == modbus.ku8MBSuccess) {
    Serial.println("\n✓ SUCCESS! Write successful.");
    logMessage("Modbus Write on CH" + String(currentChannel) + ": SUCCESS - Register " + 
               String(reg) + " = " + String(value));
    
    // Read back the value to verify
    Serial.println("Reading back value to verify...");
    delay(100);
    
    if (function == 5) {
      result = modbus.readCoils(modbusReg, 1);
      if (result == modbus.ku8MBSuccess) {
        uint16_t readValue = modbus.getResponseBuffer(0) & 0x01;
        Serial.println("Read back value: " + String(readValue));
        if (readValue == (value ? 1 : 0)) {
          Serial.println("✓ Verification successful!");
        } else {
          Serial.println("✗ Verification failed! Read value doesn't match written value.");
        }
      } else {
        Serial.println("✗ Verification read failed: " + getModbusError(result));
      }
    } else {
      result = modbus.readHoldingRegisters(modbusReg, 1);
      if (result == modbus.ku8MBSuccess) {
        uint16_t readValue = modbus.getResponseBuffer(0);
        Serial.println("Read back value: " + String(readValue));
        if (readValue == value) {
          Serial.println("✓ Verification successful!");
        } else {
          Serial.println("✗ Verification failed! Read value doesn't match written value.");
        }
      } else {
        Serial.println("✗ Verification read failed: " + getModbusError(result));
      }
    }
  } else {
    Serial.println("\n✗ FAILED! Could not write to register.");
    Serial.println("Error: " + getModbusError(result));
    logMessage("Modbus Write on CH" + String(currentChannel) + ": FAILED - " + getModbusError(result));
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, markParity ? SERIAL_8O1 : SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  Serial.println("\nModbus write completed. Previous settings restored.");
}

String getModbusError(uint8_t errorCode) {
  switch (errorCode) {
    case modbus.ku8MBIllegalFunction:
      return "Illegal Function (01)";
    case modbus.ku8MBIllegalDataAddress:
      return "Illegal Data Address (02)";
    case modbus.ku8MBIllegalDataValue:
      return "Illegal Data Value (03)";
    case modbus.ku8MBSlaveDeviceFailure:
      return "Slave Device Failure (04)";
    case modbus.ku8MBInvalidSlaveID:
      return "Invalid Slave ID";
    case modbus.ku8MBInvalidFunction:
      return "Invalid Function";
    case modbus.ku8MBResponseTimedOut:
      return "Response Timeout";
    case modbus.ku8MBInvalidCRC:
      return "Invalid CRC";
    default:
      return "Unknown Error (0x" + String(errorCode, HEX) + ")";
  }
}

void testRS232Connection() {
  Serial.println("\nTesting RS232 Connection on MUX Channel " + String(currentChannel));
  Serial.println("--------------------------------------------------");
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  bool wasMarkParity = markParity;
  monitoringActive = false;
  
  // Clear any pending data
  while (DISPENSER_SERIAL.available()) {
    DISPENSER_SERIAL.read();
  }
  
  // Test with both parity settings
  Serial.println("Testing with MARK parity (SERIAL_8O1)...");
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Send test pattern with address byte
  byte testAddress = 0x01;
  DISPENSER_SERIAL.write(testAddress);
  Serial.println("Sent address byte: 0x01");
  delay(100);
  
  // Switch to SPACE parity
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Send test command (Read Shift Total)
  byte testCmd[] = {0x03, 0x8D, 0x6F};
  DISPENSER_SERIAL.write(testCmd, 3);
  Serial.println("Sent command bytes: 03 8D 6F (Read Shift Total)");
  
  // Wait for response
  Serial.println("Waiting for response (5 seconds)...");
  unsigned long startTime = millis();
  String response = "Response: ";
  bool receivedData = false;
  
  while (millis() - startTime < 5000) {
    if (DISPENSER_SERIAL.available() > 0) {
      receivedData = true;
      byte b = DISPENSER_SERIAL.read();
      response += byteToHexString(b) + " ";
      delay(10);
    }
    
    if (receivedData && !DISPENSER_SERIAL.available() && (millis() - startTime > 1000)) {
      break; // Exit if we've received data and there's been a pause
    }
  }
  
  if (receivedData) {
    Serial.println(response);
    Serial.println("RS232 TEST RESULT: SUCCESS - Received response from device on channel " + String(currentChannel));
    logMessage("RS232 Test on CH" + String(currentChannel) + ": SUCCESS - " + response);
  } else {
    Serial.println("No response received");
    Serial.println("RS232 TEST RESULT: FAILED - No response from device on channel " + String(currentChannel));
    Serial.println("Possible issues:");
    Serial.println("1. Device not powered on");
    Serial.println("2. Incorrect wiring");
    Serial.println("3. Device not responding to this command");
    Serial.println("4. Try different parity settings or commands");
    Serial.println("5. Verify multiplexer channel selection");
    logMessage("RS232 Test on CH" + String(currentChannel) + ": FAILED - No response received");
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  if (wasMarkParity) {
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  }
  
  Serial.println("Test completed. Previous settings restored.");
  Serial.println();
}

void testRS232Loopback() {
  Serial.println("\nTesting RS232 in Loopback Mode on MUX Channel " + String(currentChannel));
  Serial.println("--------------------------------------------------------");
  Serial.println("IMPORTANT: Connect TX (GPIO17) to RX (GPIO16) for this test!");
  Serial.println("This test verifies the RS232 hardware and multiplexer channel " + String(currentChannel));
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  monitoringActive = false;
  
  // Clear any pending data
  while (DISPENSER_SERIAL.available()) {
    DISPENSER_SERIAL.read();
  }
  
  // Use standard settings for loopback test
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8N1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Send test pattern
  byte testPattern[] = {0xAA, 0x55, 0x12, 0x34, 0x78, 0x9A};
  DISPENSER_SERIAL.write(testPattern, sizeof(testPattern));
  
  Serial.print("Sent test pattern: ");
  for (size_t i = 0; i < sizeof(testPattern); i++) {
    Serial.print(byteToHexString(testPattern[i]) + " ");
  }
  Serial.println();
  
  // Wait for response
  Serial.println("Waiting for loopback data (2 seconds)...");
  unsigned long startTime = millis();
  String response = "Received: ";
  int bytesReceived = 0;
  
  while (millis() - startTime < 2000 && bytesReceived < sizeof(testPattern)) {
    if (DISPENSER_SERIAL.available() > 0) {
      byte b = DISPENSER_SERIAL.read();
      response += byteToHexString(b) + " ";
      bytesReceived++;
    }
  }
  
  Serial.println(response);
  
  if (bytesReceived == sizeof(testPattern)) {
    Serial.println("LOOPBACK TEST RESULT: SUCCESS - All bytes received correctly on channel " + String(currentChannel));
    logMessage("RS232 Loopback Test on CH" + String(currentChannel) + ": SUCCESS");
  } else {
    Serial.println("LOOPBACK TEST RESULT: FAILED - Received " + String(bytesReceived) + 
                   " of " + String(sizeof(testPattern)) + " bytes on channel " + String(currentChannel));
    Serial.println("Possible issues:");
    Serial.println("1. TX and RX not connected");
    Serial.println("2. Hardware issue with RS232 converter");
    Serial.println("3. Multiplexer not working correctly on channel " + String(currentChannel));
    logMessage("RS232 Loopback Test on CH" + String(currentChannel) + ": FAILED - Received " + String(bytesReceived) + " bytes");
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  if (markParity) {
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  } else {
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  }
  
  Serial.println("Test completed. Previous settings restored.");
  Serial.println();
}

void testU913Device() {
  Serial.println("\nTesting U913 Device on MUX Channel " + String(currentChannel));
  Serial.println("--------------------------------------------------");
  
  // Save current settings
  bool wasMonitoring = monitoringActive;
  bool wasMarkParity = markParity;
  monitoringActive = false;
  
  // Clear any pending data
  while (DISPENSER_SERIAL.available()) {
    DISPENSER_SERIAL.read();
  }
  
  // Test with both parity settings for U913
  Serial.println("Testing U913 communication protocol...");
  
  // Step 1: Set MARK parity for address byte
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Send address byte (typically 0x01 for U913)
  byte u913Address = 0x01;
  DISPENSER_SERIAL.write(u913Address);
  Serial.println("Sent U913 address byte: 0x01");
  delay(100);
  
  // Step 2: Switch to SPACE parity for command data
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  delay(100);
  
  // Send U913 status request command
  // Command format: [Length][Command Code][Checksum]
  byte u913Cmd[] = {0x03, 0x8F, 0x6D}; // Request real-time data command
  DISPENSER_SERIAL.write(u913Cmd, 3);
  Serial.println("Sent U913 command bytes: 03 8F 6D (Request real-time data)");
  
  // Wait for response from U913
  Serial.println("Waiting for U913 response (5 seconds)...");
  unsigned long startTime = millis();
  String response = "U913 Response: ";
  bool receivedData = false;
  byte responseBuffer[20]; // Buffer to store response
  int responseLength = 0;
  
  while (millis() - startTime < 5000) {
    if (DISPENSER_SERIAL.available() > 0) {
      receivedData = true;
      byte b = DISPENSER_SERIAL.read();
      responseBuffer[responseLength++] = b;
      response += byteToHexString(b) + " ";
      delay(10);
      
      // Reset timeout when receiving data
      if (responseLength == 1) {
        startTime = millis();
      }
    }
    
    // If we've received data and there's a pause, we might be done
    if (receivedData && !DISPENSER_SERIAL.available() && (millis() - startTime > 1000)) {
      break;
    }
  }
  
  if (receivedData) {
    Serial.println(response);
    
    // Analyze U913 response
    if (responseLength >= 3) {
      byte status = responseBuffer[0];
      byte cmdEcho = responseBuffer[1];
      
      Serial.println("\nU913 Response Analysis:");
      Serial.println("Status byte: " + byteToHexString(status) + " (" + (status == 0 ? "Success" : "Error") + ")");
      Serial.println("Command echo: " + byteToHexString(cmdEcho) + " (Expected: 8F)");
      
      if (responseLength >= 13 && cmdEcho == 0x8F) {
        // Parse real-time data response (expected to be 13 bytes)
        long volume = (responseBuffer[3] << 24) | (responseBuffer[4] << 16) | 
                      (responseBuffer[5] << 8) | responseBuffer[6];
        long amount = (responseBuffer[7] << 24) | (responseBuffer[8] << 16) | 
                      (responseBuffer[9] << 8) | responseBuffer[10];
        byte pumpStatus = responseBuffer[11];
        
        Serial.println("Volume: " + String(volume));
        Serial.println("Amount: " + String(amount));
        Serial.println("Pump Status: " + byteToHexString(pumpStatus));
        
        switch (pumpStatus) {
          case 0x00: Serial.println("Pump Status: Idle"); break;
          case 0x01: Serial.println("Pump Status: Authorized"); break;
          case 0x02: Serial.println("Pump Status: Fueling"); break;
          case 0x03: Serial.println("Pump Status: Fueling Complete"); break;
          case 0x04: Serial.println("Pump Status: Suspended"); break;
          default: Serial.println("Pump Status: Unknown"); break;
        }
      }
    }
    
    Serial.println("\nU913 TEST RESULT: SUCCESS - Received response from U913 device on channel " + String(currentChannel));
    logMessage("U913 Test on CH" + String(currentChannel) + ": SUCCESS - " + response);
  } else {
    Serial.println("No response received from U913");
    Serial.println("U913 TEST RESULT: FAILED - No response from device on channel " + String(currentChannel));
    Serial.println("Possible issues:");
    Serial.println("1. U913 device not powered on");
    Serial.println("2. Incorrect wiring");
    Serial.println("3. Wrong multiplexer channel selected");
    Serial.println("4. U913 device not responding to this command");
    logMessage("U913 Test on CH" + String(currentChannel) + ": FAILED - No response received");
  }
  
  // Restore previous settings
  monitoringActive = wasMonitoring;
  if (wasMarkParity) {
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  } else {
    DISPENSER_SERIAL.end();
    DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  }
  
  Serial.println("U913 test completed. Previous settings restored.");
  Serial.println();
}

void printStatus() {
  Serial.println("\nCurrent Status:");
  Serial.println("--------------");
  Serial.println("Monitoring: " + String(monitoringActive ? "Active" : "Inactive"));
  Serial.println("Parity: " + String(markParity ? "MARK" : "SPACE"));
  Serial.println("Current MUX Channel: " + String(currentChannel));
  Serial.println("Log entries: " + String(logCount));
  Serial.println();
}

void monitorSerialCommunication() {
  // Check for incoming data
  if (DISPENSER_SERIAL.available() > 0) {
    String receivedData = "RX (CH" + String(currentChannel) + "): ";
    unsigned long startTime = millis();
    
    // Read all available bytes
    while (DISPENSER_SERIAL.available() > 0 && (millis() - startTime < 500)) {
      byte b = DISPENSER_SERIAL.read();
      receivedData += byteToHexString(b) + " ";
      delay(1);
    }
    
    logMessage(receivedData);
  }
}

void sendHexData(String hexData) {
  // Parse hex string and send bytes
  hexData.replace(" ", ""); // Remove spaces
  
  if (hexData.length() % 2 != 0) {
    logMessage("Error: Invalid hex string length");
    Serial.println("Error: Invalid hex string length");
    return;
  }
  
  String txLog = "TX (CH" + String(currentChannel) + "): ";
  
  for (unsigned int i = 0; i < hexData.length(); i += 2) {
    String byteStr = hexData.substring(i, i + 2);
    byte b = strtol(byteStr.c_str(), NULL, 16);
    DISPENSER_SERIAL.write(b);
    txLog += byteToHexString(b) + " ";
  }
  
  logMessage(txLog);
}

String byteToHexString(byte b) {
  String hex = String(b, HEX);
  if (hex.length() == 1) {
    hex = "0" + hex;
  }
  hex.toUpperCase(); // This modifies the string in place
  return hex;
}

void logMessage(String message) {
  // Add timestamp
  String timestamp = String(millis() / 1000.0, 3);
  String logEntry = "[" + timestamp + "s] " + message;
  
  // Add to circular buffer
  communicationLog[logHead] = logEntry;
  logHead = (logHead + 1) % LOG_BUFFER_SIZE;
  if (logCount < LOG_BUFFER_SIZE) {
    logCount++;
  }
  
  // Also print to serial console
  Serial.println(logEntry);
}

// Function to print all logs
void printAllLogs() {
  Serial.println("\nLog History:");
  Serial.println("-----------");
  
  if (logCount == 0) {
    Serial.println("No logs available.");
    return;
  }
  
  int startIdx = (logCount < LOG_BUFFER_SIZE) ? 0 : (logHead);
  
  for (int i = 0; i < logCount; i++) {
    int idx = (startIdx + i) % LOG_BUFFER_SIZE;
    Serial.println(communicationLog[idx]);
  }
  
  Serial.println("-----------");
}
