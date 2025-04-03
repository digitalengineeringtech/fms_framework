#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\src\\_fms_lanfeng.cpp"
#include "_fms_lanfeng.h"

#if USE_LANFENG

fms_lanfeng::fms_lanfeng(void)
{
    
}

void fms_lanfeng::init(uint8_t slave,Stream &serial)
{
    node.begin(slave, serial);
    setupPins(); // Setup the pins for RS485 communication
    
}
void fms_lanfeng::setupPins()
{
    pinMode(MAX485_DE, OUTPUT);
    pinMode(MAX485_RE, OUTPUT);

    digitalWrite(MAX485_DE, LOW);
    digitalWrite(MAX485_RE, LOW);
}



void fms_lanfeng::preTransmission()
{
    digitalWrite(MAX485_DE, HIGH);
    digitalWrite(MAX485_RE, HIGH);
}

void fms_lanfeng::postTransmission()
{
    digitalWrite(MAX485_DE, LOW);
    digitalWrite(MAX485_RE, LOW);
}

void fms_lanfeng::onReceived(void (*isr)(void))
{
    _serial.onReceive(isr);
}

void fms_lanfeng::processReceivedData()
{
    while (_serial.available())
    {
        yield();
        uint8_t byte = _serial.read();
        node.receive(byte);
    }
}


uint32_t* fms_lanfeng::readData(uint16_t registerAddress, uint8_t MAX_DATA_SIZE)
{
    uint8_t data[MAX_DATA_SIZE];
    uint8_t result = node.readHoldingRegisters(registerAddress, MAX_DATA_SIZE);
    if (result == node.ku8MBSuccess)
    {
        for(int i=0;i<MAX_DATA_SIZE;i++){
            if(node.getResponseBuffer(i) !=0xFFFF)   // remove the FFF some time serial data return FFF
            {
                data[i] = node.getResponseBuffer(i); // Store the received data in the array
            }
        }
        return data; // Return the pointer to the data array
    }
    else
    {
        return 0; // Error handling
    }
}

#endif