#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\_fms_lanfeng.h"
#ifndef _FMS_LANFENG_H
#define _FMS_LANFENG_H

/*
lanfeng protocol all state
author : trion
guideline : sir thiha kyaw
copyright : iih
*/

// v 0.1 (development features)
// to change (config for lanfeng)
// add slave id and address , multiple read features

#define USE_LANFENG

#include <Arduino.h>
#include <ModbusMaster.h>

// Debug logging macro - uncomment to enable debug logging
#define FMS_DEBUG
#ifdef FMS_DEBUG
  #define FMS_LOG_DEBUG(format, ...) Serial.print("[DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_LOG_DEBUG(format, ...)
#endif

#ifdef USE_LANFENG
class fmsLanfeng {
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

    float convert_float(uint16_t highReg, uint16_t lowReg) {  // get 32 bit register address to float
      uint32_t combinedData = ((uint32_t)highReg << 16) | lowReg;
      float floatData;
      memcpy(&floatData, &combinedData, sizeof(float));
      return floatData;
    }

    void readAllData(uint16_t registerAddress, uint8_t numRegisters) { // read all data from modbus
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

    uint32_t readPumpState(uint16_t registerAddress) { // read pump state for lanfeng modbus
      uint8_t result = _node.readHoldingRegisters(registerAddress, 1);
      if (result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
    }

    uint32_t readLiveData(uint16_t registerAddress, uint32_t* data) { // read live data from lanfeng modbus
      uint8_t result = _node.readHoldingRegisters(registerAddress, 2);
      if (result == _node.ku8MBSuccess) {
        for (int i = 0; i < 2; i++) {
          data[i] = _node.getResponseBuffer(i);
        }
        return 0x01;
      } else return result;
    }

    uint32_t readLivePrice(uint16_t registerAddress) { // read live price from lanfeng modbus
      uint8_t result = _node.readHoldingRegisters(registerAddress, 1);
      if (result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
    }

    uint32_t readTotalizerLiter(uint16_t registerAddress, uint32_t* data) { // read totallizer liter
      uint8_t result = _node.readHoldingRegisters(registerAddress, 2);
      if (result == _node.ku8MBSuccess) {
        for (int i = 0; i < 2; i++) {
          data[i] = _node.getResponseBuffer(i);
        }
        return 0x01;
      } else return result;
    }

    uint32_t readTotalizerAmount(uint16_t registerAddress, uint32_t* data) {
      uint8_t result = _node.readHoldingRegisters(registerAddress, 2);
      if (result == _node.ku8MBSuccess) {
        for (int i = 0; i < 2; i++) {
          data[i] = _node.getResponseBuffer(i);
        }
        return 0x01;
      } else return result;
    }

    uint32_t readSellLiter(uint16_t registerAddress, uint32_t* data) {
      uint8_t result = _node.readHoldingRegisters(registerAddress, 2);
      if (result == _node.ku8MBSuccess) {
        for (int i = 0; i < 2; i++) {
          data[i] = _node.getResponseBuffer(i);
        }
        return 0x01;
      } else return result;
    }

    uint32_t readSellLiterPerPrice(uint16_t registerAddress) {
      uint8_t result = _node.readHoldingRegisters(registerAddress, 1);
      if (result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
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
    // setlive price to lanfeng modbus
    uint32_t setLivePrice(uint16_t registerAddress, uint16_t highword, uint16_t lowword) {
      return setValue_helper(registerAddress, highword, lowword);
    }

    // 32 bit float to lanfeng modbus (split 16 bit high bye and low byte)
    uint32_t setValue_helper(uint16_t registerAddress, uint16_t highWord, uint16_t lowWord) {
      _node.beginTransmission(registerAddress);
      _node.send(highWord);
      _node.send(lowWord);
      uint8_t result = _node.writeMultipleRegisters();
      if (result == _node.ku8MBSuccess) {
        return 0x01; // Success
      } else {
        return result; // Error code
      }
    }
 // set pump on and off (optional features current not used in code)
    uint32_t setPumpState(uint16_t registerAddress, uint16_t value) {
      uint8_t result = _node.writeSingleRegister(registerAddress, value);
      if (result == _node.ku8MBSuccess) {
        return 0x01; // Success
      } else {
        return result; // Error code
      }
    }
    // read  permit status from lanfeng modbus (different address)
    uint32_t readPermit(uint16_t registerAddress) {
      uint8_t result = _node.readHoldingRegisters(registerAddress, 1);
      if (result == _node.ku8MBSuccess) return _node.getResponseBuffer(0);
      else return result;
    }

    uint32_t writeSingleRegister(uint16_t registerAddress, uint16_t value) {
      return _node.writeSingleRegister(registerAddress, value);
    }

    // read register for lanfeng modbus optional features (current not used in code)
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
