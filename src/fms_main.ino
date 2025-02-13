#include "fms_header.h"

int app_cpu = 0;
#define chip_report_printf log_printf // for chip info debug

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
    fms_debug_log_printf("[FMSCLI] setup finish for cli uart\n\r");
    return true;
  } else {
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

void setup() {
  log_chip_info();
 if(initialize_uart_cli()) fms_debug_log_printf(" [FMSCLI] uart1 cli.. started\n\r");
  initialize_nvs_storage(); // save boot count to eeprom 
  fms_debug_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
  if(initialize_wifi()) fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
  run_sd_test();
  fms_debug_log_printf("Start initializing task \n\r");
  fms_task_create(); // rtos task create 
  log_debug_info();
}

void loop() {
  // user main code here
}
