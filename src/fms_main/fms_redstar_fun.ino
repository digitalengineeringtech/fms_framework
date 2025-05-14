
#define FMS_RED_DEBUG_OPEN
#ifdef FMS_RED_DEBUG_OPEN
  #define FMS_RED_LOG_DEBUG(format, ...) Serial.print("[REDSTAR][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_RED_LOG_ERROR(format, ...) Serial.print("[REDSTAR][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_RED_DEBUG(format, ...)
  #define FMS_RED_ERROR(format, ...)
#endif
/*
    // if (length > 0) {
    //   // Serial.print("Received data: ");
    //   for (int i = 0; i < length; i++) {
    //     Serial.print(length);
    //     Serial.print(" ");
    //     Serial.print("0x");
    //     Serial.print(response[i], HEX);
    //     Serial.print(" ");
    //   }
    // }
*/

uint8_t addresscount                          = 1;  // Address for the Redstar device
uint8_t nozzlecount                           = 2;  // Number of nozzles
const unsigned long state_query_interval      = 2;
unsigned long current_time_seconds            = 0;  // Current time in seconds
unsigned long last_pump_query_interval  = 0;  // Interval for querying state
unsigned char buffer[30];
char charArray[4]; // check nozzle id form server response 
bool nozzle1_approval = false;  // Approval status for nozzle 1
bool nozzle2_approval = false;  // Approval status for nozzle 2
uint8_t server_response_nozzle_id = 0; // Nozzle ID from server response
bool presetcount = false; // Preset count flag
bool lastpresetcount = false; // Last preset count flag
bool preset_check = false; // Preset check flag
bool lastpreset_send = false; // Last preset check flag
// eeprom data
uint8_t pump1id = 1;// Nozzle ID for pump 1
uint8_t pump2id = 2;// Nozzle ID for pump 2
uint8_t pump3id;// Nozzle ID for pump 3
uint8_t pump4id;// Nozzle ID for pump 4
uint8_t pump5id;// Nozzle ID for pump 5
uint8_t pump6id;// Nozzle ID for pump 6
uint8_t pump7id;// Nozzle ID for pump 7
uint8_t pump8id;// Nozzle ID for pump 8

  // Create an instance of the Redstar class
void red_star_init() {
  redstar.begin(9600, true, RXD2, TXD2);  // Initialize the Redstar object with the specified baud rate and pins
}
int length = 0;  // Length of the response buffer
int data_count = 0;
void red_star_main() {
  vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
  if(redstar.update()){
    unsigned char* response = redstar.parseResponse(length);  // Parse the response from the Redstar device
    Serial.print("0x");
    Serial.print(response[data_count],HEX);
    Serial.print(" ");
    data_count++;
  if(addresscount > nozzlecount) addresscount = 1;  // Reset address count if it exceeds the number of nozzles
  // normal state -> 0x12 0x57 
  if (response[data_count - 2] == 0x12 && (response[data_count - 1] == 0x57 || response[data_count - 1] == 0x56 || response[data_count - 1] == 0x53 || response[data_count - 1] == 0x52)) { 
    for (int i = 0; i < 8; i++) {
      buffer[i] = response[i];  // Store the response in the buffer
      vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
      Serial.print(data_count);
      Serial.print("/FE/");
      Serial.print("0x");
      Serial.print(buffer[i], HEX);  // Print the buffer contents in hexadecimal format
      Serial.print(" ");
    }
    FMS_RED_LOG_DEBUG("Fueling");
    data_count = 0;  // Reset length for the next response
    if (buffer[0] == 0x01) {  // Check if the first byte is 0x01
      FMS_RED_LOG_DEBUG("Nozzle %d is in use", addresscount);  // Log the nozzle in use
    } else {
      FMS_RED_LOG_DEBUG("Nozzle %d is not in use", addresscount);  // Log the nozzle not in use
    }
  }  
  // nozel lifted state -> 0x12 0x77 or 0x76,0x72,0x73 response nozzle lifted condition
  else if(response[data_count - 2] == 0x12 && (response[data_count - 1] == 0x77 || response[data_count - 1] == 0x76 || response[data_count - 1] == 0x72 || response[data_count - 1] == 0x73)) { 
    // for (int i = 0; i < 8; i++) {
    //   buffer[i] = response[i];  // Store the response in the buffer
    //   vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
    //   Serial.print(data_count);
    //   Serial.print("/FE/");
    //   Serial.print("0x");
    //   Serial.print(buffer[i], HEX);  // Print the buffer contents in hexadecimal format
    //   Serial.print(" ");
    // }
    // FMS_RED_LOG_DEBUG("nozzel lifted");
    // data_count = 0;  // Reset length for the next response
    responsne_buffer(response,data_count,"Nozzle lifted");
  }
  } else { // no response from dispenser
    FMS_RED_LOG_ERROR("No data received");
    length = 0;
    check_pump_state_interval();                          // Check the pump state at regular intervals
  }
  
}

void send_read_state() {
  if(addresscount > nozzlecount) addresscount = 1;      // Reset address count if it exceeds 2
  vTaskDelay(pdMS_TO_TICKS(50));                        // Delay for 50 milliseconds
  Serial.println(redstar.readState(addresscount),HEX);  // Read state for address count
  vTaskDelay(pdMS_TO_TICKS(10));                        // Delay for 50 milliseconds
  FMS_RED_LOG_DEBUG("SENDING READ STATE FOR ADDRESS: %d", addresscount);
  addresscount++;                                       // Increment address count
}

void check_pump_state_interval() {                      // check state in every 2 seconds
  current_time_seconds = millis() / 1000;               // Get current time in seconds
  if (current_time_seconds - last_pump_query_interval > state_query_interval) {
    //addresscount = 1;                                 // Reset address count
    send_read_state();                                  // Call the function to send read state command
    last_pump_query_interval = current_time_seconds;    // Update last pump query interval
  }
}

void responsne_buffer(unsigned char* buffer_in, int length,const char* logMessage){
for (int i = 0; i < length; i++) {
  buffer[i] = buffer_in[i];  // Store the response in the buffer
    Serial.print("0x");
    Serial.print(buffer[i], HEX);  // Print the buffer contents in hexadecimal format
    Serial.print(" ");
  }
  FMS_RED_LOG_DEBUG("%s", logMessage);  // Log the message
  data_count = 0;  // Reset length for the next response
}

void redstar_pump_setting(char* topic, String payload) {
  // permit message reply (eg: 01permit,02permit) reply 
if(String(topic) == String(approv_topic)) {
  char pumpapproArray[13];                                                      // pump appro array
  payload.toCharArray(pumpapproArray, payload.length() + 1);  // String to char convert
    Serial.print("Appro  is ");
    Serial.println(pumpapproArray);
    // response is 01approv from mqtt server , hex form is 0x30,0x31(01) 
    // hex is 0x30,0x31 (01)
    // hex is 0x30,0x32 (02)    
    // Serial.println(pumpapproArray[1]);
    charArray[0] = pumpapproArray[0];  // to check nozzle id
    charArray[1] = pumpapproArray[1];  // to check nozzle id
 // check approv message id , server response is 01approv,02approv,03approv
    if (charArray[0] == 0x30 && charArray[1] == 0x31) {  // checks message ID is 01 and define id is 1
      server_response_nozzle_id = 1;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x32) {  // checks message ID is 02 and define id is 2
      server_response_nozzle_id = 2;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x33) {  // checks message ID is 03 and define id is 3
      server_response_nozzle_id = 3;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x34) {  // checks message ID is 04 and define id is 4
      server_response_nozzle_id = 4;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x35) {  // checks message ID is 05 and define id is 5
      server_response_nozzle_id = 5;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x36) {  // checks message ID is 06 and define id is 6
      server_response_nozzle_id = 6;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x37) {  // checks message ID is 07 and define id is 7
      server_response_nozzle_id = 7;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x38) {  // checks message ID is 08 and define id is 8
      server_response_nozzle_id = 8;
    } else if (charArray[0] == 0x30 && charArray[1] == 0x39) {  // checks message ID is 09 and define id is 9
      server_response_nozzle_id = 9;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x30) {  // checks message ID is 10 and define id is 10
      server_response_nozzle_id = 10;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x31) {  // checks message ID is 11 and define id is 11
      server_response_nozzle_id = 11;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x32) {  // checks message ID is 12 and define id is 12
      server_response_nozzle_id = 12;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x33) {  // checks message ID is 13 and define id is 13
      server_response_nozzle_id = 13;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x34) {  // checks message ID is 14 and define id is 14
      server_response_nozzle_id = 14;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x35) {  // checks message ID is 15 and define id is 15
      server_response_nozzle_id = 15;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x36) {  // checks message ID is 16 and define id is 16
      server_response_nozzle_id = 16;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x37) {  // checks message ID is 17 and define id is 17
      server_response_nozzle_id = 17;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x38) {  // checks message ID is 18 and define id is 18
      server_response_nozzle_id = 18;
    } else if (charArray[0] == 0x31 && charArray[1] == 0x39) {  // checks message ID is 19 and define id is 19
      server_response_nozzle_id = 19;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x30) {  // checks message ID is 20 and define id is 20
      server_response_nozzle_id = 20;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x31) {  // checks message ID is 21 and define id is 21
      server_response_nozzle_id = 21;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x32) {  // checks message ID is 22 and define id is 22
      server_response_nozzle_id = 22;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x33) {  // checks message ID is 23 and define id is 23
      server_response_nozzle_id = 23;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x34) {  // checks message ID is 24 and define id is 24
      server_response_nozzle_id = 24;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x35) {  // checks message ID is 25 and define id is 25
      server_response_nozzle_id = 25;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x36) {  // checks message ID is 26 and define id is 26
      server_response_nozzle_id = 26;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x37) {  // checks message ID is 27 and define id is 27
      server_response_nozzle_id = 27;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x38) {  // checks message ID is 28 and define id is 28
      server_response_nozzle_id = 28;
    } else if (charArray[0] == 0x32 && charArray[1] == 0x39) {  // checks message ID is 29 and define id is 29
      server_response_nozzle_id = 29;
    } else if (charArray[0] == 0x33 && charArray[1] == 0x30) {  // checks message ID is 30 and define id is 30
      server_response_nozzle_id = 30;
    } else if (charArray[0] == 0x33 && charArray[1] == 0x31) {  // checks message ID is 31 and define id is 31
      server_response_nozzle_id = 31;
    } else if (charArray[0] == 0x33 && charArray[1] == 0x32) {  // checks message ID is 32 and define id is 32
      server_response_nozzle_id = 32;
    }

    if(pump1id == server_response_nozzle_id) {  // Check if the server response nozzle ID matches pump 1 ID
      nozzle1_approval = true;  // Set approval status for nozzle 1
      FMS_RED_LOG_DEBUG("Nozzle %d approved", server_response_nozzle_id);  // Log approval status
    } else if(pump2id == server_response_nozzle_id) {  // Check if the server response nozzle ID matches pump 2 ID
      nozzle2_approval = true;  // Set approval status for nozzle 2
      FMS_RED_LOG_DEBUG("Nozzle %d approved", server_response_nozzle_id);  // Log approval status
    }
  }
 // preset message reply (eg: 01preset,02preset) reply
if(String(topic) == String(preset_topic)) {
  char presetArray[13];                                                      // pump appro array
  payload.toCharArray(presetArray, payload.length() + 1);  // String to char convert
    Serial.print("Preset is ");
    Serial.println(presetArray);
    charArray[0] = presetArray[0];  // to check nozzle id
    charArray[1] = presetArray[1];  // to check nozzle id
    charArray[3] = presetArray[2];  // to check preset Units ( pricce or amount)

 if (charArray[0] == 0x30 && charArray[1] == 0x31) {  // checks message ID is 01 and define id is 1
  server_response_nozzle_id = 1;
} else if (charArray[0] == 0x30 && charArray[1] == 0x32) {  // checks message ID is 02 and define id is 2
  server_response_nozzle_id = 2;
} else if (charArray[0] == 0x30 && charArray[1] == 0x33) {  // checks message ID is 03 and define id is 3
  server_response_nozzle_id = 3;
} else if (charArray[0] == 0x30 && charArray[1] == 0x34) {  // checks message ID is 04 and define id is 4
  server_response_nozzle_id = 4;
} else if (charArray[0] == 0x30 && charArray[1] == 0x35) {  // checks message ID is 05 and define id is 5
  server_response_nozzle_id = 5;
} else if (charArray[0] == 0x30 && charArray[1] == 0x36) {  // checks message ID is 06 and define id is 6
  server_response_nozzle_id = 6;
} else if (charArray[0] == 0x30 && charArray[1] == 0x37) {  // checks message ID is 07 and define id is 7
  server_response_nozzle_id = 7;
} else if (charArray[0] == 0x30 && charArray[1] == 0x38) {  // checks message ID is 08 and define id is 8
  server_response_nozzle_id = 8;
} else if (charArray[0] == 0x30 && charArray[1] == 0x39) {  // checks message ID is 09 and define id is 9
  server_response_nozzle_id = 9;
} else if (charArray[0] == 0x31 && charArray[1] == 0x30) {  // checks message ID is 10 and define id is 10
  server_response_nozzle_id = 10;
} else if (charArray[0] == 0x31 && charArray[1] == 0x31) {  // checks message ID is 11 and define id is 11
  server_response_nozzle_id = 11;
} else if (charArray[0] == 0x31 && charArray[1] == 0x32) {  // checks message ID is 12 and define id is 12
  server_response_nozzle_id = 12;
} else if (charArray[0] == 0x31 && charArray[1] == 0x33) {  // checks message ID is 13 and define id is 13
  server_response_nozzle_id = 13;
} else if (charArray[0] == 0x31 && charArray[1] == 0x34) {  // checks message ID is 14 and define id is 14
  server_response_nozzle_id = 14;
} else if (charArray[0] == 0x31 && charArray[1] == 0x35) {  // checks message ID is 15 and define id is 15
  server_response_nozzle_id = 15;
} else if (charArray[0] == 0x31 && charArray[1] == 0x36) {  // checks message ID is 16 and define id is 16
  server_response_nozzle_id = 16;
} else if (charArray[0] == 0x31 && charArray[1] == 0x37) {  // checks message ID is 17 and define id is 17
  server_response_nozzle_id = 17;
} else if (charArray[0] == 0x31 && charArray[1] == 0x38) {  // checks message ID is 18 and define id is 18
  server_response_nozzle_id = 18;
} else if (charArray[0] == 0x31 && charArray[1] == 0x39) {  // checks message ID is 19 and define id is 19
  server_response_nozzle_id = 19;
} else if (charArray[0] == 0x32 && charArray[1] == 0x30) {  // checks message ID is 20 and define id is 20
  server_response_nozzle_id = 20;
} else if (charArray[0] == 0x32 && charArray[1] == 0x31) {  // checks message ID is 21 and define id is 21
  server_response_nozzle_id = 21;
} else if (charArray[0] == 0x32 && charArray[1] == 0x32) {  // checks message ID is 22 and define id is 22
  server_response_nozzle_id = 22;
} else if (charArray[0] == 0x32 && charArray[1] == 0x33) {  // checks message ID is 23 and define id is 23
  server_response_nozzle_id = 23;
} else if (charArray[0] == 0x32 && charArray[1] == 0x34) {  // checks message ID is 24 and define id is 24
  server_response_nozzle_id = 24;
} else if (charArray[0] == 0x32 && charArray[1] == 0x35) {  // checks message ID is 25 and define id is 25
  server_response_nozzle_id = 25;
} else if (charArray[0] == 0x32 && charArray[1] == 0x36) {  // checks message ID is 26 and define id is 26
  server_response_nozzle_id = 26;
} else if (charArray[0] == 0x32 && charArray[1] == 0x37) {  // checks message ID is 27 and define id is 27
  server_response_nozzle_id = 27;
} else if (charArray[0] == 0x32 && charArray[1] == 0x38) {  // checks message ID is 28 and define id is 28
  server_response_nozzle_id = 28;
} else if (charArray[0] == 0x32 && charArray[1] == 0x39) {  // checks message ID is 29 and define id is 29
  server_response_nozzle_id = 29;
} else if (charArray[0] == 0x33 && charArray[1] == 0x30) {  // checks message ID is 30 and define id is 30
  server_response_nozzle_id = 30;
} else if (charArray[0] == 0x33 && charArray[1] == 0x31) {  // checks message ID is 31 and define id is 31
  server_response_nozzle_id = 31;
} else if (charArray[0] == 0x33 && charArray[1] == 0x32) {  // checks message ID is 32 and define id is 32
  server_response_nozzle_id = 32;
}
  
if (pump1id == server_response_nozzle_id) {
  presetcount = true;  // Set preset count flag for pump 1
} else if (pump2id == server_response_nozzle_id) {
  presetcount = true;  // Set preset count flag for pump 2
} 
if(presetcount){
  preset_check = true;
  lastpreset_send = true;
  generate_preset_data();  // Call the function to generate preset data
  presetcount = false;  // Reset preset count flag
}

}
}

void generate_preset_data() {
  char price[6];
  char liter[3];
  if(charArray[2] == 0x31) pump1id = 1; // check price or amount
  if(charArray[2] == 0x32) pump1id = 2; // check price or amount
  if(charArray[3] == 'P') { 
    for (int i = 0; i < 6; i++) {
      price[i] = charArray[i + 4];  // Store the price in the price array
    }
    int preset_price = atoi(price);  // Convert price string to integer
    Serial.print("Preset price is ");
    Serial.println(preset_price);
    // Publish the preset price to the MQTT broker
  }

  }
