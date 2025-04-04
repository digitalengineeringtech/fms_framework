
 #ifndef _FMS_LANFENG_H
 #define _FMS_LANFENG_H
 
 #define USE_LANFENG

 #include <Arduino.h>
 #include <ModbusMaster.h>
 
 // Debug logging macro - uncomment to enable debug logging
 #define FMS_DEBUG
 #ifdef FMS_DEBUG
   #define FMS_LOG_DEBUG(format, ...) Serial.printf(format, ##__VA_ARGS__); Serial.println()
 #else
   #define FMS_LOG_DEBUG(format, ...)
 #endif
 
 #ifdef USE_LANFENG
 class fmsLanfeng 
 {
   private:
     uint8_t _rePin;
     uint8_t _dePin;
     ModbusMaster _node;
     uint8_t _slaveId; // Slave ID for Modbus communication
     Stream* _serial;
     // Static members to store pin information for callback functions
     static uint8_t _staticRePin;
     static uint8_t _staticDePin;
     
     static void _preTransmission();
     static void _postTransmission();
     
   public:

     fmsLanfeng(uint8_t rePin, uint8_t dePin) {
       _rePin = rePin;
       _dePin = dePin;
       // Set static pins for callback functions
       _staticRePin = rePin;
       _staticDePin = dePin;
       FMS_LOG_INFO("[LANFENG] STARTING LOADING .... ");
     }
     
  
     void init(uint8_t slaveId, Stream &serial_) {
       _slaveId = slaveId;   
       setupPins(); // Setup the pins for RS485 communication
       _node.begin(_slaveId, serial_);
       _node.preTransmission(_preTransmission);
       _node.postTransmission(_postTransmission);
       FMS_LOG_INFO("[LANFENG] LANFENG SETUP COMPLETED");
     }
     
     /**
      * Setup pins for RS485 communication
      */
     void setupPins() {
       pinMode(_rePin, OUTPUT);
       pinMode(_dePin, OUTPUT);
       
       // Init in receive mode
       digitalWrite(_rePin, LOW);
       digitalWrite(_dePin, LOW);
     }
     
     /**
      * Process received data from the serial stream
      */
    //  void processReceivedData() {
    //    while (_serial->available()) {
    //      yield();
    //      uint8_t byte = _serial->read();
    //      _node.receive(byte);
    //    }
    //  }
     

     float convert_float(uint16_t highReg, uint16_t lowReg) {
       uint32_t combinedData = ((uint32_t)highReg << 16) | lowReg;
       float floatData;
       memcpy(&floatData, &combinedData, sizeof(float));
       return floatData;
     }
     

     void readAllData(uint16_t registerAddress, uint8_t numRegisters) {
       uint8_t result = _node.readHoldingRegisters(0x02BC, 40);
       if (result == _node.ku8MBSuccess) {
         for (int i = 0; i < numRegisters; i++) {
          if (_node.getResponseBuffer(i) != 0xFFFF) reg_data[i] = _node.getResponseBuffer(i);
          vTaskDelay(pdMS_TO_TICKS(100));
         }
         String jsonResponse = "{\"rawdata\": [";
         for (int i = 0; i < numRegisters; i++) {
           jsonResponse += String(reg_data[i]);
           if (i < numRegisters - 1) jsonResponse += ",";
         }
         jsonResponse += "],";
         jsonResponse += "\"data\": [";
         jsonResponse += String(convert_float(reg_data[0], reg_data[1]), 5);
         jsonResponse += ",";
         jsonResponse += String(convert_float(reg_data[4], reg_data[5]), 5);
         jsonResponse += ",";
         jsonResponse += reg_data[8];
         jsonResponse += ",";
         jsonResponse += reg_data[12];
         jsonResponse += ",";
         jsonResponse += String(convert_float(reg_data[24], reg_data[25]), 5);
         jsonResponse += ",";
         jsonResponse += reg_data[28];
         jsonResponse += ",";
         jsonResponse += String(convert_float(reg_data[32], reg_data[33]), 5);
         jsonResponse += ",";
         jsonResponse += reg_data[34];
         jsonResponse += "]";
         jsonResponse += "}";
         // Send the response to PyQt
     
         //  17695,49152,0,0,17948,16384,0,0,3578,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16709,49807,0,0,30000,0,0,0,17075,62915,1,0,1,0,0,0
         Serial.println(jsonResponse);

       } else {
         FMS_LOG_DEBUG("Error reading holding registers: %02X", result);
       }
     }
     
   
     
     uint32_t readPumpState(uint16_t registerAddress){
        uint8_t result = _node.readHoldingRegisters(registerAddress,1);
        if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
        else result;
     }

     uint32_t readLiveData(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }

     uint32_t readLivePrice(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }

     uint32_t readTotalizerLiter(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }


     uint32_t readTotalizerAmount(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }

     uint32_t readSellLiter(uint16_t registerAddress,uint32_t* data){
      uint8_t result = _node.readHoldingRegisters(registerAddress,2);
      if(result == _node.ku8MBSuccess) 
      {
        for(int i = 0; i < 2; i++) {data[i] = _node.getResponseBuffer(i);}
        return 0x01;
      }
      else return result;
     }


     


     uint8_t writeSingleRegister(uint16_t registerAddress, uint16_t value) {
       return _node.writeSingleRegister(registerAddress, value);
     }
     
    
     uint8_t readInputRegisters(uint16_t registerAddress, uint8_t numRegisters, uint16_t* data) {
      uint8_t result = _node.readInputRegisters(registerAddress, numRegisters);
       if (result == _node.ku8MBSuccess) {
         for (int i = 0; i < numRegisters; i++) {
           data[i] = _node.getResponseBuffer(i);
         }
       } else {
         FMS_LOG_DEBUG("Error reading input registers: %02X", result);
       }
       return result;
     }
     
     ModbusMaster& getNode() {
       return _node;
     }
 };
 
 // Initialize static members
 uint8_t fmsLanfeng::_staticRePin = 0;
 uint8_t fmsLanfeng::_staticDePin = 0;
 
 // Define static methods
 void fmsLanfeng::_preTransmission() {
   digitalWrite(_staticRePin, HIGH);
   digitalWrite(_staticDePin, HIGH);
 }
 
 void fmsLanfeng::_postTransmission() {
   digitalWrite(_staticRePin, LOW);
   digitalWrite(_staticDePin, LOW);
 }
 
 #endif // USE_LANFENG
 #endif // _FMS_LANFENG_H
 