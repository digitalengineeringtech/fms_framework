# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("cli task started \n");
    rc = xTaskGenericNotify( ( heventTask ), ( ( 0 ) ), ( 4 ), ( eSetBits ), 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino" 3 4
        __null 
# 5 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
        );
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino" 3
              1000 
# 6 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_cli.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
# 2 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 2

int seriallog_level = 1;
void addLog(byte loglevel, const char *line) {
  if (true) {
    char mxtime[9];
    struct tm rtcTime;
    if (getLocalTime(&rtcTime)) snprintf(mxtime, sizeof(mxtime), ("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
    if (loglevel <= seriallog_level) Serial0.printf("%s %s\n", mxtime, line); // on and off serial print optional feature;
  }
}

void event_receive(void *arg) {
  uint32_t rv;
  BaseType_t rc;
  for (;;) {
    rc = xTaskGenericNotifyWait( ( 0 ), ( 0 ), ( 0xFFFFFFFF ), ( &rv ), ( ( TickType_t ) 0xffffffffUL ) );
    if (rc == ( ( BaseType_t ) 1 )) {
      // test code
      // if (rv & 2)
      //   printf("wifi() notified this task\n"); // test code
      // if (rv & 0b0001)
      //   printf("loop() notified this task.\n"); // test code
      // if (rv & 3)
      //   printf("sd() notified this task.\n"); // test code
      // if (rv & 0b0100)
      //   printf(" Serial interrupt notified this task with value: 0x%08X\n", rv); // test code
    }
  }
}

int app_cpu = 0;

void app_main() {
Serial0.begin(115200);
Serial0.println("Start Main");
app_cpu = xPortGetCoreID();
// serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control
// assert(serialMutex != NULL);
vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 40 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino" 3
                 1000 
# 40 "d:\\2025 iih office\\Project\\FMS Framework\\fms_main\\src\\fms_main.ino"
                 ));
}
// void setup() {
//   sysCfg.bootcount++;

//   app_cpu = xPortGetCoreID();
//   Serial.begin(115200);
//   serialMutex = xSemaphoreCreateMutex(); // for serial interrupt control 
//   assert(serialMutex != NULL);
//   vTaskDelay(1000 / portTICK_PERIOD_MS);
//   // sd card load
//   fms_sd_begin();
//   // start create task
//   fms_task_create();
// }

// void loop() {
//   BaseType_t rc;
//   //Serial.println("Main Loop");
//   rc = xTaskNotify(heventTask, 0b0001, eSetBits);
//   assert(rc == pdPASS);
// }
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
