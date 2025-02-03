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


// MQTT topics from the old project
# 106 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_header.h"
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
void addLog(byte loglevel, const char *line);
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
# 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 2
# 3 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 2

// int seriallog_level = 1;
// void addLog(byte loglevel, const char *line) {
//   if (SHOW_SYS_LOG) {
//     char mxtime[9];
//     struct tm rtcTime;
//     if (getLocalTime(&rtcTime)) snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
//     if (loglevel <= seriallog_level) Serial.printf("%s %s\n", mxtime, line);  // on and off serial print optional feature;
//   }
// }

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
  printChipInfo();
  sysCfg.bootcount++;

  app_cpu = xPortGetCoreID();
  Serial0.begin(115200);
  serialMutex = xQueueCreateMutex( ( ( uint8_t ) 1U ) ); // for serial interrupt control 
  
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 serialMutex != 
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
 __null), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 36, __PRETTY_FUNCTION__, 
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "serialMutex != NULL"
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 36 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                            ;
  vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 37 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
                   1000 
# 37 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                   ));
  //chip_report_printf("=========== Setup Start ===========\n");


  // sd card load
  fms_sd_begin();
  printAfterSetupInfo();
  // start create task
  fms_task_create();
}



void loop() {
  BaseType_t rc;
  //Serial.println("Main Loop");
  rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 0b0001 ), ( eSetBits ), 
# 53 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3 4
      __null 
# 53 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
      );
  
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 (__builtin_expect(!!(
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ), 1) ? (void)0 : __assert_func ((__builtin_strrchr( "/" "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino", '/') + 1), 54, __PRETTY_FUNCTION__, 
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
 "rc == pdPASS"
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
 ))
# 54 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                     ;
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino"
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    log_printf("  CLI TERMINAL     : %lu cli\n",1 );
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 4 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_md.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
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
