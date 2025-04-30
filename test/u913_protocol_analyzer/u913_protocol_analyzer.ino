/*
 * Fuel Dispenser Protocol Analyzer for ESP32
 * 
 * This utility helps analyze and debug the communication protocol
 * between the ESP32 and the fuel dispenser.
 * 
 * It captures and displays all serial communication in both directions,
 * showing the raw bytes and their interpretation.
 * 
 * Commands (via Serial Monitor):
 * - START: Start monitoring
 * - STOP: Stop monitoring
 * - CLEAR: Clear logs
 * - MARK: Switch to MARK parity
 * - SPACE: Switch to SPACE parity
 * - SEND <hex data>: Send hex data (e.g., SEND 01 03 8F 6D)
 */
/*
Channel	Binary	S1	  S0
0	        00	    0	  0
1	        01	    0	  1
2	        10	    1	  0
3	        11	    1	  1

uint8_t channel = 3;  // Binary: 00000011
channel >> 1:  00000001   (decimal 1)
00000001
& 00000001
-----------
  00000001  → 1
*/


#include <HardwareSerial.h>
// Simple circular buffer implementation
#define LOG_BUFFER_SIZE 100
#define MUX_S0 25
#define MUX_S1 26
#define MUX_EN 27
#define DISPENSER_RX_PIN 16  // UART RX pin via MUX
#define DISPENSER_TX_PIN 17  // Direct TX to device opto-LED

// Serial communication settings
#define DISPENSER_SERIAL Serial2
// #define DISPENSER_RX_PIN 16  // GPIO16 for RX
// #define DISPENSER_TX_PIN 17  // GPIO17 for TX
#define BAUD_RATE 2400

// Buffer for storing communication logs
String communicationLog[LOG_BUFFER_SIZE];
int logHead = 0;
int logCount = 0;

// Monitoring state
bool monitoringActive = false;
bool markParity = true;  // Start with mark parity

void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_EN, LOW);
  vTaskDelay(pdMS_TO_TICKS(2));  // Let MUX stabilize
}

void setup() {
  // Initialize serial for debugging and commands
  Serial.begin(115200);
  Serial.println("\n\nFuel Dispenser Protocol Analyzer");
  Serial.println("================================");

  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_EN, OUTPUT);

  // Initialize dispenser serial with mark parity (odd parity)
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);

  // Add initial log entry
  logMessage("Protocol Analyzer started. Waiting for communication...");

  // Print help
  printHelp();
}

void loop() {
  // Check for serial commands
  checkSerialCommands(2);

  // Monitor serial communication if active
  if (monitoringActive) {
    monitorSerialCommunication();
  }
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
  Serial.println("HELP - Show this help message");
  Serial.println("STATUS - Show current status");
  Serial.println("LOGS - Show all stored logs");
  Serial.println();
}

void checkSerialCommands(uint8_t channel) {
  selectMuxChannel(channel);
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command == "START") {
      monitoringActive = true;
      logMessage("Monitoring started");
      Serial.println("Monitoring started");
    } else if (command == "STOP") {
      monitoringActive = false;
      logMessage("Monitoring stopped");
      Serial.println("Monitoring stopped");
    } else if (command == "CLEAR") {
      logHead = 0;
      logCount = 0;
      logMessage("Log cleared");
      Serial.println("Log cleared");
    } else if (command == "MARK") {
      markParity = true;
      DISPENSER_SERIAL.end();
      DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8O1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
      logMessage("Switched to MARK parity");
      Serial.println("Switched to MARK parity");
    } else if (command == "SPACE") {
      markParity = false;
      DISPENSER_SERIAL.end();
      DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8E1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
      logMessage("Switched to SPACE parity");
      Serial.println("Switched to SPACE parity");
    } else if (command.startsWith("SEND ")) {
      String hexData = command.substring(5);
      sendHexData(hexData);
      Serial.println("Data sent: " + hexData);
    } else if (command == "HELP") {
      printHelp();
    } else if (command == "STATUS") {
      printStatus();
    } else if (command == "LOGS") {
      printAllLogs();
    } else {
      Serial.println("Unknown command. Type HELP for available commands.");
    }
  }
}

void printStatus() {
  Serial.println("\nCurrent Status:");
  Serial.println("--------------");
  Serial.println("Monitoring: " + String(monitoringActive ? "Active" : "Inactive"));
  Serial.println("Parity: " + String(markParity ? "MARK" : "SPACE"));
  Serial.println("Log entries: " + String(logCount));
  Serial.println();
}

void monitorSerialCommunication() {
  // Check for incoming data
  if (DISPENSER_SERIAL.available() > 0) {
    String receivedData = "RX: ";
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
  hexData.replace(" ", "");  // Remove spaces

  if (hexData.length() % 2 != 0) {
    logMessage("Error: Invalid hex string length");
    Serial.println("Error: Invalid hex string length");
    return;
  }

  String txLog = "TX: ";

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
  hex.toUpperCase();  // This modifies the string in place
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
