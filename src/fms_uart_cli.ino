// Last update: 2021-08-29 20:00:00
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    fms_cli_serial.begin(baudrate);
    if(fms_cli_serial){
      fms_debug_log_printf("[FMSCLI] UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      fms_debug_log_printf("[FMSCLI] UART 1 CLI start fail\n\r");
      return false;
    }
  }
}

void fms_CmndDebug(){

}

void fms_CmndAddDeviceId() {

}

void fms_CmndStroagecheck() {
  nvs_stats_t nvs_stats;
  nvs_get_stats(NULL,&nvs_stats);
  size_t freeHeap = esp_get_free_heap_size();
  UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  fms_cli_serial.printf("{\"total\":%d,\"used\":%d,\"free\":%d,\"free_heap\":%u,\"stack_high_water_mark\":%u}\n", 
  nvs_stats.total_entries, nvs_stats.used_entries, nvs_stats.free_entries,freeHeap, stackHighWaterMark);
}

void fms_CmndBootCount() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  fms_response_cmnd_handler(String(sysCfg.bootcount).c_str());
  fms_nvs_storage.end(); // close nvs storage
}

void fms_CmndWifi() {
 char ssid[32] = "ssid";
 char password[64] = "password";
  if(sscanf(fmsMailBox.data.c_str(),"ssid:\"%31[^\"]\" password:\"%63[^\"]\"", ssid, password) == 2) {
    strncpy(sysCfg.wifi_ssid, ssid, sizeof(sysCfg.wifi_ssid)-1);
    strncpy(sysCfg.wifi_password, password, sizeof(sysCfg.wifi_password)-1);
    if(SHOW_RESP_UART_SYS_LOG) {
      fms_cli_serial.printf("[FMICLI] WIFI SSID : %s\n", String(sysCfg.wifi_ssid));
      fms_cli_serial.printf("[FMICLI] WIFI PASSWORD : %s\n", String(sysCfg.wifi_password));
    }
    fms_nvs_storage.begin("fms_config", false);
    fms_nvs_storage.putString("ssid",sysCfg.wifi_ssid);
    fms_nvs_storage.putString("pass",sysCfg.wifi_password);
    fms_nvs_storage.end();
    fms_response_cmnd_handler("true");
    fms_CmndRestart();
  } else {
    fms_response_cmnd_handler("[FMICLI] Invalid format. Use: wifi \"your_ssid\" \"your_password\"");
  }
}

#define SCAN_COUNT 1                 // Number of scan iterations
void fms_CmndWifiScan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                // Disconnect from any network
    char buffer[512];               // Buffer for JSON output
    strcpy(buffer, "{\"wifiscan\":true,\"networks\":[");
    int bufferLen = strlen(buffer);
    int networkIndex = 0;
    for (int scanNum = 0; scanNum < SCAN_COUNT; scanNum++) {
        int numNetworks = WiFi.scanNetworks();
        for (int i = 0; i < numNetworks; i++) {
            if (networkIndex > 0) strcat(buffer, ","); // Add comma for JSON formatting
            char entry[128];
            snprintf(entry, sizeof(entry),
                "{\"Nr\":%d,\"SSID\":\"%s\",\"RSSI\":%d,\"CH\":%d,\"Encryption\":\"%s\"}",
                networkIndex + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                WiFi.channel(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA+WPA2"
            );
            strcat(buffer, entry);
            bufferLen += strlen(entry);
            networkIndex++;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating // similar delay(1000)
    }
    WiFi.scanDelete(); // Free memory
    strcat(buffer, "]}"); // Close JSON array
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(buffer); // Output JSON result
}

void fms_CmndRestart() {
  vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for 1 second before repeating
  fms_debug_log_printf("[DEBUG RST] Restarting...\n\r");
  fms_response_cmnd_handler("true");
  ESP.restart();
}

void fms_CmndWifiRead() {
  if(WiFi.status() == WL_CONNECTED) {
    char entry[128];
    snprintf(entry, sizeof(entry),
        "{\"SSID\":\"%s\",\"RSSI\":%d,\"IP\":\"%s\"}",
        WiFi.SSID().c_str(), WiFi.RSSI(),
        WiFi.localIP().toString().c_str()
    );
  if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(String(entry).c_str());
  fms_response_cmnd_handler("Read OK");
  } else {
    fms_response_cmnd_handler("false");
  }
}

void fms_CmndMqtt() {
  fms_response_cmnd_handler("mqttcommand");
}

void fms_response_cmnd_handler(const char* result){
  if(SHOW_RESP_UART_SYS_LOG) {
    fms_cli_serial.print(F("{\""));
    fms_cli_serial.print(fmsMailBox.command);
    fms_cli_serial.print(F("\":\""));
    fms_cli_serial.print(result);
    fms_cli_serial.print(F("\"}\n"));
  }
}

void IRAM_ATTR serialEvent() {  
  char c;
  char buffer[32]; // for testing
  while (fms_cli_serial.available()) {
    yield();
    String cmdLine = fms_cli_serial.readStringUntil('\n'); 
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
    cmdLine.trim(); // Remove leading and trailing whitespace from this command line
    int spaceIndex = cmdLine.indexOf(' ');
    if(spaceIndex == -1){
      fmsMailBox.command = cmdLine;
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
      fmsMailBox.data = "";
    }else {
      fmsMailBox.command = cmdLine.substring(0, spaceIndex);
      fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
    }
    fmsMailBox.data_len = fmsMailBox.data.length();
     for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
    if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
      Commands[i].function();
      return;
    }
  }
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Command not found\n\r");
  }
}

static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
