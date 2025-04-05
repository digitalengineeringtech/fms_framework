
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

     uint32_t readSellLiterPerPrice(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }

    uint32_t setSellLiterPerPrice(uint16_t registerAddress, uint16_t value) {
       uint8_t result = _node.writeSingleRegister(registerAddress, value);
       if (result == _node.ku8MBSuccess) {
         return 0x01; // Success
       } else {
         return result; // Error code
       }
     }

    uint32_t setPumpState(uint16_t registerAddress, uint16_t value) {
       uint8_t result = _node.writeSingleRegister(registerAddress, value);
       if (result == _node.ku8MBSuccess) {
         return 0x01; // Success
       } else {
         return result; // Error code
       }
     }

     uint32_t readPermit(uint16_t registerAddress){
      uint8_t result = _node.readHoldingRegisters(registerAddress,1);
      if(result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
     }
     

     uint32_t writeSingleRegister(uint16_t registerAddress, uint16_t value) {
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
 

 /*
  * lanfeng test code 
    //   uint32_t sellLiter = lanfeng.readSellLiter(0x02D4,s_liter);
  //   if (sellLiter == 0x01) {
  //     Serial.print("[LANFENG] SELL LITER : ");
  //     Serial.print(s_liter[0]);
  //     Serial.print(" / ");
  //     Serial.println(s_liter[1]);
  //   } else {
  //     Serial.print("[LANFENG] Error reading sell liter: ");
  //     Serial.println(sellLiter, HEX);
  //   }
  //  vTaskDelay(pdMS_TO_TICKS(1000));
  //   uint32_t pumpState = lanfeng.readPumpState(0x02DE); // fix send data error when (not included 03 function how to fix this,)
  //   Serial.print("[LANFENG] PUMP STATE :");
  //   Serial.println(pumpState,HEX);
  //   vTaskDelay(pdMS_TO_TICKS(1000));

  //   uint32_t liveData = lanfeng.readLiveData(0x02C4); // fix send data error when (not included 03 function how to fix this,)
  //   Serial.print("[LANFENG] LIVE DATA :");
  //   Serial.println(liveData,HEX);
  //   vTaskDelay(pdMS_TO_TICKS(1000));

  //   uint32_t SLP_Price = lanfeng.readSellLiterPerPrice(0x02D8); // fix se,nd data error when (not included 03 function how to fix this,)
  //   Serial.print("[LANFENG] SELL LITER PERPRICE DATA :");
  //   Serial.println(SLP_Price,HEX);
  //   vTaskDelay(pdMS_TO_TICKS(1000));

  //   uint32_t respone = lanfeng.setSellLiterPerPrice(0x02D8, 0x0001); // fix send data error when (not included 03 function how to fix this,)
  //   if (respone == 0x01) {
  //     Serial.print("[LANFENG] SET SELL LITER PERPRICE DATA :");
  //     Serial.println(respone,HEX);
  //   } else {
  //     Serial.print("[LANFENG] Error reading set sell liter per price: ");
  //     Serial.println(respone, HEX);
  //   }
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  //   uint32_t setPump = lanfeng.setPumpState(0x02DE, 0x0001); // fix send data error when (not included 03 function how to fix this,)
  //   if (setPump == 0x01) {
  //     Serial.print("[LANFENG] SET PUMP STATE :");
  //     Serial.println(setPump,HEX);
  //   } else {
  //     Serial.print("[LANFENG] Error reading set pump state: ");
  //     Serial.println(setPump, HEX);
  //   }
  //   vTaskDelay(pdMS_TO_TICKS(1000));

  //   uint32_t permit = lanfeng.readPermit(0x02E0); 
  //   Serial.print("[LANFENG] PERMIT :");
  //   Serial.println(permit,HEX);
  //   vTaskDelay(pdMS_TO_TICKS(1000));
 */