#include <Arduino.h>
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
static void cli_task(void *arg);
#line 4 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void addLog(byte loglevel, const char *line);
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void event_receive(void *arg);
#line 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void app_main();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
void fms_sd_begin();
#line 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
static void sd_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
void fms_task_create();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
static void wifi_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("cli task started \n");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
#include "fms_header.h"

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

void app_main() {
Serial.begin(115200);
Serial.println("Start Main");
app_cpu = xPortGetCoreID();
// serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control
// assert(serialMutex != NULL);
vTaskDelay(1000 / portTICK_PERIOD_MS);
}
// void setup() {
//   sysCfg.bootcount++;

//   app_cpu = xPortGetCoreID();
//   Serial.begin(115200);
//   serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control 
//   assert(serialMutex != NULL);
//   vTaskDelay(1000 / portTICK_PERIOD_MS);
//   // sd card load
//   fms_sd_begin();
//   // start create task
//   fms_task_create();
// }

// void loop() {
//   BaseType_t rc;
//   //Serial.println("Main Loop");
//   rc = xTaskNotify(heventTask, 0b0001, eSetBits);
//   assert(rc == pdPASS);
// }

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("mqtt task started \n");
    rc = xTaskNotify(heventTask, 5, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
void fms_sd_begin() {
  Serial.println("Config Loaded");
}

static void sd_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("sd task started \n");
    rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
void fms_task_create() {
  BaseType_t rc;
    rc = xTaskCreatePinnedToCore(
    event_receive,       // Task function
    "event_receive",      // Name
    3000,          // Stack size
    nullptr,       // Parameters
    1,             // Priority
    &heventTask,  // Handle
    app_cpu        // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    sd_task,       // Task function
    "sdcard",      // Name
    3000,          // Stack size
    nullptr,       // Parameters
    1,             // Priority
    &hsdCardTask,  // Handle
    app_cpu        // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    wifi_task,   // Task function
    "wifi",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    1,           // Priority
    &hwifiTask,  // Handle
    app_cpu      // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    mqtt_task,   // Task function
    "mqtt",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    1,           // Priority
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
    1,                // Priority
    &hwebServerTask,  // Handle
    app_cpu           // CPU
  );
  assert(rc == pdPASS);
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"

void IRAM_ATTR serialEvent() {
  while (Serial.available()) { // test code 
    uint8_t data = Serial.read();
    // if (bufferIndex < sizeof(serialBuffer)) serialBuffer[bufferIndex++] = data;
    // if (bufferIndex == sizeof(serialBuffer)) {
    //   if (xSemaphoreTakeFromISR(serialMutex, NULL) == pdTRUE) {
    //     uint32_t hexValue = (serialBuffer[0] << 24) | (serialBuffer[1] << 16) | (serialBuffer[2] << 8) | serialBuffer[3];
    //     xTaskNotifyFromISR(heventTask, hexValue | 0b0100, eSetBits, NULL);
    //     bufferIndex = 0;
    //     xSemaphoreGiveFromISR(serialMutex, NULL);
    //  }
    //}
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("web server stated \n");
    rc = xTaskNotify(heventTask, 6, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("wifi task started \n");
    rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}

