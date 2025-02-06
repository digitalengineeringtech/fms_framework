# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"




// Include libraries
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 9 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 10 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 11 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 12 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

# 15 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 16 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 17 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 18 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2


// Project details




// Device details





// WiFi configuration



// MQTT configuration
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
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
    uint32_t mqtt_port = 1883 /* mqtt port*/; // in datastructure uint32_t

    char* mqtt_device_id = "fms_001" /* device id*/ /* mqtt device id*/;
    char* mqtt_lwt_status[20];
    char* device_id = "fms_001" /* device id*/; // in datastructure uint32_t
    uint32_t station_id = 1 /* station id*/;

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

bool fms_log_printf(const char *line,...) {
  byte loglevel = 1;
  if (true) {
    if (loglevel <= seriallog_level) log_printf /* in build in chip-debug-report.cpp*/(line);
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
    fms_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    // Check stack size of the current task
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(
# 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino" 3 4
                                                                __null
# 28 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_log.ino"
                                                                    );
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






  fms_log_printf("CPU %d: Setup", app_cpu);
  initialize_uart();
  initialize_nvs_storage();

  serialMutex = xQueueCreateMutex( ( ( uint8_t ) 1U ) );
  
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 serialMutex != 
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
 __null), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 45, __PRETTY_FUNCTION__, 
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "serialMutex != NULL"
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                            ;
  vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
                   1000 
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                   )); // wait delay 1 second
# 56 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
  fms_log_printf("initializing task");
  fms_task_create(); // rtos task create 





}

void loop() {
  BaseType_t rc;
  rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 0b0001 ), ( eSetBits ), 
# 67 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
      __null 
# 67 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
      );
  
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 68, __PRETTY_FUNCTION__, 
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "rc == pdPASS"
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 68 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                     ;
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg) {
  BaseType_t rc;
  while(1){
 // low 
    printf("mqtt task started \n");

    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 5 ), ( eSetBits ), 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino" 3 4
        __null 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino" 3
              1000 
# 8 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_mqtt.ino"
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
    printf("sd task started \n");
    /*

    * Load config data from sd card

    */
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 3 ), ( eSetBits ), 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino" 3 4
        __null 
# 33 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_sd.ino"
        );
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
  
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 13, __PRETTY_FUNCTION__, 
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 13 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    sd_task, // Task function
    "sdcard", // Name
    3000, // Stack size
    nullptr, // Parameters
    2, // Priority
    &hsdCardTask, // Handle
    app_cpu // CPU
  );
  
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 24, __PRETTY_FUNCTION__, 
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 24 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    wifi_task, // Task function
    "wifi", // Name
    3000, // Stack size
    nullptr, // Parameters
    3, // Priority
    &hwifiTask, // Handle
    app_cpu // CPU
  );
  
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 35, __PRETTY_FUNCTION__, 
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 35 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    mqtt_task, // Task function
    "mqtt", // Name
    3000, // Stack size
    nullptr, // Parameters
    3, // Priority
    &hmqttTask, // Handle
    app_cpu // CPU
  );
  
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 46, __PRETTY_FUNCTION__, 
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
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
  
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 58, __PRETTY_FUNCTION__, 
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 58 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  rc = xTaskCreatePinnedToCore(
    web_server_task, // Task function
    "webserver", // Name
    3000, // Stack size
    nullptr, // Parameters
    4, // Priority
    &hwebServerTask, // Handle
    app_cpu // CPU
  );
  
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 (__builtin_expect(!!(
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino", '/') + 1), 69, __PRETTY_FUNCTION__, 
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
 "rc == pdPASS"
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino" 3
 ))
# 69 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_task.ino"
                     ;

  return true;
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
   fms_log_printf("uart cli is running");
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
      fms_log_printf("cli uart begin fail");
      return false;
     }
     else{
       fms_log_printf("cli uart begin success");
       return true;
     }
  }

}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
  BaseType_t rc;
  for (;;) {
    printf("web server stated \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 6 ), ( eSetBits ), 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino" 3 4
        __null 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino" 3
              1000 
# 7 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_web_server.ino"
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
