static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   fms_log_print("uart cli is running");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


bool fms_uart_cli_begin(bool flag, int baudrate) {
  if(flag){
     uart_t *uart = uartBegin(UART_NUM_0, baudrate, SERIAL_8N1, 3, 1, 256, 0, false, 112);
     if(uart == NULL){
      fms_log_print("cli uart begin fail");
      return false;
     }
     else{
       fms_log_print("cli uart begin success");
       return true;
     }
  }

}