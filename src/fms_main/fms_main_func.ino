/*
    * fms_main_func.ino
    * This file is part of the ESP32 FMS 
    * main function link file
*/

void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  FMS_LOG_INFO("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end();  // close nvs storage
}

void log_chip_info() {
#if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
#endif
}

bool initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 115200)) {
    fms_uart2_serial.onReceive(fm_rx_irq_interrupt);  // uart interrupt function
    FMS_LOG_INFO("[FMSUART2] UART2.. DONE");
    return true;
  } else {
    FMS_LOG_ERROR("[FMSUART2] UART2.. FAIL");
    return false;
  }
}

bool initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    FMS_LOG_INFO("Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    FMS_LOG_WARNING("Failed to connect to WiFi");
    return false;
  }
}

void run_sd_test() {
#if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    FMS_LOG_INFO("==================== sd card test success================");
  } else {
    FMS_LOG_ERROR("sd card test failed");
  }
#endif
}

void log_debug_info() {
#if SHOW_DEBUG_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
#endif
}

void fms_pin_mode(int pin, int mode) {
  pinMode(pin, mode);
}

void fms_dns_responder_init() {
  // Set up mDNS responder
  if (!MDNS.begin(deviceName)) {
    FMS_LOG_ERROR("Error setting up MDNS responder!");
  } else {
    FMS_LOG_INFO("mDNS responder started");
    // Add service to MDNS
    MDNS.addService("esp-ota", "tcp", 80);
    MDNS.addService("http", "tcp", 80);  // Add standard HTTP service for better discovery
  }
}
