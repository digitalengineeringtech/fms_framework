

 #ifndef FMS_REDSTAR_h
 #define FMS_REDSTAR_h
 
 #include "Arduino.h"
 #include "HardwareSerial.h"
 
 class Redstar {
   public:
 
     Redstar(HardwareSerial& serial);
     void begin(unsigned long baudRate = 2400);
    
     // redstar code 
     uint8_t calculateChecksum(const uint8_t* bytes, size_t length);
     bool readState(uint8_t nozzleId);
     bool readPrice(uint8_t nozzleId);
     bool readTotal(uint8_t nozzleId);
     bool sendApproval(uint8_t nozzleId);
     bool sendFuel(uint8_t nozzleId);
     bool presetAmount(uint8_t nozzleId, uint16_t amount);
     bool setPrice(uint8_t nozzleId, uint16_t price);
     bool getPrice(uint8_t nozzleId);
     
     /**
      * Process incoming data from the dispenser
      * Call this method regularly in your loop()
      */
     void update();
     
   private:
     HardwareSerial& _serial;
     bool sendFrame(const uint8_t* frame, size_t length);
     void printFrame(const char* prefix, const uint8_t* frame, size_t length);
     void totalState(byte hexArray[],int size); 
     void totalAmount(byte hexArray[],int size);
     // convert a subset of a hex array to a string (indices 4 to 9)

   
 };
 
 #endif