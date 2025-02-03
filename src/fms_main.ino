#include "fms_header.h"
#include "chip-debug-report.h"

int seriallog_level = 1;
void addLog(byte loglevel, const char *line) {
  if (SHOW_SYS_LOG) {
    char mxtime[9];
    struct tm rtcTime;
    if (getLocalTime(&rtcTime)) snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
    if (loglevel <= seriallog_level) Serial.printf("%s %s\n", mxtime, line);  // on and off serial print optional feature;
  }
}

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


 #define chip_report_printf log_printf

void setup() {
  printChipInfo();
  sysCfg.bootcount++;
  Serial.println("helloworld");
  app_cpu = xPortGetCoreID();
  Serial.begin(115200);
  serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control 
  assert(serialMutex != NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  //chip_report_printf("=========== Setup Start ===========\n");
  
  
  // sd card load
  fms_sd_begin();
  printAfterSetupInfo();
  // start create task
  fms_task_create();
}



void loop() {
  BaseType_t rc;
  //Serial.println("Main Loop");
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}
