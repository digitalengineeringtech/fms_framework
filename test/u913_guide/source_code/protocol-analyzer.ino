/*
 * Fuel Dispenser Protocol Analyzer for ESP32
 * 
 * This utility helps analyze and debug the communication protocol
 * between the ESP32 and the fuel dispenser.
 * 
 * It captures and displays all serial communication in both directions,
 * showing the raw bytes and their interpretation.
 */

#include <HardwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <CircularBuffer.h>

// Serial communication settings
#define DISPENSER_SERIAL Serial2
#define DISPENSER_RX_PIN 16  // GPIO16 for RX
#define DISPENSER_TX_PIN 17  // GPIO17 for TX
#define BAUD_RATE 2400

// Web server settings
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
WebServer server(80);

// Buffer for storing communication logs
CircularBuffer<String, 100> communicationLog;

// Monitoring state
bool monitoringActive = false;
bool markParity = true; // Start with mark parity

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("Fuel Dispenser Protocol Analyzer");
  
  // Initialize dispenser serial with mark parity
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
  
  // Add initial log entry
  logMessage("Protocol Analyzer started. Waiting for communication...");
}

void loop() {
  // Handle web server clients
  server.handleClient();
  
  // Monitor serial communication if active
  if (monitoringActive) {
    monitorSerialCommunication();
  }
}

void setupWebServer() {
  // Serve the web interface
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHTML());
  });
  
  // API endpoints
  server.on("/api/start", HTTP_POST, []() {
    monitoringActive = true;
    logMessage("Monitoring started");
    server.send(200, "text/plain", "Monitoring started");
  });
  
  server.on("/api/stop", HTTP_POST, []() {
    monitoringActive = false;
    logMessage("Monitoring stopped");
    server.send(200, "text/plain", "Monitoring stopped");
  });
  
  server.on("/api/clear", HTTP_POST, []() {
    communicationLog.clear();
    logMessage("Log cleared");
    server.send(200, "text/plain", "Log cleared");
  });
  
  server.on("/api/parity", HTTP_POST, []() {
    if (server.hasArg("type")) {
      String parityType = server.arg("type");
      if (parityType == "mark") {
        markParity = true;
        DISPENSER_SERIAL.end();
        DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8M1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
        logMessage("Switched to MARK parity");
      } else if (parityType == "space") {
        markParity = false;
        DISPENSER_SERIAL.end();
        DISPENSER_SERIAL.begin(BAUD_RATE, SERIAL_8S1, DISPENSER_RX_PIN, DISPENSER_TX_PIN);
        logMessage("Switched to SPACE parity");
      }
      server.send(200, "text/plain", "Parity changed to " + parityType);
    } else {
      server.send(400, "text/plain", "Missing parity type");
    }
  });
  
  server.on("/api/send", HTTP_POST, []() {
    if (server.hasArg("data")) {
      String hexData = server.arg("data");
      sendHexData(hexData);
      server.send(200, "text/plain", "Data sent: " + hexData);
    } else {
      server.send(400, "text/plain", "Missing data");
    }
  });
  
  server.on("/api/logs", HTTP_GET, []() {
    DynamicJsonDocument doc(16384);
    JsonArray logs = doc.createNestedArray("logs");
    
    for (unsigned int i = 0; i < communicationLog.size(); i++) {
      logs.add(communicationLog[i]);
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Handle not found
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });
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
  hexData.replace(" ", ""); // Remove spaces
  
  if (hexData.length() % 2 != 0) {
    logMessage("Error: Invalid hex string length");
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
  return hex.toUpperCase();
}

void logMessage(String message) {
  // Add timestamp
  String timestamp = String(millis() / 1000.0, 3);
  String logEntry = "[" + timestamp + "s] " + message;
  
  // Add to circular buffer
  communicationLog.push(logEntry);
  
  // Also print to serial console
  Serial.println(logEntry);
}

String getHTML() {
  // Return the HTML for the web interface
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fuel Dispenser Protocol Analyzer</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
            color: #333;
        }
        .container {
            max-width: 1000px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        h1 {
            color: #2c3e50;
            text-align: center;
            margin-top: 0;
        }
        .control-panel {
            display: flex;
            gap: 15px;
            margin-bottom: 20px;
            flex-wrap: wrap;
        }
        .control-group {
            flex: 1;
            min-width: 200px;
            background-color: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            border: 1px solid #e9ecef;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
            color: #495057;
        }
        input, select {
            width: 100%;
            padding: 8px;
            border: 1px solid #ced4da;
            border-radius: 4px;
            box-sizing: border-box;
            font-family: monospace;
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
            transition: background-color 0.2s;
        }
        button:hover {
            background-color: #2980b9;
        }
        button.danger {
            background-color: #e74c3c;
        }
        button.danger:hover {
            background-color: #c0392b;
        }
        button.success {
            background-color: #2ecc71;
        }
        button.success:hover {
            background-color: #27ae60;
        }
        .log-container {
            margin-top: 20px;
            padding: 15px;
            background-color: #2c3e50;
            border-radius: 4px;
            color: #ecf0f1;
            height: 400px;
            overflow-y: auto;
            font-family: 'Courier New', Courier, monospace;
            font-size: 14px;
            line-height: 1.5;
        }
        .log-entry {
            margin-bottom: 5px;
            word-wrap: break-word;
        }
        .log-entry.rx {
            color: #2ecc71;
        }
        .log-entry.tx {
            color: #3498db;
        }
        .log-entry.system {
            color: #f1c40f;
        }
        .log-entry.error {
            color: #e74c3c;
        }
        .status {
            display: inline-block;
            padding: 5px 10px;
            border-radius: 15px;
            font-size: 12px;
            font-weight: bold;
            margin-left: 10px;
        }
        .status.active {
            background-color: #2ecc71;
            color: white;
        }
        .status.inactive {
            background-color: #e74c3c;
            color: white;
        }
        .parity-indicator {
            display: inline-block;
            padding: 5px 10px;
            border-radius: 15px;
            font-size: 12px;
            font-weight: bold;
            margin-left: 10px;
            background-color: #f39c12;
            color: white;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Fuel Dispenser Protocol Analyzer</h1>
        
        <div class="control-panel">
            <div class="control-group">
                <h3>Monitoring Control</h3>
                <button id="startBtn" class="success" onclick="startMonitoring()">Start Monitoring</button>
                <button id="stopBtn" class="danger" onclick="stopMonitoring()" disabled>Stop Monitoring</button>
                <button onclick="clearLogs()">Clear Logs</button>
                <div id="monitoringStatus" class="status inactive">Inactive</div>
            </div>
            
            <div class="control-group">
                <h3>Parity Settings</h3>
                <button onclick="setParity('mark')">Set MARK Parity</button>
                <button onclick="setParity('space')">Set SPACE Parity</button>
                <div id="parityStatus" class="parity-indicator">MARK</div>
            </div>
            
            <div class="control-group">
                <h3>Send Data</h3>
                <div class="form-group">
                    <label for="hexData">Hex Data (space separated):</label>
                    <input type="text" id="hexData" placeholder="01 03 8F 6D">
                </div>
                <button onclick="sendData()">Send Data</button>
            </div>
        </div>
        
        <h3>Communication Log</h3>
        <div class="log-container" id="logContainer">
            <div class="log-entry system">Protocol Analyzer ready. Click 'Start Monitoring' to begin.</div>
        </div>
    </div>

    <script>
        let monitoring = false;
        let currentParity = 'mark';
        
        // Function to start monitoring
        function startMonitoring() {
            fetch('/api/start', { method: 'POST' })
            .then(response => {
                if (response.ok) {
                    monitoring = true;
                    updateUI();
                    pollLogs();
                }
            })
            .catch(error => {
                console.error('Error starting monitoring:', error);
                addLogEntry('Error starting monitoring: ' + error.message, 'error');
            });
        }
        
        // Function to stop monitoring
        function stopMonitoring() {
            fetch('/api/stop', { method: 'POST' })
            .then(response => {
                if (response.ok) {
                    monitoring = false;
                    updateUI();
                }
            })
            .catch(error => {
                console.error('Error stopping monitoring:', error);
                addLogEntry('Error stopping monitoring: ' + error.message, 'error');
            });
        }
        
        // Function to clear logs
        function clearLogs() {
            fetch('/api/clear', { method: 'POST' })
            .then(response => {
                if (response.ok) {
                    document.getElementById('logContainer').innerHTML = '';
                    addLogEntry('Logs cleared', 'system');
                }
            })
            .catch(error => {
                console.error('Error clearing logs:', error);
                addLogEntry('Error clearing logs: ' + error.message, 'error');
            });
        }
        
        // Function to set parity
        function setParity(parity) {
            fetch('/api/parity', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: 'type=' + parity
            })
            .then(response => {
                if (response.ok) {
                    currentParity = parity;
                    updateUI();
                    addLogEntry('Parity set to ' + parity.toUpperCase(), 'system');
                }
            })
            .catch(error => {
                console.error('Error setting parity:', error);
                addLogEntry('Error setting parity: ' + error.message, 'error');
            });
        }
        
        // Function to send data
        function sendData() {
            const hexData = document.getElementById('hexData').value;
            if (!hexData.trim()) {
                addLogEntry('Error: No data to send', 'error');
                return;
            }
            
            fetch('/api/send', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: 'data=' + encodeURIComponent(hexData)
            })
            .then(response => {
                if (response.ok) {
                    return response.text();
                }
                throw new Error('Failed to send data');
            })
            .then(data => {
                console.log('Data sent:', data);
            })
            .catch(error => {
                console.error('Error sending data:', error);
                addLogEntry('Error sending data: ' + error.message, 'error');
            });
        }
        
        // Function to poll logs
        function pollLogs() {
            if (!monitoring) return;
            
            fetch('/api/logs')
            .then(response => response.json())
            .then(data => {
                updateLogs(data.logs);
                setTimeout(pollLogs, 1000);
            })
            .catch(error => {
                console.error('Error polling logs:', error);
                setTimeout(pollLogs, 5000);
            });
        }
        
        // Function to update logs
        function updateLogs(logs) {
            const logContainer = document.getElementById('logContainer');
            logContainer.innerHTML = '';
            
            logs.forEach(log => {
                let type = 'system';
                if (log.includes('RX:')) type = 'rx';
                if (log.includes('TX:')) type = 'tx';
                if (log.includes('Error:')) type = 'error';
                
                const logEntry = document.createElement('div');
                logEntry.className = 'log-entry ' + type;
                logEntry.textContent = log;
                logContainer.appendChild(logEntry);
            });
            
            // Scroll to bottom
            logContainer.scrollTop = logContainer.scrollHeight;
        }
        
        // Function to add a log entry locally
        function addLogEntry(message, type) {
            const logContainer = document.getElementById('logContainer');
            const logEntry = document.createElement('div');
            logEntry.className = 'log-entry ' + type;
            logEntry.textContent = message;
            logContainer.appendChild(logEntry);
            logContainer.scrollTop = logContainer.scrollHeight;
        }
        
        // Function to update UI based on state
        function updateUI() {
            document.getElementById('startBtn').disabled = monitoring;
            document.getElementById('stopBtn').disabled = !monitoring;
            
            const statusEl = document.getElementById('monitoringStatus');
            statusEl.textContent = monitoring ? 'Active' : 'Inactive';
            statusEl.className = 'status ' + (monitoring ? 'active' : 'inactive');
            
            const parityEl = document.getElementById('parityStatus');
            parityEl.textContent = currentParity.toUpperCase();
        }
    </script>
</body>
</html>
  )";
  
  return html;
}
