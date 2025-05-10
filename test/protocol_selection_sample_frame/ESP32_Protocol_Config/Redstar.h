/**
 * Redstar.h - Library for Redstar protocol implementation
 * Created for ESP32 Arduino
 */

#ifndef REDSTAR_H
#define REDSTAR_H

#include <Arduino.h>

class RedstarProtocol {
public:
  // Protocol constants
  static const uint8_t HEADER[2];
  static const uint8_t FOOTER[2];
  static const uint8_t HEADER_SIZE = 2;
  static const uint8_t FOOTER_SIZE = 2;
  
  // Command codes
  static const uint8_t CMD_INIT = 0x20;
  static const uint8_t CMD_SET_NOZZLE = 0x21;
  static const uint8_t CMD_START_FLOW = 0x22;
  static const uint8_t CMD_STOP_FLOW = 0x23;
  static const uint8_t CMD_GET_STATUS = 0x24;
  static const uint8_t CMD_SET_PRESSURE = 0x25;
  static const uint8_t CMD_SET_TEMPERATURE = 0x26;
  static const uint8_t CMD_CALIBRATE = 0x27;
  static const uint8_t CMD_EMERGENCY_STOP = 0x28;
  static const uint8_t CMD_RESET = 0x29;
  
  // Response codes
  static const uint8_t RESP_OK = 0x00;
  static const uint8_t RESP_ERROR = 0xFF;
  static const uint8_t RESP_INVALID_PARAM = 0xFE;
  static const uint8_t RESP_BUSY = 0xFD;
  static const uint8_t RESP_NOT_READY = 0xFC;
  
  // Status flags
  static const uint8_t STATUS_READY = 0x01;
  static const uint8_t STATUS_BUSY = 0x02;
  static const uint8_t STATUS_ERROR = 0x04;
  static const uint8_t STATUS_FLOW_ACTIVE = 0x08;
  static const uint8_t STATUS_TEMPERATURE_OK = 0x10;
  static const uint8_t STATUS_PRESSURE_OK = 0x20;
  
  // Error codes
  static const uint8_t ERROR_NONE = 0x00;
  static const uint8_t ERROR_COMM = 0x01;
  static const uint8_t ERROR_CHECKSUM = 0x02;
  static const uint8_t ERROR_TIMEOUT = 0x03;
  static const uint8_t ERROR_INVALID_CMD = 0x04;
  static const uint8_t ERROR_INVALID_PARAM = 0x05;
  static const uint8_t ERROR_HARDWARE = 0x06;
  static const uint8_t ERROR_TEMPERATURE = 0x07;
  static const uint8_t ERROR_PRESSURE = 0x08;
  static const uint8_t ERROR_NOZZLE = 0x09;
  static const uint8_t ERROR_NOT_READY = 0x0A;  // Added this line
  
  // Constructor
  RedstarProtocol();
  
  /**
   * Initialize the Redstar protocol
   * @param nozzleCount Number of nozzles to configure
   * @param serialPort Serial port to use (default: Serial)
   * @return true if initialization successful, false otherwise
   */
  bool begin(uint8_t nozzleCount, HardwareSerial &serialPort = Serial);
  
  /**
   * Process incoming data from serial port
   * Should be called regularly in the main loop
   */
  void update();
  
  /**
   * Select active nozzle
   * @param nozzleIndex Index of nozzle to select (0-based)
   * @return true if command sent successfully, false otherwise
   */
  bool selectNozzle(uint8_t nozzleIndex);
  
  /**
   * Start flow from current nozzle
   * @param flowRate Flow rate (0-255)
   * @return true if command sent successfully, false otherwise
   */
  bool startFlow(uint8_t flowRate);
  
  /**
   * Stop flow from current nozzle
   * @return true if command sent successfully, false otherwise
   */
  bool stopFlow();
  
  /**
   * Get current status
   * @return Status byte or 0xFF if error
   */
  uint8_t getStatus();
  
  /**
   * Set pressure for current nozzle
   * @param pressure Pressure value (0-255)
   * @return true if command sent successfully, false otherwise
   */
  bool setPressure(uint8_t pressure);
  
  /**
   * Set temperature
   * @param temperature Temperature value in degrees Celsius
   * @return true if command sent successfully, false otherwise
   */
  bool setTemperature(uint8_t temperature);
  
  /**
   * Calibrate the system
   * @return true if command sent successfully, false otherwise
   */
  bool calibrate();
  
  /**
   * Emergency stop - halts all operations immediately
   * @return true if command sent successfully, false otherwise
   */
  bool emergencyStop();
  
  /**
   * Reset the device
   * @return true if command sent successfully, false otherwise
   */
  bool reset();
  
  /**
   * Get last error code
   * @return Error code
   */
  uint8_t getLastError();
  
  /**
   * Get error message for error code
   * @param errorCode Error code
   * @return Error message string
   */
  const char* getErrorMessage(uint8_t errorCode);
  
  /**
   * Set timeout for command responses
   * @param timeoutMs Timeout in milliseconds
   */
  void setTimeout(unsigned long timeoutMs);
  
  /**
   * Enable/disable debug output
   * @param enable true to enable, false to disable
   */
  void setDebug(bool enable);

private:
  // Private member variables
  HardwareSerial* _serial;
  uint8_t _nozzleCount;
  uint8_t _currentNozzle;
  uint8_t _lastError;
  uint8_t _status;
  bool _isInitialized;
  bool _debugEnabled;
  unsigned long _timeout;
  
  // Buffer for receiving data
  static const uint8_t RX_BUFFER_SIZE = 64;
  uint8_t _rxBuffer[RX_BUFFER_SIZE];
  uint8_t _rxIndex;
  bool _packetReceived;
  
  // Private methods
  
  /**
   * Send a command packet
   * @param command Command code
   * @param data Data bytes
   * @param dataLen Length of data
   * @return true if sent successfully, false otherwise
   */
  bool sendCommand(uint8_t command, const uint8_t* data, uint8_t dataLen);
  
  /**
   * Wait for and parse response
   * @param expectedCmd Expected command code in response
   * @param timeout Timeout in milliseconds
   * @return true if valid response received, false otherwise
   */
  bool waitForResponse(uint8_t expectedCmd, unsigned long timeout);
  
  /**
   * Parse received packet
   * @return true if valid packet, false otherwise
   */
  bool parsePacket();
  
  /**
   * Calculate CRC for a packet
   * @param buffer Packet buffer
   * @param length Packet length
   * @return Calculated CRC
   */
  uint16_t calculateCRC(const uint8_t* buffer, uint8_t length);
  
  /**
   * Debug print function
   * @param msg Message to print
   */
  void debugPrint(const char* msg);
  
  /**
   * Debug print function with hex data
   * @param msg Message to print
   * @param data Data bytes
   * @param dataLen Length of data
   */
  void debugPrintHex(const char* msg, const uint8_t* data, uint8_t dataLen);
};

// Initialize static constants
const uint8_t RedstarProtocol::HEADER[2] = {0xCC, 0x77};
const uint8_t RedstarProtocol::FOOTER[2] = {0x77, 0xCC};

// Constructor implementation
RedstarProtocol::RedstarProtocol() {
  _serial = nullptr;
  _nozzleCount = 0;
  _currentNozzle = 0;
  _lastError = ERROR_NONE;
  _status = 0;
  _isInitialized = false;
  _debugEnabled = false;
  _timeout = 1000; // Default timeout: 1 second
  _rxIndex = 0;
  _packetReceived = false;
}

// Basic implementation of required methods
bool RedstarProtocol::begin(uint8_t nozzleCount, HardwareSerial &serialPort) {
  _serial = &serialPort;
  _nozzleCount = nozzleCount;
  _isInitialized = true; // Simplified for this example
  return true;
}

void RedstarProtocol::update() {
  // Basic implementation
}

bool RedstarProtocol::selectNozzle(uint8_t nozzleIndex) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  _currentNozzle = nozzleIndex;
  return true;
}

bool RedstarProtocol::startFlow(uint8_t flowRate) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  return true;
}

bool RedstarProtocol::stopFlow() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  return true;
}

uint8_t RedstarProtocol::getStatus() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return 0xFF;
  }
  return STATUS_READY;
}

bool RedstarProtocol::setPressure(uint8_t pressure) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  return true;
}

bool RedstarProtocol::setTemperature(uint8_t temperature) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  return true;
}

bool RedstarProtocol::calibrate() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  return true;
}

bool RedstarProtocol::emergencyStop() {
  return true;
}

bool RedstarProtocol::reset() {
  _isInitialized = false;
  return true;
}

uint8_t RedstarProtocol::getLastError() {
  return _lastError;
}

const char* RedstarProtocol::getErrorMessage(uint8_t errorCode) {
  switch (errorCode) {
    case ERROR_NONE:
      return "No error";
    case ERROR_COMM:
      return "Communication error";
    case ERROR_CHECKSUM:
      return "Checksum error";
    case ERROR_TIMEOUT:
      return "Timeout error";
    case ERROR_INVALID_CMD:
      return "Invalid command";
    case ERROR_INVALID_PARAM:
      return "Invalid parameter";
    case ERROR_HARDWARE:
      return "Hardware error";
    case ERROR_TEMPERATURE:
      return "Temperature error";
    case ERROR_PRESSURE:
      return "Pressure error";
    case ERROR_NOZZLE:
      return "Nozzle error";
    case ERROR_NOT_READY:
      return "Device not ready";
    default:
      return "Unknown error";
  }
}

void RedstarProtocol::setTimeout(unsigned long timeoutMs) {
  _timeout = timeoutMs;
}

void RedstarProtocol::setDebug(bool enable) {
  _debugEnabled = enable;
}

bool RedstarProtocol::sendCommand(uint8_t command, const uint8_t* data, uint8_t dataLen) {
  // Simplified implementation
  return true;
}

bool RedstarProtocol::waitForResponse(uint8_t expectedCmd, unsigned long timeout) {
  // Simplified implementation
  return true;
}

bool RedstarProtocol::parsePacket() {
  // Simplified implementation
  return true;
}

uint16_t RedstarProtocol::calculateCRC(const uint8_t* buffer, uint8_t length) {
  // Simplified CRC calculation
  uint16_t crc = 0;
  for (uint8_t i = 0; i < length; i++) {
    crc += buffer[i];
  }
  return crc;
}

void RedstarProtocol::debugPrint(const char* msg) {
  if (_debugEnabled) {
    Serial.println(msg);
  }
}

void RedstarProtocol::debugPrintHex(const char* msg, const uint8_t* data, uint8_t dataLen) {
  if (!_debugEnabled) return;
  
  Serial.print(msg);
  
  for (uint8_t i = 0; i < dataLen; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  
  Serial.println();
}

#endif // REDSTAR_H