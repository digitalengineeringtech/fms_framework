#include "driver/uart.h"
#include "soc/uart_struct.h"  // gives access to UART2.conf0

#define RX_PIN 16
#define TX_PIN 17

void setUartParity(uart_port_t uart_num, uint8_t mark_or_space) {
  if (uart_num == UART_NUM_2) {
    UART2.conf0.parity_en = 1;
    UART2.conf0.parity = mark_or_space ? 1 : 0;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("U913 Protocol Test with MARK/SPACE parity");
}

void loop() {
  // ----------- MARK PARITY (parity bit = 1) -----------
  Serial2.end();
  Serial2.begin(2400, SERIAL_8N1, RX_PIN, TX_PIN);

  uart_config_t markConfig = {
    .baud_rate = 2400,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,  // overridden manually
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB
  };
  uart_param_config(UART_NUM_2, &markConfig);
  setUartParity(UART_NUM_2, 1);  // 1 = MARK


  uint8_t address = 0x01;
  Serial2.write(address);

  Serial.println("Sent address (MARK parity): 01");
  delay(100);

  // ----------- SPACE PARITY (parity bit = 0) -----------
  Serial2.end();
  Serial2.begin(2400, SERIAL_8N1, RX_PIN, TX_PIN);

  uart_config_t spaceConfig = {
    .baud_rate = 2400,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB
  };
  uart_param_config(UART_NUM_2, &spaceConfig);
  setUartParity(UART_NUM_2, 0);  // 0 = SPACE

  uint8_t cmd[] = { 0x07, 0x80, 0x00, 0x00, 0x13, 0x88, 0xDD };
  Serial2.write(cmd, sizeof(cmd));
  Serial.println("Sent command (SPACE parity): 07 80 00 00 13 88 DD");

  delay(3000);  // Wait before repeating
}
