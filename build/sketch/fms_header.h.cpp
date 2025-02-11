#include <Arduino.h>
#line 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_log_printf(const char *line,...);
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_cli_serial_printf(const char *line,...);
#line 21 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_chip_info_log();
#line 26 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_print_after_setup_info();
#line 31 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
bool fms_memory_info_log();
#line 41 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
void fms_log_task_list();
#line 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void initialize_uart();
#line 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void initialize_nvs_storage();
#line 22 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void setup();
#line 52 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
void loop();
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg);
#line 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool fms_config_load_sd_test();
#line 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
bool write_data_sd(char* input);
#line 25 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
static void sd_task(void *arg);
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool fms_task_create();
#line 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
bool fms_uart_cli_begin(bool flag, int baudrate);
#line 95 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
void fms_response_cmnd_handler(const char* result);
#line 134 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
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

// Project details
#define PROJECT                             "fms"                   // fuel management system
#define VERSION                             "0.1.0"                 // version number
#define CLI_PASSWORD                        "admin"                 // cli password // change this password

// Device details
#define DEVICE_ID                           "fms_001"               // device id
#define STATION_ID                          1                       // station id
#define SHOW_SYS_LOG                        true    
#define SHOW_SD_TEST_LOG                    false  
#define SHOW_FMS_CHIP_INFO_LOG              false
#define SHOW_UART_SYS_LOG                   true                    // show uart log
#define SHOW_RESP_UART_SYS_LOG              true
// WiFi configuration
#define WIFI_SSID                           sysCfg.wifi_ssid                     // wifi ssid
#define WIFI_PASSWORD                       sysCfg.wifi_password                     // wifi password

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
#define SD_CARD_CONFIG_FILE_NAME            "fms_config.txt"        // sd card file name change it to your file name

#define _log_printf                         log_printf              // in build in chip-debug-report.cpp
#define fms_cli_serial                      Serial                  // cli serial port

// Global objects
uart_t * fms_cli_uart;
Preferences fms_nvs_storage;
WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

bool wifi_start_event                   =   true;
// System configuration structure
struct SYSCFG {
    unsigned long       bootcount;
    unsigned long       version;

    char               wifi_ssid[32]               = "";
    char               wifi_password[64]           = "";

    char*               mqtt_server_host        = MQTT_SERVER;
    char*               mqtt_user               = MQTT_USER;
    char*               mqtt_password           = MQTT_PASSWORD;
    uint32_t            mqtt_port               = MQTT_PORT;

    char*               mqtt_device_id          = MQTT_DEVICE_ID;
    char*               mqtt_lwt_status[20];     
    char*               device_id               = DEVICE_ID;
    uint32_t            station_id              = STATION_ID;

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
    {D_CMD_DEVICEID,  fms_CmndAddDeviceId}
};


static void wifi_task(void *arg);
bool fms_wifi_init();


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

bool fms_log_printf(const char *line,...) { // debug log
  byte loglevel = 1;
  if (SHOW_SYS_LOG) {
    if (loglevel <= seriallog_level) _log_printf(line);
  }
  return true;
}

bool fms_cli_serial_printf(const char *line,...) { // uart log
  byte loglevel = 1;
  if (SHOW_UART_SYS_LOG) {
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
    fms_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    // Check stack size of the current task
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    fms_log_printf("Stack High Water Mark: %u bytes\n", stackHighWaterMark);
  return true;
}

void fms_log_task_list() {
  char buffer[512]; // Buffer for task list output
  fms_log_printf("Task List:\n");
  fms_log_printf("Name          State       Prio      Stack        Num \n\r");
  vTaskList(buffer);
  fms_log_printf(buffer);
}


#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
#include "fms_header.h"

int app_cpu = 0;
#define chip_report_printf log_printf // for chip info debug

void initialize_uart() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_log_printf("setup finish for cli uart\n\r");
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
  #if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
 #endif

 initialize_uart();
 initialize_nvs_storage(); // save boot count to eeprom 
 fms_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
 initialize_fms_wifi(wifi_start_event); // wifi connection
  /*
  user main code here
  */
  #if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    fms_log_printf("\n\r==================== sd card test success================\n");
  } else {
    fms_log_printf("sd card test failed\n");
  }
  #endif

  fms_log_printf("Start initiazling task \n\r");
  fms_task_create(); // rtos task create 

#if SHOW_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
#endif
}

void loop() {
  BaseType_t rc;
  //rc = xTaskNotify(heventTask, 0b0001, eSetBits);
  //assert(rc == pdPASS);
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  while(1){
 // low 
  
    //rc = xTaskNotify(heventTask, 5, eSetBits);
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
bool  fms_task_create() {
  
  BaseType_t rc;
  //   rc = xTaskCreatePinnedToCore(
  //   event_receive,       // Task function
  //   "event_receive",      // Name
  //   3000,               // Stack size
  //   nullptr,       // Parameters
  //   1,             // Priority
  //   &heventTask,  // Handle
  //   app_cpu        // CPU
  // );
  // assert(rc == pdPASS);

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

void IRAM_ATTR serialEvent2() {
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
// Last update: 2021-08-29 20:00:00
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    fms_cli_serial.begin(baudrate);
    fms_log_printf("UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
    return true;
  }
  return true;
}

void fms_CmndAddDeviceId() {

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
    if(SHOW_UART_SYS_LOG) {
      fms_cli_serial.printf("WIFI SSID : %s\n", String(sysCfg.wifi_ssid).c_str());
      fms_cli_serial.printf("WIFI PASSWORD : %s\n", String(sysCfg.wifi_password).c_str());
    }
    fms_response_cmnd_handler("true");
    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for 1 second  // similar delay(1000)
    wifi_start_event = true;
  } else {
    fms_response_cmnd_handler("Invalid format. Use: wifi \"your_ssid\" \"your_password\"");
  }
}

#define SCAN_COUNT 1                 // Number of scan iterations
void fms_CmndWifiScan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                // Disconnect from any network
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
    fms_cli_serial.println(buffer); // Output JSON result
}

void fms_CmndRestart() {
  vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for 1 second before repeating
  fms_log_printf("Restarting...\n");
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

void IRAM_ATTR serialEvent() {  
  char c;
  char buffer[32]; // for testing
  while (fms_cli_serial.available()) {
    yield();
    String cmdLine = fms_cli_serial.readStringUntil('\n'); 
    if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Received : %s\n\r", cmdLine.c_str());
    cmdLine.trim(); // Remove leading and trailing whitespace from this command line
    int spaceIndex = cmdLine.indexOf(' ');
    if(spaceIndex == -1){
      fmsMailBox.command = cmdLine;
      if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Received : %s\n\r", cmdLine.c_str());
      fmsMailBox.data = "";
    }else {
      fmsMailBox.command = cmdLine.substring(0, spaceIndex);
      fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
      if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
    }
    fmsMailBox.data_len = fmsMailBox.data.length();
     for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
    if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
      Commands[i].function();
      return;
    }
  }
    if(SHOW_UART_SYS_LOG) fms_cli_serial.printf("Command not found\n");
  }
}

static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    fms_log_printf("uart cli is running\n\r");
    //rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
  BaseType_t rc;
  for (;;) {
   
    //rc = xTaskNotify(heventTask, 6, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"

bool initialize_fms_wifi(bool flag) {
  if(flag)
  {
    fms_log_printf("Connecting to WiFi...");
    fms_cli_serial.print("SSID : ");
    fms_cli_serial.println(sysCfg.wifi_ssid);
    fms_cli_serial.print("PASS : ");
    fms_cli_serial.println(sysCfg.wifi_password);
   // fms_log_printf("SSID: %s , PASS: %s",sysCfg.wifi_ssid,sysCfg.wifi_password);
    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") fms_log_printf("wifi credential value is empty");
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    fms_cli_serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      fms_cli_serial.println(".");
      return false;
    }
    return true;
  }
  }


static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   
    //rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}

