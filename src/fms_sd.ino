
/*
  * fms_sd.cpp
  *
  *  Created on: 2020. 12. 10.
  *   author : thet htar khaing
*/
// bool fms_config_load_sd_test() {

// return true;
// }

// bool write_data_sd(char* input)
// {
//   //to write code to save data to sd.
//   //step 1. simple write
//   //step 2. encrypt and write
//   //setp 3. sd formarting (clicommand)


//   return true;
// }


// static void sd_task(void *arg) {
//   BaseType_t rc;
//   while(1) {
//     printf("sd task started \n");
//     /*
//     * Load config data from sd card
//     */
    
//     rc = xTaskNotify(heventTask, 3, eSetBits);
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     //write_data_sd("HELLO\n\r");
//     //
//   }
// }
#include <SPI.h>
#include <SdFat.h>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SD_CS_PIN 5  // Chip select pin for SD card adapter on ESP32
const char *configFile = "/config.txt";  // File to store WiFi credentials

// WiFi variables
String ssid = "POS_Server";
String password = "asdffdsa";

// Function prototypes
bool setupSD();
bool saveWiFiConfig(const String &ssid, const String &password);
bool loadWiFiConfig();
void connectToWiFi();
bool formatSDCard();
String encryptDecrypt(const String &data, char key = 'K');
bool writeEncryptedData(const char *filename, const String &data);
bool readAndDecryptFile(const char *filename);
static void sd_task(void *arg);

SdFat sd;
SdFile file;

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for Serial Monitor

    // Initialize SD card
    if (!setupSD()) {
        Serial.println("SD card setup failed! Halting...");
        return;
    }

    // Load WiFi credentials from SD card
    if (!loadWiFiConfig()) {
        Serial.println("Creating default config file with encrypted WiFi credentials...");
        if (!saveWiFiConfig("POS_Server", "asdffdsa")) {
            Serial.println("Error: Failed to create WiFi config file.");
            return;
        }
        Serial.println("Default WiFi config file created.");
    }

    // Connect to WiFi
    connectToWiFi();

    // Start FreeRTOS Task
    xTaskCreate(sd_task, "SD_Task", 4096, NULL, 1, NULL);
}

void loop() {
    // Nothing in the loop, as tasks handle everything.
}

// Function to initialize SD card
bool setupSD() {
    Serial.println("Initializing SD card...");

    if (!sd.begin(SD_CS_PIN, SPI_HALF_SPEED)) {
        Serial.println("SD card initialization failed!");
        return false;
    }
    Serial.println("SD card initialized.");

    return true;
}

// Function to format SD card (optional)
bool formatSDCard() {
    Serial.println("Formatting SD card...");
    if (!sd.remove(configFile)) {
        Serial.println("Error: Could not remove config file.");
        return false;
    }
    Serial.println("SD card formatted.");
    return true;
}

// Encrypt and Decrypt function
String encryptDecrypt(const String &data, char key) {
    String result = data;
    for (size_t i = 0; i < data.length(); i++) {
        result[i] = data[i] ^ key;  // XOR encryption
    }
    return result;
}

// Save WiFi credentials (Encrypted)
bool saveWiFiConfig(const String &ssid, const String &password) {
    Serial.println("Saving WiFi credentials to SD card...");

    if (!writeEncryptedData(configFile, "SSID=" + ssid + "\nPASSWORD=" + password)) {
        Serial.println("Error writing encrypted WiFi credentials!");
        return false;
    }

    Serial.println("WiFi credentials saved successfully.");
    return true;
}

// Load WiFi credentials (Decrypted)
bool loadWiFiConfig() {
    Serial.println("Loading WiFi credentials from SD card...");

    if (!file.open(configFile, O_READ)) {
        Serial.println("Config file not found.");
        return false;
    }

    String encryptedData;
    while (file.available()) {
        encryptedData += (char)file.read();
    }
    file.close();

    String decryptedData = encryptDecrypt(encryptedData, 'K');

    // Parse SSID and Password
    int ssidIndex = decryptedData.indexOf("SSID=");
    int passIndex = decryptedData.indexOf("PASSWORD=");

    if (ssidIndex == -1 || passIndex == -1) {
        Serial.println("Error: Invalid WiFi config format.");
        return false;
    }

    ssid = decryptedData.substring(ssidIndex + 5, passIndex - 1);
    password = decryptedData.substring(passIndex + 9);

    Serial.print("Loaded SSID: ");
    Serial.println(ssid);
    Serial.print("Loaded Password: ");
    Serial.println(password);

    return true;
}

// Function to write encrypted data to SD card
bool writeEncryptedData(const char *filename, const String &data) {
    Serial.println("Writing encrypted data to file...");

    if (file.open(filename, O_RDWR | O_CREAT | O_TRUNC)) {
        String encryptedData = encryptDecrypt(data, 'K');
        file.println(encryptedData);
        file.close();
        Serial.println("Encrypted data written successfully.");
        return true;
    } else {
        Serial.println("Error: Could not write to file!");
        return false;
    }
}

// Connect ESP32 to WiFi
void connectToWiFi() {
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid.c_str(), password.c_str());

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        Serial.print(".");
        delay(500);
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Check credentials.");
    }
}

// FreeRTOS Task for periodic SD card writes
static void sd_task(void *arg) {
    while (1) {
        Serial.println("SD Task running: Writing test data to SD card...");
        if (!writeEncryptedData("/log.txt", "Test Log Entry\n")) {
            Serial.println("Error writing to log file.");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));  // 5-second delay
    }
}



