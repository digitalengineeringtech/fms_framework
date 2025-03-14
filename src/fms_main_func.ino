/*
    * fms_main_func.ino
    * 
    * This file is part of the ESP32 FMS 
    * main function link file
*/


void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  fms_debug_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage
}

void log_chip_info() {
  #if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
  #endif
}

bool initialize_uart_cli() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_cli_serial.onReceive(UART_RX_IRQ); // uart interrupt function 
    fms_debug_log_printf("[FMSUART1] UART1 CLI.. DONE\n\r");
    return true;
  } else {
    fms_debug_log_printf("[FMSUART1] UART1 CLI.. FAIL\n\r");
    return false;
  }
}

bool initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 115200)) {
    fms_uart2_serial.onReceive(UART2_RX_IRQ); // uart interrupt function
    fms_debug_log_printf("[FMSUART2] UART2.. DONE\n\r"); 
    return true;
  } else {
    fms_debug_log_printf("[FMSUART2] UART2.. FAIL\n\r"); 
    return false;
  }
}


bool initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
    return true;
  } else {
    fms_debug_log_printf("[WiFi] wifi .. not connected\n");
    
    return false;
  }
}

void run_sd_test() {
  #if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    fms_debug_log_printf("\n\r==================== sd card test success================\n");
  } else {
    fms_debug_log_printf("sd card test failed\n");
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
