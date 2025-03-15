#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include "index.h" // This will be the generated header file with the gzipped HTML

// WiFi credentials
const char* ssid = "wifitest";
const char* password = "12345678";

// LED pin
const int ledPin = 2; // Built-in LED on most ESP32 dev boards

// Web server on port 80
WebServer server(80);

// Variables to track device status
unsigned long startTime;
bool ledState = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
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
  
  // Record start time
  startTime = millis();
  
  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/toggle", handleToggle);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}

void handleRoot() {
  // Serve the gzipped HTML file with proper headers
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char*)gz_index, gz_index_len);
}

void handleStatus() {
  // Calculate uptime in seconds
  unsigned long uptime = (millis() - startTime) / 1000;
  
  // Get free heap memory
  uint32_t freeMemory = ESP.getFreeHeap();
  
  // Create JSON response
  String json = "{\"uptime\":" + String(uptime) + ",\"memory\":" + String(freeMemory) + "}";
  
  server.send(200, "application/json", json);
}

void handleToggle() {
  // Toggle LED state
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  
  // Send response
  String message = "LED is now " + String(ledState ? "ON" : "OFF");
  server.send(200, "text/plain", message);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}
