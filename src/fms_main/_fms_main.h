#ifndef _FMS_MAIN_H_
#define _FMS_MAIN_H_
#include <WebServer.h>
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
#include <math.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WebSocketsServer.h>
#include <PubSubClient.h>
// protocol
#include <ModbusMaster.h>
// sd config
#include <SD.h>
#include <FS.h>
#include <SPI.h>
// ota server
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Update.h>

#include <esp_task_wdt.h>
#include <esp_ota_ops.h> 


Ticker ticker;
String firmwareVersion                = "0.1.0";              // Current firmware version
String deviceName                     = "ultramarine-v0.1-";

#define CLI_PASSWORD                    "admin"               // cli password     // change this password
#define BUILTIN_LED                     2  
// Device details
#define STATION_ID                      1                     // station id
#define SHOW_DEBUG_SYS_LOG              true
#define SHOW_DEBUG_SD_TEST_LOG          true
#define SHOW_DEBUG_FMS_CHIP_INFO_LOG    false
#define SHOW_UART_SYS_LOG               true                  // show uart log
#define SHOW_RESP_UART_SYS_LOG          true

// WiFi configuration
#define WIFI_SSID sysCfg.wifi_ssid              // wifi ssid
#define WIFI_PASSWORD sysCfg.wifi_password      // wifi password
#define MQTT_SERVER sysCfg.mqtt_server_host     // mqtt server address
#define MQTT_PORT 1883                          // mqtt port
#define MQTT_USER " "                           // mqtt user
#define MQTT_PASSWORD " "                       // mqtt password

#define MQTT_LWT_OFFLINE "offline"              // mqtt last will topic offline
#define MQTT_LWT_ONLINE "online"                // mqtt last will topic online
#define mqttTask true
#define WEB_SERVER_PORT 80                          // web server port

// SD configuration 
#define SD_CARD_CS_PIN 5                            // sd card chip select pin
#define SD_CARD_CONFIG_FILE_NAME "fms_config.txt"   // sd card file name change it to your file name

// Time configuration
#define NTP_SERVER "pool.ntp.org"                   // ntp server
#define GMT_OFFSET_SEC (7 * 3600)                   // gmt offset in seconds to fix time zone
#define DAYLIGHT_OFFSET_SEC 0                       // daylight offset in seconds to fix time zone
#define _log_printf log_printf                      // in build in chip-debug-report.cpp
#define fms_cli_serial Serial                       // cli serial port
#define fms_uart2_serial Serial1                    // uart2 serial port

#define LED_BUILTIN                 16
#define chip_report_printf          log_printf  
// lanfeng modbus
#define MAX485_DE 15
#define MAX485_RE_NEG 15
ModbusMaster node;

// nozzle config
#define MAX_NOZZLES                 2 // change your noz count
bool pump_approve[MAX_NOZZLES]      = {false};

// OTA  configuration 
bool          otaInProgress         = false;
uint8_t       otaProgress           = 0;
const char*   updateStatus          = "Idle";
size_t        contentLength         = 0;
size_t        uploadedBytes         = 0;
unsigned long uptime                = 0;
unsigned long lastUptimeUpdate      = 0;
unsigned long lastInfoRequest       = 0;
String cachedInfoResponse           = "";
const unsigned long INFO_CACHE_TIME = 1000;     // 1 second cache
const String correctUsername        = "admin";  // change your login username here
const String correctPassword        = "1234";   // change your login pass here
bool isAuthenticated                = false;    // optional ota password
// UART command flag
bool use_uart_command               = true;
bool use_serial1                    = true;
int app_cpu                         = 0;
bool testModeActive                 = false;
const size_t MAX_BUFFER_SIZE        = 4096; 
const unsigned long WIFI_TIMEOUT    = 20000;  
unsigned long currentMillis         = 0;
unsigned long ota_previousMillis    = 0;
const int LED_ONE                   = LED_BUILTIN;  
const int LED_TWO                   = 2;   
bool wifi_start_event               = true;
Preferences                         preferences;
WebServer                           server(WEB_SERVER_PORT);
String                              serialOutputBuffer;
uart_t*                             fms_cli_uart;
Preferences                         fms_nvs_storage;
WiFiClient                          wf_client;
PubSubClient                        fms_mqtt_client(wf_client);
HTTPClient                          http;
WiFiClient                          http_client;
bool permitMessageSent             = false; // for sent permit message time
// mqtt topic

const char* fms_sub_topics[] = { // subscribe topic 
  "detpos/local_server/#",
  "detpos/local_server/price",
  "detpos/local_server/preset"
};

const uint8_t fms_sub_topics_count = sizeof(fms_sub_topics)/sizeof(fms_sub_topic[0]);


// from old 
// char pumpapprobuf[22]               = "detpos/local_server/1";
// char pumppresetbuf[28]              = "detpos/local_server/preset";  // return from local server
// char reload_topic[29]               = "detpos/local_server/reload/1";  // return from local server
// char pricechange[26]                = "detpos/local_server/price";  // return from local server

char device_Id_topic[40]            = "detpos/local_server/initial1/det/0A0000";  // return from local server
char pplive[25]                     = "detpos/device/livedata/1";
char ppfinal[22]                    = "detpos/device/Final/1";
char whreqbuf[20]                   = "detpos/device/whreq";
char pricereqbuf[25]                = "detpos/device/pricereq/1";
char activebuf[23]                  = "detpos/device/active/1";
char devicebuf[2]                   = "1";
char Reset_topic[17]                = "detpos/hmi/reset";
const char permitTopic[23]          = "detpos/device/permit/";
char pumprequest[23];
char payload[10]; // for permit message                



struct SYSCFG {
  unsigned long bootcount;
  unsigned long version;
  char          wifi_ssid[32]         = "";
  char          wifi_password[64]     = " ";
  char          mqtt_server_host[32]  = "192.168.1.142";
  char*         mqtt_user             = MQTT_USER;
  char*         mqtt_password         = MQTT_PASSWORD;
  uint32_t      mqtt_port             = MQTT_PORT;
  String        mqtt_device_id        = deviceName;
  char*         mqtt_lwt_status[20];
  String         device_id             = deviceName;
  uint32_t      station_id            = STATION_ID;
} sysCfg;


struct FMSMAILBOX {
  String command;
  String data;
  uint32_t data_len;
  uint32_t payload;
  uint32_t index;
} fmsMailBox;

static TaskHandle_t heventTask;
static TaskHandle_t hwifiTask;
static TaskHandle_t hmqttTask;
static TaskHandle_t hsdCardTask;
static TaskHandle_t hwebServerTask;
static TaskHandle_t hspiTask;
static TaskHandle_t hcliTask;
static TaskHandle_t huart2Task;

volatile uint8_t serialBuffer[4];  // for testing
volatile uint8_t bufferIndex = 0;  // for testing

// free rtos task
#define  NUM_REG 40
static uint32_t reg_data[NUM_REG];



#endif  // _FMS_HEADER_H_
