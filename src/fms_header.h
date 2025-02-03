
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

#ifndef _HEADER_H_
#define _HEADER_H_

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