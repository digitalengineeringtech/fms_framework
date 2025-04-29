#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\#define MUX_A 2.cpp"
#define MUX_A 2
#define MUX_B 3
#define MUX_INH 4

#define UART_TX 17
#define UART_RX 16

HardwareSerial MySerial(2);  // Use Serial2

void setup() {
  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_INH, OUTPUT);
  digitalWrite(MUX_INH, LOW);  // Enable mux
}

void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_A, channel & 0x01);
  digitalWrite(MUX_B, (channel >> 1) & 0x01);
  delay(1);  // Switch time
}

void loop() {
  for (int device = 0; device < 4; device++) {
    selectMuxChannel(device);
    MySerial.println("Hello device " + String(device));
    delay(1000);

    if (MySerial.available()) {
      String reply = MySerial.readStringUntil('\n');
      Serial.print("From device ");
      Serial.print(device);
      Serial.print(": ");
      Serial.println(reply);
    }
  }
}

// This code is for a multiplexer setup with 4 devices connected to a single serial port. It selects each device in turn, sends a message, and reads the response.
// The multiplexer is controlled by two pins (MUX_A and MUX_B) to select the device, and an inhibit pin (MUX_INH) to enable or disable the multiplexer.
// The serial communication is handled by a hardware serial port (Serial2) with a baud rate of 9600. The main loop iterates through each device, selects it, sends a message, and reads the response.
// The code uses a delay to allow time for the multiplexer to switch channels and for the devices to respond.
// The responses are printed to the main serial monitor (Serial) for debugging purposes.
// The code is designed to be run on an Arduino board with the appropriate hardware setup.
// Make sure to connect the devices to the correct pins and configure the multiplexer accordingly.
// The code can be modified to suit specific requirements, such as changing the baud rate or the number of devices connected to the multiplexer.
// Ensure that the devices are powered correctly and that the serial communication settings match those of the devices.
// The code is a simple example of how to use a multiplexer with serial communication in Arduino.
// It can be expanded or modified to include error handling, more complex communication protocols, or additional features as needed.
// The code is written in C++ and is compatible with the Arduino IDE. It can be uploaded to an Arduino board using the IDE's upload feature.
// The code is structured to be easy to read and understand, with comments explaining each part of the process.
// The use of functions for selecting the multiplexer channel and sending/receiving data helps to keep the code organized and modular.

// The code can be used as a starting point for more complex projects involving multiple devices and serial communication.

#include <HardwareSerial.h>

// Pin Definitions for Multiplexer (74H4052)
#define MUX_A    2    // Select line A (S0)
#define MUX_B    3    // Select line B (S1)
#define MUX_INH  4    // Enable pin (INH) - Active LOW

#define UART_TX  17   // ESP32 TX pin
#define UART_RX  16   // ESP32 RX pin

// Initialize HardwareSerial on Serial2
HardwareSerial MySerial(2);  

// Flag and buffer for received data
volatile bool dataReceived = false;
String receivedData = "";

// Max number of channels supported (for 74H4052, it supports 4 channels)
#define MAX_CHANNELS 4

// Function to handle incoming data (interrupt callback)
void rx_irq_interrupt() {
  // Read incoming byte and store it in the buffer
  char incomingByte = MySerial.read();
  receivedData += incomingByte;

  // If a full message is received (for example, end with newline), flag as received
  if (incomingByte == '\n') {
    dataReceived = true;
  }
}

// Function to select the multiplexer's channel (0-3)
void selectMuxChannel(uint8_t channel) {
  if (channel >= MAX_CHANNELS) {
    Serial.println("Invalid channel selected.");
    return;
  }

  switch (channel) {
    case 0:
      digitalWrite(MUX_A, LOW);
      digitalWrite(MUX_B, LOW);
      break;
    case 1:
      digitalWrite(MUX_A, LOW);
      digitalWrite(MUX_B, HIGH);
      break;
    case 2:
      digitalWrite(MUX_A, HIGH);
      digitalWrite(MUX_B, LOW);
      break;
    case 3:
      digitalWrite(MUX_A, HIGH);
      digitalWrite(MUX_B, HIGH);
      break;
  }
}

// Function to send data and wait for a reply
void sendAndWaitForReply(const char* message, uint8_t channel, unsigned long timeout) {
  // Select the channel on the multiplexer
  selectMuxChannel(channel);

  // Send the message to the device
  MySerial.println(message);
  Serial.print("Sent to Channel ");
  Serial.print(channel);
  Serial.println(": ");
  Serial.println(message);

  unsigned long startMillis = millis();
  while (millis() - startMillis < timeout) {  // Wait for response within timeout
    if (dataReceived) {  // If data is received
      Serial.print("Received from Channel ");
      Serial.print(channel);
      Serial.print(": ");
      Serial.println(receivedData);  // Process the reply
      dataReceived = false;  // Reset the flag
      receivedData = "";  // Clear the received data buffer
      return;  // Exit function if reply received
    }
  }

  // Timeout if no response
  Serial.print("Timeout waiting for reply from Channel ");
  Serial.println(channel);
}

// Function to send data to multiple channels based on the channel count
void sendToChannels(const char* message, uint8_t numChannels, unsigned long timeout) {
  for (int i = 0; i < numChannels; i++) {
    sendAndWaitForReply(message, i, timeout);
    delay(5000);  // Wait before sending the next command
  }
}

void setup() {
  // Set up Serial communication
  Serial.begin(115200);    // Serial Monitor
  MySerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX); // Serial2 for UART communication

  // Set up MUX control pins
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_INH, OUTPUT);
  digitalWrite(MUX_INH, LOW);  // Enable the mux

  // Set the rx_irq_interrupt as the callback for receiving data
  MySerial.onReceive(rx_irq_interrupt);

  Serial.println("Setup complete. Waiting for commands...");
}

void loop() {
  // Example: Send a command to each channel and wait for reply (Timeout of 2000ms or 2 seconds)
  uint8_t channelCount = 4;  // Set the number of channels you want to use (1 to 4 in this case)
  sendToChannels("Hello Device", channelCount, 2000);
  delay(5000);  // Wait before sending the next command to the channels
}


// test code 3
#include <HardwareSerial.h>

// Pin Definitions for Multiplexer (74H4052)
#define MUX_A    2    // Select line A (S0)
#define MUX_B    3    // Select line B (S1)
#define MUX_INH  4    // Enable pin (INH) - Active LOW

#define UART_TX  17   // ESP32 TX pin
#define UART_RX  16   // ESP32 RX pin

// Initialize HardwareSerial on Serial2 (you can change to any UART)
HardwareSerial MySerial(2);  

// Flag and buffer for received data
volatile bool dataReceived = false;
String receivedData = "";

// Max number of channels supported (for 74H4052, it supports 4 channels)
#define MAX_CHANNELS 4

// Function to handle incoming data (interrupt callback)
void fm_rx_irq_interrupt() {
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = MySerial.available();  // Serial available bytes
  MySerial.printf("Got bytes on serial: %d\n", size);
  
  // Read all available bytes and store them in the buffer
  while (MySerial.available() && bytes_received < sizeof(Buffer)) {
    yield();  // Allow background tasks to run
    Buffer[bytes_received] = MySerial.read();  // Store byte in buffer
    bytes_received++;
  }

  // If we have read any data, decode it
  if (bytes_received > 0) {
    MySerial.printf("\n UART2 data processed. Bytes received: %d\n\r", bytes_received);
    fms_uart2_decode(Buffer, bytes_received);  // Decode the received data
  }
}

// Function to decode the UART data
void fms_uart2_decode(uint8_t* data, uint32_t len) {
  // Print the raw byte data for debugging
  Serial.print("[FMSUART2] Received Data: ");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i], HEX);  // Print each byte in hex format
    Serial.print(" ");
  }
  Serial.println();

  // Example of how to process the data, depending on your protocol
  // FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data); // If data is string
  // Or process the data byte by byte
}

// Function to select the multiplexer's channel (0-3)
void selectMuxChannel(uint8_t channel) {
  if (channel >= MAX_CHANNELS) {
    Serial.println("Invalid channel selected.");
    return;
  }

  // Set the MUX pins based on the selected channel
  switch (channel) {
    case 0:
      digitalWrite(MUX_A, LOW);
      digitalWrite(MUX_B, LOW);
      break;
    case 1:
      digitalWrite(MUX_A, LOW);
      digitalWrite(MUX_B, HIGH);
      break;
    case 2:
      digitalWrite(MUX_A, HIGH);
      digitalWrite(MUX_B, LOW);
      break;
    case 3:
      digitalWrite(MUX_A, HIGH);
      digitalWrite(MUX_B, HIGH);
      break;
  }
}

// Function to send data and wait for a reply
void sendAndWaitForReply(const char* message, uint8_t channel, unsigned long timeout) {
  // Select the channel on the multiplexer
  selectMuxChannel(channel);

  // Send the message to the device
  MySerial.println(message);
  Serial.print("Sent to Channel ");
  Serial.print(channel);
  Serial.println(": ");
  Serial.println(message);

  unsigned long startMillis = millis();
  while (millis() - startMillis < timeout) {  // Wait for response within timeout
    if (dataReceived) {  // If data is received
      Serial.print("Received from Channel ");
      Serial.print(channel);
      Serial.print(": ");
      Serial.println(receivedData);  // Process the reply
      dataReceived = false;  // Reset the flag
      receivedData = "";  // Clear the received data buffer
      return;  // Exit function if reply received
    }
  }

  // Timeout if no response
  Serial.print("Timeout waiting for reply from Channel ");
  Serial.println(channel);
}

// Function to send data to multiple channels based on the channel count
void sendToChannels(const char* message, uint8_t numChannels, unsigned long timeout) {
  for (int i = 0; i < numChannels; i++) {
    sendAndWaitForReply(message, i, timeout);
    delay(5000);  // Wait before sending the next command
  }
}

void setup() {
  // Set up Serial communication
  Serial.begin(115200);    // Serial Monitor
  MySerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX); // Serial2 for UART communication

  // Set up MUX control pins
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_INH, OUTPUT);
  digitalWrite(MUX_INH, LOW);  // Enable the mux

  // Set the rx_irq_interrupt as the callback for receiving data
  MySerial.onReceive(fm_rx_irq_interrupt);

  Serial.println("Setup complete. Waiting for commands...");
}

void loop() {
  // Example: Send a command to each channel and wait for reply (Timeout of 2000ms or 2 seconds)
  uint8_t channelCount = 4;  // Set the number of channels you want to use (1 to 4 in this case)
  sendToChannels("Hello Device", channelCount, 2000);
  delay(5000);  // Wait before sending the next command to the channels
}

