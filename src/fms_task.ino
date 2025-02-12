bool  fms_task_create() {
  
  BaseType_t rc;
  rc = xTaskCreatePinnedToCore(
    sd_task,       // Task function
    "sdcard",      // Name
    3000,          // Stack size
    nullptr,       // Parameters
    2,             // Priority
    &hsdCardTask,  // Handle
    app_cpu        // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    wifi_task,   // Task function
    "wifi",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
    &hwifiTask,  // Handle
    app_cpu      // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    mqtt_task,   // Task function
    "mqtt",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
    &hmqttTask,  // Handle
    app_cpu      // CPU
  );
  assert(rc == pdPASS);


  rc = xTaskCreatePinnedToCore(
    cli_task,   // Task function
    "cli",      // Name
    3000,       // Stack size
    nullptr,    // Parameters
    1,          // Priority
    &hcliTask,  // Handle
    app_cpu     // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    web_server_task,  // Task function
    "webserver",      // Name
    3000,             // Stack size
    nullptr,          // Parameters
    4,                // Priority
    &hwebServerTask,  // Handle
    app_cpu           // CPU
  );
  assert(rc == pdPASS);

  return true;
}