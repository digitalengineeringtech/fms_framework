static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    if (wifi_start_event == true) {
      fms_log_printf("Connecting to WiFi...");
      WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
      Serial.print("Connecting to WiFi");
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
      }

      Serial.println("\nConnected to WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      wifi_start_event = false;
    }

    fms_log_printf("WiFi task started\n");
    rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
