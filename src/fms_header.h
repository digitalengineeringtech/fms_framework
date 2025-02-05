
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
#include "time.h"
#include <LittleFS.h>
#include "chip-debug-report.h"
#include "esp32-hal-uart.h"
#include <Preferences.h>


// Project details
#define PROJECT                             "fms"                   // fuel management system
#define VERSION                             "0.1.0"                 // version number
#define CLI_PASSWORD                        "admin"                 // cli password // change this password

// Device details
#define DEVICE_ID                           "fms_001"               // device id
#define STATION_ID                          1                       // station id
#define SHOW_SYS_LOG                        true    
#define SHOW_SD_TEST_LOG                    true  

// WiFi configuration
#define WIFI_SSID                           "wifitest"              // wifi ssid
#define WIFI_PASSWORD                       "12345678"              // wifi password

// MQTT configuration
#define MQTT_SERVER                         " "                     // mqtt server address
#define MQTT_PORT                           1883                    // mqtt port
#define MQTT_USER                           " "                     // mqtt user
#define MQTT_PASSWORD                       " "                     // mqtt password
#define MQTT_DEVICE_ID                      DEVICE_ID               // mqtt device id
#define MQTT_LWT_OFFLINE                    "offline"               // mqtt last will topic offline
#define MQTT_LWT_ONLINE                     "online"                // mqtt last will topic online

// Web server configuration
#define WEB_SERVER_PORT                     80                      // web server port

// SD card pins configuration (check your board pin layout spi communication)
#define SD_CARD_CS_PIN                      5                       // sd card chip select pin
#define SD_CARD_MOSI_PIN                    23                      // sd card mosi pin
#define SD_CARD_MISO_PIN                    19                      // sd card miso pin
#define SD_CARD_SCK_PIN                     18                      // sd card sck pin
#define SD_CARD_SPI_SPEED                   4000000                 // sd card spi speed

// Time configuration
#define NTP_SERVER                          "pool.ntp.org"          // ntp server
#define GMT_OFFSET_SEC                      (7 * 3600)              // gmt offset in seconds to fix time zone
#define DAYLIGHT_OFFSET_SEC                 0                       // daylight offset in seconds to fix time zone

// SD card file configuration
#define SD_CARD_CONFIG_FILE_NAME            "fms_config.txt"              // sd card file name change it to your file name

#define _log_printf                         log_printf              // in build in chip-debug-report.cpp
#define fms_cli_serial                      Serial                  // cli serial port

// Global objects
Preferences fms_nvs_storage;
WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

// System configuration structure
struct SYSCFG {
    unsigned long       bootcount;
    unsigned long       version;

    char*               wifi_ssid               = WIFI_SSID;
    char*               wifi_password           = WIFI_PASSWORD;

    char*               mqtt_server_host        = MQTT_SERVER;
    char*               mqtt_user               = MQTT_USER;
    char*               mqtt_password           = MQTT_PASSWORD;
    uint32_t            mqtt_port               = MQTT_PORT; // in datastructure uint32_t

    char*               mqtt_device_id          = MQTT_DEVICE_ID;
    char*               mqtt_lwt_status[20];     
    char*               device_id               = DEVICE_ID; // in datastructure uint32_t
    uint32_t            station_id              = STATION_ID;

} sysCfg;


// RTOS task handles
static TaskHandle_t heventTask;
static TaskHandle_t hwifiTask;
static TaskHandle_t hmqttTask;
static TaskHandle_t hsdCardTask;
static TaskHandle_t hwebServerTask;
static TaskHandle_t hspiTask; 
static TaskHandle_t hcliTask;

// Mutex for serial communication
SemaphoreHandle_t serialMutex;

// Serial buffer and index
volatile uint8_t serialBuffer[4]; // for testing
volatile uint8_t bufferIndex = 0; // for testing

// UART command flag
bool use_uart_command = true;

// Function declarations
void addLog(byte loglevel, const char *line);

#endif // _FMS_HEADER_H_