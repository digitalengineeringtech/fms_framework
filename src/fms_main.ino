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

   fms_pin_mode(2, OUTPUT);
  
   initialize_uart_cli(); 
   initialize_uart2(); // Initialize UART2 for rf485 log
   initialize_nvs_storage(); // Save boot count to NVS storage
   fms_debug_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
   if (initialize_wifi()) {
       fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
       fms_task_create(); // RTOS task create
   }
   run_sd_test();
 

}

void loop() {
    // user main code here
}

