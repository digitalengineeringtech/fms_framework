#define RX_PIN 16
#define TX_PIN 17
// set price 5000 to nozel 1
uint8_t addressBytes[] = { 0x01 };
// Example data bytes to send with SPACE parity
// Replace with your actual data
//uint8_t dataBytes[] = { 0x07, 0x80, 0x00, 0x00, 0x13, 0x88, 0xDD };

// read state
uint8_t dataBytes[] = { 0x03, 0x12, 0xEA };
void setup() {
  Serial.begin(115200);
  Serial.println("MARK/SPACE Parity Protocol Test");
  // Initialize Serial2 without parity initially
  Serial2.begin(2400, SERIAL_8N1, RX_PIN, TX_PIN);
  // Configure GPIO for direct manipulation if needed
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, HIGH);  // Idle state for UART
}

void loop() {
  // Step 1: Send address bytes with MARK parity
  Serial.println("Sending address bytes with MARK parity:");
  for (int i = 0; i < sizeof(addressBytes); i++) {
    sendByteWithMarkParity(addressBytes[i]);
    Serial.print(addressBytes[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(69);

  // Step 2: Send data bytes with SPACE parity
  Serial.println("Sending data bytes with SPACE parity:");
  for (int i = 0; i < sizeof(dataBytes); i++) {
    sendByteWithSpaceParity(dataBytes[i]);
    Serial.print(dataBytes[i], HEX);
    Serial.print(" ");
    delay(5);
    // delayMicroseconds(417);
  }
  Serial.println();

  while (Serial2.read() > 0) {
    Serial.println(Serial2.read(), HEX);
  }

  delay(3000);  // Wait before repeating
}

// Send a byte with MARK parity (parity bit always 1)
void sendByteWithMarkParity(uint8_t data) {
  int bitCount = countBits(data);

  if (bitCount % 2 == 0) {
    // Even number of bits, odd parity would set parity bit to 1
    // So we can send as-is with odd parity
    Serial2.end();
    Serial2.begin(2400, SERIAL_8O1, RX_PIN, TX_PIN);
    Serial2.write(data);
  } else {
    // Odd number of bits, odd parity would set parity bit to 0
    // We need to adjust by using even parity instead
    Serial2.end();
    Serial2.begin(2400, SERIAL_8E1, RX_PIN, TX_PIN);
    Serial2.write(data);
  }
}

// Send a byte with SPACE parity (parity bit always 0)
void sendByteWithSpaceParity(uint8_t data) {
  int bitCount = countBits(data);

  if (bitCount % 2 == 0) {
    // Even number of bits, even parity would set parity bit to 0
    Serial2.end();
    Serial2.begin(2400, SERIAL_8E1, RX_PIN, TX_PIN);
    Serial2.write(data);
  } else {
    // Odd number of bits, even parity would set parity bit to 1
    // We need to adjust by using odd parity instead
    Serial2.end();
    Serial2.begin(2400, SERIAL_8O1, RX_PIN, TX_PIN);
    Serial2.write(data);
  }
}

// Helper function to count number of 1 bits in a byte
int countBits(uint8_t byte) {
  int count = 0;
  for (int i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      count++;
    }
  }
  return count;
}