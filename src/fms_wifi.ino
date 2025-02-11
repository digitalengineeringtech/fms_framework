
bool initialize_fms_wifi(bool flag) {
  if(flag)
  {
    fms_log_printf("Connecting to WiFi...");
    fms_cli_serial.print("SSID : ");
    fms_cli_serial.println(sysCfg.wifi_ssid);
    fms_cli_serial.print("PASS : ");
    fms_cli_serial.println(sysCfg.wifi_password);
   // fms_log_printf("SSID: %s , PASS: %s",sysCfg.wifi_ssid,sysCfg.wifi_password);
    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") fms_log_printf("wifi credential value is empty");
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    fms_cli_serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      fms_cli_serial.println(".");
      return false;
    }
    return true;
  }
  }


static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   
    //rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
