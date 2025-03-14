
bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, 16, 17);
    if(fms_uart2_serial){
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}


void UART2_RX_IRQ() { // interrupt RS485/RS232 function
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available(); // serial.available  // #define fms_cli_serial Serial
  fms_uart2_serial.printf("Got byes on serial : %d\n",size);
  while(fms_uart2_serial.available()) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++; 
  }
  fms_uart2_serial.printf("\n uart2  data process \n\r");
  fms_uart2_decode(Buffer, size); // decode uart2 data main function
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data);
}



// free rtos task
void fms_uart2_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}