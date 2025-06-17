
#ifdef USE_TATSUNO
/* tatsuno parameter */
#define RESPONSE_BUFFER_SIZE 50
// char* Buffer[RESPONSE_BUFFER_SIZE];  // Buffer for incoming data
#define DIR_PIN         22

String incommingMessage;
String incommingmsg1;
String sendMessage;
char ssidtemp[50];  // for WiFi.begin (SSID,        )
char passtemp[50];  
// for WiFi.begin (    , pass   )
// const char* ssidtemp = "POS_Server";
// const char* passtemp = "asdffdsa";

char ssidBuf[50];
char passBuf[50];
unsigned char showSSID[6]       = { 0X5A, 0XA5, 0X40, 0X82, 0X12, 0x00 };
unsigned char showPASS[6]       = { 0X5A, 0XA5, 0X40, 0X82, 0X13, 0x00 };
unsigned char page[9]           = { 0X5A, 0XA5, 0X07, 0X82, 0X00, 0X84, 0X5A, 0X01, 0X00 };  // Page change
unsigned char deviceary[8]      = { 0x5A, 0XA5, 0x05, 0X82, 0x31, 0x00, 0x00, 0x00 };
int wifitrytime                 = 0;
// to dispenser
uint8_t enq1[4]                 = { 0x04, 0x40, 0x51, 0x05 };
uint8_t enq2[4]                 = { 0x04, 0x41, 0x51, 0x05 };

// unsigned char ACK1[2] = { 0x10, 0x31 };
uint8_t ACK1[2]                 = { 0x10, 0x31 };

// unsigned char select1[4] = { 0x04, 0x40, 0x41, 0x05 };
// unsigned char select2[4] = { 0x04, 0x41, 0x41, 0x05 };

uint8_t select1[4]            = { 0x04, 0x40, 0x41, 0x05 };
uint8_t select2[4]            = { 0x04, 0x41, 0x41, 0x05 };

unsigned char EOT[1]              = { 0x04 };
unsigned char totalizerstatus1[7] = { 0x02, 0x40, 0x41, 0x32, 0x30, 0x03, 0x00 };
unsigned char totalizerstatus2[7] = { 0x02, 0x41, 0x41, 0x32, 0x30, 0x03, 0x01 };
unsigned char pump1statusary[7]   = { 0x02, 0x40, 0x41, 0x31, 0x35, 0x03, 0x06 };
unsigned char pump2statusary[7]   = { 0x02, 0x41, 0x41, 0x31, 0x35, 0x03, 0x07 };
// unsigned char Buffer[50];
uint8_t Buffer[50];
int i = 0;
// uint8_t CalculatedCRCdata[9] = { 0x02, 0x00, 0x51, 0x30, 0x30, 0x00, 0x00, 0x03, 0x00 };
uint8_t CalculatedCRCdata[9]      = { 0x02, 0x00, 0x51, 0x30, 0x30, 0x00, 0x00, 0x03, 0x00 };
uint8_t CalculatedApprodata[20]   = { 0x02, 0x41, 0x41, 0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x03, 0x30 };
uint8_t CalculatedPresetdata[20]  = { 0x02, 0x41, 0x41, 0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x03, 0x30 };

// uint8_t unitpriceary1[4] = { 0x32, 0x35, 0x31, 0x30 };
// uint8_t unitpriceary2[4] = { 0x32, 0x35, 0x31, 0x30 };

uint8_t unitpriceary1[4]    = { 0x30, 0x30, 0x30, 0x30 };
uint8_t unitpriceary2[4]    = { 0x30, 0x30, 0x30, 0x30 };
//mqtt
char pumpapprobuf[22]       = "detpos/local_server/1";
char pricechange[26]        = "detpos/local_server/price";
char pumppresetbuf[28]      = "detpos/local_server/preset";
char pumpfinalreloadbuf[23] = "detpos/device/Reload/1";  // reload add

// int devicenum = 1;
int pumpid1;
int pumpid2;
int pumpid3;
int pumpid4;
int pumpid5;
int pumpid6;
int pumpid7;
int pumpid8;
int nozzlenum;
int devicenum;

int pumpmqttnum;
char presetmqttary[11];
char pricechangeary[7];
char pumpapproArray[13];
char ppbuffer[50];
bool pumpapprocount = false;
bool zerocount = true;
bool myfirst = true;
int mqtttcount = 0;
bool pump1live = false;
bool pump2live = false;
int addresscount = 1;  // for pump address in looping
int pumpnum;
bool pumppresetcount = false;
char charArray[3];
char tempcharArray[1];
bool count04 = true;
#define hmi 23
bool hmivalue = false;
int crcPumpaddress = 1;
bool pumpapprocount2 = false;
char reqcancelbuf[18] = "detpos/device/req";
char reqcancelvalue[9] = "01cancel";
bool reqcancelcount1 = false;
bool reqcancelcount2 = false;
bool activecount = true;
long activetime;
long activetime1;
bool enqactivecount = true;
long enqactivetime;
long enqactivetime1;
bool pumplivefor1 = false;       // for select count
bool pumplivecountfor1 = false;  // for select count
bool pumplivefor2 = false;       // for select count
bool pumplivecountfor2 = false;  // for select count

//ak add
char reloadArry[3];
bool reloadcount = false;
bool reloadcheck = false;
char server_rpy[20] = "detpos/local_server";
char server_err[25] = "detpos/local_server/err";
char err_buffer[9];
String final_str;
unsigned long previousMillis = 0;
char server_rpy_ary[7];
int waitcount;
/* end tatsuno parameter */

/* tatsuno config pump setting parameter */
struct TatsunoConfig { 
  uint8_t devn;
  uint8_t noz;
  uint8_t pumpids[8];
};

TatsunoConfig tatsunoConfig = {
  .devn = 1,  // Device number
  .noz = 2,   // Nozzle number
  .pumpids = {1, 2, 0, 0, 0, 0, 0, 0} // Pump IDs
};

bool fms_save_tatsuno_config(TatsunoConfig& cfg);
bool fms_load_tatsuno_config(TatsunoConfig& cfg);

/*** end pump setting parameter ****/

void fms_tatsuno_init() {
  pinMode(DIR_PIN, OUTPUT); 
  tatsuno.begin(19200, true, RXD2, TXD2); 
  //fms_save_tatsuno_config(tatsunoConfig); /* Save Tatsuno configuration to NVS storage*/
  vTaskDelay(pdMS_TO_TICKS(100)); 
  fms_load_tatsuno_config(tatsunoConfig);   /* Load Tatsuno configuration from NVS storage */
  fms_tatsuno_device_setup();               /* Setup Tatsuno device*/
  enqactivetime1 = millis() / 1000;
}

/* save tatsuno config optional features curently remove this */
bool fms_save_tatsuno_config(TatsunoConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_p_config", false)) {
    FMS_LOG_ERROR("Failed to initialize NVS storage");
    return false;
  }
  
  fms_nvs_storage.putUChar("devn", cfg.devn);  
  fms_nvs_storage.putUChar("noz", cfg.noz);    

  char key[12];  // enough for "pumpidX" + '\0'
  for (int i = 0; i < 8; i++) {
    snprintf(key, sizeof(key), "pumpid%d", i + 1);
    fms_nvs_storage.putUChar(key, cfg.pumpids[i]);
  }

  fms_nvs_storage.end();
  FMS_LOG_INFO("Tatsuno configuration saved successfully");
  return true;
}

bool fms_load_tatsuno_config(TatsunoConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_d_config", true)) {
    FMS_LOG_ERROR("Failed to initialize NVS storage");
    return false;
  }
  
  cfg.devn = fms_nvs_storage.getUChar("devn", 1);
  cfg.noz = fms_nvs_storage.getUChar("noz", 2);

  char key[12];
  for (int i = 0; i < 8; i++) {
    snprintf(key, sizeof(key), "pumpid%d", i + 1);
    cfg.pumpids[i] = fms_nvs_storage.getUChar(key, i + 1);
  }

  fms_nvs_storage.end();

  // Update global or local variables if needed
  pumpid1 = cfg.pumpids[0];
  pumpid2 = cfg.pumpids[1];
  pumpid3 = cfg.pumpids[2];
  pumpid4 = cfg.pumpids[3];
  pumpid5 = cfg.pumpids[4];
  pumpid6 = cfg.pumpids[5];
  pumpid7 = cfg.pumpids[6];
  pumpid8 = cfg.pumpids[7];
  
  devicenum = cfg.devn;
  nozzlenum = cfg.noz;

  FMS_LOG_INFO("Tatsuno configuration loaded successfully");

  Serial.printf("%d  %d  %d  %d  %d  %d  %d  %d  %d  %d",
  devicenum,
  nozzlenum,
  pumpid1,
  pumpid2,
  pumpid3,
  pumpid4,
  pumpid5,
  pumpid6,
  pumpid7,
  pumpid8);
  return true;
}
/* end save tatsuno config */

void fms_tatsuno_protocol_main() {

  // if (!digitalRead(hmi)) hmivalue = true;
 hmivalue = false;  // Set to false for testing, change as needed
  if (hmivalue) {
    Serial.println("[fms_tatsuno_fun.ino]  HMI ");
    hmisetup();
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      if (!fms_mqtt_client.connected()) {
        //  serverConnectionIcon("disconnected");
        Serial.println("[fms_tatsuno_fun.ino]  Cloud disconnect");
        gpio_set_level(LED_GREEN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_GREEN,1);
        vTaskDelay(pdMS_TO_TICKS(100));
        myfirst = true;
  
      } else {
        // Serial.println("[fms_tatsuno_fun.ino]  Connected to the Cloud");
           gpio_set_level(LED_GREEN, LOW);
   
        if (myfirst) {
          sendenq(1);
          mainfun();
          myfirst = false;
          pricereqfun();
        }
        // pumpactive();
      }
    } else {
      Serial.println("[fms_tatsuno_fun.ino]  Not Connected");
    }
    mainfun();
  }
 
}

 void mainfun() {
  delay(3);
  if (tatsuno.update()) { 
    Buffer[i] = fms_uart2_serial.read();
    enqactivetime1 = millis() / 1000;
    // delay(10);
    vTaskDelay(pdMS_TO_TICKS(2)); // Adjusted delay for speed
    //delay(2); //speed change here
    Serial.printf("Buffer[%d] : 0x%02X", i ,  Buffer[i]);
    i++;
    if (Buffer[i - 1] == 0x00) i = 0;  // might delete later
    if (Buffer[i - 1] == 0x04) {
      // Serial.println(addresscount);
      // Serial.println("[fms_tatsuno_fun.ino]  get04");
      i = 0;

      addresscount++;  // enq for another pump
      Serial.printf("Address Count : %d",addresscount);
      Serial.println("[fms_tatsuno_fun.ino]  get04");

      // add reload fun

      if (reloadcount) {
        reloadfun();
        reloadcount = false;
      }

      // else
      switch (pumppresetcount) {
        case true:
          Serial.println("[fms_tatsuno_fun.ino] yep u got");
          pumppresetfun();
          pumppresetcount = false;

          // pumplivefor1 = true;
          break;
        case false:
          if (pumpapprocount) {
            pumapprofun();
            pumpapprocount2 = true;
            pumpapprocount = false;

            // pumplivefor1 = true;
          }

          else {

            if (addresscount == 2 && pump2live) {
              sendpumpstatus(2);
              // Serial.println("[fms_tatsuno_fun.ino]  i gety");
            } else if (addresscount == 1 && pump1live) {
              sendpumpstatus(1);
              Serial.println("[fms_tatsuno_fun.ino]  i gety");
            }
            else {
              Serial.println("[fms_tatsuno_fun.ino]  yep ");
              sendenq(addresscount);
              //loadoffadd
              // delay(100);  //speed
            }
          }
          break;
        default:
          // if nothing else matches, do the default
          // default is optional
          break;
      }

      if (addresscount >= 2) addresscount = 0;
    }
    else if (Buffer[i - 1] == 0x03) {  // GetdataFrom dispenser
      Buffer[i] = fms_uart2_serial.read();
      // delay(20); //speed
      delay(2); //speed
      // Serial.println("[fms_tatsuno_fun.ino]  getCRCdata");
      i = 0;
      //loadoffadd
      // delay(100);  //speed
      messageClassified();

    } else if (Buffer[i - 1] == 0x10) {  // Get ACK From dispenser
      Buffer[i] =  fms_uart2_serial.read();
      if (Buffer[i] == 0x31) {
        sendEOT();
      }
      i = 0;
    }
    // i++;
  } else pumpenqactive();
}

/* mqtt callback */
void tatsuno_pump_setting(char* topic, String payload){
 incommingMessage = payload;

  if (String(topic) == device_Id_topic) {
    DynamicJsonDocument doc(4096);  // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial.println(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    int pumpid1 = 0;
    int pumpid2 = 0;
    int pumpid3 = 0;
    int pumpid4 = 0;
    int pumpid5 = 0;
    int pumpid6 = 0;
    int pumpid7 = 0;
    int pumpid8 = 0;

    devicenum = doc["devicenum"].as<const int>();
    nozzlenum = doc["nozzlenum"].as<const int>();
    pumpid1 = doc["pumpid1"].as<const int>();
    pumpid2 = doc["pumpid2"].as<const int>();
    pumpid3 = doc["pumpid3"].as<const int>();
    pumpid4 = doc["pumpid4"].as<const int>();
    pumpid5 = doc["pumpid5"].as<const int>();
    pumpid6 = doc["pumpid6"].as<const int>();
    pumpid7 = doc["pumpid7"].as<const int>();
    pumpid8 = doc["pumpid8"].as<const int>();

Serial.printf("devicenum: %d ,", devicenum);
Serial.printf("nozzlenum: %d ,", nozzlenum);
Serial.printf("pumpid1: %d ,", pumpid1);
Serial.printf("pumpid2: %d ,", pumpid2);
Serial.printf("pumpid3: %d ,", pumpid3);
Serial.printf("pumpid4: %d ,", pumpid4);
Serial.printf("pumpid5: %d ,", pumpid5);
Serial.printf("pumpid6: %d ,", pumpid6);
Serial.printf("pumpid7: %d ,", pumpid7);
Serial.printf("pumpid8: %d ,", pumpid8);


    EEPROM.write(101, pumpid1);
    EEPROM.commit();
    EEPROM.write(102, pumpid2);
    EEPROM.commit();
    EEPROM.write(103, pumpid3);
    EEPROM.commit();
    EEPROM.write(104, pumpid4);
    EEPROM.commit();
    EEPROM.write(105, pumpid5);
    EEPROM.commit();
    EEPROM.write(106, pumpid6);
    EEPROM.commit();
    EEPROM.write(107, pumpid7);
    EEPROM.commit();
    EEPROM.write(108, pumpid8);
    EEPROM.commit();
    EEPROM.write(109, devicenum);
    EEPROM.commit();
    EEPROM.write(110, nozzlenum);
    EEPROM.commit();
    Serial.println("[fms_tatsuno_fun.ino]  yep all save");
  }


  if (String(topic) == String(pumpapprobuf)) {  // pump req appro
    // pumapprofun();
    rxledonoff();
    pumpapprocount = true;
    incommingmsg1 = incommingMessage;
  }


  if (String(topic) == "detpos/local_server/whreq") {  // whole req
    if (incommingMessage == String(devicenum)) {
      rxledonoff();
      // sendpermission = true;
      fms_mqtt_client.publish(whreqbuf, devicebuf);
      txledonoff();
    }
  }


  if (String(topic) == String(pumppresetbuf)) {  // preset change
    rxledonoff();
    incommingMessage.toCharArray(presetmqttary, incommingMessage.length() + 1);
    Serial.printf("preset is: %s", presetmqttary);


    Serial.println(presetmqttary[0], HEX);
    Serial.println(presetmqttary[1], HEX);

    charArray[0] = presetmqttary[0];
    charArray[1] = presetmqttary[1];

    pumpmqttnumchange();


    if (charArray[2] == 0x40) pumppresetcount = true;
    if (charArray[2] == 0x41) pumppresetcount = true;


    tempcharArray[0] = charArray[2];


    // if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x31) pumppresetcount = true;
    // if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x32) pumppresetcount = true;
  }

  if (String(topic) == String(pricechange)) {  // price change

    pricechangefun();
    rxledonoff();
  }

  if (String(topic) == String(reload_topic)) {
    // reloadfun();
    reloadcount = true;
  }

  if (String(topic) == String(server_rpy)) {
    incommingMessage.toCharArray(server_rpy_ary, incommingMessage.length() + 1);
    // Serial.println("[fms_tatsuno_fun.ino]  hey i am working");
    char temp_rp[4];

    for (int i = 0; i < 5; i++) {
      temp_rp[i] = server_rpy_ary[3 + i];
    }

    final_str = "";
    final_str = String(temp_rp);
  }

  if (String(topic) == String(Reset_topic)) {  // esp reset topic
    char resetid[3];
    incommingMessage.toCharArray(resetid, incommingMessage.length() + 1);
    Serial.println(resetid[1]);
    // delay(4000);
    if (atoi(resetid) == devicenum) {
      Serial.println("[fms_tatsuno_fun.ino]  yep");
      // ESP.restart();
    }
  }
}

void messageClassified() {
  // if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) {  // Incoming
  //   sendCalculatedCRC();
  //   }

  if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) {  // Incoming
    // sendCalculatedCRC();
    sendcrcfun();
  } else if (Buffer[1] == 0x40 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) {  // Incoming
    // sendCalculatedCRC();
    sendcrcfun();
  } else if (Buffer[3] == 0x36 && Buffer[4] == 0x30 && Buffer[5] == 0x31 && Buffer[6] == 0x30) {  // power on incomming
    sendACK1();
  } else if (Buffer[3] == 0x36 && Buffer[4] == 0x30 && Buffer[5] == 0x32 && Buffer[6] == 0x30) {  // power on incomming
    sendACK1();
  }

  // else if (Buffer[1] == 0x40 && Buffer[2] == 0x51 && Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x31) {  // pump req 1
  //   // pump req1
  //   // sendenq(1);
  //   pumpreqmqtt();
  //   sendACK1();
  // }

  // else if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x31) {  // pump req 2
  //   // pump req1
  //   // sendenq(1);
  //   pumpreqmqtt();
  //   sendACK1();
  // }

  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x31) {  // pump req 1
    // if (!pumpapprocount2) pumpreqmqtt();
    sendACK1();
    pumpreqmqtt();


    // if(pumplivefor1) pumplivecountfor1 = true;

    if (Buffer[1] == 0x40) {
      pump1live = true;
      pumplivecountfor1 = true;
      // if (pumplivefor1) pumplivecountfor1 = true;
    } else if (Buffer[1] == 0x41) {
      pump2live = true;
      pumplivecountfor2 = true;
      // if (pumplivefor2) pumplivecountfor2 = true;
    }

    //hmawbit edit
    // if (Buffer[1] == 0x40) {
    //   reqcancelcount1 = true;
    // } else if (Buffer[1] == 0x41) {
    //   reqcancelcount2 = true;
    // }

  }

  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x30) {  // pump req nozzle out
    // if (!pumpapprocount2) pumpreqmqtt();
    sendACK1();
    if (Buffer[1] == 0x40) {

      if (pumplivecountfor1) {

        pump1live = false;
        pumplivefor1 = true;
        cancelfinalsend();
      }
      pumplivecountfor1 = false;
    }


    if (Buffer[1] == 0x41) {

      if (pumplivecountfor2) {

        pump2live = false;
        pumplivefor2 = true;
        cancelfinalsend();
      }
      pumplivecountfor2 = false;
    }

    // if(pumplivecountfor1) {

    //   pump1live = false;
    //   pumplivefor1 = true;
    // }
    // pumplivecountfor1 = false;


    //hmawbiadd
    // if (Buffer[1] == 0x40 && reqcancelcount1) {
    //   reqcancelcount1 = false;

    //   mqttpumpidchange(pumpid1);

    //   reqcancelvalue[0] = ppbuffer[0];
    //   reqcancelvalue[1] = ppbuffer[1];


    //   client.publish(reqcancelbuf, reqcancelvalue);
    //   txledonoff();

    //   //extra add
    //   if(pump1live){
    //     finalsend();
    //   }


    // } else if (Buffer[1] == 0x41 && reqcancelcount2) {
    //   reqcancelcount2 = false;

    //   mqttpumpidchange(pumpid2);

    //   reqcancelvalue[0] = ppbuffer[0];
    //   reqcancelvalue[1] = ppbuffer[1];


    //   client.publish(reqcancelbuf, reqcancelvalue);
    //   txledonoff();

    //   //extra add
    //   if(pump2live){
    //     finalsend();
    //   }

    // }



    //nono

    // if(Buffer[1] == 0x40) sendenq(1);
    // else if(Buffer[1] == 0x41) sendenq(2);
  }


  // else if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x33) {
  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x33) {
    // sendenq(1);
    pplivemqtt();
    pumpapprocount2 = false;
    sendACK1();
  }

  // else if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x34) {
  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x34) {
    if (Buffer[1] == 0x40) {
      pumplivefor1 = false;
      pumplivecountfor1 = false;
    } else if (Buffer[1] == 0x41) {
      pumplivefor2 = false;
      pumplivecountfor2 = false;
    }

    // pumplivefor1 = false;
    // pumplivecountfor1 = false;

    // sendenq(1);
    // finalmqtt();
    finalsend();
  }

  else if (Buffer[3] == 0x36 && Buffer[4] == 0x30 && Buffer[5] == 0x33) {
    if (Buffer[1] == 0x40) {
      sendenq(1);
    } else if (Buffer[1] == 0x41) {
      sendenq(2);
    }
    
  }
}

void pumpreqmqtt() {
  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  ppbuffer[2] = 0x70;
  ppbuffer[3] = 0x65;
  ppbuffer[4] = 0x72;
  ppbuffer[5] = 0x6D;
  ppbuffer[6] = 0x69;
  ppbuffer[7] = 0x74;



  // Pump ID Change
  // if (Buffer[1] == 0x40) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x31;
  // } else if (Buffer[1] == 0x41) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x32;
  // }



  pumpidchange();
  fms_mqtt_client.publish(pumpreqbuf, ppbuffer);
  txledonoff();
}

void pplivemqtt() {

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  pumpidchange();

  // if (Buffer[1] == 0x40) pumpnum = 1;
  // else if (Buffer[1] == 0x41) pumpnum = 2;

  // if (pumpnum == 1) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x31;
  // } else if (pumpnum == 2) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x32;
  // }



  ppbuffer[2] = 'L';
  int y = 0;
  for (int j = 6; j < 12; j++) {
    if (j == 9) {
      ppbuffer[y + 3] = '.';
      y++;
      zerocount = false;  //add
    }

    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }
    if (Buffer[j] != 0x30) {
      zerocount = false;
    }

    if (Buffer[j] == 0x03) break;

    // if (j == 10) {
    //   ppbuffer[y + 3] = '.';
    //   y++;
    //   zerocount = false;  //add
    // }
    ppbuffer[y + 3] = Buffer[j];
    y++;
  }

  zerocount = true;
  ppbuffer[y + 3] = 'P';
  y++;
  for (int j = 17; j < 23; j++) {
    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }

    if (Buffer[j] != 0x30) {
      zerocount = false;
    }

    if (Buffer[j] == 0x03) break;
    ppbuffer[y + 3] = Buffer[j];
    y++;
  }
  zerocount = true;

  Serial.printf("ppbuffer : %s " ,ppbuffer);
  fms_mqtt_client.publish(pplive, ppbuffer);
  txledonoff();
}

void sendCalculatedCRC() {
  uint8_t unCalculatedCRCdata[6];
  for (int y = 0; y < 6; y++) {
    unCalculatedCRCdata[y] = Buffer[y + 3];
  }
  uint16_t polynomial = 0x8408;  // CRC-CCITT polynomial

  // uint16_t crc_result = calculate_crc(Buffer, data_length, polynomial);
  uint16_t crc_result         = calculate_crc(unCalculatedCRCdata, 6, polynomial);
  uint16_t crc_resultHighbyte = highByte(crc_result);
  uint16_t crc_resultLowbyte  = lowByte(crc_result);
  uint16_t crc_plus           = crc_resultHighbyte + crc_resultLowbyte;

  Serial.printf("crc final value %d \n",crc_plus);

  if (crc_plus > 255) {
    crc_plus = crc_plus - 256;
    Serial.println("[fms_tatsuno_fun.ino]  Warnning..................................................CRC is higher255");
  }



  Serial.println("[fms_tatsuno_fun.ino]  CRC without initial value: 0x");
  Serial.println(crc_result, HEX);

  String str;
  str = String(crc_plus, HEX);

  int str_len = str.length() + 1;

  char char_array[str_len];

  str.toCharArray(char_array, str_len);

  Serial.println(char_array[0], HEX);
  Serial.println(char_array[1], HEX);

  if(char_array[1] == 0x00){
    char_array[1] = char_array[0];
    char_array[0] = 0x30;
  }


  // CalculatedCRCdata[1] = Buffer[1];
  if (pumpnum == 1) CalculatedCRCdata[1] = 0x40;
  else if (pumpnum == 2) CalculatedCRCdata[1] = 0x41;

  if (char_array[0] == 0x66) CalculatedCRCdata[5] = 0x46;  // string A-F to byte A-F
  else if (char_array[0] == 0x65) CalculatedCRCdata[5] = 0x45;
  else if (char_array[0] == 0x64) CalculatedCRCdata[5] = 0x44;
  else if (char_array[0] == 0x63) CalculatedCRCdata[5] = 0x43;
  else if (char_array[0] == 0x62) CalculatedCRCdata[5] = 0x42;
  else if (char_array[0] == 0x61) CalculatedCRCdata[5] = 0x41;
  else CalculatedCRCdata[5] = char_array[0];

  if (char_array[1] == 0x66) CalculatedCRCdata[6] = 0x46;
  else if (char_array[1] == 0x65) CalculatedCRCdata[6] = 0x45;
  else if (char_array[1] == 0x64) CalculatedCRCdata[6] = 0x44;
  else if (char_array[1] == 0x63) CalculatedCRCdata[6] = 0x43;
  else if (char_array[1] == 0x62) CalculatedCRCdata[6] = 0x42;
  else if (char_array[1] == 0x61) CalculatedCRCdata[6] = 0x41;
  else CalculatedCRCdata[6] = char_array[1];

  // Serial.println(CalculatedCRCdata[0],HEX);
  // Serial.println(CalculatedCRCdata[1],HEX);
  // Serial.println(CalculatedCRCdata[2],HEX);
  // Serial.println(CalculatedCRCdata[3],HEX);
  // Serial.println(CalculatedCRCdata[4],HEX);
  // Serial.println(CalculatedCRCdata[5],HEX);
  // Serial.println(CalculatedCRCdata[6],HEX);
  // Serial.println(CalculatedCRCdata[7],HEX);

  CalculatedCRCdata[8] = BCCfun();

  // Serial.println(CalculatedCRCdata[8],HEX);

  // Final Sending to Dispenser
  //latest add
  // delay(100);  //speed

  delay(50); //speed

  digitalWrite(DIR_PIN, HIGH);
  // delay(20);
  fms_uart2_serial.write(CalculatedCRCdata, 9);
  delay(7);
  digitalWrite(DIR_PIN, LOW);
  Serial.println("[fms_tatsuno_fun.ino]  sending CRC");
  // delay(20);
}

unsigned char BCCfun() {
  unsigned char bccData[7];

  bccData[0] = CalculatedCRCdata[1];
  bccData[1] = CalculatedCRCdata[2];
  bccData[2] = CalculatedCRCdata[3];
  bccData[3] = CalculatedCRCdata[4];
  bccData[4] = CalculatedCRCdata[5];
  bccData[5] = CalculatedCRCdata[6];
  bccData[6] = CalculatedCRCdata[7];

  unsigned char lrc = 0x00;

  // Calculate LRC checksum
  for (int j = 0; j < sizeof(bccData); j++) {
    lrc ^= bccData[j];
  }
  // Print LRC checksum in hexadecimal and decimal format

  if (lrc < 0x10) {
    Serial.println("[fms_tatsuno_fun.ino]  0");
  }
Serial.printf("LRC Checksum (Hex): 0x%02X", lrc);
Serial.printf("LRC Checksum (Decimal): %d", lrc);

  return lrc;
}

uint16_t calculate_crc(uint8_t* data, int length, uint16_t polynomial) {
  uint16_t crc = 0;  // Initial value (or seed) is 0

  for (int i = 0; i < length; i++) {
    crc ^= data[i];  // XOR the CRC with the next byte

    for (int j = 0; j < 8; j++) {
      if (crc & 0x01) {
        crc = (crc >> 1) ^ polynomial;
      } else {
        crc >>= 1;
      }
    }
  }

  return crc;
}

void BufferClear() {
  for (int j = 0; j < 50; j++) Buffer[j] = 0x00;
}

void pumpactive() {

  activetime = millis() / 1000;

  if ((activetime - activetime1) > 5) {
    if (activecount) {
      Serial.println("[fms_tatsuno_fun.ino]  active");
      Serial.printf("topic : [%s]", activebuf);
      fms_mqtt_client.publish(activebuf, "1");
      txledonoff();
      activecount = false;
    }
    activetime1 = millis() / 1000;
  } else activecount = true;
}

void pumpenqactive() {
  txledonoff();
  enqactivetime = millis() / 1000;

  if ((enqactivetime - enqactivetime1) > 3) {
    if (enqactivecount) {
      Serial.println("[fms_tatsuno_fun.ino]  enqactive");
      sendenq(1);
      enqactivecount = false;
    }
    enqactivetime1 = millis() / 1000;
  } else enqactivecount = true;
}

// void sendfun() {
//   digitalWrite(DIR_PIN, HIGH);
//   delay(20);
//   fms_uart2_serial.write(enq1, 4);
//   Serial.println("[fms_tatsuno_fun.ino]  sending ");
//   delay(20);
// }

void fms_tatsuno_device_setup() {

  if (devicenum == 1) {
    //pumpreqbuf

    pumpreqbuf[21] = '1';
    pumpapprobuf[20] = '1';
    ppfinal[20] = '1';
    pplive[23] = '1';
    devicebuf[0] = '1';
    activebuf[21] = '1';
    pricereqbuf[23] = '1';
    reload_topic[27] = '1';
    pumpfinalreloadbuf[21] = '1';
    // rdybuf[20] = '1';
    device_Id_topic[38] = '1';
  } else if (devicenum == 2) {
    pumpreqbuf[21] = '2';
    pumpapprobuf[20] = '2';
    ppfinal[20] = '2';
    pplive[23] = '2';
    devicebuf[0] = '2';
    activebuf[21] = '2';
    pricereqbuf[23] = '2';
    reload_topic[27] = '2';
    pumpfinalreloadbuf[21] = '2';
    // rdybuf[20] = '2';
    device_Id_topic[38] = '2';
  } else if (devicenum == 3) {
    pumpreqbuf[21] = '3';
    pumpapprobuf[20] = '3';
    ppfinal[20] = '3';
    pplive[23] = '3';
    devicebuf[0] = '3';
    activebuf[21] = '3';
    pricereqbuf[23] = '3';
    reload_topic[27] = '3';
    pumpfinalreloadbuf[21] = '3';
    // rdybuf[20] = '3';
    device_Id_topic[38] = '3';
  } else if (devicenum == 4) {
    pumpreqbuf[21] = '4';
    pumpapprobuf[20] = '4';
    ppfinal[20] = '4';
    pplive[23] = '4';
    devicebuf[0] = '4';
    activebuf[21] = '4';
    pricereqbuf[23] = '4';
    reload_topic[27] = '4';
    pumpfinalreloadbuf[21] = '4';
    // rdybuf[20] = '4';
    device_Id_topic[38] = '4';
  } else if (devicenum == 5) {
    pumpreqbuf[21] = '5';
    pumpapprobuf[20] = '5';
    ppfinal[20] = '5';
    pplive[23] = '5';
    devicebuf[0] = '5';
    activebuf[21] = '5';
    pricereqbuf[23] = '5';
    reload_topic[27] = '5';
    pumpfinalreloadbuf[21] = '5';
    // rdybuf[20] = '5';
    device_Id_topic[38] = '5';
  } else if (devicenum == 6) {
    pumpreqbuf[21] = '6';
    pumpapprobuf[20] = '6';
    ppfinal[20] = '6';
    pplive[23] = '6';
    devicebuf[0] = '6';
    activebuf[21] = '6';
    pricereqbuf[23] = '6';
    reload_topic[27] = '6';
    pumpfinalreloadbuf[21] = '6';
    // rdybuf[20] = '6';
    device_Id_topic[38] = '6';
  } else if (devicenum == 7) {
    pumpreqbuf[21] = '7';
    pumpapprobuf[20] = '7';
    ppfinal[20] = '7';
    pplive[23] = '7';
    devicebuf[0] = '7';
    activebuf[21] = '7';
    pricereqbuf[23] = '7';
    reload_topic[27] = '7';
    pumpfinalreloadbuf[21] = '7';
    // rdybuf[20] = '7';
    device_Id_topic[38] = '7';
  } else if (devicenum == 8) {
    pumpreqbuf[21] = '8';
    pumpapprobuf[20] = '8';
    ppfinal[20] = '8';
    pplive[23] = '8';
    devicebuf[0] = '8';
    activebuf[21] = '8';
    pricereqbuf[23] = '8';
    reload_topic[27] = '8';
    pumpfinalreloadbuf[21] = '8';
    // rdybuf[20] = '8';
    device_Id_topic[38] = '8';
  }
}

void sendenq(int eq) {
  // last add
  // delay(50);   
  delay(10);  //speed
  digitalWrite(DIR_PIN, HIGH);
  // delay(20)
  delay(10);  //speed
  if (eq == 1) fms_uart2_serial.write(enq1, sizeof(enq1));
  else if (eq == 2) fms_uart2_serial.write(enq2, sizeof(enq2));
  Serial.printf("[fms_tatsuno_fun.ino] SendEnq : %d \n", eq);
  delay(3.5);
  // delay(6.8);
  // delay(4);
  digitalWrite(DIR_PIN, LOW);
  //last add
  // delay(20); //speed
}

void sendACK1() {
  digitalWrite(DIR_PIN, HIGH);
  fms_uart2_serial.write(ACK1, sizeof(ACK1));
  Serial.println("[fms_tatsuno_fun.ino]  sending ACK");

  delay(2);
  digitalWrite(DIR_PIN, LOW);
}

void sendEOT() {
  digitalWrite(DIR_PIN, HIGH);
  fms_uart2_serial.write(EOT, 1);  //eot
  Serial.println("[fms_tatsuno_fun.ino]  sending EOT ");
  delay(4);
  digitalWrite(DIR_PIN, LOW);

  // sendenq(1);
}

void txledonoff() {
  gpio_set_level(LED_BLUE,LOW);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // delay for 10 ms
  gpio_set_level(LED_BLUE, HIGH);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // delay for 10 ms
  
}

void rxledonoff() {
  gpio_set_level(LED_BLUE, LOW);
  vTaskDelay(20 / portTICK_PERIOD_MS);  // delay for 10 ms
  gpio_set_level(LED_BLUE, HIGH);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // delay for 10 ms
}
///// cancel final add
void cancelfinalsend() {

  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  pumpidchange();

  ppbuffer[2] = 'c';
  ppbuffer[3] = 'a';
  ppbuffer[4] = 'n';
  ppbuffer[5] = 'c';
  ppbuffer[6] = 'e';
  ppbuffer[7] = 'l';

  Serial.printf("ppbuffer : %s" ,ppbuffer);
  fms_mqtt_client.publish(reqcancelbuf, ppbuffer);
  txledonoff();
}

void finalsend() {

  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;


  finalmqtt1();


  sendACK1();

  digitalWrite(DIR_PIN, LOW);
  delay(30);
  if (fms_uart2_serial.available()) {
  int receivedByte = fms_uart2_serial.read();
  Serial.printf("U got: 0x%02X (%d)", receivedByte, receivedByte);
    delay(10);
  }

  if (pumpnum == 1) pump1Select();
  else if (pumpnum == 2) pump2Select();

  int count1;
  char charack0[2];

  digitalWrite(DIR_PIN, LOW);
  // delay(500);
  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);

    // if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
    //   pump1Totalizerstatus();
    // }
  }

  // if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
  // pump1Totalizerstatus();
  if (pumpnum == 1) pump1Totalizerstatus();
  else if (pumpnum == 2) pump2Totalizerstatus();
  // }

  // delay(50);
  digitalWrite(DIR_PIN, LOW);
  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got2 ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);
  }
  // if (charack0[0] == 0x10 && charack0[1] == 0x31) {  //
  sendenq(pumpnum);
  // }


  char totalizerary[33];
  int totalizercount = 0;
  digitalWrite(DIR_PIN, LOW);
  delay(50);
  while (fms_uart2_serial.available()) {

    totalizerary[totalizercount] =  fms_uart2_serial.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    Serial.printf("%d // 0x%02X , ", totalizercount, totalizerary[totalizercount]);
    delay(10);
    if (totalizerary[totalizercount] == 0x03) {
      totalizerary[totalizercount + 1] =  fms_uart2_serial.read();
      delay(10);
      Serial.println("[fms_tatsuno_fun.ino]  ");
      break;
    }
    totalizercount++;
  }

  // finalmqtt2
  while (totalizerary[4] != 0x35) {  // wait for totalizer

    sendACK1();
    digitalWrite(DIR_PIN, LOW);
    delay(30);
    if (fms_uart2_serial.available()) {
   char ch = fms_uart2_serial.read();
    Serial.printf("U got char: %c (0x%02X)\n", ch, ch);

    }
    sendenq(pumpnum);

    totalizercount = 0;

    digitalWrite(DIR_PIN, LOW);
    delay(50);
    while (fms_uart2_serial.available()) {

      totalizerary[totalizercount] =  fms_uart2_serial.read();
      Serial.printf("%d // 0x%02X", totalizercount, totalizerary[totalizercount]);
      // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
      delay(10);
      if (totalizerary[totalizercount] == 0x03) {
        totalizerary[totalizercount + 1] =  fms_uart2_serial.read();
        delay(10);
        Serial.println("[fms_tatsuno_fun.ino]   ");
        break;
      }
      totalizercount++;
    }
  }


  ppbuffer[mqtttcount] = 'T';

  int y = mqtttcount + 1;

  for (int j = 11; j < 21; j++) {

    if (j == 18) {
      ppbuffer[y] = '.';
      y++;
      zerocount = false;
    }

    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }
    if (Buffer[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = totalizerary[j];
    y++;
  }


  zerocount = true;
  ppbuffer[y] = 'A';
  y++;
  // Serial.println("[fms_tatsuno_fun.ino]  my j is");

  for (int j = 21; j < 31; j++) {
    // Serial.println(j);
    if (totalizerary[j] == 0x30 && zerocount) {
      // Serial.println("[fms_tatsuno_fun.ino]  zero");
      continue;
    }
    if (totalizerary[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = totalizerary[j];
    // Serial.println(j);
    // Serial.println(ppbuffer[y]);
    y++;
  }

  Serial.println("[fms_tatsuno_fun.ino]  ");
  zerocount = true;
  Serial.printf("ppbuffer : %s" ,ppbuffer);
  fms_mqtt_client.publish(ppfinal, ppbuffer);
  txledonoff();

  for (int i = 0; i < 10; i++) {
    err_buffer[i] = 0;
  }

  err_buffer[0] = ppbuffer[0];
  err_buffer[1] = ppbuffer[1];
  err_buffer[2] = '/';
  err_buffer[3] = 'e';
  err_buffer[4] = 'r';
  err_buffer[5] = 'r';
  err_buffer[6] = 'o';
  err_buffer[7] = 'r';

  waitcount = 0;
  previousMillis = millis() / 1000;
  while (waitcount < 2) {


    if (ppbuffer[0] == server_rpy_ary[0] && ppbuffer[1] == server_rpy_ary[1] && final_str == "D1S1") {
      Serial.println("[fms_tatsuno_fun.ino]  Bye ....................................");
      final_str = "";
      break;
    }


    if (millis() / 1000 - previousMillis >= 13) {

      if (waitcount < 1) {
        fms_mqtt_client.publish(ppfinal, ppbuffer);
        if(!fms_mqtt_client.connected()) {
          Serial.println("[fms_tatsuno_fun.ino] save to sd storage final message");
        }
      }

      // client.publish(pumpfinalbuf, ppbuffer);

      previousMillis = millis() / 1000;
      waitcount++;
    }

    //fms_mqtt_client.loop();
  }

  if (waitcount == 2) fms_mqtt_client.publish(server_err, err_buffer);

  sendACK1();
  if (pumpnum == 1) pump1live = false;
  else if (pumpnum == 2) pump2live = false;

  // sendenq(1);
}

void sendcrcfun() {

  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(100);  // speed
  //last add

  delay(20);

  sendACK1();
  Serial.println("[fms_tatsuno_fun.ino]  get");


  // delay(4);
  delay(20);
  if (fms_uart2_serial.available()) {
   int receivedByte = fms_uart2_serial.read();
   Serial.printf("U got: 0x%02X (%d)", receivedByte, receivedByte);

  }

  delay(50);

  if (pumpnum == 1) pump1Select();
  else if (pumpnum == 2) pump2Select();

  // sendCalculatedCRC();
  digitalWrite(DIR_PIN, LOW);
  // // delay(500);
  delay(30);
  // delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    Buffer[1] =  fms_uart2_serial.read();
    // delay(5);  // delay(20)
    Serial.printf("your name is : 0x%02X 0x%02X", Buffer[0], Buffer[1]);
    delay(2);
  }

  sendCalculatedCRC();

  delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    Buffer[1] =  fms_uart2_serial.read();
    // delay(5);  // delay(20)
    Serial.printf("your name1 is : 0x%02X 0x%02X", Buffer[0], Buffer[1]);
    delay(2);
  }

  Serial.println("[fms_tatsuno_fun.ino]  i am waiting");
  // delay(100);  //speed
  delay(20);
  
  sendenq(pumpnum);
}

void pricechangefun() {

  incommingMessage.toCharArray(pricechangeary, incommingMessage.length() + 1);

  Serial.printf("pc is :%s", pricechangeary);



  charArray[0] = pricechangeary[0];
  charArray[1] = pricechangeary[1];

  pumpmqttnumchange();

  // if (pricechangeary[0] == 0x30 && pricechangeary[1] == 0x31) {
  if (charArray[2] == 0x40) {
    unitpriceary1[0] = pricechangeary[2];
    unitpriceary1[1] = pricechangeary[3];
    unitpriceary1[2] = pricechangeary[4];
    unitpriceary1[3] = pricechangeary[5];
    //pricechangesuccess mqtt need
    pricechangeapprove1fun();
  }
  //  else if (pricechangeary[0] == 0x30 && pricechangeary[1] == 0x32) {
  else if (charArray[2] == 0x41) {
    unitpriceary2[0] = pricechangeary[2];
    unitpriceary2[1] = pricechangeary[3];
    unitpriceary2[2] = pricechangeary[4];
    unitpriceary2[3] = pricechangeary[5];
    //pricechangesuccess mqtt need
    pricechangeapprove2fun();
  }
  Serial.printf("ur noz1 price is :0x%02X 0x%02X 0x%02X 0x%02X \n",
                        unitpriceary1[0], unitpriceary1[1], unitpriceary1[2], unitpriceary1[3]);
  Serial.printf("ur noz2 price is : 0x%02X 0x%02X 0x%02X 0x%02X \n",
                        unitpriceary2[0], unitpriceary2[1], unitpriceary2[2], unitpriceary2[3]);

}

void pumpmqttnumchange() {  //change incomming pumpid(0 - 32) from Mqtt to Device pumpid(0 - 2)
  if (charArray[0] == 0x30 && charArray[1] == 0x31) {
    pumpmqttnum = 1;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x32) {
    pumpmqttnum = 2;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x33) {
    pumpmqttnum = 3;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x34) {
    pumpmqttnum = 4;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x35) {
    pumpmqttnum = 5;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x36) {
    pumpmqttnum = 6;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x37) {
    pumpmqttnum = 7;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x38) {
    pumpmqttnum = 8;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x39) {
    pumpmqttnum = 9;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x30) {
    pumpmqttnum = 10;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x31) {
    pumpmqttnum = 11;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x32) {
    pumpmqttnum = 12;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x33) {
    pumpmqttnum = 13;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x34) {
    pumpmqttnum = 14;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x35) {
    pumpmqttnum = 15;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x36) {
    pumpmqttnum = 16;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x37) {
    pumpmqttnum = 17;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x38) {
    pumpmqttnum = 18;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x39) {
    pumpmqttnum = 19;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x30) {
    pumpmqttnum = 20;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x31) {
    pumpmqttnum = 21;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x32) {
    pumpmqttnum = 22;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x33) {
    pumpmqttnum = 23;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x34) {
    pumpmqttnum = 24;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x35) {
    pumpmqttnum = 25;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x36) {
    pumpmqttnum = 26;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x37) {
    pumpmqttnum = 27;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x38) {
    pumpmqttnum = 28;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x39) {
    pumpmqttnum = 29;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x30) {
    pumpmqttnum = 30;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x31) {
    pumpmqttnum = 31;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x32) {
    pumpmqttnum = 32;
  }

  charArray[2] = 0x00;

  if (pumpid1 == pumpmqttnum) {
    charArray[2] = 0x40;

  } else if (pumpid2 == pumpmqttnum) {
    charArray[2] = 0x41;
  }
  //  else if (pumpid3 == pumpmqttnum) {
  //   charArray[3] = 0x42;

  // } else if (pumpid4 == pumpmqttnum) {
  //   charArray[3] = 0x30;

  // } else if (pumpid5 == pumpmqttnum) {
  //   charArray[3] = 0x30;

  // } else if (pumpid6 == pumpmqttnum) {
  //   charArray[3] = 0x30;

  // } else if (pumpid7 == pumpmqttnum) {
  //   charArray[3] = 0x30;

  // } else if (pumpid8 == pumpmqttnum) {
  //   charArray[3] = 0x30;

  // }
}

void sendpumpstatus(int pump) {

  if (pump == 1) pump1Select();
  else if (pump == 2) pump2Select();

  int count1;
  char charack0[2];

  digitalWrite(DIR_PIN, LOW);
  // // delay(500);
  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);

    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);

    if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
      // sendenq(2);
      // pump2status();
      if (pump == 1) pump1status();
      else if (pump == 2) pump2status();
    }
  }
  Serial.println("[fms_tatsuno_fun.ino]  shis shi");
  // delay(50);
  // digitalWrite(15, LOW);
  // // delay(500);
  delay(20);

  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got 1221");
    charack0[0] =  fms_uart2_serial.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);

    // if (charack0[0] == 0x10 && charack0[1] == 0x31) {  //
    sendenq(pump);
    // }
  }
}

void pumppresetfun() {


  if (presetmqttary[2] == 0x4C) {
    CalculatedPresetdata[5] = 0x31;  // fueling with volume limitation
    CalculatedPresetdata[6] = 0x31;  // volume

    // add volume (4+2)
    // CalculatedPresetdata[7] = presetmqttary[3];
    // CalculatedPresetdata[8] = presetmqttary[4];
    // CalculatedPresetdata[9] = presetmqttary[5];
    // CalculatedPresetdata[10] = presetmqttary[6];
    // CalculatedPresetdata[11] = presetmqttary[7];
    // CalculatedPresetdata[12] = presetmqttary[8];
    CalculatedPresetdata[7] = presetmqttary[4];
    CalculatedPresetdata[8] = presetmqttary[5];
    CalculatedPresetdata[9] = presetmqttary[6];
    CalculatedPresetdata[10] = presetmqttary[7];
    CalculatedPresetdata[11] = presetmqttary[8];
    CalculatedPresetdata[12] = presetmqttary[9];

  } else if (presetmqttary[2] == 0x50) {
    Serial.println("[fms_tatsuno_fun.ino]  yep u changed approved");
    CalculatedPresetdata[5] = 0x33;  // fueling with Prest fueling(Preset value cannot be changed at dispenser)
    CalculatedPresetdata[6] = 0x32;  // Amount

    // add amount (6)
    // CalculatedPresetdata[7] = presetmqttary[3];
    // CalculatedPresetdata[8] = presetmqttary[4];
    // CalculatedPresetdata[9] = presetmqttary[5];
    // CalculatedPresetdata[10] = presetmqttary[6];
    // CalculatedPresetdata[11] = presetmqttary[7];
    // CalculatedPresetdata[12] = presetmqttary[8];
    CalculatedPresetdata[7] = presetmqttary[4];
    CalculatedPresetdata[8] = presetmqttary[5];
    CalculatedPresetdata[9] = presetmqttary[6];
    CalculatedPresetdata[10] = presetmqttary[7];
    CalculatedPresetdata[11] = presetmqttary[8];
    CalculatedPresetdata[12] = presetmqttary[9];
  }

  //add unit price
  // CalculatedPresetdata[14] = unitpriceary1[0];
  // CalculatedPresetdata[15] = unitpriceary1[1];
  // CalculatedPresetdata[16] = unitpriceary1[2];
  // CalculatedPresetdata[17] = unitpriceary1[3];


  // if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x31) {
  if (tempcharArray[0] == 0x40) {
    CalculatedPresetdata[1] = 0x40;  // add address
    pump1Select();
    pump1live = true;
    CalculatedPresetdata[14] = unitpriceary1[0];
    CalculatedPresetdata[15] = unitpriceary1[1];
    CalculatedPresetdata[16] = unitpriceary1[2];
    CalculatedPresetdata[17] = unitpriceary1[3];
    pumpnum = 1;

    pumplivefor1 = true;  //select count
  }
  // else if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x32) {
  else if (tempcharArray[0] == 0x41) {
    CalculatedPresetdata[1] = 0x41;  // add address
    pump2Select();
    pump2live = true;
    CalculatedPresetdata[14] = unitpriceary2[0];
    CalculatedPresetdata[15] = unitpriceary2[1];
    CalculatedPresetdata[16] = unitpriceary2[2];
    CalculatedPresetdata[17] = unitpriceary2[3];
    pumpnum = 2;

    pumplivefor2 = true;  //select count
  }


  int count1;
  char charack0[2];

  digitalWrite(DIR_PIN, LOW);
  // delay(500);
  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] =  fms_uart2_serial.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);

    // if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
    sendCalculatedPreset();
    // }
  }
}

void reloadfun() {

  Serial.println("[fms_tatsuno_fun.ino]  reload fun start");

  for (int i = 0; i < 50; i++) {
    ppbuffer[i] = 0x00;
  }

  incommingMessage.toCharArray(reloadArry, incommingMessage.length() + 1);
  charArray[0] = reloadArry[0];
  charArray[1] = reloadArry[1];

  // ADDSLP
  ppbuffer[0] = charArray[0];
  ppbuffer[1] = charArray[1];
  ppbuffer[2] = 'S';
  ppbuffer[3] = '0';
  ppbuffer[4] = 'L';
  ppbuffer[5] = '.';
  ppbuffer[6] = '0';
  ppbuffer[7] = 'P';
  ppbuffer[8] = '0';

  //mqttt to pump address
  pumpmqttnumchange();

  //pumpnum 1 or 2

  if (charArray[2] == 0x40) pumpnum = 1;
  else if (charArray[2] == 0x41) pumpnum = 2;


  if (pumpnum == 1) pump1Select();
  else if (pumpnum == 2) pump2Select();

  int count1;
  char charack0[2];

  digitalWrite(DIR_PIN, LOW);

  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);
  }

  if (pumpnum == 1) pump1Totalizerstatus();
  else if (pumpnum == 2) pump2Totalizerstatus();

  digitalWrite(DIR_PIN, LOW);
  delay(30);  // to open

  if (fms_uart2_serial.available()) {  // to change if
    Serial.println("[fms_tatsuno_fun.ino]  U got2 ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);
  }

  sendenq(pumpnum);

  char totalizerary[33];
  int totalizercount = 0;
  digitalWrite(DIR_PIN, LOW);
  delay(50);

  while (fms_uart2_serial.available()) {

    totalizerary[totalizercount] =  fms_uart2_serial.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    Serial.printf("%d // 0x%02X \n", totalizercount, totalizerary[totalizercount]);

    delay(10);
    if (totalizerary[totalizercount] == 0x03) {
      totalizerary[totalizercount + 1] =  fms_uart2_serial.read();
      delay(10);
      Serial.println("[fms_tatsuno_fun.ino]   ");
      break;
    }


    totalizercount++;
  }

  // finalmqtt2
  while (totalizerary[4] != 0x35) {  // wait for totalizer
    sendACK1();
    digitalWrite(DIR_PIN, LOW);
    delay(30);
    if (fms_uart2_serial.available()) {
    int receivedByte = fms_uart2_serial.read();
    Serial.printf("U got 4 : 0x%02X (%d) \n", receivedByte, receivedByte);

    }

    sendenq(pumpnum);

    totalizercount = 0;

    digitalWrite(DIR_PIN, LOW);
    delay(50);

    while (fms_uart2_serial.available()) {

      totalizerary[totalizercount] =  fms_uart2_serial.read();
      // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
      Serial.printf("%d // 0x%02X \n", totalizercount, totalizerary[totalizercount]);
      delay(10);

      if (totalizerary[totalizercount] == 0x03) {
        totalizerary[totalizercount + 1] =  fms_uart2_serial.read();
        delay(10);
        Serial.println("[fms_tatsuno_fun.ino]   ");
        break;
      }

      if (totalizerary[0] == 0x04) {

        Serial.println("[fms_tatsuno_fun.ino]  first loop break");
        reloadcheck = true;
        break;
      }

      totalizercount++;
    }

    if (reloadcheck) {
      Serial.println("[fms_tatsuno_fun.ino]  bye reload...................");
      reloadcheck = false;
      break;
    }

  }

  mqtttcount = 0;

  ppbuffer[mqtttcount + 9] = 'T';

  int y = mqtttcount + 10;


  // for (int i = 0; i < 6; i++) {
  //   Serial.println(ppbuffer[i]);
  // }

  // Serial.println();
  Serial.printf("mqtt count is => %d \n", y);

  for (int j = 11; j < 21; j++) {

    if (j == 18) {
      ppbuffer[y] = '.';
      y++;
      zerocount = false;
    }

    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }
    if (Buffer[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = totalizerary[j];
    y++;
  }


  zerocount = true;
  ppbuffer[y] = 'A';
  y++;
  // Serial.println("[fms_tatsuno_fun.ino]  my j is");

  for (int j = 21; j < 31; j++) {
    // Serial.println(j);
    if (totalizerary[j] == 0x30 && zerocount) {
      // Serial.println("[fms_tatsuno_fun.ino]  zero");
      continue;
    }
    if (totalizerary[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = totalizerary[j];
    // Serial.println(j);
    // Serial.println(ppbuffer[y]);
    y++;
  }

  Serial.println("[fms_tatsuno_fun.ino]   ");
  zerocount = true;
  Serial.printf("ppbuffer : %s \n", ppbuffer);


  fms_mqtt_client.publish(pumpfinalreloadbuf, ppbuffer);

  txledonoff();

  // for (int i = 0; i < 10; i++) {
  //   err_buffer[i] = 0;
  // }

  // err_buffer[0] = ppbuffer[0];
  // err_buffer[1] = ppbuffer[1];
  // err_buffer[2] = '/';
  // err_buffer[3] = 'e';
  // err_buffer[4] = 'r';
  // err_buffer[5] = 'r';
  // err_buffer[6] = 'o';
  // err_buffer[7] = 'r';

  // waitcount = 0;
  // previousMillis = millis() / 1000;
  // while (waitcount < 2) {

  //   if (ppbuffer[0] == server_rpy_ary[0] && ppbuffer[1] == server_rpy_ary[1] && final_str == "D1S1") {
  //     Serial.println("[fms_tatsuno_fun.ino]  Bye ....................................");
  //     final_str = "";
  //     break;
  //   }

  //   if (millis() / 1000 - previousMillis >= 13) {

  //     if (waitcount < 1) {
  //       client.publish(pumpfinalreloadbuf, ppbuffer);
  //     }

  //     // client.publish(pumpfinalbuf, ppbuffer);

  //     previousMillis = millis() / 1000;
  //     waitcount++;
  //   }

  //   client.loop();
  // }

  // if (waitcount == 2) client.publish(server_err, err_buffer);

  reloadcount = false;

  sendACK1();

  if (pumpnum == 1) pump1live = false;
  else if (pumpnum == 2) pump2live = false;
}

void pricereqfun() {

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  mqttpumpidchange(pumpid1);
  fms_mqtt_client.publish(pricereqbuf, ppbuffer);
  mqttpumpidchange(pumpid2);
  fms_mqtt_client.publish(pricereqbuf, ppbuffer);
  txledonoff();
}

void pump1Totalizerstatus() {
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(totalizerstatus1, 7);
  Serial.println("[fms_tatsuno_fun.ino]  sending 1totalizerstatus");
  delay(5);
  digitalWrite(DIR_PIN, LOW);
}

void pump2Totalizerstatus() {
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(totalizerstatus2, 7);
  Serial.println("[fms_tatsuno_fun.ino]  sending 2totalizerstatus");
  delay(5);
  digitalWrite(DIR_PIN, LOW);
}

void pump2status() {
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(pump2statusary, 7);
  Serial.println("[fms_tatsuno_fun.ino]  sending pump2status");
  // delay(4.5);
  delay(5);
  digitalWrite(DIR_PIN, LOW);
}

void pump1status() {
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(pump1statusary, 7);
  Serial.println("[fms_tatsuno_fun.ino]  sending pump1status");
  delay(5);
  digitalWrite(DIR_PIN, LOW);
}

void pump1Select() {
  digitalWrite(DIR_PIN, HIGH);
  fms_uart2_serial.write(select1, sizeof(select1));
  Serial.println("[fms_tatsuno_fun.ino]  sending select1");
  delay(4);
  digitalWrite(DIR_PIN, LOW);
}

void pump2Select() {
  digitalWrite(DIR_PIN, HIGH);
  fms_uart2_serial.write(select2, sizeof(select2));
  Serial.println("[fms_tatsuno_fun.ino]  sending select2");
  delay(4);
  digitalWrite(DIR_PIN, LOW);
}

void pricechangeapprove2fun() {

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  mqttpumpidchange(pumpid2);
  fms_mqtt_client.publish("detpos/device/price", ppbuffer);
  txledonoff();
}

void pricechangeapprove1fun() {

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  mqttpumpidchange(pumpid1);
  fms_mqtt_client.publish("detpos/device/price", ppbuffer);
  txledonoff();
}

void finalmqtt1() {

  for (int y = 0; y < 50; y++) {
    ppbuffer[y] = 0;
  }

  pumpidchange();

  // if (Buffer[1] == 0x40) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x31;
  // } else if (Buffer[1] == 0x41) {
  //   ppbuffer[0] = 0x30;
  //   ppbuffer[1] = 0x32;
  // }

  ppbuffer[2] = 'S';

  int y = 3;

  for (int j = 13; j <= 16; j++) {

    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }
    if (Buffer[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = Buffer[j];
    y++;
  }


  zerocount = true;
  ppbuffer[y] = 'L';
  y++;

  for (int j = 6; j < 12; j++) {

    if (j == 9) {
      ppbuffer[y] = '.';
      y++;
      zerocount = false;  //add
    }

    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }
    if (Buffer[j] != 0x30) {
      zerocount = false;
    }
    ppbuffer[y] = Buffer[j];
    y++;
  }

  zerocount = true;
  ppbuffer[y] = 'P';
  y++;

  for (int j = 17; j < 23; j++) {
    if (Buffer[j] == 0x30 && zerocount) {
      continue;
    }

    if (Buffer[j] != 0x30) {
      zerocount = false;
    }

    ppbuffer[y] = Buffer[j];
    y++;
  }
  zerocount = true;
  mqtttcount = y;
}

void pumpidchange() {  //change incomming pumpid(0 - 8) from device to mqtt pumpid(0 - 32)
  if (Buffer[1] == 0x40) mqttpumpidchange(pumpid1);
  else if (Buffer[1] == 0x41) mqttpumpidchange(pumpid2);
}

void pumapprofun() {
  // char pumpapproArray[13];
  incommingmsg1.toCharArray(pumpapproArray, incommingmsg1.length() + 1);
  Serial.printf("Appro  is : %s \n", pumpapproArray);

  charArray[0] = pumpapproArray[0];
  charArray[1] = pumpapproArray[1];

  pumpmqttnumchange();


  pumappproSend();
}

void hmisetup() {

  if (Serial.available()) {
    for (int j = 0; j <= 50; j++)  //this loop will store whole frame in buffer array.
    {
      Buffer[j] = Serial.read();
      Serial.println(Buffer[j], HEX);
      Serial.println("[fms_tatsuno_fun.ino]   ");
    }
    Serial.println("[fms_tatsuno_fun.ino]   ");

    if (Buffer[4] == 0x10 && Buffer[8] == 0x01) {
      ESP.restart();
      Serial.println("[fms_tatsuno_fun.ino]  restart");
    } else if (Buffer[4] == 0x13) {  // wifi ssid
      for (int j = 0; j < 50; j++) ssidBuf[j] = 0;

      for (int j = 0; j < 50; j++) {
        if (Buffer[j + 9] == 0xFF) break;
        ssidBuf[j] = Buffer[j + 9];
      }
      Serial.printf("ssid : %s ,",ssidBuf);

      Serial.write(showSSID, 6);
      Serial.write(ssidBuf, 30);
      Serial.write(showSSID, 6);
      Serial.write(ssidBuf, 30);

    } else if (Buffer[4] == 0x14) {  // wifi password
      for (int j = 0; j < 50; j++) passBuf[j] = 0;

      for (int j = 0; j < 50; j++) {
        if (Buffer[j + 9] == 0xFF) break;
        passBuf[j] = Buffer[j + 9];
      }
      Serial.printf ("pass : %s \n" , passBuf);
      Serial.write(showPASS, 6);
      Serial.write(passBuf, 50);
      Serial.write(showPASS, 6);
      Serial.write(passBuf, 50);
    } else if (Buffer[4] == 0x11) {  // wifi connect
      WiFi.hostname("device1");
      WiFi.begin(ssidBuf, passBuf);
      wifitrytime = 0;
      while (WiFi.status() != WL_CONNECTED && wifitrytime != 15) {
      FMS_LOG_INFO("[fms_tatsuno_fun.ino:2289 [hmi]] WiFi initialized, connecting to %s... wpa:%s", ssidBuf, passBuf);
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(500));  // Wait for 1 second before repeating
      gpio_set_level(LED_YELLOW, HIGH);
      vTaskDelay(pdMS_TO_TICKS(500));  // Wait for 1 second before repeating
      wifitrytime++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf(" Wifi Connected : %s \n", WiFi.localIP().toString().c_str());
        writeString(ssidBuf,passBuf); // save to preferences nvs storage ( trion )
        // digitalWrite(wifiled, HIGH);
        Serial.write(page, 9);
        Serial.write(0x00);
        Serial.println("[fms_tatsuno_fun.ino]  home page");
        delay(1000);
      }
    } else if (Buffer[4] == 0x31) {  // devnumber
      devicenum = Buffer[8];
      Serial.printf("%d \n",devicenum);
    } else if (Buffer[4] == 0x41) {  // pumpid 1
      pumpid1 = Buffer[8];
      Serial.printf("%d \n",pumpid1);
    } else if (Buffer[4] == 0x42) {  // pumpid 2
      pumpid2 = Buffer[8];
      Serial.printf("%d \n",pumpid2);
    } else if (Buffer[4] == 0x43) {  // pumpid 3
      pumpid3 = Buffer[8];
      Serial.printf("%d \n",pumpid3);
    } else if (Buffer[4] == 0x44) {  // pumpid 4
      pumpid4 = Buffer[8];
      Serial.printf("%d \n",pumpid4);
    } else if (Buffer[4] == 0x45) {  // pumpid 5
      pumpid5 = Buffer[8];
      Serial.printf("%d \n",pumpid5);
    } else if (Buffer[4] == 0x46) {  // pumpid 6
      pumpid6 = Buffer[8];
      Serial.printf("%d \n",pumpid6);
    } else if (Buffer[4] == 0x47) {  // pumpid 7
      pumpid7 = Buffer[8];
      Serial.printf("%d \n",pumpid7);
    } else if (Buffer[4] == 0x48) {  // pumpid 8
      pumpid8 = Buffer[8];
      Serial.printf("%d \n",pumpid8);
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x00) {  // 2noz
      nozzlenum = 2;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x01) {  // 4noz
      nozzlenum = 4;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x02) {  // 8noz
      nozzlenum = 8;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x03) {  // save
      
    /* save data to nvs storage instance of eeprom */
      tatsunoConfig.devn = devicenum;  // Set device number
      tatsunoConfig.noz = nozzlenum;   // Set nozzle number
      tatsunoConfig.pumpids[0] = pumpid1;  // Set pump IDs
      tatsunoConfig.pumpids[1] = pumpid2;  // Set pump IDs
      tatsunoConfig.pumpids[2] = pumpid3;  // Set pump IDs
      tatsunoConfig.pumpids[3] = pumpid4;  // Set pump IDs
      tatsunoConfig.pumpids[4] = pumpid5;  // Set pump IDs
      tatsunoConfig.pumpids[5] = pumpid6;  // Set pump IDs
      tatsunoConfig.pumpids[6] = pumpid7;  // Set pump IDs
      tatsunoConfig.pumpids[7] = pumpid8;  // Set pump IDs
      
      Serial.println("[fms_tatsuno_fun.ino]  save tatsuno config");
      Serial.printf("Device Number: %d, Nozzle Number: %d\n", tatsunoConfig.devn, tatsunoConfig.noz);
      fms_save_tatsuno_config(tatsunoConfig);  // Save the configuration; 
      //saveall();
    }
  }
}

void saveall() {
  EEPROM.write(101, pumpid1);
  EEPROM.commit();
  EEPROM.write(102, pumpid2);
  EEPROM.commit();
  EEPROM.write(103, pumpid3);
  EEPROM.commit();
  EEPROM.write(104, pumpid4);
  EEPROM.commit();
  EEPROM.write(105, pumpid5);
  EEPROM.commit();
  EEPROM.write(106, pumpid6);
  EEPROM.commit();
  EEPROM.write(107, pumpid7);
  EEPROM.commit();
  EEPROM.write(108, pumpid8);
  EEPROM.commit();

  EEPROM.write(109, devicenum);
  EEPROM.commit();
  EEPROM.write(110, nozzlenum);
  EEPROM.commit();

  Serial.println("[fms_tatsuno_fun.ino]  yep all save");
}

void writeString(String ssid, String password) {
 // Save to preferences
  fms_nvs_storage.begin("fms_config", false);
  fms_nvs_storage.putString("ssid", ssid);
  fms_nvs_storage.putString("pass", password);
  fms_nvs_storage.end();

  Serial.println("[fms_tatsuno_fun.ino]  Wrtiting ssid and pass to eeprom");
}

void mqttpumpidchange(int pumpid) {
  if (pumpid == 1) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x31;
  } else if (pumpid == 2) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x32;
  } else if (pumpid == 3) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x33;
  } else if (pumpid == 4) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x34;
  } else if (pumpid == 5) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x35;
  } else if (pumpid == 6) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x36;
  } else if (pumpid == 7) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x37;
  } else if (pumpid == 8) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x38;
  } else if (pumpid == 9) {
    ppbuffer[0] = 0x30;
    ppbuffer[1] = 0x39;
  } else if (pumpid == 10) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x30;
  } else if (pumpid == 11) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x31;
  } else if (pumpid == 12) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x32;
  } else if (pumpid == 13) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x33;
  } else if (pumpid == 14) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x34;
  } else if (pumpid == 15) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x35;
  } else if (pumpid == 16) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x36;
  } else if (pumpid == 17) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x37;
  } else if (pumpid == 18) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x38;
  } else if (pumpid == 19) {
    ppbuffer[0] = 0x31;
    ppbuffer[1] = 0x39;
  } else if (pumpid == 20) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x30;
  } else if (pumpid == 21) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x31;
  } else if (pumpid == 22) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x32;
  } else if (pumpid == 23) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x33;
  } else if (pumpid == 24) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x34;
  } else if (pumpid == 25) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x35;
  } else if (pumpid == 26) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x36;
  } else if (pumpid == 27) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x37;
  } else if (pumpid == 28) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x38;
  } else if (pumpid == 29) {
    ppbuffer[0] = 0x32;
    ppbuffer[1] = 0x39;
  } else if (pumpid == 33) {
    ppbuffer[0] = 0x33;
    ppbuffer[1] = 0x30;
  } else if (pumpid == 31) {
    ppbuffer[0] = 0x33;
    ppbuffer[1] = 0x31;
  } else if (pumpid = 32) {
    ppbuffer[0] = 0x33;
    ppbuffer[1] = 0x32;
  }
}

void pumappproSend() {

  // if (pumpapproArray[0] == 0x30 && pumpapproArray[1] == 0x31) {
  if (charArray[2] == 0x40) {
    pump1Select();
    CalculatedApprodata[1] = 0x40;  // add address
    pump1live = true;
    CalculatedApprodata[14] = unitpriceary1[0];
    CalculatedApprodata[15] = unitpriceary1[1];
    CalculatedApprodata[16] = unitpriceary1[2];
    CalculatedApprodata[17] = unitpriceary1[3];

    pumplivefor1 = true;  //select count
  }
  // else if (pumpapproArray[0] == 0x30 && pumpapproArray[1] == 0x32) {
  else if (charArray[2] == 0x41) {
    pump2Select();
    CalculatedApprodata[1] = 0x41;  // add address
    pump2live = true;
    CalculatedApprodata[14] = unitpriceary2[0];
    CalculatedApprodata[15] = unitpriceary2[1];
    CalculatedApprodata[16] = unitpriceary2[2];
    CalculatedApprodata[17] = unitpriceary2[3];

    pumplivefor2 = true;  //select count
  }

  // pump1Select();
  int count1;
  char charack0[2];

  digitalWrite(DIR_PIN, LOW);
  // delay(500);
  delay(30);
  if (fms_uart2_serial.available()) {
    Serial.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] =  fms_uart2_serial.read();
    if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println(charack0[0], HEX);
    Serial.println(charack0[1], HEX);

    if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
      sendCalculatedAppro();
    }
  }
}

void sendCalculatedPreset() {

  CalculatedPresetdata[19] = BCCfun2();
  for (int y = 0; y < 20; y++) {
    Serial.println(CalculatedPresetdata[y], HEX);
    Serial.println("[fms_tatsuno_fun.ino]   ");
  }

  Serial.println("[fms_tatsuno_fun.ino]  ");
  
  // delay(100); //speed

  delay(20); //speed

  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(CalculatedPresetdata, 20);
  delay(2);
  fms_uart2_serial.write(CalculatedPresetdata, 20);
  // delay(2);
  Serial.println("[fms_tatsuno_fun.ino]  sending preset");
  // delay(12.5);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(DIR_PIN, LOW);

  //tgi add
  // if (Buffer[1] == 0x40) pumpnum = 1;
  // else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    delay(20);
    Buffer[1] =  fms_uart2_serial.read();
    delay(20);
    // latest pumppreset again

    Serial.println("[fms_tatsuno_fun.ino]  your name1 is");
    Serial.println(Buffer[0], HEX);
    Serial.println(Buffer[1], HEX);
    delay(2);
    //Frist time
    if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1
      // resend
      Serial.println("[fms_tatsuno_fun.ino]  motor start");
    } else {
      resendpreset();
    }
  }

  // Second time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1
    // resend
    Serial.println("[fms_tatsuno_fun.ino]  motor start");
  } else {
    resendpreset();
  }
  // Third time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1
    // resend
    Serial.println("[fms_tatsuno_fun.ino]  motor start");
  } else {
    resendpreset();
  }

  sendenq(pumpnum);


  Serial.println("[fms_tatsuno_fun.ino]  Uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu");
}

unsigned char BCCfun1() {

  unsigned char bccData[18];

  bccData[14] = pumpapproArray[8];
  bccData[15] = pumpapproArray[9];
  bccData[16] = pumpapproArray[10];
  bccData[17] = pumpapproArray[11];


  for (int y = 0; y < 18; y++) {
    bccData[y] = CalculatedApprodata[y + 1];
  }


  unsigned char lrc = 0x00;

  // Calculate LRC checksum
  for (int j = 0; j < sizeof(bccData); j++) {
    lrc ^= bccData[j];
  }
  // Print LRC checksum in hexadecimal and decimal format

  if (lrc < 0x10) {
    Serial.println("[fms_tatsuno_fun.ino]  0");
  }

  Serial.printf("LRC Checksum (Hex): 0x%02X, Decimal: %d \n", lrc, lrc);


  return lrc;
}

unsigned char BCCfun2() {

  unsigned char bccData[18];

  for (int y = 0; y < 18; y++) {
    bccData[y] = CalculatedPresetdata[y + 1];
  }


  unsigned char lrc = 0x00;

  // Calculate LRC checksum
  for (int j = 0; j < sizeof(bccData); j++) {
    lrc ^= bccData[j];
  }
  // Print LRC checksum in hexadecimal and decimal format

  if (lrc < 0x10) {
    Serial.println("[fms_tatsuno_fun.ino]  0");
  }
  Serial.printf("LRC Checksum (Hex): 0x%02X, Decimal: %d \n", lrc, lrc);


  return lrc;
}

void resendpreset() {
  // delay(100);//speed
  delay(20);

  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(CalculatedPresetdata, 20);
  Serial.println("[fms_tatsuno_fun.ino]  sending preset again");
  // delay(12.5);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(DIR_PIN, LOW);


  delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    delay(10);
    Buffer[1] =  fms_uart2_serial.read();
    delay(10);
    Serial.println("[fms_tatsuno_fun.ino]  your resend again is");
    Serial.println(Buffer[0], HEX);
    Serial.println(Buffer[1], HEX);
    delay(2);
  }
}

void sendCalculatedAppro() {

  //add unit price
  // CalculatedApprodata[14] = unitpriceary[0];
  // CalculatedApprodata[15] = unitpriceary[1];
  // CalculatedApprodata[16] = unitpriceary[2];
  // CalculatedApprodata[17] = unitpriceary[3];

  CalculatedApprodata[19] = BCCfun1();
  for (int y = 0; y < 20; y++) {
    Serial.println(CalculatedApprodata[y], HEX);
    Serial.println("[fms_tatsuno_fun.ino]   ");
  }
  Serial.println("[fms_tatsuno_fun.ino]  ");


  // delay(100); //speed

  delay(20); //speed
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(CalculatedApprodata, 20);
  delay(2);
  fms_uart2_serial.write(CalculatedApprodata, 20);
  // delay(2);
  Serial.println("[fms_tatsuno_fun.ino]  sending Appro1");
  // delay(13);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(DIR_PIN, LOW);

  //tgi add
  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    delay(20);
    Buffer[1] =  fms_uart2_serial.read();
    delay(20);
    // latest pumppreset again

    Serial.println("[fms_tatsuno_fun.ino]  your name1 is");
    Serial.println(Buffer[0], HEX);
    Serial.println(Buffer[1], HEX);
    delay(2);
    //Frist time
    if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1

      Serial.println("[fms_tatsuno_fun.ino]  app motor start");
    } else {
      resendappro();
    }
  }

  // Second time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1
    // resend
    Serial.println("[fms_tatsuno_fun.ino]  app motor start");
  } else {
    resendappro();
  }
  // Third time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) {  // Incoming ack1
    // resend
    Serial.println("[fms_tatsuno_fun.ino]  app motor start");
  } else {
    resendappro();
  }

  sendenq(pumpnum);
}

void resendappro() {
  // delay(100); //speed

  delay(20); //speed
  digitalWrite(DIR_PIN, HIGH);
  delay(20);
  fms_uart2_serial.write(CalculatedApprodata, 20);
  Serial.println("[fms_tatsuno_fun.ino]  sending resend appro");
  // delay(13);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(DIR_PIN, LOW);

  //tgi add
  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (fms_uart2_serial.available()) {

    Buffer[0] =  fms_uart2_serial.read();
    delay(20);
    Buffer[1] =  fms_uart2_serial.read();
    delay(20);
    Serial.println("[fms_tatsuno_fun.ino]  your resend again is");
    Serial.println(Buffer[0], HEX);
    Serial.println(Buffer[1], HEX);
    delay(2);
  }
}

#endif 
// end tatsuno protocol 






