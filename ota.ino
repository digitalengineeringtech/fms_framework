// modified : trion 
// modified form : esp8266 ota feature using http 

#include <WiFi.h>
#include <WebServer.h>
#include "FS.h"
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Ticker.h>
#include <WebSocketsServer.h>



// WiFi credentials
const char* ssid = "xxx";
const char* password = "xxx";

// Update server
const char* OTA_SERVER = "http://192.168.1.142:3000/firmware.json";

// Web server for API
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// HTTP client for OTA updates
HTTPClient http;
WiFiClient http_client;

// Variables
unsigned long currentMillis = 0;
unsigned long ota_previousMillis = 0;
String _firmware_version = "1.0.0";  // Current firmware version
String _firmware_v = "";             // Latest firmware version from server
bool _published = false;             // Is the update published?
String _ota_url = "";                // URL to download the firmware
bool _ota_update = false;            // Should we update?
bool _update_in_progress = false;    // Is update in progress?
int _update_progress = 0;            // Update progress (0-100)
String _update_status = "idle";      // Update status (idle, checking, updating, success, error)
String _update_error = "";           // Error message if update fails
#define LED_BUILTIN 16
// LED pins
const int LED_ONE = LED_BUILTIN;     // Built-in LED (inverted logic)
const int LED_TWO = 2;               // Status LED

// Ticker for LED blinking
Ticker ticker;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println("\n\nESP OTA with Remote Web UI");
  Serial.println("-------------------------");
  
  // Initialize LED pins
  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  digitalWrite(LED_ONE, HIGH);  // Turn off LED (inverted logic)
  digitalWrite(LED_TWO, LOW);   // Turn on status LED
  
  // Initialize file system (LittleFS is supported on ESP32)
  if (!LittleFS.begin(true)) {
    Serial.println("Failed to mount file system");
  } else {
    Serial.println("File system mounted");
    loadDeviceInfo();
  }
  
  // Connect to WiFi
  setupWiFi();
  
  // Set up web server routes
  setupWebServer();
  
  // Set up WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  
  // Set up OTA callbacks using HTTPUpdate (ESP32)
  httpUpdate.setLedPin(LED_TWO, LOW);
  httpUpdate.onStart(update_started);
  httpUpdate.onEnd(update_finished);
  httpUpdate.onProgress(update_progress);
  httpUpdate.onError(update_error);
  
  Serial.print("Device IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Firmware version: ");
  Serial.println(_firmware_version);
}

void loop() {
  currentMillis = millis();
  
  // Handle HTTP requests
  server.handleClient();
  
  // Handle WebSocket events
  webSocket.loop();
  
  // Check for OTA updates periodically
  device_fw_ota();
  
  // Your regular code here
  // ...
}

// Connect to WiFi
void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    digitalWrite(LED_ONE, LOW);  // Turn on LED
    delay(100);
    digitalWrite(LED_ONE, HIGH); // Turn off LED
    delay(400);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected to: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi!");
  }
}

void serveIndex() {
  if (LittleFS.exists("/index.html")) {
      File htmlFile = LittleFS.open("/index.html", "r");
      if (!htmlFile) {
          server.send(500, "text/plain", "Failed to open file");
          return;
      }
      server.streamFile(htmlFile, "text/html");
      htmlFile.close();
  } else {
      server.send(404, "text/plain", "Index file not found");
  }
}

// Set up web server routes
void setupWebServer() {
  // API endpoints
  server.on("/api/info", HTTP_GET, handleApiInfo);
  server.on("/api/check", HTTP_GET, handleApiCheck);
  server.on("/api/update", HTTP_GET, handleApiUpdate);
  
  // Serve static files from LittleFS
  server.serveStatic("/", LittleFS, "/", "max-age=86400");
  server.on("/", HTTP_GET, serveIndex);
  // Handle 404
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not Found");
  });
}

// API: Get device info
void handleApiInfo() {
  DynamicJsonDocument doc(512);
  
  doc["device"] = WiFi.getHostname();
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = WiFi.macAddress();
  doc["firmware_version"] = _firmware_version;
  doc["latest_version"] = _firmware_v;
  doc["update_available"] = (_firmware_v.length() > 0 && _firmware_version < _firmware_v && _published);
  doc["update_status"] = _update_status;
  doc["update_progress"] = _update_progress;
  doc["update_error"] = _update_error;
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["flash_size"] = ESP.getFlashChipSize();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// API: Check for updates
void handleApiCheck() {
  _update_status = "checking";
  broadcastStatus();
  
  bool updateFound = checkForUpdates();
  
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["update_available"] = updateFound;
  doc["current_version"] = _firmware_version;
  doc["latest_version"] = _firmware_v;
  doc["published"] = _published;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
  
  _update_status = "idle";
  broadcastStatus();
}

// API: Start update
void handleApiUpdate() {
  if (_update_in_progress) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Update already in progress\"}");
    return;
  }
  
  if (_firmware_v.length() == 0 || !_published) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"No update available\"}");
    return;
  }
  
  if (_firmware_version >= _firmware_v) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Already on latest version\"}");
    return;
  }
  
  server.send(200, "application/json", "{\"success\":true,\"message\":\"Update started\"}");
  
  // Start update in the next loop iteration
  _ota_update = true;
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WebSocket] #%u Disconnected\n", num);
      break;
    
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[WebSocket] #%u Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        // Send current status to newly connected client
        broadcastStatus();
      }
      break;
    
    case WStype_TEXT: {
        String text = String((char*)payload);
        Serial.printf("[WebSocket] #%u Received: %s\n", num, text.c_str());
        if (text == "check") {
          _update_status = "checking";
          broadcastStatus();
          bool updateFound = checkForUpdates();
          _update_status = "idle";
          broadcastStatus();
        }
        else if (text == "update") {
          if (!_update_in_progress && _firmware_v.length() > 0 && _published && _firmware_version < _firmware_v) {
            _ota_update = true;
          }
        }
      }
      break;
  }
}

// Broadcast status to all WebSocket clients
void broadcastStatus() {
  DynamicJsonDocument doc(256);
  
  doc["firmware_version"] = _firmware_version;
  doc["latest_version"] = _firmware_v;
  doc["update_available"] = (_firmware_v.length() > 0 && _firmware_version < _firmware_v && _published);
  doc["update_status"] = _update_status;
  doc["update_progress"] = _update_progress;
  doc["update_error"] = _update_error;
  
  String status;
  serializeJson(doc, status);
  webSocket.broadcastTXT(status);
}

// Check for updates (returns true if update is available)
bool checkForUpdates() {
  bool updateAvailable = false;
  
  if (http.begin(http_client, OTA_SERVER)) {
    int httpcode = http.GET();
    if (httpcode > 0) {
      Serial.printf("HTTP code %d\n", httpcode);
      if (httpcode == HTTP_CODE_OK || httpcode == HTTP_CODE_MOVED_PERMANENTLY) {
        String resultvalue = http.getString();
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, resultvalue);
        if (error) {
          Serial.println("JSON parsing failed");
          _update_error = "JSON parsing failed";
        } else {
          _firmware_v = doc["version"].as<String>();
          _published = doc["published"].as<bool>();
          _ota_url = doc["file"].as<String>();
          if (_published && _firmware_version < _firmware_v) {
            updateAvailable = true;
          } else {
            Serial.println("[OTA] No update available");
          }
        }
      }
    } else {
      Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpcode).c_str());
      _update_error = "HTTP error: " + http.errorToString(httpcode);
    }
    http.end();
  } else {
    Serial.println("HTTP unable to connect");
    _update_error = "Unable to connect to update server";
  }
  
  return updateAvailable;
}

// OTA update function
void device_fw_ota() {
  // Automatic check every 2 minutes
  if (currentMillis - ota_previousMillis >= 120000 && !_update_in_progress) {
    _update_status = "checking";
    broadcastStatus();
    
    checkForUpdates();
    
    _update_status = "idle";
    broadcastStatus();
    
    ota_previousMillis = currentMillis;
  }
  
  // Perform update if flagged
  if (_ota_update && !_update_in_progress) {
    if (_firmware_version < _firmware_v) {
      _update_in_progress = true;
      _update_status = "updating";
      _update_progress = 0;
      broadcastStatus();
      
      ticker.attach(0.3, blinkLED);
      Serial.println("[OTA] Firmware is out of date, updating...");
      
      t_httpUpdate_return ret = httpUpdate.update(http_client, _ota_url);
      
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          _update_status = "error";
          _update_error = "Update failed: " + String(httpUpdate.getLastErrorString());
          _update_in_progress = false;
          _ota_update = false;
          ticker.detach();
          digitalWrite(LED_TWO, LOW);  // Reset status LED
          Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", 
                        httpUpdate.getLastError(), 
                        httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          _update_status = "idle";
          _update_error = "No updates available";
          _update_in_progress = false;
          _ota_update = false;
          ticker.detach();
          digitalWrite(LED_TWO, LOW);  // Reset status LED
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
        case HTTP_UPDATE_OK:
          _update_status = "success";
          _update_in_progress = false;
          _ota_update = false;
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
      
      broadcastStatus();
    } else {
      _ota_update = false;
    }
  }
}

// LED blink callback for ticker
void blinkLED() {
  int state = digitalRead(LED_TWO);
  digitalWrite(LED_TWO, !state);
}

// Load device info from LittleFS
void loadDeviceInfo() {
  if (LittleFS.exists("/info.json")) {
    File file = LittleFS.open("/info.json", "r");
    if (file) {
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, file);
      if (!error && doc.containsKey("Firmware Version")) {
        _firmware_version = doc["Firmware Version"].as<String>();
      }
      file.close();
    }
  }
}

// Update callbacks
void update_started() {
  Serial.println("CALLBACK: HTTP update process started");
  _update_status = "updating";
  _update_progress = 0;
  broadcastStatus();
}

void update_finished() {
  Serial.println("CALLBACK: HTTP update process finished");
  
  // Save new firmware version to file system
  DynamicJsonDocument doc(256);
  doc["Firmware Version"] = _firmware_v;
  
  File configFile = LittleFS.open("/info.json", "w");
  if (!configFile) {
    Serial.println("[FILE SYSTEM] FAILED");
  } else {
    if (serializeJson(doc, configFile) == 0) {
      Serial.println("[FILE SYSTEM] ERROR");
    }
    configFile.close();
  }
  
  _update_status = "success";
  _update_progress = 100;
  broadcastStatus();
}

void update_progress(int cur, int total) {
  _update_progress = map(cur, 0, total, 0, 100);
  Serial.printf("CALLBACK: HTTP update process at %d of %d bytes (%d%%)\n", 
                cur, total, _update_progress);
  // Broadcast every 5% increment
  static int lastProgress = 0;
  if (_update_progress >= lastProgress + 5 || _update_progress == 100) {
    lastProgress = _update_progress;
    broadcastStatus();
  }
}

void update_error(int err) {
  Serial.printf("CALLBACK: HTTP update fatal error code %d\n", err);
  _update_status = "error";
  _update_error = "Error code: " + String(err);
  broadcastStatus();
}
