# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"



// Include libraries
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 9 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 10 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 11 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

# 14 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 15 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 16 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 17 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 18 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 19 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

// Project details




// Device details
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// WiFi configuration



// MQTT configuration
# 47 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// Web server configuration


// SD card pins configuration (check your board pin layout spi communication)






// Time configuration




// SD card file configuration





// Global objects
uart_t * fms_cli_uart;
Preferences fms_nvs_storage;
WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

bool wifi_start_event = true;

// System configuration structure
struct SYSCFG {
    unsigned long bootcount;
    unsigned long version;
    char wifi_ssid[32] = "";
    char wifi_password[64] = "";
    char* mqtt_server_host = " " /* mqtt server address*/;
    char* mqtt_user = " " /* mqtt user*/;
    char* mqtt_password = " " /* mqtt password*/;
    uint32_t mqtt_port = 1883 /* mqtt port*/;
    char* mqtt_device_id = "fms_001" /* device id*/ /* mqtt device id*/;
    char* mqtt_lwt_status[20];
    char* device_id = "fms_001" /* device id*/;
    uint32_t station_id = 1 /* station id*/;
} sysCfg;

/*

* fms command cli setting

*/
# 96 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// Command list
# 107 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
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

// command table
const struct COMMAND {
    const char* name;
    void (*function)();
} Commands[] = {
    {"wifi", fms_CmndWifi},
    {"restart", fms_CmndRestart},
    {"wifiscan", fms_CmndWifiScan},
    {"mqtt", fms_CmndMqtt},
    {"wifiread", fms_CmndWifiRead},
    {"bootcount", fms_CmndBootCount},
    {"devid", fms_CmndAddDeviceId},
    {"debug on", fms_CmndDebug},
    {"stgcheck" /* read nvs storage*/,fms_CmndStroagecheck}
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
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

bool fms_debug_log_printf(const char *line,...) { // debug log
  byte loglevel = 1;
  if (true) {
    if (loglevel <= seriallog_level) log_printf /* in build in chip-debug-report.cpp*/(line);
  }
  return true;
}

bool fms_cli_serial_printf(const char *line,...) { // uart log
  byte loglevel = 1;
  if (true) {
    if (loglevel <= seriallog_level) Serial0 /* cli serial port*/.print(line);
  }
  return true;
}

bool fms_chip_info_log(){
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
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino" 3 4
                                                                __null
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
                                                                    );
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
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
# 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 2

int app_cpu = 0;


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




}

bool initialize_uart_cli() {
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    fms_debug_log_printf("[FMSCLI] setup finish for cli uart\n\r");
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







}

void log_debug_info() {




}

void setup() {
  log_chip_info();
 if(initialize_uart_cli()) fms_debug_log_printf(" [FMSCLI] uart1 cli.. started\n\r");
  initialize_nvs_storage(); // save boot count to eeprom 
  fms_debug_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
  if(initialize_wifi()) fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
  run_sd_test();
  fms_debug_log_printf("Start initializing task \n\r");
  fms_task_create(); // rtos task create 
  log_debug_info();
}

void loop() {
  // user main code here
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  while(1){

    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino" 3
              1000 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
              ) / ( TickType_t ) 1000U ) ));

  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"

/*

  * fms_sd.cpp

  *

  *  Created on: 2020. 12. 10.

  *   author : thet htar khaing

*/
# 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
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
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
    //rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino" 3
              1000 
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
              ) / ( TickType_t ) 1000U ) ));
    //write_data_sd("HELLO\n\r");
    //
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
bool fms_task_create() {

  BaseType_t sd_rc;
  sd_rc = xTaskCreatePinnedToCore(
    sd_task, // Task function
    "sdcard", // Name
    3000, // Stack size
    nullptr, // Parameters
    2, // Priority
    &hsdCardTask, // Handle
    app_cpu // CPU
  );
  
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 sd_rc == ( ( ( BaseType_t ) 1 ) )
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 13, __PRETTY_FUNCTION__, 
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "sd_rc == pdPASS"
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                        ;
  if(sd_rc != ( ( ( BaseType_t ) 1 ) )){
    fms_debug_log_printf("sd task created fail");
  }
  BaseType_t wifi_rc;
  wifi_rc = xTaskCreatePinnedToCore(
    wifi_task, // Task function
    "wifi", // Name
    3000, // Stack size
    nullptr, // Parameters
    3, // Priority
    &hwifiTask, // Handle
    app_cpu // CPU
  );
  
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 wifi_rc == ( ( ( BaseType_t ) 1 ) )
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 27, __PRETTY_FUNCTION__, 
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "wifi_rc == pdPASS"
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 27 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                          ;
  BaseType_t mqtt_rc;
  mqtt_rc = xTaskCreatePinnedToCore(
    mqtt_task, // Task function
    "mqtt", // Name
    3000, // Stack size
    nullptr, // Parameters
    3, // Priority
    &hmqttTask, // Handle
    app_cpu // CPU
  );
  
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 mqtt_rc == ( ( ( BaseType_t ) 1 ) )
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 38, __PRETTY_FUNCTION__, 
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "mqtt_rc == pdPASS"
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 38 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                          ;

  BaseType_t cli_rc;
  cli_rc = xTaskCreatePinnedToCore(
    cli_task, // Task function
    "cli", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hcliTask, // Handle
    app_cpu // CPU
  );
  
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 cli_rc == ( ( ( BaseType_t ) 1 ) )
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 50, __PRETTY_FUNCTION__, 
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "cli_rc == pdPASS"
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                         ;
  BaseType_t webserver_rc;
  webserver_rc = xTaskCreatePinnedToCore(
    web_server_task, // Task function
    "webserver", // Name
    3000, // Stack size
    nullptr, // Parameters
    4, // Priority
    &hwebServerTask, // Handle
    app_cpu // CPU
  );
  
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 webserver_rc == ( ( ( BaseType_t ) 1 ) )
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 61, __PRETTY_FUNCTION__, 
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "webserver_rc == pdPASS"
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 61 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                               ;

  return true;
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"

void __attribute__((section(".iram1" "." "0"))) serialEvent2() {
  while (Serial0.available()) { // test code 
    uint8_t data = Serial0.read();
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
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
// Last update: 2021-08-29 20:00:00
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    Serial0 /* cli serial port*/.begin(baudrate);
    if(Serial0 /* cli serial port*/){
      fms_debug_log_printf("[FMSCLI] UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3
                1000 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
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
  nvs_get_stats(
# 26 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3 4
               __null
# 26 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
                   ,&nvs_stats);
  size_t freeHeap = esp_get_free_heap_size();
  UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(
# 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3 4
                                                              __null
# 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
                                                                  );
  Serial0 /* cli serial port*/.printf("{\"total\":%d,\"used\":%d,\"free\":%d,\"free_heap\":%u,\"stack_high_water_mark\":%u}\n",
  nvs_stats.total_entries, nvs_stats.used_entries, nvs_stats.free_entries,freeHeap, stackHighWaterMark);
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
    if(true) {
      Serial0 /* cli serial port*/.printf("[FMICLI] WIFI SSID : %s\n", String(sysCfg.wifi_ssid));
      Serial0 /* cli serial port*/.printf("[FMICLI] WIFI PASSWORD : %s\n", String(sysCfg.wifi_password));
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


void fms_CmndWifiScan() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(); // Disconnect from any network
    char buffer[512]; // Buffer for JSON output
    strcpy(buffer, "{\"wifiscan\":true,\"networks\":[");
    int bufferLen = strlen(buffer);
    int networkIndex = 0;
    for (int scanNum = 0; scanNum < 1 /* Number of scan iterations*/; scanNum++) {
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
        vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 83 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3
                  1000 
# 83 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
                  ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating // similar delay(1000)
    }
    WiFi.scanDelete(); // Free memory
    strcat(buffer, "]}"); // Close JSON array
    if(true) Serial0 /* cli serial port*/.println(buffer); // Output JSON result
}

void fms_CmndRestart() {
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 2000 ) * ( TickType_t ) 
# 91 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3
            1000 
# 91 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
            ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
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
  if(true) Serial0 /* cli serial port*/.println(String(entry).c_str());
  fms_response_cmnd_handler("Read OK");
  } else {
    fms_response_cmnd_handler("false");
  }
}

void fms_CmndMqtt() {
  fms_response_cmnd_handler("mqttcommand");
}

void fms_response_cmnd_handler(const char* result){
  if(true) {
    Serial0 /* cli serial port*/.print(((reinterpret_cast<const __FlashStringHelper *>(("{\"")))));
    Serial0 /* cli serial port*/.print(fmsMailBox.command);
    Serial0 /* cli serial port*/.print(((reinterpret_cast<const __FlashStringHelper *>(("\":\"")))));
    Serial0 /* cli serial port*/.print(result);
    Serial0 /* cli serial port*/.print(((reinterpret_cast<const __FlashStringHelper *>(("\"}\n")))));
  }
}

void __attribute__((section(".iram1" "." "1"))) serialEvent() {
  char c;
  char buffer[32]; // for testing
  while (Serial0 /* cli serial port*/.available()) {
    yield();
    String cmdLine = Serial0 /* cli serial port*/.readStringUntil('\n');
    if(true) Serial0 /* cli serial port*/.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
    cmdLine.trim(); // Remove leading and trailing whitespace from this command line
    int spaceIndex = cmdLine.indexOf(' ');
    if(spaceIndex == -1){
      fmsMailBox.command = cmdLine;
      if(true) Serial0 /* cli serial port*/.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
      fmsMailBox.data = "";
    }else {
      fmsMailBox.command = cmdLine.substring(0, spaceIndex);
      fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
      if(true) Serial0 /* cli serial port*/.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
    }
    fmsMailBox.data_len = fmsMailBox.data.length();
     for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
    if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
      Commands[i].function();
      return;
    }
  }
    if(true) Serial0 /* cli serial port*/.printf("[FMSCLI] Command not found\n\r");
  }
}

static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {

    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 159 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3
              1000 
# 159 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
  BaseType_t rc;
  for (;;) {


    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino" 3
              1000 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"

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

    Serial0 /* cli serial port*/.print("[DEBUG WiFi] SSID : ");
    Serial0 /* cli serial port*/.println(String(sysCfg.wifi_ssid).c_str());
    Serial0 /* cli serial port*/.print("[DEBUG WiFi] PASS : ");
    Serial0 /* cli serial port*/.println(String(sysCfg.wifi_password).c_str());


    WiFi.mode(WIFI_MODE_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      if(true) Serial0 /* cli serial port*/.println(".");
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino" 3
                1000 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
      return false;
    }
    return true;
  }
  }

uint8_t count = 1;
static void wifi_task(void *arg) {
  BaseType_t rc;
  while(1) {
   if(WiFi.status() != WL_CONNECTED){
    fms_debug_log_printf("[WiFi] retry .. connecting\n\r");
   }
   else fms_debug_log_printf("[WiFi] wifi .. connected\n\r");

    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 49 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino" 3
              1000 
# 49 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
              ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
  }
}
