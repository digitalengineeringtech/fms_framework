
bool initialize_fms_wifi(bool flag) {
  if(flag)
  {
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    fms_nvs_storage.end();

    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);
  
    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") 
    {
      fms_log_printf("wifi credential value is empty");
      return false;
    }

    fms_log_printf("Connecting to WiFi...\n");

    #if SHOW_RESP_UART_SYS_LOG
    fms_cli_serial.print("SSID : ");
    fms_cli_serial.println(String(sysCfg.wifi_ssid).c_str());
    fms_cli_serial.print("PASS : ");
    fms_cli_serial.println(String(sysCfg.wifi_password).c_str());
    #endif


    WiFi.mode(WIFI_STA);
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    fms_cli_serial.println("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      fms_cli_serial.println(".");
      //return false;
    }
    return true;
  }

  }


static void wifi_task(void *arg) {
  BaseType_t rc;
  while(1) {
   if(WiFi.status() != WL_CONNECTED){
    fms_log_printf("[WiFi] wifi .. connecting\n\r");
   }else {
    fms_log_printf("[WiFi] wifi .. connected\n\r");
   }
    //rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
