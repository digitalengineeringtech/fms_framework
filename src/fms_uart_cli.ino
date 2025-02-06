
bool fms_uart_cli_begin(bool flag, int baudrate) {
  if(flag){
   Serial.begin(baudrate);
  fms_log_printf("cli uart begin success\n\r");
  return true;
  }
  return true;
}

void IRAM_ATTR serialEvent() { // weak function 
  char c ;
  while(Serial.available()){
    c = Serial.read();
  
    //Serial.write(c);
    if(c == 0x0D){
      fms_log_printf("Enter key pressed\n\r");
    }else{
      fms_log_printf("received\n\r");
      fms_log_printf("%s",c);
    }
  }
 }

// #define uart _uart

static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
   fms_log_printf("uart cli is running\n\r");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}




