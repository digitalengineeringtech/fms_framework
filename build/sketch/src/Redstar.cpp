#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\Redstar.cpp"


#include "Redstar.h"

Redstar::Redstar(HardwareSerial& serial) : 
  _serial(serial),
  _debug(false),
  _timeout(1000),
  _bufferIndex(0),
  _lastCommandTime(0)
{
  memset(_buffer, 0, REDSTAR_BUFFER_SIZE);
  _lastResponse.valid = false;
}

void Redstar::begin(unsigned long baudRate, bool debug,uint8_t rxPin, uint8_t txPin) {
  _serial.begin(baudRate, SERIAL_8N1, rxPin, txPin);
  _debug = debug;
  // configure gpio for direct manipulation
  pinMode(txPin, OUTPUT);
  delay(100); // Give some time for serial to initialize
  if (_debug) {
    Serial.println(F("[DEBUG]Redstar: Initialized"));
  }
}

bool Redstar::readState(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Read command
  frame[2] = REDSTAR_SUBCMD_STATE;       // Read state code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent read_state frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

bool Redstar::readPrice(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Read command
  frame[2] = REDSTAR_SUBCMD_PRICE;       // Read price code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent read_price frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

bool Redstar::readTotal(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Read command
  frame[2] = REDSTAR_SUBCMD_TOTAL;       // Read total code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent read_total frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

bool Redstar::sendApproval(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Command
  frame[2] = REDSTAR_SUBCMD_APPROVAL;    // Approval code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent send_approval frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

bool Redstar::sendFuel(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Command
  frame[2] = REDSTAR_SUBCMD_FUEL;        // Fuel code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent send_fuel frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

bool Redstar::presetAmount(uint8_t nozzleId, uint16_t amount) {
  uint8_t frame[8];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_WRITE;          // Command MSB
  frame[2] = REDSTAR_SUBCMD_PRESET;      // Command LSB (preset amount)
  frame[3] = 0x00;                       // Reserved
  frame[4] = 0x00;                       // Reserved
  frame[5] = (amount >> 8) & 0xFF;       // Amount MSB
  frame[6] = amount & 0xFF;              // Amount LSB
  frame[7] = calculateChecksum(frame, 7); // Checksum
  
  if (_debug) {
    printFrame("Sent preset_amount frame:", frame, 8);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 8);
}

bool Redstar::setPrice(uint8_t nozzleId, uint16_t price) {
  uint8_t frame[8];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_WRITE;          // Command MSB
  frame[2] = REDSTAR_SUBCMD_SET_PRICE;   // Command LSB (Set Price)
  frame[3] = 0x00;                       // Reserved
  frame[4] = 0x00;                       // Reserved
  frame[5] = (price >> 8) & 0xFF;        // Price MSB
  frame[6] = price & 0xFF;               // Price LSB
  frame[7] = calculateChecksum(frame, 7); // Checksum
  
  if (_debug) {
    printFrame("Sent set_price frame:", frame, 8);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 8);
}

bool Redstar::getPrice(uint8_t nozzleId) {
  uint8_t frame[4];
  frame[0] = nozzleId;                   // Nozzle ID
  frame[1] = REDSTAR_CMD_READ;           // Command
  frame[2] = REDSTAR_SUBCMD_GET_PRICE;   // Get price code
  frame[3] = calculateChecksum(frame, 3); // Checksum
  
  if (_debug) {
    printFrame("Sent get_price frame:", frame, 4);
  }
  
  clearResponseBuffer();
  _lastCommandTime = millis();
  return sendFrame(frame, 4);
}

uint8_t Redstar::calculateChecksum(const uint8_t* bytes, size_t length) {
  uint16_t totalSum = 0;
  for (size_t i = 0; i < length; i++) {
    totalSum += bytes[i];
  }
  
  totalSum &= 0xFF;
  uint8_t complement = ~totalSum & 0xFF;
  uint8_t checksum = (complement + 1) & 0xFF;
  
  return checksum;
}

bool Redstar::sendFrame(const uint8_t* frame, size_t length) {
  if (!_serial) {
    if (_debug) {
      Serial.println(F("Redstar: Serial not initialized"));
    }
    return false;
  }
  uint8_t addressByte = frame[0];
  sendByteMarkParity(addressByte);
  delay(69);
  for (size_t i = 1; i < length; i++) {
    sendByteSpaceParity(frame[i]);
    delay(5); // adjust the frame lenht , address and command frame delay is 5 
  }
  
  return true;
}

void Redstar::printFrame(const char* prefix, const uint8_t* frame, size_t length) {
  if (!_debug) return;
  
  Serial.print(prefix);
  for (size_t i = 0; i < length; i++) {
    Serial.print(F(" 0x"));
    if (frame[i] < 16) {
      Serial.print(F("0"));
    }
    Serial.print(frame[i], HEX);
  }
  Serial.println();
}

bool Redstar::update() {
  // Process any incoming data from the dispenser
  while (_serial.available() && _bufferIndex < REDSTAR_BUFFER_SIZE) {
    uint8_t inByte = _serial.read();
    _buffer[_bufferIndex++] = inByte;
    
    // Check if we have a complete response
    // This is a simplified approach - you may need to adjust based on your protocol
    if (_bufferIndex >= 4) {
      // Check if we have a valid response by verifying the checksum
      uint8_t calculatedChecksum = calculateChecksum(_buffer, _bufferIndex - 1);
      if (calculatedChecksum == _buffer[_bufferIndex - 1]) {
        if (_debug) {
          printFrame("Received valid frame:", _buffer, _bufferIndex);
        }
        return parseResponse();
      }
    }
  }
  
  return false;
}

bool Redstar::parseResponse() {
  // Reset the last response
  _lastResponse.valid = false;
  _lastResponse.rawResponse = "";
  
  // Convert buffer to hex string for raw response
  for (uint8_t i = 0; i < _bufferIndex; i++) {
    if (_buffer[i] < 0x10) {
      _lastResponse.rawResponse += "0";
    }
    _lastResponse.rawResponse += String(_buffer[i], HEX);
    _lastResponse.rawResponse += " ";
  }
  
  // Basic validation - check minimum length
  if (_bufferIndex < 4) {
    if (_debug) {
      Serial.println(F("Redstar: Response too short"));
    }
    return false;
  }
  
  // Extract nozzle ID
  _lastResponse.nozzleId = _buffer[0];
  
  // Determine response type based on command byte
  uint8_t command = _buffer[1];
  uint8_t subCommand = _buffer[2];
  
  if (command == REDSTAR_CMD_READ) {
    if (subCommand == REDSTAR_SUBCMD_STATE) {
      // State response
      if (_bufferIndex >= 5) {
        uint8_t stateValue = _buffer[3];
        
        // Map state value to enum
        switch (stateValue) {
          case 0x01:
            _lastResponse.state = REDSTAR_STATE_IDLE;
            break;
          case 0x02:
            _lastResponse.state = REDSTAR_STATE_NOZZLE_LIFTED;
            break;
          case 0x03:
            _lastResponse.state = REDSTAR_STATE_FUELING;
            break;
          case 0x04:
            _lastResponse.state = REDSTAR_STATE_FUELING_COMPLETE;
            break;
          default:
            _lastResponse.state = REDSTAR_STATE_UNKNOWN;
        }
        
        _lastResponse.valid = true;
      }
    } else if (subCommand == REDSTAR_SUBCMD_PRICE) {
      // Price response
      if (_bufferIndex >= 6) {
        _lastResponse.price = (_buffer[3] << 8) | _buffer[4];
        _lastResponse.valid = true;
      }
    } else if (subCommand == REDSTAR_SUBCMD_TOTAL) {
      // Total response
      if (_bufferIndex >= 16) {
        // Extract total volume (bytes 4-9)
        _lastResponse.totalVolume = hexArrayToDecimal(_buffer, 4, 9);
        
        // Extract total amount (bytes 10-15)
        _lastResponse.totalAmount = hexArrayToDecimal(_buffer, 10, 15);
        
        _lastResponse.valid = true;
      }
    }
  }
  
  if (_debug && _lastResponse.valid) {
    Serial.println(F("Redstar: Response parsed successfully"));
    Serial.print(F("Nozzle ID: "));
    Serial.println(_lastResponse.nozzleId);
    Serial.print(F("State: "));
    Serial.println(_lastResponse.state);
    Serial.print(F("Total Volume: "));
    Serial.println(_lastResponse.totalVolume);
    Serial.print(F("Total Amount: "));
    Serial.println(_lastResponse.totalAmount);
    Serial.print(F("Price: "));
    Serial.println(_lastResponse.price);
  }
  
  return _lastResponse.valid;
}

uint32_t Redstar::hexArrayToDecimal(const uint8_t* hexArray, int startIndex, int endIndex) {
  uint32_t result = 0;
  uint32_t multiplier = 1;
  
  // Process from least significant byte to most significant
  for (int i = startIndex; i <= endIndex; i++) {
    result += hexArray[i] * multiplier;
    multiplier *= 256; // Move to next byte position
  }
  
  return result;
}

String Redstar::hexArrayToString(const uint8_t* hexArray, int startIndex, int endIndex) {
  String result = "";
  bool firstNonZero = false;
  
  // Process from most significant byte to least significant
  for (int i = endIndex; i >= startIndex; i--) {
    // Skip leading zeros
    if (hexArray[i] == 0 && !firstNonZero) {
      continue;
    }
    
    firstNonZero = true;
    
    // Add leading zero for single digit hex values
    if (hexArray[i] < 0x10) {
      result += "0";
    }
    
    result += String(hexArray[i], HEX);
  }
  
  // If all bytes were zero, return "0"
  if (result.length() == 0) {
    result = "0";
  }
  
  return result;
}

void Redstar::clearResponseBuffer() {
  memset(_buffer, 0, REDSTAR_BUFFER_SIZE);
  _bufferIndex = 0;
  _lastResponse.valid = false;
}

RedstarResponse Redstar::getLastResponse() const {
  return _lastResponse;
}

void Redstar::setTimeout(unsigned long timeout) {
  _timeout = timeout;
}

bool Redstar::waitForResponse() {
  unsigned long startTime = millis();
  
  while ((millis() - startTime) < _timeout) {
    if (update()) {
      return true;
    }
    yield(); // Allow other processes to run
  }
  
  if (_debug) {
    Serial.println(F("Redstar: Response timeout"));
  }
  
  return false;
}

void Redstar::sendByteMarkParity(uint8_t byte) {
  // Set TX pin to HIGH (mark)
  digitalWrite(_serial->getTxPin(), HIGH);
  delayMicroseconds(100); // Adjust delay as needed
  int bitCount = countBits(byte);
  if(bitCount % 2 == 0) {
    _serial->end();
    _serial->begin(2400, SERIAL_8O1, _serial.getRxPin(), _serial.getTxPin());
  } else {
   _serial->end();
    _serial->begin(2400, SERIAL_8E1, _serial.getRxPin(), _serial.getTxPin());
  }
  _serial.write(byte);
}

void Redstar::sendByteSpaceParity(uint8_t byte) {
  // Set TX pin to LOW (space)
  digitalWrite(_serial->getTxPin(), LOW);
  delayMicroseconds(100); // Adjust delay as needed
  int bitCount = countBits(byte);
  if(bitCount % 2 == 0) {
    _serial->end();
    _serial->begin(2400, SERIAL_8E1, _serial.getRxPin(), _serial.getTxPin());
  } else {
   _serial->end();
    _serial->begin(2400, SERIAL_801, _serial.getRxPin(), _serial.getTxPin());
  }
  _serial.write(byte);
}

int Redstar::countBits(uint8_t byte) {
  int count = 0;
  for (int i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      count++;
    }
  }
  return count;
 }