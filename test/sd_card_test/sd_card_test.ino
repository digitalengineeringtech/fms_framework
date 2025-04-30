#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <JY901.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/************** Change Note **************/
/*
 * Baud Rate change to 57600 to match with Telemetry
 * 
 */

/*
NUG FLY Board with logging features
*/
/********************** Boards Parameters **************************************/
const int led1 =  25;// the number of the LED pin
const int led2 =  26;// the number of the LED pin
const int led3 =  27;// the number of the LED pin


/********************** Servos Parameters **************************************/
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
// Published values for SG90 servos; adjust if needed
int minUs = 500;
int maxUs = 2500;

// create four servo objects 
int servo1Pin = 12;
int servo2Pin = 13;
int servo3Pin = 15;
int servo4Pin = 16;

int servoDelay = 5;

int pos = 90;      // position in degrees
ESP32PWM pwm;

/********************** GPS **************************************/
TinyGPSPlus gps;


/********************** BLE Parameters **************************************/
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
bool convert = false;
String rxString = "";
std::string rxValue;       // rxValue gathers input data

// UART service UUID data
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

/********************** IMU Parameters **************************************/
float acc_X;
float acc_Y;
float acc_Z;
char str_acc_X[10];
char str_acc_Y[10];
char str_acc_Z[10];
float gyro_X;
float gyro_Y;
float gyro_Z;
char str_gyro_X[10];
char str_gyro_Y[10];
char str_gyro_Z[10];
float angle_X;
float angle_Y;
float angle_Z;
char str_angle_X[10];
char str_angle_Y[10];
char str_angle_Z[10];

int mag_X;
int mag_Y;
int mag_Z;

int pressure;
float altitude;
char str_altitude[10];

int gps_long;
float gps_long_deg;
int gps_latt;
float gps_latt_deg;

char str_gps_long[10];
char str_gps_long_degree[10];
char str_gps_latt[10];
char str_gps_latt_degree[10];

float gps_height;
float gps_yaw;
float gps_velocity;

char str_gps_height[10];
char str_gps_yaw[10];
char str_gps_velocity[10];



/********************** Start BLE Functions **************************************/
class BleServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class BleCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0)  {
      
      Serial.println(" ");
      rxString = "";
      Serial.print("Received data: ");
      for (int i = 0; i < rxValue.length(); i++) { 
        Serial.print(rxValue[i]);
        rxString = rxString + rxValue[i]; // build string from received data 
      } 
      convert = true;      // flag to invoke receiveCMD routine
    } 
  } 
}; 
/********************** Stop BLE Functions **************************************/



/********************** Start SD Card Functions **************************************/

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
/********************** END SD Card Functions **************************************/

void setup() 
{
  Serial.begin(57600);
  JY901.StartIIC();
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  /********************** Start BLE Setup **************************************/

  BLEDevice::init("ROBINHOOD"); // give the BLE device a name
  
  BLEServer *pServer = BLEDevice::createServer(); // create BLE server
  pServer->setCallbacks(new BleServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY);                    
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new BleCallbacks());
  
  pService->start(); // start the service

  pServer->getAdvertising()->start(); // start advertising
  Serial.println("Waiting a client connection to notify...");

  /********************** Stop BLE Setup **************************************/
  

  /********************** Start SD Card Setup **************************************/

  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    appendFile(SD, "/Data.csv","Time,AccX,AccY,AccZ,GyroX,GyroY,GyroZ,AngleX,AngleY,AngleZ,MagX,MagY,MagZ,Pressure,Altitude,Long,Long_Deg(m),Latt,Latt_Deg(m),GPS_Height(m),GPS_Yaw(m),GPS_Velocity(km//h)\n");


    /********************** END SD Card Setup **************************************/
} 


void loop() {


  char buf_IMU_value[500];

  JY901.GetTime();
  JY901.GetAcc();
  JY901.GetGyro();  
  JY901.GetAngle();
  JY901.GetMag();
  JY901.GetPress();
  JY901.GetLonLat();
  JY901.GetGPSV();

  acc_X=(float)JY901.stcAcc.a[0]/32768*16;
  acc_Y=(float)JY901.stcAcc.a[1]/32768*16;
  acc_Z=(float)JY901.stcAcc.a[2]/32768*16;

  dtostrf(acc_X, 3, 3, str_acc_X);
  dtostrf(acc_Y, 3, 3, str_acc_Y);
  dtostrf(acc_Z, 3, 3, str_acc_Z);

  gyro_X = (float)JY901.stcGyro.w[0]/32768*2000;
  gyro_Y = (float)JY901.stcGyro.w[1]/32768*2000;
  gyro_Z = (float)JY901.stcGyro.w[2]/32768*2000;

  dtostrf(gyro_X, 3, 3, str_gyro_X);
  dtostrf(gyro_Y, 3, 3, str_gyro_Y);
  dtostrf(gyro_Z, 3, 3, str_gyro_Z);

  angle_X = (float)JY901.stcAngle.Angle[0]/32768*180;
  angle_Y = (float)JY901.stcAngle.Angle[1]/32768*180;
  angle_Z = (float)JY901.stcAngle.Angle[2]/32768*180;

  dtostrf(angle_X, 3, 3, str_angle_X);
  dtostrf(angle_Y, 3, 3, str_angle_Y);
  dtostrf(angle_Z, 3, 3, str_angle_Z);

  mag_X = JY901.stcMag.h[0];
  mag_Y = JY901.stcMag.h[1];
  mag_Z = JY901.stcMag.h[2];

  pressure = JY901.stcPress.lPressure;
  altitude = (float)JY901.stcPress.lAltitude/100;

  dtostrf(altitude, 3, 3, str_altitude);

  gps_long = JY901.stcLonLat.lLon/10000000;
  gps_long_deg = (double)(JY901.stcLonLat.lLon % 10000000)/1e5;
  gps_latt = JY901.stcLonLat.lLat/10000000;
  gps_latt_deg = (double)(JY901.stcLonLat.lLat % 10000000)/1e5;

  dtostrf(gps_long_deg, 3, 3, str_gps_long_degree);
  dtostrf(gps_latt_deg, 3, 3, str_gps_latt_degree);

  gps_height = (float)JY901.stcGPSV.sGPSHeight/10;
  gps_yaw = (float)JY901.stcGPSV.sGPSYaw/10;
  gps_velocity = (float)JY901.stcGPSV.lGPSVelocity/1000;

  dtostrf(gps_height, 3, 3, str_gps_height);
  dtostrf(gps_yaw, 3, 3, str_gps_yaw);
  dtostrf(gps_velocity, 3, 3, str_gps_velocity);

  sprintf(buf_IMU_value, "20%d-%d-%d %d:%d:%d, %s, %s, %s, %s, %s, %s, %s, %s, %s, %d, %d, %d, %d , %s, %d, %s, %d, %s, %s, %s, %s \n", 
  JY901.stcTime.ucYear, JY901.stcTime.ucMonth, JY901.stcTime.ucDay,JY901.stcTime.ucHour ,JY901.stcTime.ucMinute ,JY901.stcTime.ucSecond, str_acc_X,str_acc_Y,str_acc_Z, str_gyro_X, str_gyro_Y, str_gyro_Z, str_angle_X, str_angle_Y, str_angle_Z, 
  mag_X, mag_Y, mag_Z, pressure, str_altitude,
  gps_long, str_gps_long_degree, gps_latt, str_gps_latt_degree,
  str_gps_height, str_gps_yaw, str_gps_velocity);

   Serial.println(buf_IMU_value);
  
  if (deviceConnected) {

      pCharacteristic->notify();          // send the value to the app!
      pCharacteristic->setValue(" ");     // send a space
      pCharacteristic->notify();
      Serial.println(" ");
      Serial.println(buf_IMU_value);
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
    
  }
  else
  {
    //Save in SD Card
    appendFile(SD, "/Data.csv", buf_IMU_value);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    
  }
  
  
  

}