void handle_wifi_command(const std::vector<String>& args) {
    if (args.size() != 2) {
        fms_cli.respond("wifi", "Usage: wifi <ssid> <password>", false);
        return;
    }
    String ssid = args[0];
    String password = args[1];
    // Save to preferences
    preferences.begin("fms_config", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", password);
    preferences.end();
    
    fms_cli.respond("wifi", "WiFi settings updated. SSID: " + ssid);
}

void handle_restart_command(const std::vector<String>& args) {
    fms_cli.respond("restart", "Restarting system...");
    delay(1000);
    ESP.restart();
}

// Alternative WiFi scan implementation that uses even less memory
void handle_wifi_scan_safe_command(const std::vector<String>& args) {
    // Set a very conservative limit on the number of networks to scan
    const int MAX_NETWORKS = 5;
    
    // Start scan
    fms_cli.respond("wifiscan_safe", "Scanning for networks...");
    
    // Set WiFi to station mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Use synchronous scan with limited time per channel
    int numNetworks = WiFi.scanNetworks(false, true, false, 200);
    
    if (numNetworks <= 0) {
        fms_cli.respond("wifiscan_safe", numNetworks == 0 ? "No networks found" : "Scan failed");
        return;
    }
    
    // Limit the number of networks to prevent memory issues
    if (numNetworks > MAX_NETWORKS) {
        numNetworks = MAX_NETWORKS;
    }
    
    // Send a simplified response with minimal memory usage
    fms_cli.respond("wifiscan_safe", "Found " + String(numNetworks) + " networks");
    
    // Process each network individually
    for (int i = 0; i < numNetworks; i++) {
        // Send SSID
        Serial.print("Network ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(WiFi.SSID(i));
        
        // Send RSSI
        Serial.print("  Signal: ");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm");
        
        // Send Channel
        Serial.print("  Channel: ");
        Serial.println(WiFi.channel(i));
        
        // Send Encryption
        Serial.print("  Security: ");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA/WPA2");
        
        Serial.println("---");
        
        // Give the system time to process
        delay(10);
        yield();
    }
    
    // Free the memory used by the scan
    WiFi.scanDelete();
    
    // Final message
    fms_cli.respond("wifiscan_safe", "Scan complete");
}

void handle_wifi_connect_command(const std::vector<String>& args) {
    if (args.size() != 2) {
        fms_cli.respond("wifi_connect", "Usage: wifi_connect <ssid> <password>", false);
        return;
    }
    
    String ssid = args[0];
    String password = args[1];
    
    // Disconnect from any current WiFi connection
    WiFi.disconnect();
    delay(100);
    
    // Set WiFi mode to station
    WiFi.mode(WIFI_STA);
    
    // Begin connection attempt
    fms_cli.respond("wifi_connect", "Connecting to " + ssid + "...");
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    // Check connection result
    if (WiFi.status() == WL_CONNECTED) {
        // Use individual prints instead of building a large JSON string
        Serial.println("{");
        Serial.print("  \"command\": \"wifi_connect\",");
        Serial.print("  \"result\": \"Connected successfully\",");
        Serial.print("  \"success\": true,");
        Serial.print("  \"ip\": \"");
        Serial.print(WiFi.localIP().toString());
        Serial.print("\",");
        Serial.print("  \"ssid\": \"");
        Serial.print(WiFi.SSID());
        Serial.print("\",");
        Serial.print("  \"rssi\": ");
        Serial.print(WiFi.RSSI());
        Serial.println("}");
    } else {
        // Connection failed, report error code
        String errorMsg;
        switch (WiFi.status()) {
            case WL_NO_SSID_AVAIL:
                errorMsg = "SSID not available";
                break;
            case WL_CONNECT_FAILED:
                errorMsg = "Connection failed (wrong password?)";
                break;
            case WL_CONNECTION_LOST:
                errorMsg = "Connection lost";
                break;
            case WL_DISCONNECTED:
                errorMsg = "Disconnected";
                break;
            default:
                errorMsg = "Unknown error: " + String(WiFi.status());
        }
        
        fms_cli.respond("wifi_connect", "Failed to connect: " + errorMsg, false);
    }
}

void handle_wifi_read_command(const std::vector<String>& args) {
    if (WiFi.status() == WL_CONNECTED) {
        // Use individual prints instead of building a large string
        Serial.println("{");
        Serial.print("  \"ssid\": \"");
        Serial.print(WiFi.SSID());
        Serial.print("\",");
        Serial.print("  \"rssi\": ");
        Serial.print(WiFi.RSSI());
        Serial.print(",");
        Serial.print("  \"ip\": \"");
        Serial.print(WiFi.localIP().toString());
        Serial.println("\"");
        Serial.println("}");
    } else {
        fms_cli.respond("wifiread", "WiFi not connected", false);
    }
}

// Test command to run a series of tests
void handle_test_command(const std::vector<String>& args) {
    fms_cli.respond("test", "Running fms_cli tests...");
    
    // Test help command
    Serial.println("\n--- Testing help command ---");
    fms_cli.execute_test_command("help");
    
    // Test echo command
    Serial.println("\n--- Testing echo command ---");
    fms_cli.execute_test_command("echo on");
    fms_cli.execute_test_command("echo off");
    fms_cli.execute_test_command("echo");
    
    // Test wifi read command
    Serial.println("\n--- Testing wifiread command ---");
    fms_cli.execute_test_command("wifiread");
    
    // Test invalid command
    Serial.println("\n--- Testing invalid command ---");
    fms_cli.execute_test_command("invalidcommand");
    
    fms_cli.respond("test", "Tests completed");
}

// WiFi connection test command
void handle_wifi_test_command(const std::vector<String>& args) {
    fms_cli.respond("wifi_test", "Running WiFi connection tests...");
    
    // Get stored WiFi credentials
    preferences.begin("fms_config", true); // Read-only mode
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("pass", "");
    preferences.end();
    
    if (ssid.length() == 0 || password.length() == 0) {
        fms_cli.respond("wifi_test", "No WiFi credentials stored. Use 'wifi <ssid> <password>' first.", false);
        return;
    }
    
    // Test WiFi connection
    Serial.println("\n--- Testing WiFi connection ---");
    Serial.printf("Connecting to %s...\n", ssid.c_str());
    
    // Disconnect from any current WiFi connection
    WiFi.disconnect();
    delay(100);
    
    // Set WiFi mode to station
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    bool connected = false;
    
    while (millis() - startTime < WIFI_TIMEOUT) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    if (connected) {
        Serial.printf("Connected to %s\n", ssid.c_str());
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Signal strength (RSSI): %d dBm\n", WiFi.RSSI());
        
        // Test ping to gateway
        IPAddress gateway = WiFi.gatewayIP();
        Serial.printf("Gateway IP: %s\n", gateway.toString().c_str());
        
        fms_cli.respond("wifi_test", "WiFi connection test successful");
    } else {
        String errorMsg;
        switch (WiFi.status()) {
            case WL_NO_SSID_AVAIL:
                errorMsg = "SSID not available";
                break;
            case WL_CONNECT_FAILED:
                errorMsg = "Connection failed (wrong password?)";
                break;
            case WL_CONNECTION_LOST:
                errorMsg = "Connection lost";
                break;
            case WL_DISCONNECTED:
                errorMsg = "Disconnected";
                break;
            default:
                errorMsg = "Unknown error: " + String(WiFi.status());
        }
        
        fms_cli.respond("wifi_test", "WiFi connection test failed: " + errorMsg, false);
    }
}

// Custom print function that captures output for the web interface
size_t custom_print(const uint8_t *buffer, size_t size) {
    if (testModeActive) {
        // Add to buffer
        for (size_t i = 0; i < size; i++) {
            serialOutputBuffer += (char)buffer[i];
        }
        
        // Trim buffer if it gets too large
        if (serialOutputBuffer.length() > MAX_BUFFER_SIZE) {
            serialOutputBuffer = serialOutputBuffer.substring(serialOutputBuffer.length() - MAX_BUFFER_SIZE);
        }
    }
    
    // Pass through to original Serial
    return Serial.write(buffer, size);
}

// Web server handlers
void handle_root() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    
    server.streamFile(file, "text/html");
    file.close();
}

void handle_command() {
    if (!server.hasArg("cmd")) {
        server.send(400, "text/plain", "Missing command parameter");
        return;
    }
    
    String cmd = server.arg("cmd");
    
    // Clear the buffer before executing the command
    serialOutputBuffer = "";
    
    // Enable test mode to capture output
    testModeActive = true;
    
    // Execute the command
    fms_cli.execute_test_command(cmd);
    
    // Give some time for the command to complete
    delay(500);
    
    // Disable test mode
    testModeActive = false;
    
    // Send the captured output
    server.send(200, "application/json", serialOutputBuffer);
}

void handle_output() {
    server.send(200, "application/json", serialOutputBuffer);
}

void setup_web_server() {
    // Initialize SPIFFS
    if (!LittleFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
    
    // Create the HTML file in SPIFFS
    File file = LittleFS.open("/index.html", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    
    // Write the HTML content (same as before)
    file.print(R"(
 <!DOCTYPE html>
 <html lang="en">
 <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>FMS fms_cli Web Interface</title>
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
            border-radius: 5px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #333;
            margin-top: 0;
        }
        .input-group {
            display: flex;
            margin-bottom: 20px;
        }
        input[type="text"] {
            flex: 1;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px 0 0 4px;
            font-size: 16px;
        }
        button {
            padding: 10px 15px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 0 4px 4px 0;
            cursor: pointer;
            font-size: 16px;
        }
        button:hover {
            background-color: #45a049;
        }
        .output {
            background-color: #f8f8f8;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 15px;
            height: 300px;
            overflow-y: auto;
            font-family: monospace;
            white-space: pre-wrap;
        }
        .command-list {
            margin-top: 20px;
            border-top: 1px solid #ddd;
            padding-top: 20px;
        }
        .command-list h3 {
            margin-top: 0;
        }
        .command-button {
            margin: 5px;
            padding: 8px 12px;
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        .command-button:hover {
            background-color: #0b7dda;
        }
        .wifi-form {
            margin-top: 20px;
            border-top: 1px solid #ddd;
            padding-top: 20px;
        }
        .wifi-form h3 {
            margin-top: 0;
        }
        .wifi-form input {
            margin-bottom: 10px;
            padding: 8px;
            width: 100%;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        .wifi-form button {
            border-radius: 4px;
            width: 100%;
        }
    </style>
 </head>
 <body>
    <div class="container">
        <h1> Sixth Kendra Web Interface</h1>
        
        <div class="input-group">
            <input type="text" id="command-input" placeholder="Enter command...">
            <button id="send-button">Send</button>
        </div>
        
        <div class="output" id="output"></div>
        
        <div class="command-list">
            <h3>Common Commands</h3>
            <button class="command-button" data-cmd="help">help</button>
            <button class="command-button" data-cmd="wifiscan_safe">WiFi Scan (Safe)</button>
            <button class="command-button" data-cmd="wifiread">WiFi Status</button>
            <button class="command-button" data-cmd="wifi_test">WiFi Test</button>
            <button class="command-button" data-cmd="test">Run Tests</button>
        </div>
        
        <div class="wifi-form">
            <h3>WiFi Connection</h3>
            <input type="text" id="ssid-input" placeholder="SSID">
            <input type="password" id="password-input" placeholder="Password">
            <button id="connect-button">Connect</button>
        </div>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const commandInput = document.getElementById('command-input');
            const sendButton = document.getElementById('send-button');
            const output = document.getElementById('output');
            const commandButtons = document.querySelectorAll('.command-button');
            const ssidInput = document.getElementById('ssid-input');
            const passwordInput = document.getElementById('password-input');
            const connectButton = document.getElementById('connect-button');
            
            // Function to send command
            function sendCommand(cmd) {
                output.innerHTML += `<div style="color: #4CAF50;">> ${cmd}</div>`;
                output.scrollTop = output.scrollHeight;
                
                fetch(`/command?cmd=${encodeURIComponent(cmd)}`)
                    .then(response => response.text())
                    .then(data => {
                        console.log(data);
                        output.innerHTML += `<div>${data}</div>`;
                        output.scrollTop = output.scrollHeight;
                    })
                    .catch(error => {
                        output.innerHTML += `<div style="color: red;">Error: ${error}</div>`;
                        output.scrollTop = output.scrollHeight;
                    });
            }
            
            // Send button fms_click
            sendButton.addEventListener('fms_click', function() {
                const cmd = commandInput.value.trim();
                if (cmd) {
                    sendCommand(cmd);
                    commandInput.value = '';
                }
            });
            
            // Enter key press
            commandInput.addEventListener('keypress', function(e) {
                if (e.key === 'Enter') {
                    const cmd = commandInput.value.trim();
                    if (cmd) {
                        sendCommand(cmd);
                        commandInput.value = '';
                    }
                }
            });
            
            // Command buttons
            commandButtons.forEach(button => {
                button.addEventListener('fms_click', function() {
                    const cmd = this.getAttribute('data-cmd');
                    sendCommand(cmd);
                });
            });
            
            // Connect button
            connectButton.addEventListener('fms_click', function() {
                const ssid = ssidInput.value.trim();
                const password = passwordInput.value.trim();
                
                if (ssid && password) {
                    const cmd = `wifi_connect "${ssid}" "${password}"`;
                    sendCommand(cmd);
                } else {
                    output.innerHTML += `<div style="color: red;">Error: SSID and password are required</div>`;
                    output.scrollTop = output.scrollHeight;
                }
            });
            
            // Initial message
            output.innerHTML = '<div>FMS fms_cli Web Interface Ready. Type a command or fms_click a button.</div>';
        });
    </script>
 </body>
 </html>
    )");
    
    file.close();
    
    // Set up server routes
    server.on("/", handle_root);
    server.on("/command", handle_command);
    server.on("/output", handle_output);
    
    // Start server
    server.begin();
    Serial.println("Web server started");
}

static void cli_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
