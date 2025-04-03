class fmsLanfeng 
{
    private:
        uint8_t _rePin;
        uint8_t _dePin;
        uint8_t _rxPin;
        uint8_t _txPin;
        ModbusMaster _node;
        static void _preTransmission();
        static void _postTransmission();
        static uint8_t _staticRePin;
        static uint8_t _staticDePin;
        Stream *_serial; // Pointer to the serial stream for communication
        uint8_t _slaveId; // Slave ID for Modbus communication
    public:
        fmsLanfeng(uint8_t rePin, uint8_t dePin,Stream &serial){
            _rePin = rePin;
            _dePin = dePin;
            _serial = &serial; // Initialize the serial stream pointer
        }

        void init(Stream &serial, uint8_t slave) {
            setupPins(); // Setup the pins for RS485 communication
            _node.begin(slave, serial);
            _node.preTransmission(_preTransmission);
            _node.postTransmission(_postTransmission);
        }

        void setupPins() {
            pinMode(_rePin, OUTPUT);
            pinMode(_dePin, OUTPUT);

            digitalWrite(_rePin, LOW);
            digitalWrite(_dePin, LOW);
        }

        void _preTransmission() {
            digitalWrite(_rePin, HIGH);
            digitalWrite(_dePin, HIGH);
        }

        void _postTransmission() {
            digitalWrite(_rePin,LOW);
            digitalWrite(_dePin,LOW);
        }

        void onReceived(void (*isr)(void)) {
            _serial->onReceive(isr);
        }

        void processReceivedData() {
            while (_serial->available()) {
                yield();
                uint8_t byte = _serial->read();
                _node.receive(byte);
            }
        }

        float convert_float(uint32_t highReg,uint32_t lowReg){
            uint32_t data[2];
            float floatData;
            data[0] = (uint32_t)highReg << 16; // Shift high register to the left by 16 bits
            data[1] = (uint32_t)lowReg; // Low register remains as is 0x12340000 to 0x 0000 1234
            uint32_t combinedData = data[0] | data[1]; // Combine high and low registers
            memcpy(&floatData, &combinedData, sizeof(float)); // Copy the combined data into a float variable
            return floatData; // Return the float value
        }

        uint8_t*  readHoldingRegister(uint16_t registerAddress, uint8_t MAX_DATA_SIZE) {
            uint8_t data[MAX_DATA_SIZE];
            uint8_t result = _node.readHoldingRegisters(registerAddress, MAX_DATA_SIZE);
            if (result == _node.ku8MBSuccess) {
                for (int i = 0; i < MAX_DATA_SIZE; i++) {
                    uint16_t value = _node.getResponseBuffer(i); // Get the value from the response buffer
                    data[i] = (value != 0xFFFF)?(uint8_t)value:0x00;
                }
                return data;
            } else {
                FMS_LOG_DEBUG("Error reading holding registers: %02X", result);
                return 0;
            }
            
        }
}