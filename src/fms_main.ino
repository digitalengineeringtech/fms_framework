#include "fms_header.h"

void event_receive(void *arg) {
  uint32_t rv;
  BaseType_t rc;
  for (;;) {
    rc = xTaskNotifyWait(0, 0xFFFFFFFF, &rv, portMAX_DELAY);
    if (rc == pdTRUE) {
    }
  }
}

int app_cpu = 0;




void setup() {
fms_log_printf("CPU %d: Setup", app_cpu);
if (fms_uart_cli_begin(use_uart_command,115200)) fms_log_printf("uart cli begin\n"); // serial begin 

/***********************************************************************/
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();

  fms_log_printf("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

  serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control 
  assert(serialMutex != NULL);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  fms_sd_begin(); // start sd card
  fms_log_print("intializing task");
  // start create task
  fms_task_create();
}



void loop() {
  BaseType_t rc;
  //Serial.println("Main Loop");
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}
