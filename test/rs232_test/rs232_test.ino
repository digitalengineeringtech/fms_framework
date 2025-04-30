unsigned char Show_infor[6] = { 0x5A, 0xA5, 0x21, 0x82, 0x80, 0x00 };
int byte_count = 6;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial2.write(Show_infor, 6);
}


void loop() {
  Serial2.write(Show_infor, 6);
  delay(500);
  if (Serial2.available() > 0) {
    char receive = Serial2.read();
    // if (byte_count > 6) byte_count = 0;
    Serial.println(receive, HEX);
  }
}
