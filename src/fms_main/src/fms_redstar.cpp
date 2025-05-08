#include "fms_redstar.h"

 Redstar::Redstar(HardwareSerial& serial) : _serial(serial) {
 }
 
 void Redstar::begin(unsigned long baudRate) {
   _serial.begin(baudRate);
   delay(100); // Give some time for serial to initialize
 }
 
 // opp calculation for redstart fuel dispenser
 uint8_t Redstar::calculateChecksum(const uint8_t* bytes, size_t length) {
   uint16_t totalSum = 0;
   for (size_t i = 0; i < length; i++) {
     totalSum += bytes[i];
   }
   
   totalSum &= 0xFF;
   uint8_t complement = ~totalSum & 0xFF;
   uint8_t checksum = (complement + 1) & 0xFF;
   
   return checksum;
 }
 
 bool Redstar::readState(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x12;        // Read state code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_state frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::readPrice(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x8C;        // Read price code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_price frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::readTotal(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x8E;        // Read total code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_total frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::sendApproval(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x08;        // Approval code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent send_approval frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::sendFuel(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x8F;        // Fuel code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent send_fuel frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::presetAmount(uint8_t nozzleId, uint16_t amount) {
   uint8_t frame[8];
   frame[0] = nozzleId;           // Nozzle ID
   frame[1] = 0x07;               // Command MSB
   frame[2] = 0x89;               // Command LSB (preset amount)
   frame[3] = 0x00;               // Reserved
   frame[4] = 0x00;               // Reserved
   frame[5] = (amount >> 8) & 0xFF;  // Amount MSB
   frame[6] = amount & 0xFF;         // Amount LSB
   frame[7] = calculateChecksum(frame, 7);  // Checksum
   
   printFrame("Sent preset_amount frame:", frame, 8);
   return sendFrame(frame, 8);
 }
 
 bool Redstar::setPrice(uint8_t nozzleId, uint16_t price) {
   uint8_t frame[8];
   frame[0] = nozzleId;           // Nozzle ID
   frame[1] = 0x07;               // Command MSB
   frame[2] = 0x80;               // Command LSB (Set Price)
   frame[3] = 0x00;               // Reserved
   frame[4] = 0x00;               // Reserved
   frame[5] = (price >> 8) & 0xFF;  // Price MSB
   frame[6] = price & 0xFF;         // Price LSB
   frame[7] = calculateChecksum(frame, 7);  // Checksum
   
   printFrame("Sent set_price frame:", frame, 8);
   return sendFrame(frame, 8);
 }
 
 bool Redstar::getPrice(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x80;        // Get price code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent get_price frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool Redstar::sendFrame(const uint8_t* frame, size_t length) {
   if (!_serial) {
     return false;
   }
   
   _serial.write(frame, length);
   return true;
 }
 
 void Redstar::printFrame(const char* prefix, const uint8_t* frame, size_t length) {
   Serial.print(prefix);
   for (size_t i = 0; i < length; i++) {
     Serial.print(" 0x");
     if (frame[i] < 16) {
       Serial.print("0");
     }
     Serial.print(frame[i], HEX);
   }
   Serial.println();
 }
 
 void Redstar::update() { // some data update  // fix this feature , in main program weuse serail interrupt
   // Process any incoming data from the dispenser
   // This is a placeholder for response handling logic
   while (_serial.available()) {
     uint8_t inByte = _serial.read();
     // Process incoming byte
     // This would need to be expanded based on the protocol details
   }
 }

 void Redstar:totalState(byte hexArray[], int size) { // hexarray to string conversion
   // Convert a subset of a hex array to a string (indices 4 to 9)
   String result = "";
   bool firsttime = true;         // To handle leading zero values
   for (int i = 9; i > 3; i--) {  // Process indices 9 to 4 in reverse order
 
     if (hexArray[i] == 0x00 && firsttime == true) continue;  // Skip initial 0x00 values
     else firsttime = false;
 
     if (String(hexArray[i], HEX).length() < 2 && String(hexArray[i], HEX) != "0")
       result += "0";  // Add leading zero if necessary
 
     result += String(hexArray[i], HEX);  // Convert the byte to a hexadecimal string
 
     if (String(hexArray[i], HEX) == "0")
       result += "0";  // Add an additional "0" for 0x00 bytes
   }
   return result;  // Return the final hexadecimal string
 }

 void Redstar::total_amount(byte hexArray[], int size) { // hexarray to string conversion

  // Convert a subset of a hex array to a string (indices 10 to 15)
  String result = "";
  bool firsttime = true;          // To handle leading zero values
  for (int i = 15; i > 9; i--) {  // Process indices 15 to 10 in reverse order

    if (hexArray[i] == 0x00 && firsttime == true) continue;  // Skip initial 0x00 values
    else firsttime = false;

    if (String(hexArray[i], HEX).length() < 2 && String(hexArray[i], HEX) != "0")
      result += "0";  // Add leading zero if necessary

    result += String(hexArray[i], HEX);  // Convert the byte to a hexadecimal string

    if (String(hexArray[i], HEX) == "0")
      result += "0";  // Add an additional "0" for 0x00 bytes
  }
  return result;  // Return the final hexadecimal string
 }