# 🚀 ESP32 Memory Optimization Guide

Optimizing memory usage on the ESP32 ensures **efficient performance**, prevents **crashes**, and extends **device reliability**. Below are key strategies for **heap** and **stack** memory optimization.

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

## 🟢 5. Monitor and Debug Memory Usage
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

---

