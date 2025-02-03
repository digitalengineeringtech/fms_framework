#include <header.h>

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
      // test code
      // if (rv & 2)
      //   printf("wifi() notified this task\n"); // test code
      // if (rv & 0b0001)
      //   printf("loop() notified this task.\n"); // test code
      // if (rv & 3)
      //   printf("sd() notified this task.\n"); // test code
      // if (rv & 0b0100)
      //   printf(" Serial interrupt notified this task with value: 0x%08X\n", rv); // test code
    }
  }
}

int app_cpu = 0;
void setup() {
  sysCfg.bootcount++;

  app_cpu = xPortGetCoreID();
  Serial.begin(115200);
  serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control 
  assert(serialMutex != NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // sd card load
  fms_sd_begin();
  // start create task
  fms_task_create();
}

void loop() {
  BaseType_t rc;
  //Serial.println("Main Loop");
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}
