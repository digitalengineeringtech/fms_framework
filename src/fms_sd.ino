
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
#include <SD.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SD_CS_PIN 5  // Chip select pin
const char *filename = "/test.txt";
extern TaskHandle_t heventTask;

// Function prototypes
bool fms_config_load_sd_test();
bool write_data_sd(const char* input);
static void sd_task(void *arg);
void formatSDCard();
void writeEncryptedData(const char *filename, const String &data);
void readAndDecryptFile(const char *filename);
String encryptDecrypt(const String &data, char key = 'K');

void setup_sd() {
    Serial.println("Initializing SD card...");

    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    formatSDCard();

    if (!SD.exists(filename)) {
        Serial.println("File does not exist. Creating and writing encrypted data...");
        writeEncryptedData(filename, "Hello, SD card!");
    }

    readAndDecryptFile(filename);

    xTaskCreate(sd_task, "SD_Task", 4096, NULL, 1, NULL);
}

bool fms_config_load_sd_test() {
    return true;
}

bool write_data_sd(const char* input) {
    Serial.println("Writing to SD card...");
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
        file.println(input);
        file.close();
        Serial.println("Data written successfully.");
        return true;
    } else {
        Serial.println("Error: Could not write to file!");
        return false;
    }
}

void formatSDCard() {
    Serial.println("Formatting SD card...");
    SD.remove(filename);
    Serial.println("SD card formatted.");
}

String encryptDecrypt(const String &data, char key) {
    String result = data;
    for (size_t i = 0; i < data.length(); i++) {
        result[i] = data[i] ^ key;
    }
    return result;
}

void writeEncryptedData(const char *filename, const String &data) {
    Serial.println("Writing encrypted data to file...");
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
        String encryptedData = encryptDecrypt(data, 'K');
        file.println(encryptedData);
        file.close();
        Serial.println("Encrypted data written successfully.");
    } else {
        Serial.println("Error: Could not write to file!");
    }
}

void readAndDecryptFile(const char *filename) {
    Serial.println("Reading file and decrypting contents...");
    File file = SD.open(filename);
    if (!file) {
        Serial.println("Error: Could not open file!");
        return;
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
}

static void sd_task(void *arg) {
    while (1) {
        Serial.println("SD task started");
        fms_config_load_sd_test();
        write_data_sd("HELLO\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

