// #include "fms_header.h"

// void event_receive(void *arg) {
//   uint32_t rv;
//   BaseType_t rc;
//   for (;;) {
//     rc = xTaskNotifyWait(0, 0xFFFFFFFF, &rv, portMAX_DELAY);
//     if (rc == pdTRUE) {
//       // Handle the event here
//     }
//   }
// }

// int app_cpu = 0;
// #define chip_report_printf log_printf

// void initialize_uart() {
//   if (fms_uart_cli_begin(use_uart_command, 115200)) {
//     fms_log_printf("uart cli begin\n");
//   }
// }

// void initialize_nvs_storage() {
//   fms_nvs_storage.begin("fms_config", false);
//   sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
//   sysCfg.bootcount++;
//   app_cpu = xPortGetCoreID();
//   fms_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
//   fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
//   fms_nvs_storage.end(); // close nvs storage
// }

// void setup() {
//   fms_chip_info_log();
//   fms_memory_info_log();
//   fms_log_printf("CPU %d: Setup", app_cpu);

//   initialize_uart();
//   initialize_nvs_storage();

//   serialMutex = xSemaphoreCreateMutex();
//   assert(serialMutex != NULL);
//   vTaskDelay(1000 / portTICK_PERIOD_MS); // wait delay 1 second

//   #if SHOW_SD_TEST_LOG
//   if(fms_config_load_sd_test()){fms_log_printf("\n\r==================== sd card test success================\n");}
//   else {fms_log_printf("sd card test failed\n");}
//   #endif

//   fms_log_printf("initializing task");
//   fms_task_create(); // rtos task create 

//   fms_print_after_setup_info();
//  // fms_task_usage_check(); 
// }

// void loop() {
//   BaseType_t rc;
//   rc = xTaskNotify(heventTask, 0b0001, eSetBits);
//   assert(rc == pdPASS);
// }
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fms_header.h"

TaskHandle_t heventTask;

void event_receive(void *arg);
void initialize_uart();
void initialize_nvs_storage();
void setup_sd();  // Declare SD setup function from fms_sd.ino

int app_cpu = 0;
#define chip_report_printf log_printf

void setup() {
    Serial.begin(115200);
    while (!Serial);

    fms_chip_info_log();
    fms_memory_info_log();
    fms_log_printf("CPU %d: Setup", app_cpu);

    initialize_uart();
    initialize_nvs_storage();
    
    serialMutex = xSemaphoreCreateMutex();
    assert(serialMutex != NULL);
    vTaskDelay(pdMS_TO_TICKS(1000));

    #if SHOW_SD_TEST_LOG
    if (fms_config_load_sd_test()) {
        fms_log_printf("\n\r==================== SD card test success =================\n");
    } else {
        fms_log_printf("SD card test failed\n");
    }
    #endif

    fms_log_printf("Initializing tasks");
    fms_task_create();
    fms_print_after_setup_info();

    // Initialize SD operations
    setup_sd();

    // Create event handling task
    xTaskCreate(event_receive, "Event_Receive", 4096, NULL, 1, &heventTask);
}

void loop() {
    BaseType_t rc;
    rc = xTaskNotify(heventTask, 0b0001, eSetBits);
    assert(rc == pdPASS);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

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

void initialize_uart() {
    if (fms_uart_cli_begin(use_uart_command, 115200)) {
        fms_log_printf("UART CLI begin\n");
    }
}

void initialize_nvs_storage() {
    fms_nvs_storage.begin("fms_config", false);
    sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
    sysCfg.bootcount++;
    app_cpu = xPortGetCoreID();
    fms_log_printf("CPU %d: Boot count: %lu\n\r", app_cpu, sysCfg.bootcount);
    fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
    fms_nvs_storage.end();
}
New file
