#include <Arduino.h>
#line 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_log_printf(const char *line,...);
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_chip_info_log();
#line 18 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_print_after_setup_info();
#line 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_memory_info_log();
#line 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_task_usage_check();
#line 3 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void event_receive(void *arg);
#line 17 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void initialize_uart();
#line 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void initialize_nvs_storage();
#line 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void setup();
#line 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void loop();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg);
#line 4 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool fms_config_load_sd_test();
#line 16 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool test_sd_init();
#line 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool test_read();
#line 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool fms_config_load_sd();
#line 56 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool write_data_sd(char* input);
#line 82 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
static void sd_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool fms_task_create();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
static void cli_task(void *arg);
#line 11 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
bool fms_uart_cli_begin(bool flag, int baudrate);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
static void wifi_task(void *arg);
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
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

bool fms_log_printf(const char *line,...) {
  byte loglevel = 1;
  if (SHOW_SYS_LOG) {
    if (loglevel <= seriallog_level) _log_printf(line);
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
    fms_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    // Check stack size of the current task
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    fms_log_printf("Stack High Water Mark: %u bytes\n", stackHighWaterMark);
  return true;
}

bool fms_task_usage_check(){
  char taskBuffer[256];
  fms_log_printf("Task Name\tPriority\tState\tStack High Water Mark\n");
  //vTaskList(taskBuffer);
  //fms_log_printf("\n%s\n",taskBuffer); // fix this error over flow
  
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
#include "fms_header.h"

void event_receive(void *arg) {
  uint32_t rv;
  BaseType_t rc;
  for (;;) {
    rc = xTaskNotifyWait(0, 0xFFFFFFFF, &rv, portMAX_DELAY);
    if (rc == pdTRUE) {
      // Handle the event here
    }
  }
}

int app_cpu = 0;
#define chip_report_printf log_printf

void initialize_uart() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_log_printf("uart cli begin\n");
  }
}

void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  fms_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage
}

void setup() {
  fms_chip_info_log();
  fms_memory_info_log();
  fms_log_printf("CPU %d: Setup", app_cpu);

  initialize_uart();
  initialize_nvs_storage();

  serialMutex = xSemaphoreCreateMutex();
  assert(serialMutex != NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS); // wait delay 1 second

  #if SHOW_SD_TEST_LOG
  if(fms_config_load_sd_test()){fms_log_printf("\n\r==================== sd card test success================\n");}
  else {fms_log_printf("sd card test failed\n");}
  #endif

  fms_log_printf("initializing task");
  fms_task_create(); // rtos task create 

  fms_print_after_setup_info();
 // fms_task_usage_check(); 
}

void loop() {
  BaseType_t rc;
  rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  assert(rc == pdPASS);
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  while(1){
 // low 
    printf("mqtt task started \n");

    rc = xTaskNotify(heventTask, 5, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));

  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"



bool fms_config_load_sd_test() {
if(!write_data_sd("134")) {
  return false;
}
if(!fms_config_load_sd()){
  return false;
}
return true;
}



bool test_sd_init()
{
  fms_log_printf("initializing sd card\n\r");
  if (!LittleFS.begin(true)) {
    fms_log_printf("SD Card Mount Failed\n\r");
    return false;
  }
  fms_log_printf("SD Card Mount Success\n\r");
  return true;
}


bool test_read() {
  fms_log_printf("testing read\n\r");
  test_sd_init();

  File file = LittleFS.open("/example.txt",FILE_READ);
  if (!file) {
    fms_log_printf("Failed to open file for reading\n\r");
    return false;
  }
  fms_log_printf("File Content:");
  while (file.available()) {
    fms_log_printf("%s", file.read());
  }
  file.close();
  return true;
}

bool fms_config_load_sd() {
  fms_log_printf("loading config data from sd cardn\n\r");
  if (!test_read()) {
    fms_log_printf("failed to read data from sd card\n\r");
    return false;
  }
  return true;
}



bool write_data_sd(char* input)
{
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)

  fms_log_printf("writing data to sd card\n\r");
  test_sd_init();
  File file = LittleFS.open("/example.txt", FILE_WRITE);
  if (!file) {
    fms_log_printf("Failed to open file for writing\n\r");
    return false;
  }
  if (file.write((const uint8_t*)input,sizeof(input)-1)) {
    fms_log_printf("File written\n\r");
    return true;
  } else {
    fms_log_printf("Write failed\n\r");
    return false;
  }

  return true;
}


static void sd_task(void *arg) {
  BaseType_t rc;
  while(1) {
    printf("sd task started \n");
    /*
    * Load config data from sd card
    */

    rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}


#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool  fms_task_create() {
  
  BaseType_t rc;
    rc = xTaskCreatePinnedToCore(
    event_receive,       // Task function
    "event_receive",      // Name
    3000,               // Stack size
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
    2,             // Priority
    &hsdCardTask,  // Handle
    app_cpu        // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    wifi_task,   // Task function
    "wifi",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
    &hwifiTask,  // Handle
    app_cpu      // CPU
  );
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    mqtt_task,   // Task function
    "mqtt",      // Name
    3000,        // Stack size
    nullptr,     // Parameters
    3,           // Priority
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
    4,                // Priority
    &hwebServerTask,  // Handle
    app_cpu           // CPU
  );
  assert(rc == pdPASS);

  return true;
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
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   fms_log_printf("uart cli is running");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


bool fms_uart_cli_begin(bool flag, int baudrate) {
  if(flag){
     uart_t *uart = uartBegin(UART_NUM_0, baudrate, SERIAL_8N1, 3, 1, 256, 0, false, 112);
     if(uart == NULL){
      fms_log_printf("cli uart begin fail");
      return false;
     }
     else{
       fms_log_printf("cli uart begin success");
       return true;
     }
  }

}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
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

