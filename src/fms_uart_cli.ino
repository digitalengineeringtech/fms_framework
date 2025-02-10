// Last update: 2021-08-29 20:00:00
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    fms_cli_serial.begin(baudrate);
    fms_log_printf("UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
    return true;
  }
  return true;
}


void fms_CmndWifi() {
 char ssid[32] = "ssid";
 char password[64] = "password";
  if(sscanf(fmsMailBox.data.c_str(),"ssid:\"%31[^\"]\" password:\"%63[^\"]\"", ssid, password) == 2) {
    strncpy(sysCfg.wifi_ssid, ssid, sizeof(sysCfg.wifi_ssid)-1);
    strncpy(sysCfg.wifi_password, password, sizeof(sysCfg.wifi_password)-1);
    if(SHOW_UART_SYS_LOG) {
      fms_cli_serial.printf("WIFI SSID : %s\n", String(sysCfg.wifi_ssid).c_str());
      fms_cli_serial.printf("WIFI PASSWORD : %s\n", String(sysCfg.wifi_password).c_str());
    }
    fms_response_cmnd_handler("true");
  } else {
    fms_response_cmnd_handler("Invalid format. Use: wifi \"your_ssid\" \"your_password\"");
  }
}

#define SCAN_COUNT 1 // Number of scan iterations
void fms_CmndWifiScan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  //fms_response_cmnd_handler("scanning wifi -- 3 time");
  
    char buffer[512]; // Buffer for JSON output
    strcpy(buffer, "{\"wifiscan\":true,\"networks\":[");
    int bufferLen = strlen(buffer);

    int networkIndex = 0;
    
    for (int scanNum = 0; scanNum < SCAN_COUNT; scanNum++) {
        int numNetworks = WiFi.scanNetworks();
       // fms_cli_serial.printf("%d networks found\n", numNetworks);/

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
        delay(1000); // Short delay for better scan results
    }

    WiFi.scanDelete(); // Free memory
    strcat(buffer, "]}"); // Close JSON array

    fms_cli_serial.println(buffer); // Output JSON result



 //fms_response_cmnd_handler("true");
}

void fms_CmndRestart() {
  fms_log_printf("Restart command\n");
  //fms_response_cmnd_handler("Restart command");
  //return true;
}

void fms_CmndWifiRead() {
  fms_response_cmnd_handler("wifiread");
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
    if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Received : %s\n\r", cmdLine.c_str());
    cmdLine.trim();
    int spaceIndex = cmdLine.indexOf(' ');
    if(spaceIndex == -1){
      fmsMailBox.command = cmdLine;
      if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Received : %s\n\r", cmdLine.c_str());
      fmsMailBox.data = "";
    }else {
      fmsMailBox.command = cmdLine.substring(0, spaceIndex);
      fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
      if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
    }
    fmsMailBox.data_len = fmsMailBox.data.length();
    

    // for(uint32_t i = 0; i < sizeof(Commands)/sizeof(COMMAND); i++) {
    //   if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("loop receive : %s\n\r", Commands[i].name);
    //   if(strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) { // strcasecmp is case insensitive
    //   Commands[i].function();
    //   return;
    //   }
    // }

     for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
    if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
      Commands[i].function();
      return;
    }
  }


    if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Command not found\n");
  }
}


static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    fms_log_printf("uart cli is running\n\r");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
