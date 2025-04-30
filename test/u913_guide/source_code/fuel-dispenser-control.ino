/*
 * Fuel Dispenser Control System for ESP32
 * 
 * This program allows an ESP32 to communicate with a fuel dispenser
 * using the same serial communication protocol as the original VB6 application.
 * 
 * Features:
 * - Price change
 * - Read shift total
 * - Clear shift total
 * - Stop pump
 * - Request real-time amount and volume
 * 
 * Protocol details:
 * - Address byte sent with MARK parity (2400,m,8,1)
 * - Command data sent with SPACE parity (2400,s,8,1)
 * - Checksum calculation: (0x10000 - (address + all command bytes)) & 0xFF
 */

#include <HardwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Serial communication settings
#define DISPENSER_SERIAL Serial2
#define DISPENSER_RX_PIN 16  // GPIO16 for RX
#define DISPENSER_TX_PIN 17  // GPIO17 for TX
#define BAUD_RATE 2400

// Web server settings
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
WebServer server(80);

// Command definitions
#define CMD_PRICE_CHANGE 0
#define CMD_CLEAR_SHIFT_TOTAL 1
#define CMD_READ_SHIFT_TOTAL 2
#define CMD_STOP_PUMP 3
#define CMD_REQUEST_REALTIME 4

// Global variables
byte address = 1;  // Default nozzle ID
String responseMessage = "";
bool waitingForResponse = false;
unsigned long responseTimeout = 0;
byte inputBuffer[32];
int inputLength = 0;
int expectedResponseLength = 0;
int currentCommand = -1;
int comPort = 2;  // Default COM port

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("Fuel Dispenser Control System");
  
  // Initialize SPIFFS for storing web files
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Initialize dispenser serial
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8M1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server routes
  setupWebServer();
  
  // Start web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle web server clients
  server.handleClient();
  
  // Check for serial data from dispenser
  if (waitingForResponse && DISPENSER_SERIAL.available() > 0) {
    readDispenserResponse();
  }
  
  // Check for response timeout
  if (waitingForResponse && millis() > responseTimeout) {
    waitingForResponse = false;
    responseMessage += "Response timeout\n";
    Serial.println("Response timeout");
  }
}

void setupWebServer() {
  // Serve the web interface
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHTML());
  });
  
  // API endpoints
  server.on("/api/command", HTTP_POST, handleCommand);
  server.on("/api/status", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    doc["address"] = address;
    doc["comPort"] = comPort;
    doc["response"] = responseMessage;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Handle not found
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });
}

void handleCommand() {
  if (!server.hasArg("command") || !server.hasArg("address")) {
    server.send(400, "text/plain", "Missing parameters");
    return;
  }
  
  int command = server.arg("command").toInt();
  address = server.arg("address").toInt();
  comPort = server.hasArg("comPort") ? server.arg("comPort").toInt() : 2;
  String data = server.hasArg("data") ? server.arg("data") : "1234";
  
  executeCommand(command, address, data);
  
  server.send(200, "text/plain", "Command sent");
}

void executeCommand(int command, byte nozzleAddress, String data) {
  // Store current command
  currentCommand = command;
  
  // Clear previous response
  responseMessage = "";
  
  // Set address with MARK parity
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8M1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  // Send address byte first
  DISPENSER_SERIAL.write(nozzleAddress);
  delay(50);  // Small delay to ensure proper transmission
  
  // Change to SPACE parity for command data
  DISPENSER_SERIAL.end();
  DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8S1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
  
  // Clear input buffer
  while(DISPENSER_SERIAL.available()) {
    DISPENSER_SERIAL.read();
  }
  
  // Prepare command based on command type
  switch (command) {
    case CMD_PRICE_CHANGE: {
      // Price change command
      long price = data.toInt();
      byte cmdBytes[7];
      cmdBytes[0] = 0x07;  // Length
      cmdBytes[1] = 0x80;  // Command code
      cmdBytes[2] = (price / 0x1000000) & 0xFF;
      cmdBytes[3] = (price / 0x10000) & 0xFF;
      cmdBytes[4] = (price / 0x100) & 0xFF;
      cmdBytes[5] = price & 0xFF;
      cmdBytes[6] = calculateChecksum(nozzleAddress, cmdBytes, 6);
      
      DISPENSER_SERIAL.write(cmdBytes, 7);
      
      // Log command
      responseMessage = "TXD: " + byteToHexString(nozzleAddress) + " ";
      for (int i = 0; i < 7; i++) {
        responseMessage += byteToHexString(cmdBytes[i]) + " ";
      }
      responseMessage += "\nRXD: ";
      
      // Expect 3 bytes response
      waitForResponse(3);
      break;
    }
    
    case CMD_CLEAR_SHIFT_TOTAL: {
      // Clear shift total command
      byte cmdBytes[3];
      cmdBytes[0] = 0x03;  // Length
      cmdBytes[1] = 0x04;  // Command code
      cmdBytes[2] = calculateChecksum(nozzleAddress, cmdBytes, 2);
      
      DISPENSER_SERIAL.write(cmdBytes, 3);
      
      // Log command
      responseMessage = "TXD: " + byteToHexString(nozzleAddress) + " ";
      for (int i = 0; i < 3; i++) {
        responseMessage += byteToHexString(cmdBytes[i]) + " ";
      }
      responseMessage += "\nRXD: ";
      
      // Expect 3 bytes response
      waitForResponse(3);
      break;
    }
    
    case CMD_READ_SHIFT_TOTAL: {
      // Read shift total command
      byte cmdBytes[3];
      cmdBytes[0] = 0x03;  // Length
      cmdBytes[1] = 0x8D;  // Command code
      cmdBytes[2] = calculateChecksum(nozzleAddress, cmdBytes, 2);
      
      DISPENSER_SERIAL.write(cmdBytes, 3);
      
      // Log command
      responseMessage = "TXD: " + byteToHexString(nozzleAddress) + " ";
      for (int i = 0; i < 3; i++) {
        responseMessage += byteToHexString(cmdBytes[i]) + " ";
      }
      responseMessage += "\nRXD: ";
      
      // Expect 15 bytes response
      waitForResponse(15);
      break;
    }
    
    case CMD_STOP_PUMP: {
      // Stop pump command
      byte cmdBytes[3];
      cmdBytes[0] = 0x03;  // Length
      cmdBytes[1] = 0x10;  // Command code
      cmdBytes[2] = calculateChecksum(nozzleAddress, cmdBytes, 2);
      
      DISPENSER_SERIAL.write(cmdBytes, 3);
      
      // Log command
      responseMessage = "TXD: " + byteToHexString(nozzleAddress) + " ";
      for (int i = 0; i < 3; i++) {
        responseMessage += byteToHexString(cmdBytes[i]) + " ";
      }
      responseMessage += "\nRXD: ";
      
      // Expect 3 bytes response
      waitForResponse(3);
      break;
    }
    
    case CMD_REQUEST_REALTIME: {
      // Request real-time amount and volume
      byte cmdBytes[3];
      cmdBytes[0] = 0x03;  // Length
      cmdBytes[1] = 0x8F;  // Command code
      cmdBytes[2] = calculateChecksum(nozzleAddress, cmdBytes, 2);
      
      DISPENSER_SERIAL.write(cmdBytes, 3);
      
      // Log command
      responseMessage = "TXD: " + byteToHexString(nozzleAddress) + " ";
      for (int i = 0; i < 3; i++) {
        responseMessage += byteToHexString(cmdBytes[i]) + " ";
      }
      responseMessage += "\nRXD: ";
      
      // Expect 13 bytes response
      waitForResponse(13);
      break;
    }
    
    default:
      responseMessage = "Unknown command";
      break;
  }
  
  Serial.println(responseMessage);
}

byte calculateChecksum(byte address, byte* data, int length) {
  long sum = address;
  for (int i = 0; i < length; i++) {
    sum += data[i];
  }
  return (0x10000 - sum) & 0xFF;
}

void waitForResponse(int expectedLength) {
  waitingForResponse = true;
  responseTimeout = millis() + 3000;  // 3 second timeout
  inputLength = 0;
  expectedResponseLength = expectedLength;
}

void readDispenserResponse() {
  while (DISPENSER_SERIAL.available() > 0 && waitingForResponse) {
    inputBuffer[inputLength] = DISPENSER_SERIAL.read();
    inputLength++;
    
    // Process response if we have received the expected number of bytes
    if (inputLength >= expectedResponseLength) {
      for (int i = 0; i < inputLength; i++) {
        responseMessage += byteToHexString(inputBuffer[i]) + " ";
      }
      
      // Parse response based on command type
      parseResponse();
      
      waitingForResponse = false;
      break;
    }
  }
}

void parseResponse() {
  // Add response parsing logic here based on the command type
  responseMessage += "\n\nResponse received. ";
  
  switch (currentCommand) {
    case CMD_PRICE_CHANGE:
      if (inputLength == 3) {
        byte status = inputBuffer[0];
        if (status == 0) {
          responseMessage += "Price change successful.";
        } else {
          responseMessage += "Price change failed. Status: " + String(status);
        }
      }
      break;
      
    case CMD_CLEAR_SHIFT_TOTAL:
      if (inputLength == 3) {
        byte status = inputBuffer[0];
        if (status == 0) {
          responseMessage += "Shift total cleared successfully.";
        } else {
          responseMessage += "Failed to clear shift total. Status: " + String(status);
        }
      }
      break;
      
    case CMD_READ_SHIFT_TOTAL:
      if (inputLength == 15) {
        // Extract volume, amount and transaction count from response
        long volume = (inputBuffer[3] << 24) | (inputBuffer[4] << 16) | (inputBuffer[5] << 8) | inputBuffer[6];
        long amount = (inputBuffer[7] << 24) | (inputBuffer[8] << 16) | (inputBuffer[9] << 8) | inputBuffer[10];
        int transactions = (inputBuffer[11] << 8) | inputBuffer[12];
        
        responseMessage += "Shift Total Data:\n";
        responseMessage += "Volume: " + String(volume) + "\n";
        responseMessage += "Amount: " + String(amount) + "\n";
        responseMessage += "Transactions: " + String(transactions);
      }
      break;
      
    case CMD_STOP_PUMP:
      if (inputLength == 3) {
        byte status = inputBuffer[0];
        if (status == 0) {
          responseMessage += "Pump stopped successfully.";
        } else {
          responseMessage += "Failed to stop pump. Status: " + String(status);
        }
      }
      break;
      
    case CMD_REQUEST_REALTIME:
      if (inputLength == 13) {
        // Extract volume and amount from response
        long volume = (inputBuffer[3] << 24) | (inputBuffer[4] << 16) | (inputBuffer[5] << 8) | inputBuffer[6];
        long amount = (inputBuffer[7] << 24) | (inputBuffer[8] << 16) | (inputBuffer[9] << 8) | inputBuffer[10];
        byte status = inputBuffer[11];
        
        responseMessage += "Real-time Data:\n";
        responseMessage += "Volume: " + String(volume) + "\n";
        responseMessage += "Amount: " + String(amount) + "\n";
        responseMessage += "Status: " + byteToHexString(status);
        
        // Interpret status byte
        responseMessage += "\n\nPump Status: ";
        switch (status) {
          case 0x00: responseMessage += "Idle"; break;
          case 0x01: responseMessage += "Authorized"; break;
          case 0x02: responseMessage += "Fueling"; break;
          case 0x03: responseMessage += "Fueling Complete"; break;
          case 0x04: responseMessage += "Suspended"; break;
          default: responseMessage += "Unknown (" + byteToHexString(status) + ")";
        }
      }
      break;
  }
}

String byteToHexString(byte b) {
  String hex = String(b, HEX);
  if (hex.length() == 1) {
    hex = "0" + hex;
  }
  return hex.toUpperCase();
}

String getHTML() {
  // Return the HTML for the web interface
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fuel Dispenser Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        h1 {
            color: #2c3e50;
            text-align: center;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }
        input, select {
            width: 100%;
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
        }
        button {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 10px 15px;
            border-radius: 4px;
            cursor: pointer;
            margin-right: 10px;
            margin-bottom: 10px;
        }
        button:hover {
            background-color: #2980b9;
        }
        .response {
            margin-top: 20px;
            padding: 15px;
            background-color: #f9f9f9;
            border: 1px solid #ddd;
            border-radius: 4px;
            white-space: pre-wrap;
            font-family: monospace;
            min-height: 200px;
            max-height: 400px;
            overflow-y: auto;
        }
        .button-group {
            display: flex;
            flex-wrap: wrap;
            margin-bottom: 20px;
        }
        .notice {
            background-color: #fff8e1;
            padding: 10px;
            border-left: 4px solid #ffc107;
            margin-bottom: 20px;
        }
        .settings {
            border-bottom: 1px solid #eee;
            padding-bottom: 15px;
            margin-bottom: 15px;
        }
        .command-section {
            border-top: 1px solid #eee;
            padding-top: 15px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Fuel Dispenser Control System</h1>
        
        <div class="settings">
            <h2>Settings</h2>
            
            <div class="form-group">
                <label for="nozzleId">Nozzle ID:</label>
                <input type="number" id="nozzleId" value="1" min="1" max="255">
            </div>
            
            <div class="form-group">
                <label for="comPort">COM Port:</label>
                <input type="number" id="comPort" value="2" min="1" max="15">
            </div>
            
            <div class="notice">
                <p><strong>Notice:</strong></p>
                <p>* Please confirm that the nozzle ID is consistent with the central control address in the fuel dispenser parameters</p>
                <p>* The COM Port number should be from 1 to 15 in this system</p>
            </div>
        </div>
        
        <div class="command-section">
            <h2>Commands</h2>
            
            <div class="form-group">
                <label for="priceData">Price Data (for Price Change):</label>
                <input type="number" id="priceData" value="1234">
            </div>
            
            <div class="button-group">
                <button onclick="sendCommand(0)">Price Change</button>
                <button onclick="sendCommand(2)">Read Shift Total</button>
                <button onclick="sendCommand(1)">Clear Shift Total</button>
                <button onclick="sendCommand(3)">Stop Pump</button>
                <button onclick="sendCommand(4)">Request Real-time Data</button>
            </div>
        </div>
        
        <h3>Response:</h3>
        <div class="response" id="response">No response yet.</div>
    </div>

    <script>
        // Function to send command to the API
        function sendCommand(commandId) {
            const nozzleId = document.getElementById('nozzleId').value;
            const comPort = document.getElementById('comPort').value;
            const priceData = document.getElementById('priceData').value;
            
            // Create form data
            const formData = new FormData();
            formData.append('command', commandId);
            formData.append('address', nozzleId);
            formData.append('comPort', comPort);
            formData.append('data', priceData);
            
            // Update response area
            document.getElementById('response').textContent = 'Sending command...';
            
            // Send request
            fetch('/api/command', {
                method: 'POST',
                body: formData
            })
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.text();
            })
            .then(data => {
                console.log('Command sent successfully');
                // Poll for response
                pollForResponse();
            })
            .catch(error => {
                console.error('Error sending command:', error);
                document.getElementById('response').textContent = 'Error sending command: ' + error.message;
            });
        }
        
        // Function to poll for response
        function pollForResponse() {
            const pollInterval = setInterval(() => {
                fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('response').textContent = data.response;
                    if (!data.response.includes('Sending') && !data.response.includes('waiting')) {
                        clearInterval(pollInterval);
                    }
                })
                .catch(error => {
                    console.error('Error polling for response:', error);
                    clearInterval(pollInterval);
                });
            }, 500);
        }
    </script>
</body>
</html>
  )";
  
  return html;
}
