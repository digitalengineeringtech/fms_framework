/* 
ota server 
  * author : Trion
    Guided By : Sir Thiha Kyaw
    Copyright : @2025 iih
    ota server & dns reponseder on network 
    using esp ota lib
    file : _fms_main.h
    file : fms_ota_server.ino (rtos task) 
    reference from : mongoose wizard
    description : lite_version ota 
    v 0.1 ota server
*/

/* version 2 ota */
#define USE_V1_OTA_SERVER
#undef  USE_V2_OTA_SERVER
/*

#ifndef USE_V2_OTA_SERVER


#define HTTP_UPLOAD_BUFLEN 8192         // Increased buffer size for better performance
#define WDT_TIMEOUT_S 60                // Increased watchdog timeout for large files
#define PROGRESS_UPDATE_INTERVAL 5      // Update progress every 5%
#define MAX_RETRY_ATTEMPTS 3

// Global variables for better progress tracking
static size_t totalSize = 0;
static size_t writtenSize = 0;
static uint8_t lastReportedProgress = 0;
static unsigned long lastProgressUpdate = 0;
static unsigned long otaStartTime = 0;

void fms_info_response() {
  JsonBuilder json;
  json.addString("deviceName", deviceName);
  json.addString("firmwareVersion", firmwareVersion);
  json.addString("ipAddress", WiFi.localIP().toString());
  json.addString("macAddress", WiFi.macAddress());
  json.addInt("rssi", WiFi.RSSI());
  json.addLong("uptime", uptime);
  json.addInt("freeHeap", ESP.getFreeHeap());
  json.addInt("totalHeap", ESP.getHeapSize());
  json.addInt("cpuFreqMHz", ESP.getCpuFreqMHz());
  json.addString("sdkVersion", ESP.getSdkVersion());
  json.addString("status", updateStatus);
  json.addInt("progress", otaProgress);
  json.addBool("otaInProgress", otaInProgress);
  
  // Add network quality indicators
  json.addString("wifiMode", WiFi.getMode() == WIFI_STA ? "STA" : "AP");
  json.addString("channel", String(WiFi.channel()));
  
  // Add OTA timing information if in progress
  if (otaInProgress && otaStartTime > 0) {
    unsigned long elapsed = (millis() - otaStartTime) / 1000;
    json.addLong("otaElapsedSeconds", elapsed);
    if (totalSize > 0 && writtenSize > 0) {
      // Use addInt or addLong instead of addFloat
      json.addLong("otaSpeedBps", (long)(writtenSize / (elapsed > 0 ? elapsed : 1)));
      json.addLong("otaTotalBytes", totalSize);
      json.addLong("otaWrittenBytes", writtenSize);
    }
  }
  
  cachedInfoResponse = json.toString();
}

void handleDashboard() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Redirecting to login...");
    return;
  }
  
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.sendHeader("Cache-Control", "max-age=86400");
    server.sendHeader("Connection", "keep-alive");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Dashboard not found");
  }
}

void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");
    
    if (user == correctUsername && pass == correctPassword) {
      isAuthenticated = true;
      server.sendHeader("Set-Cookie", "auth=true; Max-Age=3600; Path=/");
      server.send(200,"text/plain", "Login Successful!");

      //server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Login successful\"}");
    } else {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid credentials\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing credentials\"}");
  }
}

void handleLogout() {
  isAuthenticated = false;
  server.sendHeader("Set-Cookie", "auth=; Max-Age=0; Path=/");
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Logged out successfully\"}");
}

// Improved progress reporting function
void updateOTAProgress() {
  if (totalSize > 0) {
    uint8_t currentProgress = (writtenSize * 100) / totalSize;
    
    // Only update if progress changed significantly or enough time has passed
    if (currentProgress != lastReportedProgress && 
        (currentProgress - lastReportedProgress >= PROGRESS_UPDATE_INTERVAL || 
         millis() - lastProgressUpdate > 2000)) {
      
      otaProgress = currentProgress;
      lastReportedProgress = currentProgress;
      lastProgressUpdate = millis();
      
      FMS_LOG_INFO("OTA Progress: %u%% (%u / %u bytes) - Speed: %.2f KB/s\n",
                   currentProgress,
                   writtenSize,
                   totalSize,
                   (float)(writtenSize * 1000) / (millis() - otaStartTime) / 1024.0);
    }
  }
}

void fms_set_ota_server() {
  FMS_LOG_INFO("[fms_ota_server.ino] OTA server created with enhanced performance");
  
  // Enable CORS with specific headers for better compatibility
  server.enableCORS(true);
  
  const char* cacheControl = "max-age=86400, public";
  server.serveStatic("/", LittleFS, "/login.html", cacheControl);

  server.on("/login", handleLogin);
  server.on("/dashboard",  handleDashboard);


  server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);

  // Enhanced info API with better caching
  server.on("/api/info", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    if (otaInProgress || millis() - lastInfoRequest > INFO_CACHE_TIME || cachedInfoResponse.length() == 0) {
      fms_info_response();
      lastInfoRequest = millis();
    }
    
    server.sendHeader("Cache-Control", otaInProgress ? "no-cache" : "max-age=5");
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", cachedInfoResponse);
  });

  server.on("/logout", HTTP_POST, handleLogout);
  // Enhanced OTA update handler
  server.on("/api/update", HTTP_POST, 
    []() {
      // Response handler - called after upload completes
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Content-Type", "application/json");
      
      String response;
      if (Update.hasError()) {
        response = "{\"status\":\"error\",\"message\":\"Update failed\"}";
        server.send(500, "application/json", response);
      } else {
        response = "{\"status\":\"success\",\"message\":\"Update completed successfully\"}";
        server.send(200, "application/json", response);
      }

      // Re-enable watchdog with original timeout
      esp_task_wdt_config_t wdtConfig = {
        .timeout_ms = WDT_TIMEOUT_S * 1000,
        .idle_core_mask = 0,
        .trigger_panic = true
      };
      esp_task_wdt_init(&wdtConfig);

      if (!Update.hasError()) {
        FMS_LOG_INFO("OTA Update successful, restarting in 2 seconds...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP.restart();
      }
    },
    []() {
      // Upload handler - processes the actual file upload
      HTTPUpload& upload = server.upload();
      
      if (upload.status == UPLOAD_FILE_START) {
        // Disable watchdog for update process
        esp_task_wdt_deinit();
        
        FMS_LOG_INFO("Starting OTA update: %s", upload.filename.c_str());
        updateStatus = "Initializing update...";
        otaInProgress = true;
        otaProgress = 0;
        lastReportedProgress = 0;
        writtenSize = 0;
        otaStartTime = millis();

        // Get content length from headers
        totalSize = 0;
        if (server.hasHeader("Content-Length")) {
          totalSize = server.header("Content-Length").toInt();
          FMS_LOG_INFO("Expected file size: %u bytes (%.2f MB)", totalSize, totalSize / 1024.0 / 1024.0);
        }

        // Optimize memory before update
        FMS_LOG_INFO("Free heap before update: %d bytes", ESP.getFreeHeap());
        
        // Force garbage collection if available
        #ifdef ESP_IDF_VERSION_MAJOR
        if (ESP_IDF_VERSION_MAJOR >= 4) {
          heap_caps_malloc_extmem_enable(1024);
        }
        #endif
        
        // Start update with proper size
        size_t updateSize = totalSize > 0 ? totalSize : UPDATE_SIZE_UNKNOWN;
        if (!Update.begin(updateSize, U_FLASH)) {
          Update.printError(Serial);
          updateStatus = "Failed to start update";
          otaInProgress = false;
          
          // Re-enable watchdog on error
          esp_task_wdt_config_t wdtConfig = {
            .timeout_ms = WDT_TIMEOUT_S * 1000,
            .idle_core_mask = 0,
            .trigger_panic = true
          };
          esp_task_wdt_init(&wdtConfig);
        } else {
          updateStatus = "Receiving firmware...";
          FMS_LOG_INFO("OTA update started successfully");
        }
        
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Write firmware data
        size_t written = Update.write(upload.buf, upload.currentSize);
        if (written != upload.currentSize) {
          Update.printError(Serial);
          updateStatus = "Write error occurred";
          FMS_LOG_ERROR("Write error: expected %u bytes, wrote %u bytes", upload.currentSize, written);
        } else {
          writtenSize += written;
          updateOTAProgress();
          
          // Fix string concatenation - use snprintf instead
          char progressBuffer[50];
          snprintf(progressBuffer, sizeof(progressBuffer), "Writing firmware... %d%%", otaProgress);
          updateStatus = progressBuffer;
        }
        
        // Yield periodically to prevent watchdog issues
        if (writtenSize % (32 * 1024) == 0) { // Every 32KB
          yield();
          vTaskDelay(1); // Brief delay to allow other tasks
        }
        
      } else if (upload.status == UPLOAD_FILE_END) {
        // Finalize update
        updateStatus = "Finalizing update...";
        
        if (Update.end(true)) {
          FMS_LOG_INFO("OTA Update completed successfully: %u bytes written", writtenSize);
          updateStatus = "Update completed successfully";
          otaProgress = 100;
          
          // Verify the update
          if (Update.isFinished()) {
            FMS_LOG_INFO("Update verification successful");
          } else {
            FMS_LOG_ERROR("Update verification failed");
            updateStatus = "Update verification failed";
          }
        } else {
          Update.printError(Serial);
          updateStatus = "Update finalization failed";
          FMS_LOG_ERROR("Update end failed");
        }
        
        otaInProgress = false;
        
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        otaInProgress = false;
        updateStatus = "Update was aborted";
        FMS_LOG_INFO("OTA Update aborted by user or network error");
        
        // Re-enable watchdog
        esp_task_wdt_config_t wdtConfig = {
          .timeout_ms = WDT_TIMEOUT_S * 1000,
          .idle_core_mask = 0,
          .trigger_panic = true
        };
        esp_task_wdt_init(&wdtConfig);
      }
      
      // Yield to prevent blocking
      yield();
    }
  );

  // Enhanced restart endpoint
  server.on("/api/restart", HTTP_POST, []() {
    if (!isAuthenticated) {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
      return;
    }
    
    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Restarting device...\"}");
    FMS_LOG_INFO("Manual restart requested");
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP.restart();
  });

  // Handle preflight OPTIONS requests
  server.on("/api/update", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Content-Length");
    server.send(200);
  });

  // 404 handler
  server.onNotFound([]() {
    if (server.uri().startsWith("/api/")) {
      server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"API endpoint not found\"}");
    } else {
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "Redirecting...");
    }
  });
}

static void web_server_task(void* arg) {
  // Enhanced mDNS setup
  String mdnsName = deviceName + String(WiFi.localIP()).c_str();
  mdnsName.replace(":", "");
  
  if (!MDNS.begin(mdnsName.c_str())) {
    Serial.println("[DNS] Error setting up MDNS responder!");
  } else {
    Serial.printf("[DNS] mDNS responder started as %s.local\n", mdnsName.c_str());
    
    // Add services for better discovery
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("esp-ota", "tcp", 80);
    
    // Add service properties if supported by your MDNS implementation
    #ifdef ESP_ARDUINO_VERSION_MAJOR
    if (ESP_ARDUINO_VERSION_MAJOR >= 2) {
      MDNS.addServiceTxt("http", "tcp", "version", firmwareVersion);
      MDNS.addServiceTxt("http", "tcp", "device", deviceName);
    }
    #endif
  }
  
  // Removed setTimeout call as it's not supported
  
  fms_set_ota_server();
  server.begin();
  
  FMS_LOG_INFO("Web server started on http://%s.local or http://%s", 
               mdnsName.c_str(), WiFi.localIP().toString().c_str());

  while (1) {
    server.handleClient();
    
    // Removed MDNS.update() call as it's not supported
    
    // Update uptime counter
    if (millis() - lastUptimeUpdate >= 1000) {
      uptime++;
      lastUptimeUpdate = millis();
    }
    
    // Shorter delay for better responsiveness during OTA
    vTaskDelay(otaInProgress ? pdMS_TO_TICKS(10) : pdMS_TO_TICKS(50));
  }
}

// Optional: Add network optimization function
void optimizeNetworkSettings() {
  // Set WiFi power save mode for better performance during OTA
  WiFi.setSleep(false);
  
  // Increase TCP buffer sizes if possible
  FMS_LOG_INFO("Network optimizations applied");
}
#endif // USE_V2_OTA_SERVER

*/

/* version 1 ota */
#ifdef USE_V1_OTA_SERVER

#define HTTP_UPLOAD_BUFLEN 4096         // Increased from default 1460
#define WDT_TIMEOUT_S 30

void fms_info_response() {            // mini version show in ota page
  JsonBuilder json;
  json.addString("deviceName",        deviceName);
  json.addString("firmwareVersion",   firmwareVersion);
  json.addString("ipAddress",         WiFi.localIP().toString());
  json.addString("macAddress",        WiFi.macAddress());
  json.addInt("rssi",                 WiFi.RSSI());
  json.addLong("uptime",              uptime);
  json.addInt("freeHeap",             ESP.getFreeHeap());
  json.addInt("totalHeap",            ESP.getHeapSize());
  json.addInt("cpuFreqMHz",           ESP.getCpuFreqMHz());
  json.addString("sdkVersion",        ESP.getSdkVersion());
  json.addString("status",            updateStatus);
  json.addInt("progress",             otaProgress);
  json.addBool("otaInProgress",       otaInProgress);
  //json.addLong("flashChipSize", ESP.getFlashChipSize());
  cachedInfoResponse = json.toString();
}

void handleDashboard() { // login auth
  if (!isAuthenticated) {
    // Redirect to login if not authenticated
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Redirecting to login...");
    return;
  }
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.sendHeader("Cache-Control", "max-age=86400");
    server.streamFile(file, "text/html");
    file.close();
  }
}

void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");

    if (user == correctUsername && pass == correctPassword) {
      isAuthenticated = true;  // Mark as logged in
      server.send(200, "text/plain", "Login Successful!");
    } else {
      server.send(401, "text/plain", "Invalid Username or Password");
    }
  } else {
    server.send(400, "text/plain", "Missing credentials");
  }
}

void handleLogout() {
  isAuthenticated = false;
  server.send(200, "text/plain", "Logged out successfully!");
}

void fms_set_ota_server() {
  FMS_LOG_INFO("[fms_ota_server.ino:75] ota server created");
  server.enableCORS(true);
  const char* cacheControl = "max-age=86400";
  server.serveStatic("/", LittleFS, "/login.html", cacheControl);

  server.on("/login", handleLogin);
  server.on("/dashboard", handleDashboard);

  server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);
  server.on("/api/info", HTTP_GET, []() {
    if (millis() - lastInfoRequest < INFO_CACHE_TIME && cachedInfoResponse.length() > 0) {
      server.send(200, "application/json", cachedInfoResponse);
      return;
    }
    fms_info_response();
    lastInfoRequest = millis();
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send(200, "application/json", cachedInfoResponse);
  });
  server.on("/logout", handleLogout);  // logout ota server
  server.on(
    "/api/update", HTTP_POST, []() {
      // This handler is called after the upload is complete
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");

      // Re-enable task watchdog if it was disabled
      esp_task_wdt_config_t wdtConfig;
      wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;
      wdtConfig.idle_core_mask = 0;
      wdtConfig.trigger_panic = true;
      esp_task_wdt_init(&wdtConfig);

      vTaskDelay(pdMS_TO_TICKS(1000));
      ESP.restart();
    },
    []() {
      // This handler processes the actual upload
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        // Disable task watchdog for update process
        esp_task_wdt_deinit();
        FMS_LOG_INFO("Update: %s\n", upload.filename.c_str());
        updateStatus = "Update started";
        otaInProgress = true;
        otaProgress = 0;
        contentLength = 0;
        uploadedBytes = 0;

        // Get content length from headers if available
        if (server.hasHeader("Content-Length")) {
          contentLength = server.header("Content-Length").toInt();
          FMS_LOG_INFO("Content-Length: %d bytes\n", contentLength);
        }
        // Free up as much memory as possible
        FMS_LOG_INFO("Free heap before update: %d bytes\n", ESP.getFreeHeap());
        // Start the update with the correct partition type
        if (!Update.begin(contentLength > 0 ? contentLength : UPDATE_SIZE_UNKNOWN, U_FLASH)) {
          Update.printError(Serial);
          updateStatus = "OTA Error";
          otaInProgress = false;
          // Re-enable task watchdog
          esp_task_wdt_config_t wdtConfig;
          wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;
          wdtConfig.idle_core_mask = 0;
          wdtConfig.trigger_panic = true;
          esp_task_wdt_init(&wdtConfig);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Process upload chunks in larger batches for better performance
        uploadedBytes += upload.currentSize;
        // Calculate progress
        if (contentLength > 0) {
          otaProgress = (uploadedBytes * 100) / contentLength;
        } else if (Update.size()) {
          otaProgress = (uploadedBytes * 100) / Update.size();
        }
        // Write the received bytes to flash
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
          updateStatus = "OTA Error";
        }
        // Log progress less frequently to reduce overhead
        if (otaProgress % 10 == 0) {
          FMS_LOG_INFO("Progress: %u%% (%u / %u bytes)\n",
                       otaProgress,
                       uploadedBytes,
                       contentLength > 0 ? contentLength : Update.size());
        }
        // Yield to avoid watchdog trigger
        yield();
      } else if (upload.status == UPLOAD_FILE_END) {
        // Finalize the update
        if (Update.end(true)) {
          FMS_LOG_INFO("Update Success: %u bytes\nRebooting...\n", uploadedBytes);
          updateStatus = "Update successful";
          otaProgress = 100;
        } else {
          Update.printError(Serial);
          updateStatus = "OTA Error";
        }
        otaInProgress = false;
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        otaInProgress = false;
        updateStatus = "Update aborted";
        FMS_LOG_INFO("Update aborted");
        // Re-enable task watchdog
        esp_task_wdt_config_t wdtConfig;
        wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;
        wdtConfig.idle_core_mask = 0;
        wdtConfig.trigger_panic = true;
        esp_task_wdt_init(&wdtConfig);
      }
      yield();
    });

  // Handle restart request
  server.on("/api/restart", HTTP_POST, []() {
    server.send(200, "text/plain", "Restarting...");
    delay(1000);
    ESP.restart();
  });

  // Set up mDNS responder
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
}

static void web_server_task(void* arg) {
  if (!MDNS.begin(deviceName)) {                // Set up mDNS responder
    Serial.println("[DNS] Error setting up MDNS responder!");
  } else {
    Serial.println("[DNS] mDNS responder started");
    MDNS.addService("esp-ota", "tcp", 80);      // Add service to MDNS
    MDNS.addService("http", "tcp", 80);         // Add standard HTTP service for better discovery
  }
  deviceName = deviceName + String(WiFi.macAddress()).c_str();
  fms_set_ota_server();
  server.begin();

  while (1) {
    server.handleClient();
    // Update uptime counter (every second)
    if (millis() - lastUptimeUpdate >= 1000) {
      uptime++;
      lastUptimeUpdate = millis();
    }
   
    vTaskDelay(100 / portTICK_PERIOD_MS);
   // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#endif // USE_V1_OTA
