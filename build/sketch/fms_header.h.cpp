#include <Arduino.h>
#line 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_debug_log_printf(const char *line,...);
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_cli_serial_printf(const char *line,...);
#line 21 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_chip_info_log();
#line 26 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_print_after_setup_info();
#line 31 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_memory_info_log();
#line 40 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
void fms_log_task_list();
#line 15 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void setup();
#line 19 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void loop();
#line 11 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
void initialize_nvs_storage();
#line 21 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
void log_chip_info();
#line 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
bool initialize_uart_cli();
#line 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
bool initialize_uart2();
#line 48 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
bool initialize_wifi();
#line 59 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
void run_sd_test();
#line 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
void log_debug_info();
#line 76 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
void fms_pin_mode(int pin, int mode);
#line 10 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
void fms_mqtt_reconnect();
#line 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool fms_config_load_sd_test();
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool write_data_sd(char* input);
#line 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
static void sd_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc);
#line 19 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool fms_task_create();
#line 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate);
#line 17 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"
void UART2_RX_IRQ();
#line 31 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"
void fms_uart2_decode(uint8_t* data, uint32_t len);
#line 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"
void fms_uart2_task(void *arg);
#line 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
bool fms_uart_cli_begin(bool flag, int baudrate);
#line 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
void fms_Cmndhelp();
#line 126 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
void fms_response_cmnd_handler(const char* result);
#line 136 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
void fms_cli_command_decode(String cmdLine);
#line 162 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
void UART_RX_IRQ();
#line 174 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
static void cli_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg);
#line 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag);
#line 0 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
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
#include <ModbusMaster.h>
#include <math.h>


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
#define fms_uart2_serial                    Serial1                 // uart2 serial port

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
//void fms_Cmndhelp();

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
static TaskHandle_t huart2Task;

// Mutex for serial communication
SemaphoreHandle_t serialMutex;

// Serial buffer and index
volatile uint8_t serialBuffer[4]; // for testing
volatile uint8_t bufferIndex = 0; // for testing

// UART command flag
bool use_uart_command = true;
bool use_serial1 = true;

// Function declarations
void addLog(byte loglevel, const char *line);

#endif // _FMS_HEADER_H_

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

bool fms_debug_log_printf(const char *line,...) { // debug log
  byte loglevel = 1;
  if (SHOW_DEBUG_SYS_LOG) {
    if (loglevel <= seriallog_level) _log_printf(line);
  }
  return true;
}

bool fms_cli_serial_printf(const char *line,...) { // uart log
  byte loglevel = 1;
  if (SHOW_RESP_UART_SYS_LOG) {
    if (loglevel <= seriallog_level) fms_cli_serial.print(line);
  }
  return true;
}

bool  fms_chip_info_log(){
  printBeforeSetupInfo();
  return true;
}

bool fms_print_after_setup_info(){
  printAfterSetupInfo();
  return true;
}

bool fms_memory_info_log(){
// Check free heap size
    size_t freeHeap = esp_get_free_heap_size();
    fms_debug_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    fms_debug_log_printf("Stack High Water Mark: %u bytes\n", stackHighWaterMark);
  return true;
}

void fms_log_task_list() {
  char buffer[512]; // Buffer for task list output
  fms_debug_log_printf("Task List:\n");
  fms_debug_log_printf("Name          State       Prio      Stack        Num \n\r");
  vTaskList(buffer);
  fms_debug_log_printf(buffer);
}


#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
/*
  *  FMS main source file
  *  Author: Trion
  *  Date: 2025
  *  Guided By Senior Engineer : Sir Thiha Kyaw
  *  Description: This file is the main source file for FMS project.
*/

#include "fms_header.h"

/* Function Prototypes */
void initialize_system();

/* Main function */
void setup() {
  initialize_system();
}

void loop() {
  // user main code here
}

/* Function Definitions */
void initialize_system() {
  // Initialize CLI UART and UART2 for RS485
  if (initialize_uart_cli()) {
    fms_debug_log_printf(" [FMSCLI] UART1.. DONE\n\r");
  }
  if (initialize_uart2()) {
    fms_debug_log_printf(" [FMSUART2] UART2.. DONE\n\r");
  }

  // Initialize GPIO pin
  fms_pin_mode(2, OUTPUT);

  // Read configuration data from SD card
  initialize_nvs_storage(); // Save boot count to NVS storage
  fms_debug_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);

  // Initialize WiFi
  if (initialize_wifi()) {
    fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
  }

  // Run SD card test
  run_sd_test();
  fms_debug_log_printf("Start initializing task \n\r");

  // Initialize FreeRTOS scheduler
  fms_task_create(); // RTOS task create
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main_func.ino"
/*
    * fms_main_func.ino
    * 
    * This file is part of the ESP32 FMS 
    * main function link file
*/

int app_cpu = 0;
#define chip_report_printf log_printf // for chip info debug

void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  fms_debug_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage
}

void log_chip_info() {
  #if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
  #endif
}

bool initialize_uart_cli() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_debug_log_printf("[FMSCLI] setup finish for cli uart\n\r");
    fms_cli_serial.onReceive(UART_RX_IRQ); // uart interrupt function 
    return true;
  } else {
    return false;
  }
}

bool initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 115200)) {
    fms_debug_log_printf("[FMSUART2] setup finish for uart2\n\r");
    fms_uart2_serial.onReceive(UART2_RX_IRQ); // uart interrupt function 
    return true;
  } else {
    return false;
  }
}

bool initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
    return true;
  } else {
    fms_debug_log_printf("[WiFi] wifi .. not connected\n");
    
    return false;
  }
}

void run_sd_test() {
  #if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    fms_debug_log_printf("\n\r==================== sd card test success================\n");
  } else {
    fms_debug_log_printf("sd card test failed\n");
  }
  #endif
}

void log_debug_info() {
  #if SHOW_DEBUG_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
  #endif
}

void fms_pin_mode(int pin, int mode) {
  pinMode(pin, mode);
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"

void fms_mqtt_callback(char* topic,byte* payload,unsigned int length){
  fms_debug_log_printf("Message arrived [%c]",topic);
  for (int i = 0; i < length; i++) {
    fms_cli_serial.print((char)payload[i]);
  }
  fms_cli_serial.println();
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()){
    fms_debug_log_printf("[Mqtt] connection .. fail\n\r");
    String clientId = String(DEVICE_ID).c_str();
    clientId += String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str())){
        fms_debug_log_printf("[Mqtt] connected ..");
        fms_mqtt_client.subscribe("fms/test/data");
        /*
        user mqtt topic here
        */
    } else {
      fms_debug_log_printf("[Mqtt] connection .. failed, rc=%d try again in 5 second\n\r",fms_mqtt_client.state());
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

static void mqtt_task(void *arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(MQTT_SERVER,1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while(mqttTask){
    fms_mqtt_client.loop();
    if(!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
    }
    else fms_debug_log_printf("[Mqtt] mqtt .. connected\n\r");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"

/*
  * fms_sd.cpp
  *
  *  Created on: 2020. 12. 10.
  *   author : thet htar khaing
*/
bool fms_config_load_sd_test() {

return true;
}

bool write_data_sd(char* input)
{
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)


  return true;
}

static void sd_task(void *arg) {
  BaseType_t rc;
  while(1) {
  
    /*
    * Load config data from sd card
    */
    
    //rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}


#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc) {
  rc = xTaskCreatePinnedToCore(
    task_func,  // Task function
    name,       // Name
    stack_size, // Stack size
    nullptr,    // Parameters
    priority,   // Priority
    handle,     // Handle
    app_cpu     // CPU
  );
  assert(rc == pdPASS);
  if (rc != pdPASS) {
    fms_debug_log_printf("[TASK] %s task created fail", name);
    return false;
  }
  return true;
}

bool fms_task_create() {
  BaseType_t sd_rc, wifi_rc, mqtt_rc, cli_rc, uart2_rc, webserver_rc;

  if (!create_task(sd_task, "sdcard", 3000, 2, &hsdCardTask, sd_rc)) return false;
  if (!create_task(wifi_task, "wifi", 3000, 3, &hwifiTask, wifi_rc)) return false;
  if (!create_task(mqtt_task, "mqtt", 3000, 3, &hmqttTask, mqtt_rc)) return false;
  if (!create_task(cli_task, "cli", 3000, 1, &hcliTask, cli_rc)) return false;
  if (!create_task(fms_uart2_task, "uart2", 3000, 1, &huart2Task, uart2_rc)) return false;
  if (!create_task(web_server_task, "webserver", 3000, 4, &hwebServerTask, webserver_rc)) return false;

  return true;
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"

bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, 16, 17);
    if(fms_uart2_serial){
      fms_debug_log_printf("[FMSUART2] UART 2  (Baudrate : %d) started successfully\n\r",baudrate);
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      fms_debug_log_printf("[FMSUART2] UART 2  start fail\n\r");
      return false;
    }
  }
}


void UART2_RX_IRQ() { // interrupt RS485/RS232 function
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available(); // serial.available  // #define fms_cli_serial Serial
  fms_uart2_serial.printf("Got byes on serial : %d\n",size);
  while(fms_uart2_serial.available()) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++; 
  }
  fms_uart2_serial.printf("\n uart2  data process \n\r");
  fms_uart2_decode(Buffer, size); // decode uart2 data main function
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  fms_debug_log_printf("[FMSUART2] Received : %s\n\r", data);
}



// free rtos task
void fms_uart2_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
// Last update: 2021-08-29 20:00:00
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    fms_cli_serial.begin(baudrate);
    if(fms_cli_serial){
      fms_debug_log_printf("[FMSCLI] UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      fms_debug_log_printf("[FMSCLI] UART 1 CLI start fail\n\r");
      return false;
    }
  }
}

void fms_CmndDebug(){

}

void fms_CmndAddDeviceId() {

}

void fms_CmndStroagecheck() {
  nvs_stats_t nvs_stats;
  nvs_get_stats(NULL,&nvs_stats);
  size_t freeHeap = esp_get_free_heap_size();
  UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  fms_cli_serial.printf("{\"total\":%d,\"used\":%d,\"free\":%d,\"free_heap\":%u,\"stack_high_water_mark\":%u}\n", 
  nvs_stats.total_entries, nvs_stats.used_entries, nvs_stats.free_entries,freeHeap, stackHighWaterMark);
}

void fms_Cmndhelp() {
    fms_cli_serial.println("+------------------+------------------+");
    fms_cli_serial.println("| Command         | Description       |");
    fms_cli_serial.println("+------------------+------------------+");
    for (const auto& cmd : Commands) {
      fms_cli_serial.printf("| %-16s | %-16s |\n", cmd.name, "Executes Command");
    }
    fms_cli_serial.println("+------------------+------------------+");
}

void fms_CmndBootCount() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  fms_response_cmnd_handler(String(sysCfg.bootcount).c_str());
  fms_nvs_storage.end(); // close nvs storage
}

void fms_CmndWifi() {
 char ssid[32] = "ssid";
 char password[64] = "password";
  if(sscanf(fmsMailBox.data.c_str(),"ssid:\"%31[^\"]\" password:\"%63[^\"]\"", ssid, password) == 2) {
    strncpy(sysCfg.wifi_ssid, ssid, sizeof(sysCfg.wifi_ssid)-1);
    strncpy(sysCfg.wifi_password, password, sizeof(sysCfg.wifi_password)-1);
    if(SHOW_RESP_UART_SYS_LOG) {
      fms_cli_serial.printf("[FMICLI] WIFI SSID : %s\n", String(sysCfg.wifi_ssid));
      fms_cli_serial.printf("[FMICLI] WIFI PASSWORD : %s\n", String(sysCfg.wifi_password));
    }
    fms_nvs_storage.begin("fms_config", false);
    fms_nvs_storage.putString("ssid",sysCfg.wifi_ssid);
    fms_nvs_storage.putString("pass",sysCfg.wifi_password);
    fms_nvs_storage.end();
    fms_response_cmnd_handler("true");
    fms_CmndRestart();
  } else {
    fms_response_cmnd_handler("[FMICLI] Invalid format. Use: wifi \"your_ssid\" \"your_password\"");
  }
}

#define SCAN_COUNT 1                 // Number of scan iterations
void fms_CmndWifiScan() {
  WiFi.mode(WIFI_STA);
 // WiFi.disconnect();                // Disconnect from any network
    char buffer[512];               // Buffer for JSON output
    strcpy(buffer, "{\"wifiscan\":true,\"networks\":[");
    int bufferLen = strlen(buffer);
    int networkIndex = 0;
    for (int scanNum = 0; scanNum < SCAN_COUNT; scanNum++) {
        int numNetworks = WiFi.scanNetworks();
        for (int i = 0; i < numNetworks; i++) {
            if (networkIndex > 0) strcat(buffer, ","); // Add comma for JSON formatting
            char entry[128];
            snprintf(entry, sizeof(entry),
                "{\"Nr\":%d,\"SSID\":\"%s\",\"RSSI\":%d,\"CH\":%d,\"Encryption\":\"%s\"}",
                networkIndex + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                WiFi.channel(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA+WPA2"
            );
            strcat(buffer, entry);
            bufferLen += strlen(entry);
            networkIndex++;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating // similar delay(1000)
    }
    WiFi.scanDelete(); // Free memory
    strcat(buffer, "]}"); // Close JSON array
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(buffer); // Output JSON result
}

void fms_CmndRestart() {
  vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for 1 second before repeating
  fms_debug_log_printf("[DEBUG RST] Restarting...\n\r");
  fms_response_cmnd_handler("true");
  ESP.restart();
}

void fms_CmndWifiRead() {
  if(WiFi.status() == WL_CONNECTED) {
    char entry[128];
    snprintf(entry, sizeof(entry),
        "{\"SSID\":\"%s\",\"RSSI\":%d,\"IP\":\"%s\"}",
        WiFi.SSID().c_str(), WiFi.RSSI(),
        WiFi.localIP().toString().c_str()
    );
  if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(String(entry).c_str());
  fms_response_cmnd_handler("Read OK");
  } else {
    fms_response_cmnd_handler("false");
  }
}

void fms_CmndMqtt() {
  fms_response_cmnd_handler("mqttcommand");
}

void fms_response_cmnd_handler(const char* result){
  if(SHOW_RESP_UART_SYS_LOG) {
    fms_cli_serial.print(F("{\""));
    fms_cli_serial.print(fmsMailBox.command);
    fms_cli_serial.print(F("\":\""));
    fms_cli_serial.print(result);
    fms_cli_serial.print(F("\"}\n"));
  }
}

void  fms_cli_command_decode(String cmdLine) {  
  char c;
  char buffer[32]; // for testing
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
    cmdLine.trim(); // Remove leading and trailing whitespace from this command line
    int spaceIndex = cmdLine.indexOf(' ');
    if(spaceIndex == -1){
      fmsMailBox.command = cmdLine;
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
      fmsMailBox.data = "";
    }else {
      fmsMailBox.command = cmdLine.substring(0, spaceIndex);
      fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
    }
    fmsMailBox.data_len = fmsMailBox.data.length();
     for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
    if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
      Commands[i].function();
      return;
    }
  }
    if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Command not found\n\r");
  
}

void UART_RX_IRQ() { // interrupt function
  String cmd_ ;
  uint16_t size = fms_cli_serial.available(); // serial.available  // #define fms_cli_serial Serial
  fms_cli_serial.printf("Got byes on serial : %d\n",size);
  while(fms_cli_serial.available()) {
    yield();
     cmd_ = fms_cli_serial.readStringUntil('\n'); 
  }
  fms_cli_serial.printf("\n cli terminal data process \n\r");
  fms_cli_command_decode(cmd_);
}

static void cli_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
  BaseType_t rc;
  for (;;) {
   
   
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"

bool initialize_fms_wifi(bool flag) {
  if(flag)
  {
    // get ssid and password from nvs storage 
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    fms_nvs_storage.end();

    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);

    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") 
    {
      fms_debug_log_printf("[DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }

    fms_debug_log_printf("[DEBUG WiFi] wifi .. connecting \n\r");
    #if SHOW_RESP_UART_SYS_LOG
    fms_cli_serial.print("[DEBUG WiFi] SSID : ");
    fms_cli_serial.println(String(sysCfg.wifi_ssid).c_str());
    fms_cli_serial.print("[DEBUG WiFi] PASS : ");
    fms_cli_serial.println(String(sysCfg.wifi_password).c_str());
    #endif

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(".");
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return false;
    }
    return true;
  }
  }

bool wifi_led_ticker() {
  static bool state = false;
  gpio_set_level(GPIO_NUM_2,state);
  state = !state;
}

uint8_t count = 1;
static void wifi_task(void *arg) {
  BaseType_t rc;
  while(1) {
   if(WiFi.status() != WL_CONNECTED){
    fms_debug_log_printf("[WiFi] retry .. connecting\n\r");
    gpio_set_level(GPIO_NUM_2,HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_NUM_2,LOW);
    vTaskDelay(pdMS_TO_TICKS(500));

   }
   else  {
    fms_debug_log_printf("[WiFi] wifi .. connected\n\r");
    gpio_set_level(GPIO_NUM_2,HIGH);
  }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}

