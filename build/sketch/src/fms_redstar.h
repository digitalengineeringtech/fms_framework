#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\fms_redstar.h"


 #ifndef FuelDispenser_h
 #define FuelDispenser_h
 
 #include "Arduino.h"
 #include "HardwareSerial.h"
 
 class FuelDispenser {
   public:
     /**
      * Constructor
      * @param serial Reference to HardwareSerial object (e.g., Serial, Serial1, Serial2)
      */
     FuelDispenser(HardwareSerial& serial);
     
     /**
      * Begin communication with specified baud rate
      * @param baudRate Communication speed (default: 9600)
      */
     void begin(unsigned long baudRate = 9600);
     
     /**
      * Calculate checksum for a byte array
      * @param bytes Pointer to byte array
      * @param length Length of the byte array
      * @return Calculated checksum byte
      */
     uint8_t calculateChecksum(const uint8_t* bytes, size_t length);
     
     /**
      * Read state from a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool readState(uint8_t nozzleId);
     
     /**
      * Read price from a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool readPrice(uint8_t nozzleId);
     
     /**
      * Read total from a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool readTotal(uint8_t nozzleId);
     
     /**
      * Send approval to a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool sendApproval(uint8_t nozzleId);
     
     /**
      * Send fuel command to a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool sendFuel(uint8_t nozzleId);
     
     /**
      * Preset amount for a specific nozzle
      * @param nozzleId ID of the nozzle
      * @param amount Amount to preset (16-bit integer)
      * @return true if command was sent successfully
      */
     bool presetAmount(uint8_t nozzleId, uint16_t amount);
     
     /**
      * Set price for a specific nozzle
      * @param nozzleId ID of the nozzle
      * @param price Price to set (16-bit integer)
      * @return true if command was sent successfully
      */
     bool setPrice(uint8_t nozzleId, uint16_t price);
     
     /**
      * Get price from a specific nozzle
      * @param nozzleId ID of the nozzle
      * @return true if command was sent successfully
      */
     bool getPrice(uint8_t nozzleId);
     
     /**
      * Process incoming data from the dispenser
      * Call this method regularly in your loop()
      */
     void update();
     
   private:
     HardwareSerial& _serial;
     
     /**
      * Send a frame to the dispenser
      * @param frame Pointer to the frame buffer
      * @param length Length of the frame
      * @return true if frame was sent successfully
      */
     bool sendFrame(const uint8_t* frame, size_t length);
     
     /**
      * Print frame to Serial for debugging
      * @param prefix Message prefix
      * @param frame Pointer to the frame buffer
      * @param length Length of the frame
      */
     void printFrame(const char* prefix, const uint8_t* frame, size_t length);
 };
 
 #endif