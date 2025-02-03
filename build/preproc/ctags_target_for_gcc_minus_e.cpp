# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"

/*

# author 

    Trion 30.1.2025 

# project

    Fuel Management System (FMS)

# date

    30/01/2025

# version

    1.0.0



+-------------------+       +-------------------+       +-------------------+

|                   |       |                   |       |                   |

|     Task 1        |       |     Queue         |       |     Task 2        |

|  (Producer)       |       |  (Data Buffer)    |       |  (Consumer)       |

|                   |       |                   |       |                   |

| 1. Generate       |       |                   |       | 1. Wait for       |

|    Random Number  |       |                   |       |    Semaphore      |

|                   |       |                   |       |                   |

| 2. Send Number    | ----> |  Store Number     | <---- | 2. Receive Number |

|    to Queue       |       |    in Queue       |       |    from Queue     |

|                   |       |                   |       |                   |

| 3. Give Semaphore | ----> |                   |       | 3. Process Number |

|    (Signal Task 2)|       |                   |       |    (Print to Serial)

|                   |       |                   |       |                   |

+-------------------+       +-------------------+       +-------------------+

            ^                                                       |

            |                                                       |

            |                                                       |

            +-------------------------------------------------------+

                                  Binary Semaphore

*/
# 37 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
// Include libraries
# 39 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 40 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 41 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 42 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 43 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 45 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 46 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

# 48 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 49 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2
# 50 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h" 2

// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/semphr.h>

// Project details



// Device details


// WiFi configuration



// MQTT configuration







// Web server configuration


// SD card pins configuration (check your board pin layout spi communication)






// Time configuration




// SD card file configuration






WiFiClient wf_client;
PubSubClient mqtt_client(wf_client);

 struct SYSCFG{
  unsigned long bootcount;
  unsigned long version;
 } sysCfg;

 // rtos task handle 
static TaskHandle_t heventTask;
static TaskHandle_t hwifiTask;
static TaskHandle_t hmqttTask;
static TaskHandle_t hsdCardTask;
static TaskHandle_t hwebServerTask;
static TaskHandle_t hspiTask; // use this for something 
static TaskHandle_t hcliTask;

SemaphoreHandle_t serialMutex; // Mutex to protect the buffer test code 



volatile uint8_t serialBuffer[4]; // Buffer to store received hex value // test code  
volatile uint8_t bufferIndex = 0; // Index for the buffer // test code 
bool use_uart_command = true; // use uart command flag // test code

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
    if (loglevel <= seriallog_level) log_printf /* in build in chip-debug-report.cpp*/("%s %s\n", mxtime, line);
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
    }
  }
}

int app_cpu = 0;




void setup() {
log_printf /* in build in chip-debug-report.cpp*/("CPU %d: Setup", app_cpu);
if (fms_uart_cli_begin(use_uart_command,115200)) log_printf /* in build in chip-debug-report.cpp*/("UART CLI Begin\n"); // serial begin 

/***********************************************************************/
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();

  log_printf /* in build in chip-debug-report.cpp*/("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

  serialMutex = xQueueCreateMutex( ( ( uint8_t ) 1U ) ); // for serial interrupt control 
  
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 serialMutex != 
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
 __null), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 29, __PRETTY_FUNCTION__, 
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "serialMutex != NULL"
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 29 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                            ;

  vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 31 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
                   1000 
# 31 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                   ));
  fms_sd_begin(); // start sd card
  fms_log_print("intializing task");
  // start create task
  fms_task_create();
}



void loop() {
  BaseType_t rc;
  //Serial.println("Main Loop");
  rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 0b0001 ), ( eSetBits ), 
# 43 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
      __null 
# 43 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
      );
  
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 44, __PRETTY_FUNCTION__, 
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "rc == pdPASS"
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 44 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
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
void fms_sd_begin() {
  Serial0.println("Config Loaded");
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
