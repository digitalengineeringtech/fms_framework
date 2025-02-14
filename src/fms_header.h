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
#include <nvs.h>
#include <nvs_flash.h>
#include "Ticker.h"
#include "PubSubClient.h"

// Project details
#define PROJECT                             "fms"                   // fuel management system
#define VERSION                             "0.1.0"                 // version number
#define CLI_PASSWORD                        "admin"                 // cli password // change this password

// Device details
#define DEVICE_ID                           "fms_001"               // device id
#define STATION_ID                          1                       // station id
#define SHOW_DEBUG_SYS_LOG                  true    
#define SHOW_DEBUG_SD_TEST_LOG              false  
#define SHOW_DEBUG_FMS_CHIP_INFO_LOG        false
#define SHOW_UART_SYS_LOG                   true                    // show uart log
#define SHOW_RESP_UART_SYS_LOG              true

// WiFi configuration
#define WIFI_SSID                           sysCfg.wifi_ssid        // wifi ssid
#define WIFI_PASSWORD                       sysCfg.wifi_password    // wifi password

// MQTT configuration
#define MQTT_SERVER                         sysCfg.mqtt_server_host                     // mqtt server address
#define MQTT_PORT                           1883                    // mqtt port
#define MQTT_USER                           " "                     // mqtt user
#define MQTT_PASSWORD                       " "                     // mqtt password
#define MQTT_DEVICE_ID                      DEVICE_ID               // mqtt device id
#define MQTT_LWT_OFFLINE                    "offline"               // mqtt last will topic offline
#define MQTT_LWT_ONLINE                     "online"                // mqtt last will topic online
#define mqttTask                             true
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
#define SD_CARD_CONFIG_FILE_NAME            "fms_config.txt"        // sd card file name change it to your file name

#define _log_printf                         log_printf              // in build in chip-debug-report.cpp
#define fms_cli_serial                      Serial                  // cli serial port

// Global objects
uart_t * fms_cli_uart;
Preferences fms_nvs_storage;
WiFiClient wf_client;
PubSubClient fms_mqtt_client(wf_client);

bool wifi_start_event = true;

// System configuration structure
struct SYSCFG {
    unsigned long bootcount;
    unsigned long version;
    char wifi_ssid[32]                      = "";
    char wifi_password[64]                  = "";
    char mqtt_server_host[32]               = "192.168.1.142";
    char* mqtt_user                         = MQTT_USER;
    char* mqtt_password                     = MQTT_PASSWORD;
    uint32_t mqtt_port                      = MQTT_PORT;
    char* mqtt_device_id                    = MQTT_DEVICE_ID;
    char* mqtt_lwt_status[20];     
    char* device_id                         = DEVICE_ID;
    uint32_t station_id                     = STATION_ID;
} sysCfg;

/*
* fms command cli setting
*/

// Command list
#define D_CMND_WIFI         "wifi"
#define D_CMND_RESTART      "restart"
#define D_CMND_WIFISCAN     "wifiscan"
#define D_CMND_MQTT         "mqtt"
#define D_CMND_WIFIREAD     "wifiread"
#define D_CMND_BOOTCOUNT    "bootcount"
#define D_CMD_DEVICEID      "devid"
#define D_CMD_DEBUG         "debug on"
#define D_CMD_NVS_STORAGE   "stgcheck" // read nvs storage

struct FMSMAILBOX {
    String command;
    String data;
    uint32_t data_len;
    uint32_t payload;
    uint32_t index;
} fmsMailBox;

// Command functions
void fms_CmndWifi();
void fms_CmndRestart();
void fms_CmndWifiScan();
void fms_CmndMqtt();
void fms_CmndWifiRead();
void fms_CmndBootCount();
void fms_CmndAddDeviceId();
void fms_CmndDebug();
void fms_CmndStroagecheck();
void fms_Cmndhelp();

// command table
const struct COMMAND {
    const char* name;
    void (*function)();
} Commands[] = {
    {D_CMND_WIFI, fms_CmndWifi},
    {D_CMND_RESTART, fms_CmndRestart},
    {D_CMND_WIFISCAN, fms_CmndWifiScan},
    {D_CMND_MQTT, fms_CmndMqtt},
    {D_CMND_WIFIREAD, fms_CmndWifiRead},
    {D_CMND_BOOTCOUNT, fms_CmndBootCount},
    {D_CMD_DEVICEID,  fms_CmndAddDeviceId},
    {D_CMD_DEBUG, fms_CmndDebug},
    {D_CMD_NVS_STORAGE,fms_CmndStroagecheck},
    {"help",fms_Cmndhelp}
};

Ticker wifi_ticker;
static void wifi_task(void *arg);
bool fms_wifi_init();
bool wifi_led_ticker();
// mqtt
static void mqtt_task(void *arg);
void fms_mqtt_callback(char* topic,byte* payload,unsigned int length);

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
