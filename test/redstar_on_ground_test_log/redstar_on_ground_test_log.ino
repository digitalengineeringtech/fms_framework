/*
 * Fuel Dispenser Control - Arduino Implementation
 * With LittleFS logging and web download interface
 * 
 * This sketch allows an Arduino to communicate with a fuel dispenser
 * using the same protocol as the original VB6 application.
 * It also logs all communication to LittleFS and provides a web interface
 * to download the logs.
 * 
 * Commands are sent via Serial Monitor in the format:
 * COMMAND,ADDRESS,DATA
 * 
 * Where:
 * - COMMAND is one of: PRICE, CLEAR, READ, STOP, REALTIME
 * - ADDRESS is the nozzle ID (1-255)
 * - DATA is optional and only used for price change (default: 1234)
 * 
 * Examples:
 * PRICE,1,1234    - Change price to 1234 for nozzle 1
 * READ,1          - Read shift total for nozzle 1
 * CLEAR,1         - Clear shift total for nozzle 1
 * STOP,1          - Stop pump for nozzle 1
 * REALTIME,1      - Request real-time amount and volume for nozzle 1
 */

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <LittleFS.h>
  ESP8266WebServer server(80);
#else
  #include <WiFi.h>
  #include <WebServer.h>
  #include <FS.h>
  #include <LittleFS.h>
  WebServer server(80);
#endif

#include <time.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// File system variables
#define LOG_FILENAME "/dispenser_log.txt"
#define MAX_LOG_SIZE 1048576  // 1MB max log size
File logFile;

// Define pins for hardware serial to communicate with fuel dispenser
// We'll use Serial1 for dispenser communication (pins 0,1 on many Arduinos)
// and the default Serial for debug output and commands

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
  
  // Initialize serial communication with fuel dispenser
  // Note: Some Arduinos have multiple hardware serial ports (Serial1, Serial2, etc.)
  // If your Arduino doesn't have multiple hardware serial ports, you might need
  // to use the SoftwareSerial library instead.
  Serial1.begin(2400, SERIAL_8O1); // 8 data bits, odd parity, 1 stop bit
  
  Serial.println(F("Fuel Dispenser Control"));
  Serial.println(F("Commands:"));
  Serial.println(F("  PRICE,address,value   - Change price"));
  Serial.println(F("  CLEAR,address         - Clear shift total"));
  Serial.println(F("  READ,address          - Read shift total"));
  Serial.println(F("  STOP,address          - Stop pump"));
  Serial.println(F("  REALTIME,address      - Request real-time data"));
  Serial.println();
  
  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println(F("LittleFS initialization failed!"));
  } else {
    Serial.println(F("LittleFS initialized successfully."));
    // Check log file size and rotate if needed
    checkLogFileSize();
  }
  
  // Connect to WiFi
  setupWiFi();
  
  // Setup web server
  setupWebServer();
  
  // Get time from NTP server
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  // Handle web server clients
  server.handleClient();
  
  // Check for commands from Serial Monitor
  if (Serial.available()) {
    processSerialCommand();
  }
  
  // Check for response from dispenser if we're waiting for one
  if (waitingForResponse) {
    checkDispenserResponse();
    
    // Check for timeout (3 seconds)
    if (millis() - commandStartTime > 3000) {
      String logMessage = "Response timeout";
      Serial.println(logMessage);
      writeToLog(logMessage);
      waitingForResponse = false;
    }
  }
}

void setupWiFi() {
  Serial.print(F("Connecting to WiFi"));
  WiFi.begin(ssid, password);
  
  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print(F("Connected to "));
    Serial.println(ssid);
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println(F("Failed to connect to WiFi. Operating in offline mode."));
  }
}

void setupWebServer() {
  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/download", HTTP_GET, handleDownload);
  server.on("/clear", HTTP_GET, handleClearLog);
  
  // Start server
  server.begin();
  Serial.println(F("HTTP server started"));
}

void handleRoot() {
  String html = "<html><head><title>Fuel Dispenser Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += "h1 { color: #333; }";
  html += ".btn { display: inline-block; padding: 10px 20px; margin: 10px 0; ";
  html += "background-color: #4CAF50; color: white; text-decoration: none; border-radius: 4px; }";
  html += ".btn.red { background-color: #f44336; }";
  html += ".info { margin: 20px 0; padding: 10px; background-color: #f1f1f1; border-radius: 4px; }";
  html += "</style></head><body>";
  html += "<h1>Fuel Dispenser Control</h1>";
  
  html += "<div class='info'>";
  html += "<p><strong>Device IP:</strong> " + WiFi.localIP().toString() + "</p>";
  
  // Get log file size
  if (LittleFS.exists(LOG_FILENAME)) {
    File f = LittleFS.open(LOG_FILENAME, "r");
    size_t fileSize = f.size();
    f.close();
    html += "<p><strong>Log File Size:</strong> " + formatBytes(fileSize) + "</p>";
  } else {
    html += "<p><strong>Log File:</strong> Not created yet</p>";
  }
  html += "</div>";
  
  html += "<a href='/download' class='btn'>Download Log</a> ";
  html += "<a href='/clear' class='btn red'>Clear Log</a>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleDownload() {
  if (LittleFS.exists(LOG_FILENAME)) {
    File file = LittleFS.open(LOG_FILENAME, "r");
    if (file) {
      server.sendHeader("Content-Disposition", "attachment; filename=dispenser_log.txt");
      server.streamFile(file, "text/plain");
      file.close();
      return;
    }
  }
  
  server.send(404, "text/plain", "Log file not found");
}

void handleClearLog() {
  if (LittleFS.exists(LOG_FILENAME)) {
    LittleFS.remove(LOG_FILENAME);
    
    // Create a new empty log file
    File file = LittleFS.open(LOG_FILENAME, "w");
    if (file) {
      file.println("Log cleared at " + getTimeStamp());
      file.close();
    }
    
    server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='2;url=/'></head><body><p>Log cleared successfully. Redirecting...</p></body></html>");
  } else {
    server.send(404, "text/plain", "Log file not found");
  }
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0, 2) + " KB";
  } else {
    return String(bytes / 1024.0 / 1024.0, 2) + " MB";
  }
}

void checkLogFileSize() {
  if (LittleFS.exists(LOG_FILENAME)) {
    File file = LittleFS.open(LOG_FILENAME, "r");
    size_t fileSize = file.size();
    file.close();
    
    if (fileSize > MAX_LOG_SIZE) {
      // Rotate log file - rename current to .old and create new one
      if (LittleFS.exists(LOG_FILENAME ".old")) {
        LittleFS.remove(LOG_FILENAME ".old");
      }
      
      LittleFS.rename(LOG_FILENAME, LOG_FILENAME ".old");
      
      // Create a new log file with header
      File newFile = LittleFS.open(LOG_FILENAME, "w");
      if (newFile) {
        newFile.println("Log rotated at " + getTimeStamp());
        newFile.println("Previous log saved as dispenser_log.txt.old");
        newFile.println("-------------------------------------------");
        newFile.close();
      }
    }
  }
}

void writeToLog(String message) {
  // Check log file size and rotate if needed
  checkLogFileSize();
  
  // Open log file for appending
  File logFile = LittleFS.open(LOG_FILENAME, "a");
  if (logFile) {
    logFile.println(getTimeStamp() + " - " + message);
    logFile.close();
  }
}

String getTimeStamp() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  
  char buffer[24];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  
  return String(buffer);
}

void processSerialCommand() {
  // Read command from Serial Monitor
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  // Parse command
  int firstComma = command.indexOf(',');
  if (firstComma == -1) {
    String logMessage = "Invalid command format. Use: COMMAND,ADDRESS[,DATA]";
    Serial.println(logMessage);
    writeToLog(logMessage);
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
    writeToLog("Command: PRICE CHANGE, Address: " + String(dispenserAddress) + ", Value: " + String(priceValue));
    sendCommand(CMD_PRICE_CHANGE);
  } 
  else if (cmdType == "CLEAR") {
    writeToLog("Command: CLEAR SHIFT TOTAL, Address: " + String(dispenserAddress));
    sendCommand(CMD_CLEAR_SHIFT);
  } 
  else if (cmdType == "READ") {
    writeToLog("Command: READ SHIFT TOTAL, Address: " + String(dispenserAddress));
    sendCommand(CMD_READ_SHIFT);
  } 
  else if (cmdType == "STOP") {
    writeToLog("Command: STOP PUMP, Address: " + String(dispenserAddress));
    sendCommand(CMD_STOP_PUMP);
  } 
  else if (cmdType == "REALTIME") {
    writeToLog("Command: REALTIME DATA, Address: " + String(dispenserAddress));
    sendCommand(CMD_REALTIME);
  } 
  else {
    String logMessage = "Unknown command: " + cmdType + ". Valid commands: PRICE, CLEAR, READ, STOP, REALTIME";
    Serial.println(logMessage);
    writeToLog(logMessage);
  }
}

void sendCommand(int command) {
  // Store current command
  currentCommand = command;
  
  // Step 1: Send address byte with mark parity (simulated)
  // For mark parity (always 1), we'll use odd parity and ensure the byte has odd parity
  Serial1.end();
  Serial1.begin(2400, SERIAL_8O1); // 8 data bits, odd parity, 1 stop bit
  
  // Ensure the byte has odd parity by setting the MSB if needed
  // This simulates mark parity by ensuring the parity bit is always 1
  byte addressByte = dispenserAddress;
  if (!hasOddParity(addressByte)) {
    // If it doesn't have odd parity, toggle the MSB to make it odd
    addressByte |= 0x80;
  }
  
  Serial1.write(addressByte);
  delay(10); // Short delay to ensure byte is sent
  
  // Step 2: Send command with space parity (simulated)
  // For space parity (always 0), we'll use even parity and ensure the byte has even parity
  Serial1.end();
  Serial1.begin(2400, SERIAL_8E1); // 8 data bits, even parity, 1 stop bit
  
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
  
  // Send command bytes with even parity (simulating space parity)
  for (int i = 0; i < outLength; i++) {
    // Ensure the byte has even parity by setting the MSB if needed
    // This simulates space parity by ensuring the parity bit is always 0
    byte cmdByte = outBytes[i];
    if (hasOddParity(cmdByte)) {
      // If it has odd parity, toggle the MSB to make it even
      cmdByte |= 0x80;
    }
    Serial1.write(cmdByte);
    delay(2); // Small delay between bytes
  }
  
  // Debug output
  String txdStr = "TXD: ";
  txdStr += String(dispenserAddress, HEX) + " ";
  for (int i = 0; i < outLength; i++) {
    if (outBytes[i] < 0x10) txdStr += "0";
    txdStr += String(outBytes[i], HEX) + " ";
  }
  Serial.println(txdStr);
  writeToLog(txdStr);
  
  // Set up for response
  String rxdStr = "RXD: ";
  Serial.print(rxdStr);
  writeToLog(rxdStr);
  
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

// Function to check if a byte has odd parity
bool hasOddParity(byte b) {
  byte count = 0;
  for (byte i = 0; i < 8; i++) {
    if (b & (1 << i)) {
      count++;
    }
  }
  return (count % 2) == 1; // Odd number of 1 bits
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
  String rxdStr = "";
  for (int i = 0; i < bufferIndex; i++) {
    if (inputBuffer[i] < 0x10) rxdStr += "0";
    rxdStr += String(inputBuffer[i], HEX) + " ";
  }
  Serial.println(rxdStr);
  writeToLog(rxdStr);
  
  String resultStr = "";
  
  // Process response based on command type
  switch (currentCommand) {
    case CMD_PRICE_CHANGE:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          resultStr = "Price change: ACK (Success)";
        } else {
          resultStr = "Price change: NACK (Failed)";
        }
      }
      break;
      
    case CMD_CLEAR_SHIFT:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          resultStr = "Clear shift total: ACK (Success)";
        } else {
          resultStr = "Clear shift total: NACK (Failed)";
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
                 
        resultStr = "Shift Total - Volume: " + String(volume) + ", Amount: " + String(amount);
      }
      break;
      
    case CMD_STOP_PUMP:
      if (bufferIndex >= 3) {
        if (inputBuffer[1] == 0x06) {
          resultStr = "Stop pump: ACK (Success)";
        } else {
          resultStr = "Stop pump: NACK (Failed)";
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
                 
        resultStr = "Real-time data - Status: " + String(status, HEX) + 
                    ", Volume: " + String(volume) + 
                    ", Amount: " + String(amount);
      }
      break;
  }
  
  if (resultStr.length() > 0) {
    Serial.println(resultStr);
    writeToLog(resultStr);
  }
}
