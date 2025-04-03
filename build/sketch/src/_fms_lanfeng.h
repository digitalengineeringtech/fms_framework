#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\_fms_lanfeng.h"

 #ifndef fmsLanfeng_h
 #define fmsLanfeng_h
 
 #include <Arduino.h>
 #include <ModbusMaster.h>
 
 class fmsLanfeng {
   public:
 
     fmsLanfeng(uint8_t rePin, uint8_t dePin);
     void begin();
     void beginModbus(uint8_t nodeId = 1);
     ModbusMaster& getNode();
     uint8_t readHoldingRegisters(uint8_t deviceAddress, uint16_t startAddress, uint16_t quantity, uint16_t* result);
     uint8_t writeSingleRegister(uint8_t deviceAddress, uint16_t registerAddress, uint16_t value);
     uint8_t readInputRegisters(uint8_t deviceAddress, uint16_t startAddress, uint16_t quantity, uint16_t* result);
     
   private:
     uint8_t _rePin;
     uint8_t _dePin;
     uint8_t _rxPin;
     uint8_t _txPin;
     ModbusMaster _node;
     static void _preTransmission();
     static void _postTransmission();
     static uint8_t _staticRePin;
     static uint8_t _staticDePin;
 };
 
 #endif
 