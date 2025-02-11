#include "fms_header.h"

int app_cpu = 0;
#define chip_report_printf log_printf // for chip info debug

void initialize_uart() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_log_printf("setup finish for cli uart\n\r");
  }
}

void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  fms_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage
}

void setup() {
  #if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
 #endif

 initialize_uart();
 initialize_nvs_storage(); // save boot count to eeprom 
 fms_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
 initialize_fms_wifi(wifi_start_event); // wifi connection
  /*
  user main code here
  */
  #if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    fms_log_printf("\n\r==================== sd card test success================\n");
  } else {
    fms_log_printf("sd card test failed\n");
  }
  #endif

  fms_log_printf("Start initiazling task \n\r");
  fms_task_create(); // rtos task create 

#if SHOW_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
#endif
}

void loop() {

}
