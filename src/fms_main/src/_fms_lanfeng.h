
 #ifndef _FMS_LANFENG_H
 #define _FMS_LANFENG_H
 
 #define USE_LANFENG

 #include <Arduino.h>
 #include <ModbusMaster.h>
 
 // Debug logging macro - uncomment to enable debug logging
 // #define FMS_DEBUG
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
     Stream* _serial; // Pointer to the serial stream for communication
     ModbusMaster _node;
     uint8_t _slaveId; // Slave ID for Modbus communication
     
     // Static members to store pin information for callback functions
     static uint8_t _staticRePin;
     static uint8_t _staticDePin;
     
     static void _preTransmission();
     static void _postTransmission();
     
   public:
     /**
      * Constructor
      * @param rePin RE pin on MAX485 (Receiver Enable, active low)
      * @param dePin DE pin on MAX485 (Driver Enable, active high)
      * @param serial Reference to the serial stream for communication
      */
     fmsLanfeng(uint8_t rePin, uint8_t dePin, Stream &serial) {
       _rePin = rePin;
       _dePin = dePin;
       _serial = &serial; // Initialize the serial stream pointer
       
       // Set static pins for callback functions
       _staticRePin = rePin;
       _staticDePin = dePin;
     }
     
     /**
      * Initialize the Modbus communication
      * @param slaveId Modbus slave ID
      * @param serial Reference to the serial stream for communication
      */
     void init(uint8_t slaveId, Stream &serial) {
       _slaveId = slaveId;
       _serial = &serial;
       
       setupPins(); // Setup the pins for RS485 communication
       _node.begin(slaveId, serial);
       _node.preTransmission(_preTransmission);
       _node.postTransmission(_postTransmission);
       FMS_LOG_INFO("SETUP COMPLETED");
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
     
     /**
      * Convert two 16-bit registers to a 32-bit float
      * @param highReg High register value (upper 16 bits)
      * @param lowReg Low register value (lower 16 bits)
      * @return Float value
      */
     float convert_float(uint16_t highReg, uint16_t lowReg) {
       uint32_t combinedData = ((uint32_t)highReg << 16) | lowReg;
       float floatData;
       memcpy(&floatData, &combinedData, sizeof(float));
       return floatData;
     }
     
     /**
      * Read holding registers and return as byte array
      * @param registerAddress Starting register address
      * @param numRegisters Number of registers to read
      * @param data Pointer to array where data will be stored
      * @return Modbus exception code (0 if success)
      */
     uint8_t _readHoldingRegisters(uint16_t registerAddress, uint8_t numRegisters) {
       uint8_t result = _node.readHoldingRegisters(registerAddress, numRegisters);
       if (result == _node.ku8MBSuccess) {
         for (int i = 0; i < numRegisters; i++) {
          if (_node.getResponseBuffer(i) != 0xFFFF) reg_data[i] = _node.getResponseBuffer(i);
          Serial.print("Register ");
          Serial.print(i);
          Serial.print(": ");
         // reg_data[NUM_REG]
          Serial.println(reg_data[i]);
            vTaskDelay(pdMS_TO_TICKS(1000));
         }
       } else {
         FMS_LOG_DEBUG("Error reading holding registers: %02X", result);
       }
        return result;
     }
     
     /**
      * Read holding registers and return as uint16_t array
      * @param registerAddress Starting register address
      * @param numRegisters Number of registers to read
      * @param data Pointer to array where data will be stored
      * @return Modbus exception code (0 if success)
      */
    //  uint8_t readHoldingRegistersRaw(uint16_t registerAddress, uint8_t numRegisters) {
    //    uint8_t result = _node.readHoldingRegisters(registerAddress, numRegisters);
       
    //    if (result == _node.ku8MBSuccess) {
    //      for (int i = 0; i < numRegisters; i++) {
    //       if(_node.getResponseBuffer(i) != 0xFFFF)  reg_data[i] = _node.getResponseBuffer(i);
    //        vTaskDelay(pdMS_TO_TICKS(1000));
    //      }
    //    } else {
    //      FMS_LOG_DEBUG("Error reading holding registers: %02X", result);
    //    }
       
     
    //  }
     
     /**
      * Write single register
      * @param registerAddress Register address to write
      * @param value Value to write
      * @return Modbus exception code (0 if success)
      */
     uint8_t writeSingleRegister(uint16_t registerAddress, uint16_t value) {
       return _node.writeSingleRegister(registerAddress, value);
     }
     
     /**
      * Read input registers
      * @param registerAddress Starting register address
      * @param numRegisters Number of registers to read
      * @param data Pointer to array where data will be stored
      * @return Modbus exception code (0 if success)
      */
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
     
     /**
      * Get the ModbusMaster node instance for direct access
      * @return Reference to the ModbusMaster node
      */
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
 