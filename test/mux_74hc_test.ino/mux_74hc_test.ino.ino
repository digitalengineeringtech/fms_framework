
// test firmware for multiplexer and pc817 
// demo test not use for production 
// author : trion,
// guideline : sir thiha & ko sai
// test circuit fixed in 21.4.2025

#define MUX_S0 25
#define MUX_S1 26
#define MUX_EN 27
#define MUX_COM 16  // UART RX pin via MUX

#define LED4 32
#define LED3 33
#define LED2 14
#define LED1 13

#define TX_PIN 17  // Direct TX to device opto-LED

HardwareSerial mySerial(2);
unsigned char read_state_1[4] = { 0x01, 0x03, 0x12, 0xEA };

unsigned char Buffer[30];
int bufcount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(MUX_COM, INPUT_PULLDOWN);
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_EN, OUTPUT);
  digitalWrite(MUX_EN, LOW);  // Enable MUX

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);


  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  mySerial.begin(2400, SERIAL_8N1, MUX_COM, TX_PIN);  // RX = MUX, TX = direct
}
/*
Channel	Binary	S1	  S0
0	        00	    0	  0
1	        01	    0	  1
2	        10	    1	  0
3	        11	    1	  1

uint8_t channel = 3;  // Binary: 00000011
channel >> 1:  00000001   (decimal 1)
00000001
& 00000001
-----------
  00000001  → 1
*/
void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_EN, LOW);
  vTaskDelay(pdMS_TO_TICKS(2));  // Let MUX stabilize
}

void sendAndReceive(uint8_t channel) {
  selectMuxChannel(channel);
  // Send data (e.g., "PING\n")
  delay(500);
  digitalWrite(LED1, LOW);
  mySerial.write(read_state_1, 4);
  Serial.printf("[CH %d] Sent: readstate\n", channel);
  // Wait and read response
  unsigned long timeout = millis() + 1000;
  while (millis() < timeout) {
    if (mySerial.available() > 0) {
      Serial.printf("[CH %d] Received: ", channel);
        digitalWrite(LED3, LOW);
        Buffer[bufcount] = mySerial.read();
        bufcount++;
        Serial.print("RECEIVED : ");
        Serial.println(Buffer[bufcount], HEX);
        if (bufcount >= 4) {
          bufcount = 0;
        }
      Serial.println();
      break;
    }
  }
  digitalWrite(LED3, HIGH);
  Serial.println("Read Time Out");
}

void loop() {
  sendAndReceive(2);
  delay(1000);
  digitalWrite(LED1, HIGH);
}
