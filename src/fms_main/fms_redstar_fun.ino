
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
int preset_price = 0; // Preset price
int preset_liters = 0; // Preset liters
char preset_state[2]; // check preset noz id and amount or liters (0x01,'P')

String pump2_status = "ide";  // control for nozzle one request final data
String pump1_status = "ide";  // control for nozzle two request final data

String permit_msg = "permit"; // Permit message
String cancel_msg = "cancel"; // Cancel message

bool reload_check_1 = false;       // control reload function // pump reloading check 
bool reload_check_2 = false;       // control reload function

char volume_char[4];  // separate real-time buffer data to get volume (liter) data convert into Decimal data
char amount_char[4];  // separate real-time buffer data to get amount (price) data convert into Decimal data

bool pump1_cancel = false;  // nozzle one cancel count
bool pump2_cancel = false;  // nozzle two cancel count


String price_state;   // to store converted price request data
String total_state;   // to store converted  totalizer liter data
String total_amount;  // to store converted toatlizer ammount data


String price;
String liter;

char mqttdatabuf[50]; // Buffer for MQTT data
// eeprom data hard coding
uint8_t pump1id = 1;// Nozzle ID for pump 1
uint8_t pump2id = 2;// Nozzle ID for pump 2
uint8_t pump3id;// Nozzle ID for pump 3
uint8_t pump4id;// Nozzle ID for pump 4
uint8_t pump5id;// Nozzle ID for pump 5
uint8_t pump6id;// Nozzle ID for pump 6
uint8_t pump7id;// Nozzle ID for pump 7
uint8_t pump8id;// Nozzle ID for pump 8

// device id 
uint8_t device_id = 0; // Device ID
uint8_t nozzle_count = 0; // Nozzle count


char presetArray[13];
char pricechangeArray[7];  // Price change array     
  // Create an instance of the Redstar class
void red_star_init() {
  redstar.begin(9600, true, RXD2, TXD2);  // Initialize the Redstar object with the specified baud rate and pins
}

int length = 0;  // Length of the response buffer
int data_count = 0;
unsigned char* response;
// red start main function (included , pump state, nozzle lifted, fueling)
// check the response from dispenser
void red_star_main() {
  vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
  if(redstar.update()){
    response = redstar.parseResponse(length);  // Parse the response from the Redstar device
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
    FMS_RED_LOG_DEBUG("Data Count: %d", data_count);
    FMS_RED_LOG_DEBUG("Length: %d", length);
    FMS_RED_LOG_DEBUG("Data: ");
    FMS_RED_LOG_DEBUG("RESPONSE: 0x%02X ",response[data_count]);
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
    // Serial.print("0x");
    // Serial.print(response[data_count],HEX);
    // Serial.print(" ");
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
    length = 0;  // Reset length for the next response
    if (buffer[0] == 0x01 && pump1_status == "fuel" || reload_check_1) {
      addresscount = 1;  // Check if the first byte is 0x01
      send_fuel_fun(response);  // Request fuel data from the dispenser
      FMS_RED_LOG_DEBUG("------- FINAL-------     1");
      send_read_price(response);  // Request price data from the dispenser
      send_read_total();  // Request total data from the dispenser
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
    responsne_buffer(response,8,"Nozzle lifted");
    if(preset_check){
      if(lastpreset_send) {
        send_preset_state(response);
        lastpreset_send = false; // Reset the last preset send flag
        preset_check = false; // Reset the preset check flag
      } else {
        send_read_state(); // Request pump state
        preset_check = false; // Reset the preset check flag
      }
    } else {
      if(nozzle1_approval == true && buffer[0] == 0x01) {
        send_approve_state(); // Send approval for pump 1
        nozzle1_approval = false; // Reset the approval status for pump 1
      } else if (nozzle2_approval == true && buffer[0] == 0x02) {
        send_approve_state(); // Send approval for pump 2
        nozzle2_approval = false; // Reset the approval status for pump 2
      } else {
        memset(mqttdatabuf, '\0', 50); // Clean the MQTT data buffer
        if (buffer[0] == 0x01) pump1_cancel = true; // Set pump 1 cancel flag
        else if (buffer[0] == 0x02) pump2_cancel = true; // Set pump 2 cancel flag
        pumpidchange(); // Nozzle ID authentication for sending permit MQTT data
        permit_msg.toCharArray(&mqttdatabuf[2], permit_msg.length() + 1); // Add permit string to MQTT data buffer
        fms_mqtt_client.publish(pumpreqbuf, mqttdatabuf); // Send permit message from MQTT
        preset_check = false; // Reset the preset check flag
        send_read_state(); // Request pump state
      }
    }
  }

  } else { // no response from dispenser
    FMS_RED_LOG_ERROR("No data received");
    length = 0;
    data_count = 0; // Reset length and data count for the next response
    check_pump_state_interval();                          // Check the pump state at regular intervals
  }

}

void send_read_price(unsigned char* buffer_in){
  while(!redstar.update()) { // Wait for the Redstar device to update
    vTaskDelay(pdMS_TO_TICKS(5)); // Delay for 5 milliseconds
  }
  data_count = 0; // Reset data count
  while(redstar.update()) { // Wait for the Redstar device to update
    Serial.print("0x");
    Serial.print(buffer_in[data_count],HEX); // Print the response in hexadecimal format
    Serial.print(" ");
    data_count++;
   
  }
  data_count = 0; // Reset data count
  if(buffer_in[0] == 0x01) { // Check if the first byte is 0x01
    FMS_RED_LOG_DEBUG("READING PRICE 1");
  redstar.readPrice(pump1id); // Set the price for pump 1
  } else if(buffer_in[0] == 0x02) { // Check if the first byte is 0x02
    FMS_RED_LOG_DEBUG("READING PRICE 2");
    redstar.readPrice(pump2id); // Set the price for pump 2
}
vTaskDelay(pdMS_TO_TICKS(10)); // Delay for 10 milliseconds
FMS_RED_LOG_DEBUG("SENDING READ PRICE FOR ADDRESS: %d", buffer_in[0]); // Log the address count
while(!redstar.update()) { // Wait for the Redstar device to update
  vTaskDelay(pdMS_TO_TICKS(5)); // Delay for 5 milliseconds 
}

data_count = 0; // Reset data count
while(redstar.update()) { // Wait for the Redstar device to update
  Serial.print("0x");
  Serial.print(buffer_in[data_count],HEX); // Print the response in hexadecimal format
  Serial.print(" ");
  data_count++;
}
data_count = 0; // Reset data count
price_state = (buffer_in[4] << 24 | buffer_in[5] << 16) | (buffer_in[6] << 8) | buffer_in[7];  // convert hex to String
FMS_RED_LOG_DEBUG("Price is => %s", price_state.c_str()); // Log the price

send_read_state(); // Request pump state
}

void send_fuel_fun(unsigned char* buffer_in) { // similar send_fuel_fun
  if(buffer[0] == 0x01) {
    FMS_RED_LOG_DEBUG("SENDING FULE 1");
    redstar.sendFuel(pump1id);      // Send fuel command for pump 1
    pump1_cancel = false;           // Reset pump 1 cancel flag
  } else if(buffer[0] == 0x02) {
    FMS_RED_LOG_DEBUG("SENDING FUEL 2");
    redstar.sendFuel(pump2id);      // Send fuel command for pump 2
    pump2_cancel = false;           // Reset pump 2 cancel flag
  }

  while(redstar.update()) {        // Wait for response
    Serial.print("0x");
    Serial.print(buffer_in[data_count],HEX);
    Serial.print(" ");
    data_count++;
    vTaskDelay(pdMS_TO_TICKS(5));   // Delay for 5 milliseconds
  }
  data_count = 0;                  // Reset data count
 for (int i = 0; i < 4; i++) {
  amount_char[i] = buffer_in[4 + i]; // Extract amount data
 }

  for (int i = 0; i < 4; i++) {
    volume_char[i] = buffer_in[8 + i]; // Extract volume data
  }

  price =  (amount_char[0] << 24 | amount_char[1] << 16) | (amount_char[2] << 8) | amount_char[3];  // Calculate price
  FMS_RED_LOG_DEBUG("Price is => %s", price.c_str()); // Log the price

  liter = (volume_char[0] << 24 | volume_char[1] << 16) | (volume_char[2] << 8) | volume_char[3];  // Calculate volume
  FMS_RED_LOG_DEBUG("Liter is => %s", liter.c_str()); // Log the volume
 
  if (!reload_check_2 && !reload_check_1) { // Check reload flags
    //mqttpplive(); // Send MQTT live update
  }

  send_read_state(); // Request pump state




}
// check the dispenser condition
void send_read_state() {
  if(addresscount > nozzlecount) addresscount = 1;      // Reset address count if it exceeds 2
  vTaskDelay(pdMS_TO_TICKS(50));                        // Delay for 50 milliseconds
  Serial.println(redstar.readState(addresscount),HEX);  // Read state for address count
  vTaskDelay(pdMS_TO_TICKS(10));                        // Delay for 50 milliseconds
  FMS_RED_LOG_DEBUG("SENDING READ STATE FOR ADDRESS: %d", addresscount);
  addresscount++;                                       // Increment address count
}

void send_read_total() {
  while(!redstar.update()) { // Wait for the Redstar device to update
    vTaskDelay(pdMS_TO_TICKS(5)); // Delay for 5 milliseconds
  }

  data_count = 0; // Reset data count
  length = 0; // Reset length
  while(redstar.update()){
    response = redstar.parseResponse(length); // Parse the response from the Redstar device
    vTaskDelay(pdMS_TO_TICKS(5)); // Delay for 5 milliseconds
    Serial.print(length);
    Serial.print("/");
    Serial.print(response[data_count], HEX);
    Serial.print(" ");
    data_count++;
  }
  data_count = 0; // Reset data count
}
// send preset state
void send_preset_state(unsigned char* buffer_in) { // similar send_preset_fun
 FMS_RED_LOG_DEBUG("Send preset state");
 
 if(preset_state[1] == 'P') {
  redstar.presetAmount(preset_state[0], preset_price); // Send preset liters
 } else if (preset_state[1] == 'L') {
  redstar.presetLiters(preset_state[0], preset_liters); // Send preset amount
 }
 vTaskDelay(pdMS_TO_TICKS(80)); // Delay for 80 milliseconds
 responsne_buffer(buffer_in,5,"Preset  response");
 if (buffer[1] == 0x04 && buffer[2] == 0x89) { // Check response for expected values
    FMS_RED_LOG_DEBUG("Preset motor start");
  } else {
   send_preset_state(buffer_in); // Retry sending preset
  }
  if (buffer[0] == 0x01) { // Check Buffer for pump1 status
    pump1_status = "fuel";
  } else if (buffer[0] == 0x02) { // Check Buffer for pump2 status
    pump2_status = "fuel";
  }
  vTaskDelay(pdMS_TO_TICKS(100)); // Wait before the next operation
  send_read_state(); // request pump state
}

void send_approve_state() {
  if (nozzle1_approval) {
    redstar.sendApproval(pump1id); // Send approval for pump 1
    FMS_RED_LOG_DEBUG("Send approval for pump 1");
  } else if (nozzle2_approval) {
    redstar.sendApproval(pump2id); // Send approval for pump 2
    FMS_RED_LOG_DEBUG("Send approval for pump 2");
  }
}

void pumpidchange() {
  if (buffer[0] == 0x01) mqtt_msg_pump_id_change(pump1id); // Change pump ID for pump 1
  else if (buffer[0] == 0x02) mqtt_msg_pump_id_change(pump2id); // Change pump ID for pump 2
  else if (buffer[0] == 0x03) mqtt_msg_pump_id_change(pump3id); // Change pump ID for pump 3
  else if (buffer[0] == 0x04) mqtt_msg_pump_id_change(pump4id); // Change pump ID for pump 4
  else if (buffer[0] == 0x05) mqtt_msg_pump_id_change(pump5id); // Change pump ID for pump 5
  else if (buffer[0] == 0x06) mqtt_msg_pump_id_change(pump6id); // Change pump ID for pump 6
  else if (buffer[0] == 0x07) mqtt_msg_pump_id_change(pump7id); // Change pump ID for pump 7
  else if (buffer[0] == 0x08) mqtt_msg_pump_id_change(pump8id); // Change pump ID for pump 8
}

void mqtt_msg_pump_id_change(uint8_t pumpid) {
  if (pumpid == 1) {  // match Id with initial setup id and append ID from mqtt data arry
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x31;
  } else if (pumpid == 2) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x32;
  } else if (pumpid == 3) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x33;
  } else if (pumpid == 4) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x34;
  } else if (pumpid == 5) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x35;
  } else if (pumpid == 6) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x36;
  } else if (pumpid == 7) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x37;
  } else if (pumpid == 8) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x38;
  } else if (pumpid == 9) {
    mqttdatabuf[0] = 0x30;
    mqttdatabuf[1] = 0x39;
  } else if (pumpid == 10) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x30;
  } else if (pumpid == 11) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x31;
  } else if (pumpid == 12) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x32;
  } else if (pumpid == 13) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x33;
  } else if (pumpid == 14) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x34;
  } else if (pumpid == 15) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x35;
  } else if (pumpid == 16) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x36;
  } else if (pumpid == 17) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x37;
  } else if (pumpid == 18) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x38;
  } else if (pumpid == 19) {
    mqttdatabuf[0] = 0x31;
    mqttdatabuf[1] = 0x39;
  } else if (pumpid == 20) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x30;
  } else if (pumpid == 21) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x31;
  } else if (pumpid == 22) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x32;
  } else if (pumpid == 23) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x33;
  } else if (pumpid == 24) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x34;
  } else if (pumpid == 25) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x35;
  } else if (pumpid == 26) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x36;
  } else if (pumpid == 27) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x37;
  } else if (pumpid == 28) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x38;
  } else if (pumpid == 29) {
    mqttdatabuf[0] = 0x32;
    mqttdatabuf[1] = 0x39;
  } else if (pumpid == 33) {
    mqttdatabuf[0] = 0x33;
    mqttdatabuf[1] = 0x30;
  } else if (pumpid == 31) {
    mqttdatabuf[0] = 0x33;
    mqttdatabuf[1] = 0x31;
  } else if (pumpid = 32) {
    mqttdatabuf[0] = 0x33;
    mqttdatabuf[1] = 0x32;
  }

}
// change pump state idle or some other state (everty 2 seconds)
void check_pump_state_interval() {                      // check state in every 2 seconds
  current_time_seconds = millis() / 1000;               // Get current time in seconds
  if (current_time_seconds - last_pump_query_interval > state_query_interval) {
    //addresscount = 1;                                 // Reset address count
    send_read_state();                                  // Call the function to send read state command
    last_pump_query_interval = current_time_seconds;    // Update last pump query interval
  }
}

// check the serail buffer return from dispenser
void responsne_buffer(unsigned char* buffer_in, int length,const char* logMessage){
for (int i = 0; i < length; i++) {
  buffer[i] = buffer_in[i];  // Store the response in the buffer
    Serial.print("0x");
    Serial.print(buffer[i], HEX);  // Print the buffer contents in hexadecimal format
    Serial.print(" ");
  }
  FMS_RED_LOG_DEBUG("%s", logMessage);  // Log the message
  data_count = 0;  // Reset length for the next response
  length = 0;  // Reset length for the next response
}

// mqtt server response buffer (for all control , preset,pricechange,approv)
void redstar_pump_setting(char* topic, String payload) {
  // Permit message reply (e.g., 01permit, 02permit) reply
  if (String(topic) == String(approv_topic)) {
    char pumpapproArray[13]; // Pump approval array
    payload.toCharArray(pumpapproArray, payload.length() + 1); // String to char conversion
    Serial.print("Approval is ");
    Serial.println(pumpapproArray);

    // Response is 01approv from MQTT server, hex form is 0x30, 0x31 (01)
    charArray[0] = pumpapproArray[0]; // To check nozzle ID
    charArray[1] = pumpapproArray[1]; // To check nozzle ID

    // Check approval message ID, server response is 01approv, 02approv, 03approv
    check_server_response_nozzle_id(true);

    if (pump1id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 1 ID
      nozzle1_approval = true; // Set approval status for nozzle 1
      FMS_RED_LOG_DEBUG("Nozzle %d approved", server_response_nozzle_id); // Log approval status
    } else if (pump2id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 2 ID
      nozzle2_approval = true; // Set approval status for nozzle 2
      FMS_RED_LOG_DEBUG("Nozzle %d approved", server_response_nozzle_id); // Log approval status
    }
  }

  if (String(topic) == String(preset_topic)) {
    payload.toCharArray(presetArray, payload.length() + 1); // String to char conversion
    Serial.print("Preset is ");
    Serial.println(presetArray);

    charArray[0] = presetArray[0]; // To check nozzle ID
    charArray[1] = presetArray[1]; // To check nozzle ID
    charArray[3] = presetArray[2]; // To check preset units (price or amount)

    /* Debug section
    for (int i = 0; i < sizeof(presetArray); i++) {
      Serial.print("0x");
      Serial.print(presetArray[i], HEX); // Print the buffer contents in hexadecimal format
      Serial.print(" ");
    }
    */
    check_server_response_nozzle_id(true);
    if (pump1id == server_response_nozzle_id) {
      presetcount = true; // Set preset count flag for pump 1
    } else if (pump2id == server_response_nozzle_id) {
      presetcount = true; // Set preset count flag for pump 2
    }
    if (presetcount) {
      preset_check = true;
      lastpreset_send = true;
      generate_preset_data(); // Call the function to generate preset data
      presetcount = false; // Reset preset count flag
    }
  }

  if (String(topic) == String(price_change_topic)) {
    // Get price change data from server and send to dispenser
    generate_price_change_data(payload); // Call the function to generate price change data
  }

  if (String(topic) == String(device_Id_topic)) {
    // from old function 
    // we change eeprom data to preference data
    DynamicJsonDocument doc(4096);                                        // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, payload);  // check error

    if (error) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    device_id = doc["devicenum"].as<const int>();  // assign device id and nozzle id from JSON data
    nozzle_count = doc["nozzlenum"].as<const int>();
    pump1id = doc["pumpid1"].as<const int>();
    pump2id = doc["pumpid2"].as<const int>();
    pump3id = doc["pumpid3"].as<const int>();
    pump4id = doc["pumpid4"].as<const int>();
    pump5id = doc["pumpid5"].as<const int>();
    pump6id = doc["pumpid6"].as<const int>();
    pump7id = doc["pumpid7"].as<const int>();
    pump8id = doc["pumpid8"].as<const int>();

    if(!fms_nvs_storage.begin("dis_config", false)) {  // Open the preferences storage
       FMS_RED_DEBUG_OPEN("Failed to open dis config storage");
    } else {
      FMS_RED_DEBUG_OPEN("Dis config storage opened");
    }

    fms_nvs_storage.putInt("pumpid1", pump1id);
    fms_nvs_storage.putInt("pumpid2", pump2id);
    fms_nvs_storage.putInt("pumpid3", pump3id);
    fms_nvs_storage.putInt("pumpid4", pump4id);
    fms_nvs_storage.putInt("pumpid5", pump5id);
    fms_nvs_storage.putInt("pumpid6", pump6id);
    fms_nvs_storage.putInt("pumpid7", pump7id);
    fms_nvs_storage.putInt("pumpid8", pump8id);
    fms_nvs_storage.putInt("devnum", device_id);
    fms_nvs_storage.putInt("noznum", nozzle_count);
    fms_nvs_storage.end();
    FMS_RED_DEBUG_OPEN("Data Save Done For Dispenser Setting");  // Log the device ID

    vTaskDelay(pdMS_TO_TICKS(100));  // Delay for 100 milliseconds
    FMS_RED_DEBUG_OPEN("Config Loading Testing...");  // Log the device ID
    fms_nvs_storage.begin("dis_config", true); // Open in read-only mode

  int pumpid1 = fms_nvs_storage.getInt("pumpid1", 0);
  int pumpid2 = fms_nvs_storage.getInt("pumpid2", 0);
  int pumpid3 = fms_nvs_storage.getInt("pumpid3", 0);
  int pumpid4 = fms_nvs_storage.getInt("pumpid4", 0);
  int pumpid5 = fms_nvs_storage.getInt("pumpid5", 0);
  int pumpid6 = fms_nvs_storage.getInt("pumpid6", 0);
  int pumpid7 = fms_nvs_storage.getInt("pumpid7", 0);
  int pumpid8 = fms_nvs_storage.getInt("pumpid8", 0);
  int devicenum = fms_nvs_storage.getInt("devnum", 0);
  int nozzlenum = fms_nvs_storage.getInt("noznum", 0);

  fms_nvs_storage.end();

  Serial.println("Loaded config:");
  Serial.println(devicenum);
  Serial.println(nozzlenum);
  Serial.println(pumpid1);
  Serial.println(pumpid2);
  Serial.println(pumpid3);
  Serial.println(pumpid4);
  Serial.println(pumpid5);
  Serial.println(pumpid6);
  Serial.println(pumpid7);
  Serial.println(pumpid8);
  }
}

// helper function (modified : nck)
// generate preset price and liter data form server reponse 01P0001000, 01L0001000
void generate_preset_data() {
  char price[6];
  char liter[3];
  if(charArray[2] == 0x31) preset_state[0] = 0x01; // check price or amount
  if(charArray[2] == 0x32) preset_state[0] = 0x02; // check price or amount
  if(charArray[3] == 'P') { 
    preset_state[1] = 'P';  // Set preset state to price
    for (int i = 0; i < 6; i++) {
      price[i] = presetArray[4+i];  // Store the price in the price array
    }
     preset_price = atoi(price);  // Convert price string to integer
    FMS_RED_LOG_DEBUG("Preset price is %d", preset_price);  // Log the preset price
  } else if (charArray[3] == 'L') {
    preset_state[1] = 'L';  // Set preset state to liter
    for(int i = 0; i < 3; i++) {
      liter[i] = presetArray[4+i];  // Store the liter in the liter array
    }
     preset_liters = atoi(liter);  // Convert liter string to integer
    FMS_RED_LOG_DEBUG("Preset liter is %d", preset_liters);  // Log the preset liter
  }
  }
// generate price change data for mqtt server 
void generate_price_change_data(String message) { 
  char change_price[4];
  message.toCharArray(pricechangeArray, message.length() + 1);  // String to char convert
  Serial.print("Price change is ");
  Serial.println(pricechangeArray);

  charArray[0] = pricechangeArray[0];  // to check nozzle id
  charArray[1] = pricechangeArray[1];  // to check nozzle id
   // start check nozzle id form server response 
   check_server_response_nozzle_id(true);  // Call the function to check server response nozzle ID
   // end check nozzle id form server response
   bool price1 = false;
   bool price2 = false;
   bool price3 = false;
   bool price4 = false;

  if(pump1id == server_response_nozzle_id) {  // Check if the server response nozzle ID matches pump 1 ID
    price1 = true;  // Set price flag for pump 1
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
    FMS_RED_LOG_DEBUG("Nozzle %d price change", server_response_nozzle_id);
   
     // Log approval status
  } else if(pump2id == server_response_nozzle_id) {  // Check if the server response nozzle ID matches pump 2 ID
    price2 = true;  // Set price flag for pump 2
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
    FMS_RED_LOG_DEBUG("Nozzle %d pirce change", server_response_nozzle_id);  // Log approval status
  }
 // get price change data fom server response 
  for(int i = 0; i < 4; i++) {
    change_price[i] = pricechangeArray[2+i];  // Store the price in the price array
  }

  int price_change = atoi(change_price);  // Convert price string to integer
  FMS_RED_LOG_DEBUG("Price change is %d", price_change);  // Log the preset price
  FMS_RED_LOG_DEBUG("+--------------------------------------+");
  // price change mean 92 : 3400 MMK , 95 : 3700 MMK
  // send change price to dispenser
  if(price1) {
    redstar.setPrice(1, price_change);  // Set price for pump 1
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay for 100 milliseconds
    FMS_RED_LOG_DEBUG("Price change for pump 1: %d", price_change);  // Log the price change for pump 1
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
  } else if(price2) {
    redstar.setPrice(2, price_change);  // Set price for pump 2
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay for 100 milliseconds
    FMS_RED_LOG_DEBUG("Price change for pump 2: %d", price_change);  // Log the price change for pump 2
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
  }
}
// check server response nozzle id
void check_server_response_nozzle_id(bool check) {
  if (check) {
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
}
}
  