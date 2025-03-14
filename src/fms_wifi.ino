
bool initialize_fms_wifi(bool flag) {
  if(flag)
  {
    // get ssid and password from nvs storage 
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    fms_nvs_storage.end();

    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);

    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") 
    {
      fms_debug_log_printf("[DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }

    fms_debug_log_printf("[DEBUG WiFi] wifi .. connecting \n\r");
    #if SHOW_RESP_UART_SYS_LOG
    fms_cli_serial.print("[DEBUG WiFi] SSID : ");
    fms_cli_serial.println(String(sysCfg.wifi_ssid).c_str());
    fms_cli_serial.print("[DEBUG WiFi] PASS : ");
    fms_cli_serial.println(String(sysCfg.wifi_password).c_str());
    #endif

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.print(".");
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      
    }
    return true;
  }
  }

bool wifi_led_ticker() {
  static bool state = false;
  gpio_set_level(GPIO_NUM_2,state);
  state = !state;
}

uint8_t count = 1;
static void wifi_task(void *arg) {
  BaseType_t rc;
  while(1) {
   if(WiFi.status() != WL_CONNECTED){
    fms_debug_log_printf("[WiFi] retry .. connecting\n\r");
    gpio_set_level(GPIO_NUM_2,HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_NUM_2,LOW);
    vTaskDelay(pdMS_TO_TICKS(500));

   }
   else  {
    fms_debug_log_printf("[WiFi] wifi .. connected\n\r");
    gpio_set_level(GPIO_NUM_2,HIGH);
  }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
