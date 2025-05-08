#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\fms_redstart.cpp"
/**
 * FuelDispenser.cpp - Implementation for fuel dispenser communication library
 * Created by v0, May 6, 2025
 */

 #include "FuelDispenser.h"

 FuelDispenser::FuelDispenser(HardwareSerial& serial) : _serial(serial) {
 }
 
 void FuelDispenser::begin(unsigned long baudRate) {
   _serial.begin(baudRate);
   delay(100); // Give some time for serial to initialize
 }
 
 uint8_t FuelDispenser::calculateChecksum(const uint8_t* bytes, size_t length) {
   uint16_t totalSum = 0;
   
   // Sum all bytes
   for (size_t i = 0; i < length; i++) {
     totalSum += bytes[i];
   }
   
   // Keep only the lowest byte
   totalSum &= 0xFF;
   
   // One's complement
   uint8_t complement = ~totalSum & 0xFF;
   
   // Add 1 and ensure it fits in one byte (two's complement)
   uint8_t checksum = (complement + 1) & 0xFF;
   
   return checksum;
 }
 
 bool FuelDispenser::readState(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x12;        // Read state code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_state frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::readPrice(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x8C;        // Read price code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_price frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::readTotal(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Read command
   frame[2] = 0x8E;        // Read total code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent read_total frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::sendApproval(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x08;        // Approval code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent send_approval frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::sendFuel(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x8F;        // Fuel code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent send_fuel frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::presetAmount(uint8_t nozzleId, uint16_t amount) {
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
 
 bool FuelDispenser::setPrice(uint8_t nozzleId, uint16_t price) {
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
 
 bool FuelDispenser::getPrice(uint8_t nozzleId) {
   uint8_t frame[4];
   frame[0] = nozzleId;    // Nozzle ID
   frame[1] = 0x03;        // Command
   frame[2] = 0x80;        // Get price code
   frame[3] = calculateChecksum(frame, 3);  // Checksum
   
   printFrame("Sent get_price frame:", frame, 4);
   return sendFrame(frame, 4);
 }
 
 bool FuelDispenser::sendFrame(const uint8_t* frame, size_t length) {
   if (!_serial) {
     return false;
   }
   
   _serial.write(frame, length);
   return true;
 }
 
 void FuelDispenser::printFrame(const char* prefix, const uint8_t* frame, size_t length) {
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
 
 void FuelDispenser::update() {
   // Process any incoming data from the dispenser
   // This is a placeholder for response handling logic
   while (_serial.available()) {
     uint8_t inByte = _serial.read();
     // Process incoming byte
     // This would need to be expanded based on the protocol details
   }
 }