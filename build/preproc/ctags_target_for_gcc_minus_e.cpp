# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
/*

* @file    fms_header.h

*

* @brief   This is a header file for FMS project.

*

* @autho   Trion

*/
# 11 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// Include libraries
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 14 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 15 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 16 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 17 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 18 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 19 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

# 21 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 22 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

// Project details




// Device details




// WiFi configuration



// MQTT configuration
# 48 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// Web server configuration


// SD card pins configuration (check your board pin layout spi communication)






// Time configuration




// SD card file configuration





// Global objects
Preferences fms_nvs_storage;
WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

// System configuration structure
struct SYSCFG {
    unsigned long bootcount;
    unsigned long version;
    char* wifi_ssid = "wifitest" /* wifi ssid*/;
    char* wifi_password = "12345678" /* wifi password*/;

    char* mqtt_server_host = " " /* mqtt server address*/;
    char* mqtt_user = " " /* mqtt user*/;
    char* mqtt_password = " " /* mqtt password*/;
    char* mqtt_port = 1883 /* mqtt port*/; // in datastructure uint32_t

    char* mqtt_device_id = "fms_001" /* device id*/ /* mqtt device id*/;
    char* mqtt_lwt_status[7] = "offline" /* mqtt last will topic offline*/;
    char* device_id = "fms_001" /* device id*/; // in datastructure uint32_t
    char* station_id = 1 /* station id*/;
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
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

void fms_log_print(const char *line) {
  byte loglevel = 1;
  if (true) {
    char mxtime[9];
    struct tm rtcTime;
    if (getLocalTime(&rtcTime)) snprintf(mxtime, sizeof(mxtime), ("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
    if (loglevel <= seriallog_level) log_printf /* in build in chip-debug-report.cpp*/("%s\n", line);
  }
}

void fms_chip_info_log(){
  printChipInfo(); // build in chip-debug-report.cpp
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
# 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 2

void event_receive(void *arg) {
  uint32_t rv;
  BaseType_t rc;
  for (;;) {
    rc = xTaskGenericNotifyWait( ( 0 ), ( 0 ), ( 0xFFFFFFFF ), ( &rv ), ( ( TickType_t ) 0xffffffffUL ) );
    if (rc == ( ( BaseType_t ) 1 )) {
      // Handle the event here
    }
  }
}


int app_cpu = 0;


void setup() {
  log_printf /* in build in chip-debug-report.cpp*/("CPU %d: Setup", app_cpu);
  if (fms_uart_cli_begin(use_uart_command, 115200)) {
    log_printf /* in build in chip-debug-report.cpp*/("uart cli begin\n");
  }

  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  log_printf /* in build in chip-debug-report.cpp*/("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage

  serialMutex = xQueueCreateMutex( ( ( uint8_t ) 1U ) );
  
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 serialMutex != 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
 __null), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 33, __PRETTY_FUNCTION__, 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "serialMutex != NULL"
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                            ;
  vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
                   1000 
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                   )); // wait delay 1 second

  fms_config_load_sd(); // load config data from sd card

  fms_log_print("initializing task");
  fms_task_create(); // rtos task create 
}

void loop() {
  BaseType_t rc;
  rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 0b0001 ), ( eSetBits ), 
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
      __null 
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
      );
  
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 45, __PRETTY_FUNCTION__, 
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "rc == pdPASS"
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                     ;
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("mqtt task started \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 5 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
void fms_config_load_sd() {
fms_log_print("config load");
}

static void sd_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("sd task started \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 3 ), ( eSetBits ), 
# 9 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino" 3 4
        __null 
# 9 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 10 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino" 3
              1000 
# 10 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
void fms_task_create() {
  BaseType_t rc;
    rc = xTaskCreatePinnedToCore(
    event_receive, // Task function
    "event_receive", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &heventTask, // Handle
    app_cpu // CPU
  );
  
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 12, __PRETTY_FUNCTION__, 
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    sd_task, // Task function
    "sdcard", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hsdCardTask, // Handle
    app_cpu // CPU
  );
  
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 23, __PRETTY_FUNCTION__, 
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 23 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    wifi_task, // Task function
    "wifi", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hwifiTask, // Handle
    app_cpu // CPU
  );
  
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 34, __PRETTY_FUNCTION__, 
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 34 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    mqtt_task, // Task function
    "mqtt", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hmqttTask, // Handle
    app_cpu // CPU
  );
  
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 45, __PRETTY_FUNCTION__, 
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;


  rc = xTaskCreatePinnedToCore(
    cli_task, // Task function
    "cli", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hcliTask, // Handle
    app_cpu // CPU
  );
  
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 57, __PRETTY_FUNCTION__, 
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 57 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    web_server_task, // Task function
    "webserver", // Name
    3000, // Stack size
    nullptr, // Parameters
    1, // Priority
    &hwebServerTask, // Handle
    app_cpu // CPU
  );
  
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 68, __PRETTY_FUNCTION__, 
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart2.ino"

void __attribute__((section(".iram1" "." "0"))) serialEvent() {
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
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   fms_log_print("uart cli is running");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 4 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}


bool fms_uart_cli_begin(bool flag, int baudrate) {
  if(flag){
     uart_t *uart = uartBegin(UART_NUM_0, baudrate, SERIAL_8N1, 3, 1, 256, 0, false, 112);
     if(uart == 
# 14 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino" 3 4
               __null
# 14 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_uart_cli.ino"
                   ){
      fms_log_print("cli uart begin fail");
      return false;
     }
     else{
       fms_log_print("cli uart begin success");
       return true;
     }
  }

}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("web server stated \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 6 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("wifi task started \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 2 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_wifi.ino"
              ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
  }
}
