#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\_fms_tatsuno.h"
#ifndef _FMS_TATSUNO_H
#define _FMS_TATSUNO_H

#include <Arduino.h>

// Debug logging configuration
#ifdef FMS_TATSUNO_DEBUG_OPEN
  #define FMS_TATSUNO_LOG_DEBUG(format, ...) Serial.print("[fms_tatsuno_fun.ino][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_TATSUNO_LOG_ERROR(format, ...) Serial.print("[fms_tatsuno_fun.ino][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_TATSUNO_LOG_HEX(format, ...) Serial.print("[fms_tatsuno_fun.ino][DEBUG]"); Serial.print(format,HEX); Serial.println()
#else
  #define FMS_TATSUNO_LOG_DEBUG(format, ...) 
  #define FMS_TATSUNO_LOG_ERROR(format, ...)
#endif

class TatsunoProtocol {
private:
    HardwareSerial& _serial;  // Reference to the HardwareSerial object for communication
    unsigned char _buffer[50];  // Buffer for incoming data
public:

    // Constructor using initializer list
    TatsunoProtocol(HardwareSerial& serial) : _serial(serial) {}
    
    void begin(unsigned long baudRate, bool debug = false, 
              uint8_t rxPin = RXD2, uint8_t txPin = TXD2) {
        _serial.begin(baudRate, SERIAL_8E1, rxPin, txPin);
        
        if (debug) {
            FMS_TATSUNO_LOG_DEBUG("Tatsuno Protocol Initialized");
            FMS_TATSUNO_LOG_DEBUG("Baud rate: %lu, RX Pin: %d, TX Pin: %d", 
                                 baudRate, rxPin, txPin);
        }
    }
    
    bool update() {
        // This method should be called in the main loop to process incoming data
        if (_serial.available()) {
            return true;
        } else {
            //FMS_TATSUNO_LOG_DEBUG("No data available");
            return false;
        }
    }
    
    unsigned char* parseResponse(int& length) {
        if (_serial.available()) {
            unsigned char byte = _serial.read();
                _buffer[length++] = byte;
        }
        return _buffer;  // Return the parsed buffer
    }
    
    // Add other public methods as needed
};

#endif // _FMS_TATSUNO_H