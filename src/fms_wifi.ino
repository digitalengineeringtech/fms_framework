
bool fms_wifi_init() {
  if (wifi_start_event == true) {
    fms_log_printf("Connecting to WiFi...");
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    fms_log_printf("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      fms_log_printf(".");
      return false;
    }

    fms_log_printf("\nConnected to WiFi!");
    fms_log_printf("IP Address:%d ",WiFi.localIP());
    wifi_start_event = false;
    return true;
  }
}

static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    fms_wifi_init();
    rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
