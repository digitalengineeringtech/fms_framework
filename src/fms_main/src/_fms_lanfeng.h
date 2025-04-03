#ifndef _FMS_LANFENG_H
#define _FMS_LANFENG_H

#include <Arduino.h>
#include <ModbusMaster.h>
#include <math.h>

#define MAX485_DE 15
#define MAX485_RE 15


class lanfeng
{
public:
   void lanfengProtocol(uint8_t slaveId,HardwareSerial &serial,uint8_t baudrate = 9600);
private:
    ModbusMaster node;
    void preTransmission();
    void postTransmission();
    void setupPins();
    void initializeNode();
    uint8_t _slaveId;
    HardwareSerial &_serial;
    uint8_t _baudrate;
};
#endif