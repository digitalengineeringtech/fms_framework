bool fms_uart_cli_begin(bool flag, int baudrate) {
  if (flag) {
    fms_cli_serial.begin(baudrate);
    fms_log_printf("UART 1 CLI (Baudrate : %d) started successfully\n\r",baudrate);
    return true;
  }
  return true;
}

void IRAM_ATTR serialEvent() { 
  char c;
  char buffer[32]; 
  while (fms_cli_serial.available()) {
    yield();
    c = fms_cli_serial.read(); 
    snprintf(buffer, sizeof(buffer), "[CLI] command : %c\n\r", c);
    fms_log_printf("Enter key pressed\n\r");
    fms_cli_serial.println(buffer);
  }
}

static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    fms_log_printf("uart cli is running\n\r");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
