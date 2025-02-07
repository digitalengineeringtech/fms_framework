
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SD_CS_PIN 5  // Chip select pin
const char *filename = "/test.txt";
extern TaskHandle_t heventTask;

// Function prototypes
bool fms_config_load_sd_test();
bool write_data_sd(const char* input);
static void sd_task(void *arg);
bool formatSDCard();
bool writeEncryptedData(const char *filename, const String &data);
bool readAndDecryptFile(const char *filename);
String encryptDecrypt(const String &data, char key = 'K');

SdFat sd;
SdFile file;

// Setup SD card initialization
bool setup_sd() {
    Serial.println("Initializing SD card...");

    if (!sd.begin(SD_CS_PIN, SPI_HALF_SPEED)) {
        Serial.println("SD card initialization failed!");
        return false;
    }
    Serial.println("SD card initialized.");

    if (!formatSDCard()) {
        Serial.println("Error: SD card format failed!");
        return false;
    }

    if (!sd.exists(filename)) {
        Serial.println("File does not exist. Creating and writing encrypted data...");
        if (!writeEncryptedData(filename, "Hello, SD card!")) {
            Serial.println("Error writing encrypted data!");
            return false;
        }
    }

    if (!readAndDecryptFile(filename)) {
        Serial.println("Error reading and decrypting file!");
        return false;
    }

    xTaskCreate(sd_task, "SD_Task", 4096, NULL, 1, NULL);
    return true;
}

bool fms_config_load_sd_test() {
    return true;
}

bool write_data_sd(const char* input) {
    Serial.println("Writing to SD card...");
    if (file.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
        file.println(input);
        file.close();
        Serial.println("Data written successfully.");
        return true;
    } else {
        Serial.println("Error: Could not write to file!");
        return false;
    }
}

bool formatSDCard() {
    Serial.println("Formatting SD card...");
    if (!sd.remove(filename)) {
        Serial.println("Error: Could not remove file.");
        return false;
    }
    Serial.println("SD card formatted.");
    return true;
}

String encryptDecrypt(const String &data, char key) {
    String result = data;
    for (size_t i = 0; i < data.length(); i++) {
        result[i] = data[i] ^ key;
    }
    return result;
}

bool writeEncryptedData(const char *filename, const String &data) {
    Serial.println("Writing encrypted data to file...");
    if (file.open(filename, O_RDWR | O_CREAT)) {
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

bool readAndDecryptFile(const char *filename) {
    Serial.println("Reading file and decrypting contents...");
    if (!file.open(filename, O_READ)) {
        Serial.println("Error: Could not open file!");
        return false;
    }

    String encryptedData;
    while (file.available()) {
        encryptedData += (char)file.read();
    }
    file.close();

    Serial.print("Encrypted data: ");
    Serial.println(encryptedData);

    String decryptedData = encryptDecrypt(encryptedData, 'K');
    Serial.print("Decrypted data: ");
    Serial.println(decryptedData);

    return true;
}

static void sd_task(void *arg) {
    while (1) {
        Serial.println("SD task started");
        fms_config_load_sd_test();
        if (!write_data_sd("HELLO\n")) {
            Serial.println("Error writing data to SD.");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



