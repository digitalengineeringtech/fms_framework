
bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, 16, 17);

    if (fms_uart2_serial) {
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}


void fm_rx_irq_interrupt() {  // interrupt RS485/RS232 function
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available();  // serial.available  // #define fms_cli_serial Serial
  fms_uart2_serial.printf("Got byes on serial : %d\n", size);
  while (fms_uart2_serial.available()) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++;
  }
  fms_uart2_serial.printf("\n uart2  data process \n\r");
  fms_uart2_decode(Buffer, size);  // decode uart2 data main function
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data);
}

unsigned long lastUpdate = 0;

uint32_t value[40];
uint32_t s_liter[2];
void fms_uart2_task(void* arg) {
  BaseType_t rc;
  while (1) {
    #ifdef USE_LANFENG
    uint32_t sellLiter = lanfeng.readSellLiter(0x02D4,s_liter);
    if (sellLiter == 0x01) {
      Serial.print("[LANFENG] SELL LITER : ");
      Serial.print(s_liter[0]);
      Serial.print(" / ");
      Serial.println(s_liter[1]);
    } else {
      Serial.print("[LANFENG] Error reading sell liter: ");
      Serial.println(sellLiter, HEX);
    }
   vTaskDelay(pdMS_TO_TICKS(1000));
    uint32_t pumpState = lanfeng.readPumpState(0x02DE); // fix send data error when (not included 03 function how to fix this,)
    Serial.print("[LANFENG] PUMP STATE :");
    Serial.println(pumpState,HEX);
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t liveData = lanfeng.readLiveData(0x02C4); // fix send data error when (not included 03 function how to fix this,)
    Serial.print("[LANFENG] LIVE DATA :");
    Serial.println(liveData,HEX);
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t SLP_Price = lanfeng.readSellLiterPerPrice(0x02D8); // fix se,nd data error when (not included 03 function how to fix this,)
    Serial.print("[LANFENG] SELL LITER PERPRICE DATA :");
    Serial.println(SLP_Price,HEX);
    vTaskDelay(pdMS_TO_TICKS(1000));
    #endif
  }
}
