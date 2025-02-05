#include "fms_header.h"

void event_receive(void *arg) {
  uint32_t rv;
  BaseType_t rc;
  for (;;) {
    rc = xTaskNotifyWait(0, 0xFFFFFFFF, &rv, portMAX_DELAY);
    if (rc == pdTRUE) {
      // Handle the event here
    }
  }
}

int app_cpu = 0;
#define chip_report_printf log_printf

void initialize_uart() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_log_printf("uart cli begin\n");
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
  fms_chip_info_log();
  fms_memory_info_log();
  fms_log_printf("CPU %d: Setup", app_cpu);

  initialize_uart();
  initialize_nvs_storage();

  serialMutex = xSemaphoreCreateMutex();
  assert(serialMutex != NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS); // wait delay 1 second

  fms_log_printf("*********************Testing SD card**************");
  write_data_sd("Hello World");
  fms_config_load_sd(); // load config data from sd card
  vTaskDelay(1000 / portTICK_PERIOD_MS); // wait delay 1 second
  
  fms_log_printf("initializing task");
  fms_task_create(); // rtos task create 

  fms_print_after_setup_info();
 // fms_task_usage_check(); 
}

void loop() {
  BaseType_t rc;
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}
