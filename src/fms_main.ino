/*
  *  FMS main source file
  *  Author: Trion
  *  Date: 2025
  *  Guided By Senior Engineer : Sir Thiha Kyaw
  *  Description: This file is the main source file for FMS project.
*/

#include "fms_header.h"


/* Main function */
void setup() {


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

void loop() {
  // user main code here
}


