/**
 * Huangyang.h - Library for Huangyang protocol implementation
 * Created for ESP32 Arduino
 */

#ifndef HUANGYANG_H
#define HUANGYANG_H

#include <Arduino.h>

class HuangyangProtocol {
public:
  // Protocol constants
  static const uint8_t HEADER[2];
  static const uint8_t FOOTER[2];
  static const uint8_t HEADER_SIZE = 2;
  static const uint8_t FOOTER_SIZE = 2;
  
  // Command codes
  static const uint8_t CMD_INIT = 0x10;
  static const uint8_t CMD_SET_NOZZLE = 0x11;
  static const uint8_t CMD_START_FLOW = 0x12;
  static const uint8_t CMD_STOP_FLOW = 0x13;
  static const uint8_t CMD_GET_STATUS = 0x14;
  static const uint8_t CMD_SET_PRESSURE = 0x15;
  static const uint8_t CMD_SET_TEMPERATURE = 0x16;
  static const uint8_t CMD_CALIBRATE = 0x17;
  static const uint8_t CMD_EMERGENCY_STOP = 0x18;
  static const uint8_t CMD_RESET = 0x19;
  
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
  HuangyangProtocol();
  
  /**
   * Initialize the Huangyang protocol
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
   * Calculate checksum for a packet
   * @param buffer Packet buffer
   * @param length Packet length
   * @return Calculated checksum
   */
  uint8_t calculateChecksum(const uint8_t* buffer, uint8_t length);
  
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
const uint8_t HuangyangProtocol::HEADER[2] = {0xBB, 0x66};
const uint8_t HuangyangProtocol::FOOTER[2] = {0x66, 0xBB};

// Constructor
HuangyangProtocol::HuangyangProtocol() {
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

bool HuangyangProtocol::begin(uint8_t nozzleCount, HardwareSerial &serialPort) {
  _serial = &serialPort;
  _nozzleCount = nozzleCount;
  _currentNozzle = 0;
  _lastError = ERROR_NONE;
  _isInitialized = false;
  _rxIndex = 0;
  _packetReceived = false;
  
  // Send initialization command
  uint8_t data[1] = {_nozzleCount};
  
  debugPrint("Initializing Huangyang protocol...");
  
  if (sendCommand(CMD_INIT, data, 1)) {
    if (waitForResponse(CMD_INIT, _timeout)) {
      _isInitialized = true;
      debugPrint("Huangyang protocol initialized successfully");
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
      debugPrint("Initialization timeout");
    }
  } else {
    _lastError = ERROR_COMM;
    debugPrint("Failed to send initialization command");
  }
  
  return false;
}

void HuangyangProtocol::update() {
  if (!_serial || !_isInitialized) return;
  
  // Check for incoming data
  while (_serial->available() > 0) {
    uint8_t byte = _serial->read();
    
    // Add byte to buffer if there's space
    if (_rxIndex < RX_BUFFER_SIZE) {
      _rxBuffer[_rxIndex++] = byte;
      
      // Check if we have a complete packet
      if (_rxIndex >= 4 && // Minimum packet size: HEADER(2) + CMD(1) + LEN(1)
          _rxIndex >= (4 + _rxBuffer[3] + 1 + FOOTER_SIZE)) { // HEADER + CMD + LEN + DATA + CHECKSUM + FOOTER
        
        if (parsePacket()) {
          _packetReceived = true;
        } else {
          // Invalid packet, reset buffer
          _rxIndex = 0;
        }
      }
    } else {
      // Buffer overflow, reset
      _rxIndex = 0;
      _lastError = ERROR_COMM;
    }
  }
}

bool HuangyangProtocol::selectNozzle(uint8_t nozzleIndex) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  if (nozzleIndex >= _nozzleCount) {
    _lastError = ERROR_INVALID_PARAM;
    return false;
  }
  
  uint8_t data[1] = {nozzleIndex};
  
  if (sendCommand(CMD_SET_NOZZLE, data, 1)) {
    if (waitForResponse(CMD_SET_NOZZLE, _timeout)) {
      _currentNozzle = nozzleIndex;
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::startFlow(uint8_t flowRate) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  uint8_t data[2] = {_currentNozzle, flowRate};
  
  if (sendCommand(CMD_START_FLOW, data, 2)) {
    if (waitForResponse(CMD_START_FLOW, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::stopFlow() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  uint8_t data[1] = {_currentNozzle};
  
  if (sendCommand(CMD_STOP_FLOW, data, 1)) {
    if (waitForResponse(CMD_STOP_FLOW, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

uint8_t HuangyangProtocol::getStatus() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return 0xFF;
  }
  
  if (sendCommand(CMD_GET_STATUS, nullptr, 0)) {
    if (waitForResponse(CMD_GET_STATUS, _timeout)) {
      // Status is in the first data byte of the response
      if (_rxIndex >= 6) { // HEADER(2) + CMD(1) + LEN(1) + DATA(1+) + ...
        _status = _rxBuffer[4];
        return _status;
      }
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return 0xFF;
}

bool HuangyangProtocol::setPressure(uint8_t pressure) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  uint8_t data[2] = {_currentNozzle, pressure};
  
  if (sendCommand(CMD_SET_PRESSURE, data, 2)) {
    if (waitForResponse(CMD_SET_PRESSURE, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::setTemperature(uint8_t temperature) {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  uint8_t data[1] = {temperature};
  
  if (sendCommand(CMD_SET_TEMPERATURE, data, 1)) {
    if (waitForResponse(CMD_SET_TEMPERATURE, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::calibrate() {
  if (!_isInitialized) {
    _lastError = ERROR_NOT_READY;
    return false;
  }
  
  if (sendCommand(CMD_CALIBRATE, nullptr, 0)) {
    if (waitForResponse(CMD_CALIBRATE, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::emergencyStop() {
  // Emergency stop should work even if not fully initialized
  
  if (sendCommand(CMD_EMERGENCY_STOP, nullptr, 0)) {
    if (waitForResponse(CMD_EMERGENCY_STOP, _timeout)) {
      return true;
    } else {
      _lastError = ERROR_TIMEOUT;
    }
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

bool HuangyangProtocol::reset() {
  // Reset should work even if not fully initialized
  
  if (sendCommand(CMD_RESET, nullptr, 0)) {
    // After reset, we don't expect a response
    _isInitialized = false;
    return true;
  } else {
    _lastError = ERROR_COMM;
  }
  
  return false;
}

uint8_t HuangyangProtocol::getLastError() {
  return _lastError;
}

const char* HuangyangProtocol::getErrorMessage(uint8_t errorCode) {
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

void HuangyangProtocol::setTimeout(unsigned long timeoutMs) {
  _timeout = timeoutMs;
}

void HuangyangProtocol::setDebug(bool enable) {
  _debugEnabled = enable;
}

bool HuangyangProtocol::sendCommand(uint8_t command, const uint8_t* data, uint8_t dataLen) {
  if (!_serial) return false;
  
  // Calculate total packet size
  uint8_t packetSize = HEADER_SIZE + 1 + 1 + dataLen + 1 + FOOTER_SIZE;
  uint8_t* packet = new uint8_t[packetSize];
  
  if (!packet) {
    _lastError = ERROR_COMM;
    return false;
  }
  
  // Build packet
  uint8_t index = 0;
  
  // Header
  packet[index++] = HEADER[0];
  packet[index++] = HEADER[1];
  
  // Command
  packet[index++] = command;
  
  // Data length
  packet[index++] = dataLen;
  
  // Data
  if (data && dataLen > 0) {
    memcpy(&packet[index], data, dataLen);
    index += dataLen;
  }
  
  // Calculate checksum (XOR of all bytes)
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < index; i++) {
    checksum ^= packet[i];
  }
  packet[index++] = checksum;
  
  // Footer
  packet[index++] = FOOTER[0];
  packet[index++] = FOOTER[1];
  
  // Debug output
  if (_debugEnabled) {
    debugPrintHex("TX: ", packet, packetSize);
  }
  
  // Send packet
  _serial->write(packet, packetSize);
  
  delete[] packet;
  return true;
}

bool HuangyangProtocol::waitForResponse(uint8_t expectedCmd, unsigned long timeout) {
  unsigned long startTime = millis();
  _packetReceived = false;
  
  while ((millis() - startTime) < timeout) {
    update();
    
    if (_packetReceived) {
      _packetReceived = false;
      
      // Check if this is the response we're waiting for
      if (_rxIndex >= 3 && _rxBuffer[2] == expectedCmd) {
        return true;
      }
    }
    
    // Small delay to prevent hogging CPU
    delay(1);
  }
  
  return false;
}

bool HuangyangProtocol::parsePacket() {
  // Check minimum packet size
  if (_rxIndex < (HEADER_SIZE + 1 + 1 + 1 + FOOTER_SIZE)) {
    return false;
  }
  
  // Check header
  if (_rxBuffer[0] != HEADER[0] || _rxBuffer[1] != HEADER[1]) {
    return false;
  }
  
  // Get data length
  uint8_t dataLen = _rxBuffer[3];
  
  // Check if we have enough bytes
  if (_rxIndex < (HEADER_SIZE + 1 + 1 + dataLen + 1 + FOOTER_SIZE)) {
    return false;
  }
  
  // Calculate expected packet size
  uint8_t packetSize = HEADER_SIZE + 1 + 1 + dataLen + 1 + FOOTER_SIZE;
  
  // Check footer
  if (_rxBuffer[packetSize - 2] != FOOTER[0] || _rxBuffer[packetSize - 1] != FOOTER[1]) {
    return false;
  }
  
  // Verify checksum (XOR of all bytes except checksum and footer)
  uint8_t receivedChecksum = _rxBuffer[HEADER_SIZE + 1 + 1 + dataLen];
  uint8_t calculatedChecksum = 0;
  for (uint8_t i = 0; i < (HEADER_SIZE + 1 + 1 + dataLen); i++) {
    calculatedChecksum ^= _rxBuffer[i];
  }
  
  if (receivedChecksum != calculatedChecksum) {
    _lastError = ERROR_CHECKSUM;
    return false;
  }
  
  // Debug output
  if (_debugEnabled) {
    debugPrintHex("RX: ", _rxBuffer, packetSize);
  }
  
  return true;
}

uint8_t HuangyangProtocol::calculateChecksum(const uint8_t* buffer, uint8_t length) {
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < length; i++) {
    checksum ^= buffer[i];
  }
  return checksum;
}

void HuangyangProtocol::debugPrint(const char* msg) {
  if (_debugEnabled) {
    Serial.println(msg);
  }
}

void HuangyangProtocol::debugPrintHex(const char* msg, const uint8_t* data, uint8_t dataLen) {
  if (!_debugEnabled) return;
  
  Serial.print(msg);
  
  for (uint8_t i = 0; i < dataLen; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  
  Serial.println();
}

#endif // HUANGYANG_H