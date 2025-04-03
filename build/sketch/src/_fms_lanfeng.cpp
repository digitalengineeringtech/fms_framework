#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\_fms_lanfeng.cpp"
#include "_fms_lanfeng.h"

#if USE_LANFENG

lanfeng::lanfeng(uint8_t slaveId, HardwareSerial &serial, uint8_t baudrate)
    : _slaveId(slaveId), _serial(serial), _baudrate(baudrate)
{
    setupPins();
    initializeNode();
}

void lanfeng::setupPins()
{
    pinMode(MAX485_DE, OUTPUT);
    pinMode(MAX485_RE, OUTPUT);
    digitalWrite(MAX485_DE, LOW);
    digitalWrite(MAX485_RE, LOW);
}

void lanfeng::initializeNode()
{
    _serial.begin(_baudrate,SERIAL_8N1, 16, 17); // RX, TX pins
    node.begin(_slaveId, _serial);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}

void lanfeng::preTransmission()
{
    digitalWrite(MAX485_DE, HIGH);
    digitalWrite(MAX485_RE, HIGH);
}

void lanfeng::postTransmission()
{
    digitalWrite(MAX485_DE, LOW);
    digitalWrite(MAX485_RE, LOW);
}

#endif