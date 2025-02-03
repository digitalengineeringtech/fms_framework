#include <Arduino.h>
#line 14 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void event_receive(void *arg);
#line 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void setup();
#line 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void loop();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino"
static void cli_task(void *arg);
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
#line 0 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"

/*
# author 
    Trion 30.1.2025 
# project
    Fuel Management System (FMS)
# date
    30/01/2025
# version
    1.0.0

+-------------------+       +-------------------+       +-------------------+
|                   |       |                   |       |                   |
|     Task 1        |       |     Queue         |       |     Task 2        |
|  (Producer)       |       |  (Data Buffer)    |       |  (Consumer)       |
|                   |       |                   |       |                   |
| 1. Generate       |       |                   |       | 1. Wait for       |
|    Random Number  |       |                   |       |    Semaphore      |
|                   |       |                   |       |                   |
| 2. Send Number    | ----> |  Store Number     | <---- | 2. Receive Number |
|    to Queue       |       |    in Queue       |       |    from Queue     |
|                   |       |                   |       |                   |
| 3. Give Semaphore | ----> |                   |       | 3. Process Number |
|    (Signal Task 2)|       |                   |       |    (Print to Serial)
|                   |       |                   |       |                   |
+-------------------+       +-------------------+       +-------------------+
            ^                                                       |
            |                                                       |
            |                                                       |
            +-------------------------------------------------------+
                                  Binary Semaphore
*/

#ifndef _FMS_HEADER_H_
#define _FMS_HEADER_H_

// Include libraries
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Preferences.h>
#include "time.h"
#include "LittleFS.h"
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/semphr.h>

// Project details
#define PROJECT                         "fms"                   // fuel management system
#define VERSION                         "0.1.0"                 // version number
#define CLI_PASSWORD                    "admin"                 // cli password // change this password
// Device details
#define DEVICE_ID                       "fms_001"               // device id
#define SHOW_SYS_LOG                     true      

// WiFi configuration
#define WIFI_SSID                       "wifitest"                     // wifi ssid
#define WIFI_PASSWORD                   "12345678"                     // wifi password

// MQTT configuration
#define MQTT_SERVER                     " "                     // mqtt server address
#define MQTT_PORT                       1883                    // mqtt port
#define MQTT_USER                       " "                     // mqtt user
#define MQTT_PASSWORD                   " "                     // mqtt password
#define MQTT_DEVICE_ID                  DEVICE_ID               // mqtt device id
#define MQTT_LWT_OFFLINE                "offline"               // mqtt last will topic offline
#define MQTT_LWT_ONLINE                 "online"                // mqtt last will topic online
// Web server configuration
#define WEB_SERVER_PORT                 80                      // web server port

// SD card pins configuration (check your board pin layout spi communication)
#define SD_CARD_CS_PIN                  5                       // sd card chip select pin
#define SD_CARD_MOSI_PIN                23                      // sd card mosi pin
#define SD_CARD_MISO_PIN                19                      // sd card miso pin
#define SD_CARD_SCK_PIN                 18                      // sd card sck pin
#define SD_CARD_SPI_SPEED               4000000                 // sd card spi speed

// Time configuration
#define NTP_SERVER                      "pool.ntp.org"          // ntp server
#define GMT_OFFSET_SEC                   7 * 3600                      // gmt offset in seconds to fix time zone
#define DAYLIGHT_OFFSET_SEC              0                      // daylight offset in seconds to fix time zone

// SD card file configuration
#define SD_CARD_CONFIG_FILE_NAME        "/fms.txt"              // sd card file name change it to your file name

// MQTT topics from the old project
#define MQTT_TOPIC_PUMP_APPROVAL        "detpos/local_server/1"
#define MQTT_TOPIC_PUMP_REQUEST         "detpos/device/permit/1"
#define MQTT_TOPIC_PUMP_PRESET          "detpos/local_server/preset"
#define MQTT_TOPIC_PUMP_LIVE            "detpos/device/livedata/1"
#define MQTT_TOPIC_PUMP_FINAL           "detpos/device/Final/1"
#define MQTT_TOPIC_WH_REQUEST           "detpos/device/whreq"
#define MQTT_TOPIC_PRICE_CHANGE         "detpos/local_server/price"
#define MQTT_TOPIC_PRICE_REQUEST        "detpos/device/pricereq/1"
#define MQTT_TOPIC_ACTIVE               "detpos/device/active/1"
#define MQTT_TOPIC_DEVICE               "1"
#define MQTT_TOPIC_DEVICE_ID            "detpos/local_server/initial1/det/0A0000"
#define MQTT_TOPIC_RESET                "detpos/hmi/reset"
#define MQTT_TOPIC_RELOAD               "detpos/local_server/reload/1"
#define MQTT_TOPIC_PUMP_FINAL_RELOAD    "detpos/device/Reload/1"

WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

 struct SYSCFG{
  unsigned long bootcount;
  unsigned long version;
 } sysCfg;

 // rtos task handle 
static TaskHandle_t heventTask;
static TaskHandle_t hwifiTask;
static TaskHandle_t hmqttTask;
static TaskHandle_t hsdCardTask;
static TaskHandle_t hwebServerTask;
static TaskHandle_t hspiTask; // use this for something 
static TaskHandle_t hcliTask;

SemaphoreHandle_t serialMutex;                   // Mutex to protect the buffer test code 



volatile uint8_t serialBuffer[4];                // Buffer to store received hex value // test code  
volatile uint8_t bufferIndex                = 0; // Index for the buffer // test code 
void addLog(byte loglevel, const char *line);
#endif
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
#include "fms_header.h"
#include "chip-debug-report.h"

// int seriallog_level = 1;
// void addLog(byte loglevel, const char *line) {
//   if (SHOW_SYS_LOG) {
//     char mxtime[9];
//     struct tm rtcTime;
//     if (getLocalTime(&rtcTime)) snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
//     if (loglevel <= seriallog_level) Serial.printf("%s %s\n", mxtime, line);  // on and off serial print optional feature;
//   }
// }

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

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino"
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    chip_report_printf("  CLI TERMINAL     : %lu cli\n",1 );
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

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

