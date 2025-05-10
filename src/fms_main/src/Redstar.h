/**
 * @file Redstar.h
 * @brief Communication library for Redstar fuel dispensers
 * 
 * This library provides an interface for communicating with Redstar fuel dispensers
 * using a serial protocol. It handles command construction, checksum calculation,
 * and response parsing.
 */

 #ifndef REDSTAR_H
 #define REDSTAR_H
 
 #include <Arduino.h>
 
 // Command codes
 #define REDSTAR_CMD_READ          0x03
 #define REDSTAR_CMD_WRITE         0x07
 
 // Subcommand codes
 #define REDSTAR_SUBCMD_APPROVAL   0x08
 #define REDSTAR_SUBCMD_STATE      0x12
 #define REDSTAR_SUBCMD_SET_PRICE  0x80
 #define REDSTAR_SUBCMD_GET_PRICE  0x80
 #define REDSTAR_SUBCMD_PRICE      0x8C
 #define REDSTAR_SUBCMD_TOTAL      0x8E
 #define REDSTAR_SUBCMD_FUEL       0x8F
 #define REDSTAR_SUBCMD_PRESET     0x89
 
 // Response buffer size
 #define REDSTAR_BUFFER_SIZE       32
 
 // Response states
 enum RedstarState {
   REDSTAR_STATE_UNKNOWN = 0,
   REDSTAR_STATE_IDLE = 1,
   REDSTAR_STATE_NOZZLE_LIFTED = 2,
   REDSTAR_STATE_FUELING = 3,
   REDSTAR_STATE_FUELING_COMPLETE = 4,
   REDSTAR_STATE_ERROR = 5
 };
 
 // Response structure
 struct RedstarResponse {
   bool valid;                  // Whether the response is valid
   uint8_t nozzleId;            // Nozzle ID
   RedstarState state;          // Current state
   uint32_t totalVolume;        // Total volume dispensed (in smallest unit)
   uint32_t totalAmount;        // Total amount (in smallest currency unit)
   uint16_t price;              // Price per unit (in smallest currency unit)
   String rawResponse;          // Raw response for debugging
 };
 
 class Redstar {
 public:
   Redstar(HardwareSerial& serial);
   void begin(unsigned long baudRate, bool debug = false);
   bool readState(uint8_t nozzleId);
   bool readPrice(uint8_t nozzleId);
   bool readTotal(uint8_t nozzleId);
   bool sendApproval(uint8_t nozzleId);
   bool sendFuel(uint8_t nozzleId);
   bool presetAmount(uint8_t nozzleId, uint16_t amount);
   bool setPrice(uint8_t nozzleId, uint16_t price);
   bool getPrice(uint8_t nozzleId);
   bool update();
   RedstarResponse getLastResponse() const;
   void clearResponseBuffer();
   void setTimeout(unsigned long timeout);
   bool waitForResponse();
 private:
   HardwareSerial& _serial;           // Serial interface
   bool _debug;                        // Debug mode flag
   unsigned long _timeout;             // Response timeout in milliseconds
   uint8_t _buffer[REDSTAR_BUFFER_SIZE]; // Response buffer
   uint8_t _bufferIndex;               // Current position in buffer
   unsigned long _lastCommandTime;     // Time of last command sent
   RedstarResponse _lastResponse;      // Last parsed response
   uint8_t calculateChecksum(const uint8_t* bytes, size_t length);
   bool sendFrame(const uint8_t* frame, size_t length);
   void printFrame(const char* prefix, const uint8_t* frame, size_t length);
   bool parseResponse();
   uint32_t hexArrayToDecimal(const uint8_t* hexArray, int startIndex, int endIndex);
   String hexArrayToString(const uint8_t* hexArray, int startIndex, int endIndex);
   int countBits(uint8_t byte);
   void sendByteMarkParity(uint8_t byte);
   void sendByteSpaceParity(uint8_t byte);
 };
 
 #endif // REDSTAR_H