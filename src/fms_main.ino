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


void setup() {
  fms_log_printf("CPU %d: Setup", app_cpu);
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_log_printf("uart cli begin\n");
  }
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  fms_log_printf("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  serialMutex = xSemaphoreCreateMutex();
  assert(serialMutex != NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  fms_sd_begin();
  fms_log_print("initializing task");
  fms_task_create();
}

void loop() {
  BaseType_t rc;
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}
