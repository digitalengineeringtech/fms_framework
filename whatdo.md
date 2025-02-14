# 🚀 ESP32 Memory Optimization Guide

Optimizing memory usage on the ESP32 ensures **efficient performance**, prevents **crashes**, and extends **device reliability**. Below are key strategies for **heap**, **stack**, and **GPIO** memory optimization.

---

## 🟢 1. Optimize Stack Memory (Avoid Overflows)
### ✅ Reduce Large Local Variables
- Large local variables take up **stack space**, increasing overflow risks.
- **Solution:** Move large variables to the **heap** instead.

**🔴 Bad (Stack Usage - Risky!)**
```cpp
void function() {
    char buffer[4096];  // Too big! Might cause stack overflow.
}
```
**🟢 Good (Heap Usage - Safer)**
```cpp
void function() {
    char *buffer = (char*) malloc(4096);
    if (buffer) {
        // Use buffer...
        free(buffer);
    }
}
```

### ✅ Increase Stack Size for Tasks (FreeRTOS)
If your **task crashes** due to stack overflow, increase its stack size in **xTaskCreate()**:
```cpp
xTaskCreatePinnedToCore(myTask, "MyTask", 4096, NULL, 1, NULL, 0);
```
📌 **Tip:** Use `uxTaskGetStackHighWaterMark(NULL);` to check **remaining stack space**.

---

## 🟢 2. Optimize Heap Memory (Prevent Leaks & Fragmentation)
### ✅ Use Heap Only When Necessary
Heap allocations (`malloc()`, `new`) consume memory **dynamically**. Unused allocations cause **memory leaks**.

**🔴 Bad (Heap Leaks - Never Freed!)**
```cpp
void allocateMemory() {
    int *arr = (int*) malloc(100 * sizeof(int));
    arr[0] = 42;  // Memory is never freed!
}
```
**🟢 Good (Proper Cleanup)**
```cpp
void allocateMemory() {
    int *arr = (int*) malloc(100 * sizeof(int));
    if (arr) {
        arr[0] = 42;
        free(arr);  // Free memory to avoid leaks!
    }
}
```
📌 **Tip:** Use `esp_get_free_heap_size();` to check available heap.

### ✅ Use PSRAM (External RAM) for Large Buffers
If your ESP32 has **PSRAM (ESP32-WROVER, ESP32-S3)**, use it for large allocations:
```cpp
#include "esp_heap_caps.h"
uint8_t *bigBuffer = (uint8_t*) heap_caps_malloc(50000, MALLOC_CAP_SPIRAM);
```
📌 **Tip:** Use PSRAM for **camera buffers, large arrays, and AI models**.

---

## 🟢 3. Optimize Flash Storage (Reduce NVS Usage)
### ✅ Minimize NVS (Non-Volatile Storage) Writes
Frequent NVS writes **wear out** flash memory. Store only necessary settings.
```cpp
#include <Preferences.h>
Preferences prefs;

void setup() {
    prefs.begin("my_app", false);  // Open NVS
    prefs.putInt("boot_count", prefs.getInt("boot_count", 0) + 1);
    prefs.end();  // Close to save space!
}
```
📌 **Tip:** Use `nvs_get_stats()` to monitor NVS usage.

---

## 🟢 4. Reduce String Usage (Avoid Dynamic `String` Objects)
ESP32’s `String` class causes **heap fragmentation** due to unpredictable resizing.

**🔴 Bad (`String` Causes Fragmentation)**
```cpp
String message = "Hello";
message += " World";  // Memory allocation every time it's modified!
```
**🟢 Good (Use Character Arrays)**
```cpp
char message[20] = "Hello";
strcat(message, " World");
```

---

## 🟢 5. Optimize GPIO Usage (Efficient Digital Output)
### ✅ Use `gpio_set_level()` Efficiently
Instead of using `digitalWrite()`, use `gpio_set_level()` for **faster** and **optimized** GPIO control:
```cpp
#include "driver/gpio.h"
#define LED_PIN GPIO_NUM_2

void setup() {
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void loop() {
    gpio_set_level(LED_PIN, 1); // Turn LED ON
    delay(500);
    gpio_set_level(LED_PIN, 0); // Turn LED OFF
    delay(500);
}
```
📌 **Tip:** `gpio_set_level()` is significantly faster than `digitalWrite()` on ESP32.

---

## 🟢 6. Monitor and Debug Memory Usage
### ✅ Check Heap and Stack Usage in Runtime
```cpp
void checkMemory() {
    Serial.printf("Free Heap: %d bytes\n", esp_get_free_heap_size());
    Serial.printf("Task Stack High Water: %d bytes\n", uxTaskGetStackHighWaterMark(NULL));
}
```

### ✅ Use ESP-IDF Memory Debugging Tools
Enable **heap debugging** in PlatformIO or ESP-IDF:
```cpp
heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
```

---

## 🔥 Final Summary: Key Optimization Techniques

| Problem 🚨         | Solution ✅ |
|--------------------|------------|
| Stack Overflow | Reduce local variable size, increase stack size |
| Memory Leaks | Always free `malloc()` allocations |
| Heap Fragmentation | Avoid `String`, use fixed-size char arrays |
| Flash Wear | Minimize NVS writes |
| Large Data Buffers | Use PSRAM on ESP32-WROVER |
| Slow GPIO Performance | Use `gpio_set_level()` instead of `digitalWrite()` |

---

# PubSubClient API Documentation

## 1. Include the Library

To use the **PubSubClient** library in your Arduino sketch, include the library as shown below:

```cpp
#include <PubSubClient.h>
```

## 2. Initializing the Client

The `PubSubClient` object can be initialized using the Ethernet or WiFi client as a transport layer:

### For WiFi:

```cpp
WiFiClient espClient;
PubSubClient client(espClient);
```

### For Ethernet:

```cpp
EthernetClient ethClient;
PubSubClient client(ethClient);
```

## 3. MQTT Connection Functions

- **setServer**: Set the broker's IP address (or hostname) and port.

```cpp
client.setServer("mqtt.example.com", 1883);
```

- **setCallback**: Set the callback function to handle incoming messages from the broker.

```cpp
client.setCallback(callback);
```

The callback function is defined like this:

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  // Handle message here
}
```

- **connect**: Connect to the MQTT broker with optional username and password.

```cpp
if (client.connect("client_id", "username", "password")) {
  // Successfully connected
}
```

- **connect with Last Will and Testament (LWT)**: Set a Last Will message when connecting to the broker.

```cpp
client.connect("client_id", "username", "password", "lwt_topic", 1, true, "LWT Message");
```

## 4. Publishing Messages

- **publish**: Publish a message to a topic.

```cpp
client.publish("topic", "message");
```

You can also publish messages with QoS (Quality of Service) level and retain flags:

- **QoS 1** (Message is guaranteed to be delivered at least once):

```cpp
client.publish("topic", "message", true, 1);
```

- **Retain flag**: If `true`, the broker will retain the message:

```cpp
client.publish("topic", "message", true, 0);
```

## 5. Subscribing to Topics

- **subscribe**: Subscribe to one or more topics.

```cpp
client.subscribe("topic");
```

- **subscribe with QoS**: Subscribe to a topic with a specified QoS level:

```cpp
client.subscribe("topic", 1);
```

- **unsubscribe**: Unsubscribe from a topic.

```cpp
client.unsubscribe("topic");
```

## 6. Loop and Connection Management

- **loop**: This function must be called repeatedly to maintain the connection to the MQTT broker and process incoming messages.

```cpp
client.loop();
```

- **connected**: Check if the client is connected to the broker.

```cpp
if (client.connected()) {
  // The client is connected
}
```

- **disconnect**: Disconnect the client from the broker.

```cpp
client.disconnect();
```

## 7. Additional Configuration Methods

- **setKeepAlive**: Set the keep-alive period (in seconds). The client will send a ping request to the broker if no other message is sent within this time.

```cpp
client.setKeepAlive(60); // 60 seconds
```

- **setBufferSize**: Set the buffer size for incoming messages.

```cpp
client.setBufferSize(1024); // 1024 bytes
```

- **setReconnecting**: Configure the automatic reconnection behavior.

```cpp
client.setReconnecting(true);
```

## 8. Last Will and Testament (LWT)

Set a last will message that is sent to a specific topic if the client unexpectedly disconnects:

```cpp
client.setWill("home/temperature", 0, true, "Offline");
```

## 9. Example Sketch

Here is an example of using **PubSubClient** with an ESP8266 or ESP32 to connect to an MQTT broker, subscribe to a topic, and handle incoming messages:

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "yourSSID";
const char* password = "yourPassword";

// MQTT broker settings
const char* mqtt_server = "mqtt.example.com";
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  setup_wifi();

  // Set MQTT server and callback function
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Connect to MQTT broker
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe("home/temperature");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  client.loop(); // Keep the connection alive
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  
  Serial.println("Connected!");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
}
```

## 10. Summary of Key PubSubClient Methods

- **client.setServer**: Set the broker address and port.
- **client.setCallback**: Set the callback function for incoming messages.
- **client.connect**: Connect to the MQTT broker.
- **client.publish**: Publish a message to a topic.
- **client.subscribe**: Subscribe to a topic.
- **client.unsubscribe**: Unsubscribe from a topic.
- **client.loop**: Keep the MQTT connection alive and handle messages.
- **client.disconnect**: Disconnect from the broker.
- **client.setWill**: Set the Last Will and Testament (LWT).
```


