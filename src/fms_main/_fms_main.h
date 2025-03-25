#ifndef _FMS_MAIN_H_
#define _FMS_MAIN_H_

// Include libraries

#include <WebServer.h>
// #include <PubSubClient.h>
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

String _firmware_version = "0.1.0";  // Current firmware version
String deviceName = "ultramarine-v0.1-";
Ticker ticker;
#define PROJECT "fms"         // fuel management system
#define CLI_PASSWORD "admin"  // cli password // change this password
#define BUILTIN_LED 2         //
// Device details
#define DEVICE_ID "fms_001"  // device id
#define STATION_ID 1         // station id
#define SHOW_DEBUG_SYS_LOG true
#define SHOW_DEBUG_SD_TEST_LOG true
#define SHOW_DEBUG_FMS_CHIP_INFO_LOG false
#define SHOW_UART_SYS_LOG true  // show uart log
#define SHOW_RESP_UART_SYS_LOG true

// WiFi configuration
#define WIFI_SSID sysCfg.wifi_ssid          // wifi ssid
#define WIFI_PASSWORD sysCfg.wifi_password  // wifi password

// MQTT configuration
#define MQTT_SERVER sysCfg.mqtt_server_host  // mqtt server address
#define MQTT_PORT 1883                       // mqtt port
#define MQTT_USER " "                        // mqtt user
#define MQTT_PASSWORD " "                    // mqtt password
#define MQTT_DEVICE_ID DEVICE_ID             // mqtt device id
#define MQTT_LWT_OFFLINE "offline"           // mqtt last will topic offline
#define MQTT_LWT_ONLINE "online"             // mqtt last will topic online
#define mqttTask true
// Web server configuration
#define WEB_SERVER_PORT 80  // web server port

// SD card pins configuration (check your board pin layout spi communication)
#define SD_CARD_CS_PIN 5           // sd card chip select pin
#define SD_CARD_MOSI_PIN 23        // sd card mosi pin
#define SD_CARD_MISO_PIN 19        // sd card miso pin
#define SD_CARD_SCK_PIN 18         // sd card sck pin
#define SD_CARD_SPI_SPEED 4000000  // sd card spi speed

// Time configuration
#define NTP_SERVER "pool.ntp.org"  // ntp server
#define GMT_OFFSET_SEC (7 * 3600)  // gmt offset in seconds to fix time zone
#define DAYLIGHT_OFFSET_SEC 0      // daylight offset in seconds to fix time zone

// SD card file configuration
#define SD_CARD_CONFIG_FILE_NAME "fms_config.txt"  // sd card file name change it to your file name

#define _log_printf log_printf    // in build in chip-debug-report.cpp
#define fms_cli_serial Serial     // cli serial port
#define fms_uart2_serial Serial1  // uart2 serial port


// mqtt topic config protocol

//mqtt
char pumpapprobuf[22] = "detpos/local_server/1";
char pumpreqbuf[23] = "detpos/device/permit/1";
char pumppresetbuf[28] = "detpos/local_server/preset";  // return from local server
char pplive[25] = "detpos/device/livedata/1";
char ppfinal[22] = "detpos/device/Final/1";
char whreqbuf[20] = "detpos/device/whreq";
char pricechange[26] = "detpos/local_server/price";  // return from local server
char pricereqbuf[25] = "detpos/device/pricereq/1";
char activebuf[23] = "detpos/device/active/1";
char devicebuf[2] = "1";
char device_Id_topic[40] = "detpos/local_server/initial1/det/0A0000";  // return from local server
char Reset_topic[17] = "detpos/hmi/reset";
char reload_topic[29] = "detpos/local_server/reload/1";  // return from local server

// mqtt end


// Global objects
uart_t* fms_cli_uart;
Preferences fms_nvs_storage;
WiFiClient wf_client;

// HTTP client for OTA updates
HTTPClient http;
WiFiClient http_client;

PubSubClient fms_mqtt_client(wf_client);

bool wifi_start_event = true;

// System configuration structure
struct SYSCFG {
  unsigned long bootcount;
  unsigned long version;
  char wifi_ssid[32] = "";
  char wifi_password[64] = "";
  char mqtt_server_host[32] = "192.168.1.142";
  char* mqtt_user = MQTT_USER;
  char* mqtt_password = MQTT_PASSWORD;
  uint32_t mqtt_port = MQTT_PORT;
  char* mqtt_device_id = MQTT_DEVICE_ID;
  char* mqtt_lwt_status[20];
  char* device_id = DEVICE_ID;
  uint32_t station_id = STATION_ID;
} sysCfg;

/*
* fms command cli setting
*/

// Command list
#define D_CMND_WIFI "wifi"
#define D_CMND_RESTART "restart"
#define D_CMND_WIFISCAN "wifiscan"
#define D_CMND_MQTT "mqtt"
#define D_CMND_WIFIREAD "wifiread"
#define D_CMND_BOOTCOUNT "bootcount"
#define D_CMD_DEVICEID "devid"
#define D_CMD_DEBUG "debug on"
#define D_CMD_NVS_STORAGE "stgcheck"  // read nvs storage

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
//void fms_Cmndhelp();

// command table
//const struct COMMAND {
//    const char* name;
//    void (*function)();
//} Commands[] = {
//    {D_CMND_WIFI, fms_CmndWifi},
//    {D_CMND_RESTART, fms_CmndRestart},
//    {D_CMND_WIFISCAN, fms_CmndWifiScan},
//    {D_CMND_MQTT, fms_CmndMqtt},
//    {D_CMND_WIFIREAD, fms_CmndWifiRead},
//    {D_CMND_BOOTCOUNT, fms_CmndBootCount},
//    {D_CMD_DEVICEID,  fms_CmndAddDeviceId},
//    {D_CMD_DEBUG, fms_CmndDebug},
//    {D_CMD_NVS_STORAGE,fms_CmndStroagecheck},
//    {"help",fms_Cmndhelp}
//};

Ticker wifi_ticker;
static void wifi_task(void* arg);
bool fms_wifi_init();
bool wifi_led_ticker();
// mqtt
static void mqtt_task(void* arg);
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length);

// RTOS task handles
static TaskHandle_t heventTask;
static TaskHandle_t hwifiTask;
static TaskHandle_t hmqttTask;
static TaskHandle_t hsdCardTask;
static TaskHandle_t hwebServerTask;
static TaskHandle_t hspiTask;
static TaskHandle_t hcliTask;
static TaskHandle_t huart2Task;

// Mutex for serial communication
SemaphoreHandle_t serialMutex;

// Serial buffer and index
volatile uint8_t serialBuffer[4];  // for testing
volatile uint8_t bufferIndex = 0;  // for testing

// UART command flag
bool use_uart_command = true;
bool use_serial1 = true;

// Function declarations
void addLog(byte loglevel, const char* line);
int app_cpu = 0;
/* FMS CLI */
//#define FM_CLI_TEST // for cli terminal command test
Preferences preferences;
WebServer server(80);
// Flag to indicate if test mode is active
bool testModeActive = false;
// Buffer to store serial output for web interface
String serialOutputBuffer;
const size_t MAX_BUFFER_SIZE = 4096;  // change your buzzer size
// WiFi connection timeout in milliseconds
const unsigned long WIFI_TIMEOUT = 20000;  // 20 mill sec

const char* OTA_SERVER = "http://192.168.1.142:3003/firmware.json";
// fms ota
unsigned long currentMillis = 0;
unsigned long ota_previousMillis = 0;
String firmwareVersion = "1.0.0";  // Current firmware version
String _firmware_v = "";           // Latest firmware version from server
bool _published = false;           // Is the update published?
String _ota_url = "";              // URL to download the firmware
bool _ota_update = false;          // Should we update?
bool _update_in_progress = false;  // Is update in progress?
int _update_progress = 0;          // Update progress (0-100)
String _update_status = "idle";    // Update status (idle, checking, updating, success, error)
String _update_error = "";         // Error message if update fails
#define LED_BUILTIN 16
// LED pins
const int LED_ONE = LED_BUILTIN;  // Built-in LED (inverted logic)
const int LED_TWO = 2;            // Status LED

/* Function Prototypes */
void initialize_system();
bool initialize_uart_cli();
bool initialize_uart2();
bool initialize_wifi();
void run_sd_test();
void initialize_nvs_storage();
void log_chip_info();
#define chip_report_printf log_printf  // for chip info debug
//#define fms_debug_log_printf log_printf // for fms debug log

#endif  // _FMS_HEADER_H_
