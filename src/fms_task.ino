bool  fms_task_create() {
  
  BaseType_t sd_rc;
  sd_rc = xTaskCreatePinnedToCore(
    sd_task,       // Task function
    "sdcard",      // Name
    3000,          // Stack size
    nullptr,       // Parameters
    2,             // Priority
    &hsdCardTask,  // Handle
    app_cpu        // CPU
  );
  assert(sd_rc == pdPASS);
  if(sd_rc != pdPASS){
    fms_debug_log_printf("sd task created fail");
  }
  BaseType_t wifi_rc;
  wifi_rc = xTaskCreatePinnedToCore(
    wifi_task,   // Task function
    "wifi",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
    &hwifiTask,  // Handle
    app_cpu      // CPU
  );
  assert(wifi_rc == pdPASS);
  BaseType_t mqtt_rc;
  mqtt_rc = xTaskCreatePinnedToCore(
    mqtt_task,   // Task function
    "mqtt",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
    &hmqttTask,  // Handle
    app_cpu      // CPU
  );
  assert(mqtt_rc == pdPASS);

  BaseType_t cli_rc;
  cli_rc = xTaskCreatePinnedToCore(
    cli_task,   // Task function
    "cli",      // Name
    3000,       // Stack size
    nullptr,    // Parameters
    1,          // Priority
    &hcliTask,  // Handle
    app_cpu     // CPU
  );
  assert(cli_rc == pdPASS);
  BaseType_t webserver_rc;
  webserver_rc = xTaskCreatePinnedToCore(
    web_server_task,  // Task function
    "webserver",      // Name
    3000,             // Stack size
    nullptr,          // Parameters
    4,                // Priority
    &hwebServerTask,  // Handle
    app_cpu           // CPU
  );
  assert(webserver_rc == pdPASS);

  return true;
}