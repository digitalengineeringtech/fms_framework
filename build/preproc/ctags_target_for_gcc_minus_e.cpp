# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
/*

     FMS main source file

     Author: Trion

     Date: 2025

     Guided By Senior Engineer : Sir Thiha Kyaw

     Description: This file is the main source file of FMS project.

*/
# 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
# 9 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 11 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 12 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 13 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 14 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 15 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2


// //#define DISABLE_MQTT_DEBUG
// #ifdef DISABLE_MQTT_DEBUG
// #undef FMS_MQTT_DEBUG
// #endif
// #define USE_MQTT_DEBUG


//#define DISABLE_LANFENG




FMS_FileManager fileManager;
fms_cli fms_cli(Serial0 /* cli serial port*/, "admin" /* cli password     // change this password*/); // Use "admin" as the default password change your admin pass here
Redstar redstar(Serial1 /* uart2 serial port*/); // create redstar object
fmsLanfeng lanfeng(22, 22); // set re de pin (DTR PIN)s

/* Main function */
void setup() {
  init_staus_leds(); // initialize status LEDs

  fms_cli.begin(115200); // Initialize the CLI with a baud rate of 115200
  fms_cli.register_command("wifi", "Configure WiFi settings", handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect", "Connect to WiFi network", handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart", "Restart the system", handle_restart_command);
  fms_cli.register_command("wifiscan_safe", "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread", "Read current WiFi status", handle_wifi_read_command);
  fms_cli.register_command("wifi_test", "Test WiFi connection", handle_wifi_test_command);
  fms_cli.register_command("uuid_change", "Change Your Device Id unique address", handle_device_id_change_command, 1, 1);

  //fms_initialize_uart2();                   // uart 2
  fms_pin_mode(2, 0x03);

  fms_pin_mode(25, 0x03); // Multiplexer
  fms_pin_mode(26, 0x03);
  fms_pin_mode(27 /* enable input (active LOW) */, 0x03);
  enable_mux(27 /* enable input (active LOW) */); // enable multiplexer (active low)

  fms_run_sd_test(); // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(true); // show serial logging data on Serial Monitor
  fms_boot_count(true); // boot count





  red_star_init(); // redstar init

  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)
  if (fms_initialize_wifi()) { // wifi is connected create all task s
    fms_task_create();
  }
}

void loop() {
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
// cli command function
void handle_wifi_command(const std::vector<String>& args) {
  if (args.size() != 2) {
    fms_cli.respond("wifi", "Usage: wifi <ssid> <password>", false);
    return;
  }
  String ssid = args[0];
  String password = args[1];
  // Save to preferences
  fms_nvs_storage.begin("fms_config", false);
  fms_nvs_storage.putString("ssid", ssid);
  fms_nvs_storage.putString("pass", password);
  fms_nvs_storage.end();

  fms_cli.respond("wifi", "WiFi settings updated. SSID: " + ssid);
}

void handle_restart_command(const std::vector<String>& args) {
  fms_cli.respond("restart", "Restarting system...");
  delay(1000);
  ESP.restart();
}

// Alternative WiFi scan implementation that uses even less memory
void handle_wifi_scan_safe_command(const std::vector<String>& args) {
  // Set a very conservative limit on the number of networks to scan
  const int MAX_NETWORKS = 5;

  // Start scan
  fms_cli.respond("wifiscan_safe", "Scanning for networks...");

  // Set WiFi to station mode
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  delay(100);

  // Use synchronous scan with limited time per channel
  int numNetworks = WiFi.scanNetworks(false, true, false, 200);

  if (numNetworks <= 0) {
    fms_cli.respond("wifiscan_safe", numNetworks == 0 ? "No networks found" : "Scan failed");
    return;
  }

  // Limit the number of networks to prevent memory issues
  if (numNetworks > MAX_NETWORKS) {
    numNetworks = MAX_NETWORKS;
  }

  // Send a simplified response with minimal memory usage
  fms_cli.respond("wifiscan_safe", "Found " + String(numNetworks) + " networks");

  // Process each network individually
  for (int i = 0; i < numNetworks; i++) {
    // Send SSID
    Serial0.print("Network ");
    Serial0.print(i + 1);
    Serial0.print(": ");
    Serial0.println(WiFi.SSID(i));

    // Send RSSI
    Serial0.print("  Signal: ");
    Serial0.print(WiFi.RSSI(i));
    Serial0.println(" dBm");

    // Send Channel
    Serial0.print("  Channel: ");
    Serial0.println(WiFi.channel(i));

    // Send Encryption
    Serial0.print("  Security: ");
    Serial0.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA/WPA2");

    Serial0.println("---");

    // Give the system time to process
    delay(10);
    yield();
  }

  // Free the memory used by the scan
  WiFi.scanDelete();

  // Final message
  fms_cli.respond("wifiscan_safe", "Scan complete");
}

void handle_wifi_connect_command(const std::vector<String>& args) {
  if (args.size() != 2) {
    fms_cli.respond("wifi_connect", "Usage: wifi_connect <ssid> <password>", false);
    return;
  }

  String ssid = args[0];
  String password = args[1];

  // Disconnect from any current WiFi connection
  WiFi.disconnect();
  delay(100);

  // Set WiFi mode to station
  WiFi.mode(WIFI_MODE_STA);

  // Begin connection attempt
  fms_cli.respond("wifi_connect", "Connecting to " + ssid + "...");

  WiFi.begin(ssid.c_str(), password.c_str());

  // Wait for connection with timeout
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
    delay(500);
    Serial0.print(".");
  }
  Serial0.println();

  // Check connection result
  if (WiFi.status() == WL_CONNECTED) {
    // Use individual prints instead of building a large JSON string
    Serial0.println("{");
    Serial0.print("  \"command\": \"wifi_connect\",");
    Serial0.print("  \"result\": \"Connected successfully\",");
    Serial0.print("  \"success\": true,");
    Serial0.print("  \"ip\": \"");
    Serial0.print(WiFi.localIP().toString());
    Serial0.print("\",");
    Serial0.print("  \"ssid\": \"");
    Serial0.print(WiFi.SSID());
    Serial0.print("\",");
    Serial0.print("  \"rssi\": ");
    Serial0.print(WiFi.RSSI());
    Serial0.println("}");
  } else {
    // Connection failed, report error code
    String errorMsg;
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        errorMsg = "SSID not available";
        break;
      case WL_CONNECT_FAILED:
        errorMsg = "Connection failed (wrong password?)";
        break;
      case WL_CONNECTION_LOST:
        errorMsg = "Connection lost";
        break;
      case WL_DISCONNECTED:
        errorMsg = "Disconnected";
        break;
      default:
        errorMsg = "Unknown error: " + String(WiFi.status());
    }

    fms_cli.respond("wifi_connect", "Failed to connect: " + errorMsg, false);
  }
}

void handle_wifi_read_command(const std::vector<String>& args) {
  if (WiFi.status() == WL_CONNECTED) {
    // Use individual prints instead of building a large string
    Serial0.println("{");
    Serial0.print("  \"ssid\": \"");
    Serial0.print(WiFi.SSID());
    Serial0.print("\",");
    Serial0.print("  \"rssi\": ");
    Serial0.print(WiFi.RSSI());
    Serial0.print(",");
    Serial0.print("  \"ip\": \"");
    Serial0.print(WiFi.localIP().toString());
    Serial0.println("\"");
    Serial0.println("}");
  } else {
    fms_cli.respond("wifiread", "WiFi not connected", false);
  }
}

// Test command to run a series of tests
void handle_test_command(const std::vector<String>& args) {
  fms_cli.respond("test", "Running fms_cli tests...");

  // Test help command
  Serial0.println("\n--- Testing help command ---");
  fms_cli.execute_test_command("help");

  // Test echo command
  Serial0.println("\n--- Testing echo command ---");
  fms_cli.execute_test_command("echo on");
  fms_cli.execute_test_command("echo off");
  fms_cli.execute_test_command("echo");

  // Test wifi read command
  Serial0.println("\n--- Testing wifiread command ---");
  fms_cli.execute_test_command("wifiread");

  // Test invalid command
  Serial0.println("\n--- Testing invalid command ---");
  fms_cli.execute_test_command("invalidcommand");

  fms_cli.respond("test", "Tests completed");
}

// WiFi connection test command
void handle_wifi_test_command(const std::vector<String>& args) {
  fms_cli.respond("wifi_test", "Running WiFi connection tests...");

  // Get stored WiFi credentials
  fms_nvs_storage.begin("fms_config", true); // Read-only mode
  String ssid = fms_nvs_storage.getString("ssid", "");
  String password = fms_nvs_storage.getString("pass", "");
  fms_nvs_storage.end();

  if (ssid.length() == 0 || password.length() == 0) {
    fms_cli.respond("wifi_test", "No WiFi credentials stored. Use 'wifi <ssid> <password>' first.", false);
    return;
  }

  // Test WiFi connection
  Serial0.println("\n--- Testing WiFi connection ---");
  Serial0.printf("Connecting to %s...\n", ssid.c_str());

  // Disconnect from any current WiFi connection
  WiFi.disconnect();
  delay(100);

  // Set WiFi mode to station
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  // Wait for connection with timeout
  unsigned long startTime = millis();
  bool connected = false;

  while (millis() - startTime < WIFI_TIMEOUT) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    delay(500);
    Serial0.print(".");
  }
  Serial0.println();

  if (connected) {
    Serial0.printf("Connected to %s\n", ssid.c_str());
    Serial0.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial0.printf("Signal strength (RSSI): %d dBm\n", WiFi.RSSI());

    // Test ping to gateway
    IPAddress gateway = WiFi.gatewayIP();
    Serial0.printf("Gateway IP: %s\n", gateway.toString().c_str());

    fms_cli.respond("wifi_test", "WiFi connection test successful");
  } else {
    String errorMsg;
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        errorMsg = "SSID not available";
        break;
      case WL_CONNECT_FAILED:
        errorMsg = "Connection failed (wrong password?)";
        break;
      case WL_CONNECTION_LOST:
        errorMsg = "Connection lost";
        break;
      case WL_DISCONNECTED:
        errorMsg = "Disconnected";
        break;
      default:
        errorMsg = "Unknown error: " + String(WiFi.status());
    }

    fms_cli.respond("wifi_test", "WiFi connection test failed: " + errorMsg, false);
  }
}

// Device Id Change Command
void handle_device_id_change_command(const std::vector<String>& args) {
  if (args.size() != 2) {
    fms_cli.respond("device", "Usage: UUID <id> ", false);
    return;
  }
  String uuid = args[0];
  // Save to preferences
  fms_nvs_storage.begin("fms_config", false);
  fms_nvs_storage.putString("uuid", uuid);
  fms_nvs_storage.end();
  fms_cli.respond("UUID", "UUID  updated. UUID: " + uuid);
}

// Custom print function that captures output for the web interface
size_t custom_print(const uint8_t* buffer, size_t size) {
  if (testModeActive) {
    // Add to buffer
    for (size_t i = 0; i < size; i++) {
      serialOutputBuffer += (char)buffer[i];
    }
    // Trim buffer if it gets too large
    if (serialOutputBuffer.length() > MAX_BUFFER_SIZE) {
      serialOutputBuffer = serialOutputBuffer.substring(serialOutputBuffer.length() - MAX_BUFFER_SIZE);
    }
  }
  // Pass through to original Serial
  return Serial0.write(buffer, size);
}

static void cli_task(void* arg) {
  BaseType_t rc;
  // cli command
  while (1) {
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 309 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
              1000 
# 309 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_lanfeng_fun.ino"
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
/*

    * fms_main_func.ino

    * This file is part of the ESP32 FMS 

    * main function link file

*/
# 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_boot_count(bool bootsave) {
  if (!bootsave) {
    return;
  }

  if (!fms_nvs_storage.begin("fms_config", false)) {
    fmsLog(FMS_LOG_ERROR, "Failed to initialize NVS storage");
    return;
  }

  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0) + 1;
  app_cpu = xPortGetCoreID();
  fmsLog(FMS_LOG_INFO, "CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // Close NVS storage
}

void log_chip_info() {




}

bool fms_initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 9600)) {
    //fms_uart2_serial.onReceive(fm_rx_irq_interrupt);  // uart interrupt function
    fmsLog(FMS_LOG_INFO, "[FMSUART2] UART2.. DONE");
    return true;
  } else {
    fmsLog(FMS_LOG_ERROR, "[FMSUART2] UART2.. FAIL");
    return false;
  }
}

bool fms_initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    fmsLog(FMS_LOG_INFO, "Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    fmsLog(FMS_LOG_WARNING, "Failed to connect to WiFi");
    return false;
  }
}

void fms_run_sd_test() {

  fms_config_load_sd_test();
  if (!LittleFS.begin(true)) { // preference storage (1MB)
    Serial0.println("[STORAGE] Failed to mount file system");
  } else {
    Serial0.println("[STORAGE] File system mounted");
  }
  // load system config file

}

void log_debug_info() {




}

void fms_pin_mode(int pin, int mode) { // pin out declare
  pinMode(pin, mode);
}

void fms_dns_responder_init() {
  // Set up mDNS responder
  if (!MDNS.begin(deviceName)) {
    fmsLog(FMS_LOG_ERROR, "Error setting up MDNS responder!");
  } else {
    fmsLog(FMS_LOG_INFO, "mDNS responder started");
    // Add service to MDNS
    MDNS.addService("esp-ota", "tcp", 80);
    MDNS.addService("http", "tcp", 80); // Add standard HTTP service for better discovery
  }
}

// format priceperliter/LSellLiter/SellLiter*PricePreLiter/Totalizer/TotalizerAmount
// generate Final Data format 
String fms_generateFinalData(int pump_id,float sell_price_liters,float sell_liters,float price,float totalizer,unsigned long long totalizer_amount){
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "%02dS%.3fL%.3fP%.2fT%.3fA%llu",
           pump_id, sell_price_liters, sell_liters, price, totalizer, totalizer_amount);
  return String(buffer);
}

// generate live data format
String fms_generateLiveData(int pump_id,float price_liters,float live_liters){
  float liveLiterPrice = price_liters * live_liters; // S = P × L
  char buffer[50]; // Buffer to store formatted string
  // Format: "01S1097L18.232P20000"
  snprintf(buffer, sizeof(buffer), "%02dP%.2fL%.3f",
           pump_id, price_liters, live_liters);
  return String(buffer);
}

// decode preset amount from server
int fms_decodePresetAmount(String presetData){
  int p_index = presetData.indexOf('P');
  if(p_index == -1 || p_index + 1 >= presetData.length()){
    return -1; // Invalid format
  }
  String amountStr = presetData.substring(p_index + 1);
  return amountStr.toInt(); // Convert to integer
}

// decode pump id from server
int fms_decodePumpId(String presetData){
  return presetData.substring(0,2).toInt();
}

/*

{

  "device": "U8990",

  "payload": "N01L18.90P2000",

  "crc_hex": "A1B2",

  "crc_dec": 41394

}

*/
# 131 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void init_staus_leds() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_32) | (1ULL << GPIO_NUM_14) |
                        (1ULL << GPIO_NUM_13) | (1ULL << GPIO_NUM_33),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(GPIO_NUM_32, 1);
    gpio_set_level(GPIO_NUM_14, 1);
    gpio_set_level(GPIO_NUM_13, 1);
    gpio_set_level(GPIO_NUM_33, 1);
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
//#define FMS_MQTT_DEBUG
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
char fms_nmf_tp_prefix[64];

void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial0.print("Message arrived [");
  Serial0.print(topic);
  Serial0.print("] ");
  String incommingMessage = "";
  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
//   FMS_MQTT_LOG_DEBUG("INCOMMING TIOPIC [%s] : %s",topic,incommingMessage);
//   bool tp_match = false;

//   String topic_ = String(topic);
//  // get topic last value(for /) deptos/local_server/1 , /1 value or /permit or /price 
//   int last = topic_.lastIndexOf('/');
//   String topic_value = topic_.substring(last+1);
//   // some return topic contain noz id detpos/local_server/1 , check noz id or other 
//   int nozzle_num = topic_value.toInt();
//   FMS_MQTT_LOG_DEBUG("Topic value : [%s]:%d", topic_value.c_str(),nozzle_num);
//   // check if the topic is approved message or not
//   if(nozzle_num >=1 && nozzle_num <= MAX_NOZZLES){
//     snprintf(approvmsg,sizeof(approvmsg),"%02dappro",nozzle_num);
//     FMS_MQTT_LOG_DEBUG("APPROVED MESSAGE GENERTED : %s",approvmsg);
//     if (incommingMessage == String(approvmsg)){
//       pump_approve[nozzle_num-1] = true;
//       tp_match = true;
//       FMS_MQTT_LOG_DEBUG("APPROVED MESSAGE for Nozzle %d: %s", nozzle_num, incommingMessage.c_str());
//     }
//   }


//   for (int i = 0 ; i < fms_sub_topics_value_count; i++){
//       const char* sub_tp_value = fms_sub_topics_value[i]; // declare in main.h file
//     if(strcmp(sub_tp_value,topic_value.c_str()) == 0)
//     {
//       tp_match = true;
//       switch (i){
//         case 0: {
//           FMS_MQTT_LOG_DEBUG("preset topic matched: %s", topic_value.c_str());
//           int pumpID = fms_decodePumpId(incommingMessage);
//           int presetAmount = fms_decodePresetAmount(incommingMessage);
//           presetMessageGet = true; // for preset message get from mqtt broker
//           // pump_approve[pumpID-1] = true;
//           FMS_MQTT_LOG_DEBUG("Pump ID: %d, Preset Amount: %d", pumpID, presetAmount);
//           break;
//         }
//         case 1: {
//           FMS_MQTT_LOG_DEBUG("price topic matched: %s", topic_value.c_str());
//           break;
//         }
//       }
//       FMS_MQTT_LOG_DEBUG("MATCH TRUE");
//       break;
//     } 
//     else {
//         FMS_MQTT_LOG_DEBUG("not matched : [%s] == %s",topic,fms_sub_topics_value[i]);
//     }
//   }

//   if (!tp_match) {
//     FMS_MQTT_LOG_ERROR("Topic not matched : %s", topic);
//   }
//   #ifdef USE_REDSTAR
   redstar_pump_setting(topic,incommingMessage); // call redstar pump setting function
//   #endif
}


void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    ;
    fms_mqtt_client.subscribe(fms_sub_topics[i]);
  }
}

void fms_mqtt_reconnect() {
  // for check client connection online or offline
  const char* willTopic = "device/status";
  const char* willMessage = "offline";
  bool willRetain = true;
  uint8_t willQos = 1;

  while (!fms_mqtt_client.connected()) {
    ;
    String clientId = String(deviceName) + String(random(0xffff), 16);
    if (fms_mqtt_client.connect(clientId.c_str(),sysCfg.mqtt_user,sysCfg.mqtt_password,willTopic,willQos,willRetain,willMessage)) {

      ;
      fms_mqtt_client.publish(willTopic, "online", true);
      fms_subsbribe_topics();
      // Uncomment the following lines to subscribe to additional topics
      // fms_mqtt_client.subscribe("detpos/#");
      // fms_mqtt_client.subscribe("detpos/local_server/#");
      // fms_mqtt_client.subscribe("detpos/local_server/price");
      // fms_mqtt_client.subscribe("detpos/local_server/preset");
      // Add additional topic subscriptions if necessary
    } else {
      ;
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5000 ) * ( TickType_t ) 
# 107 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
                1000 
# 107 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
                ) / ( TickType_t ) 1000U ) ));
    }
  }
}

unsigned long previousMillis = 0;
const long interval = 1000; // Interval for sending messages
bool ledState_ = false;

static void mqtt_task(void* arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(sysCfg.mqtt_server_host /* mqtt server address*/, 1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (true) {
    unsigned long currentMillis = millis();

    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        ledState_ = !ledState_;
        gpio_set_level(GPIO_NUM_14, ledState_);
      }
    } else {
      ;
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 135 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
              1000 
# 135 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"

// reference from 74hc lib channel chooser
 void selectMuxChannel(uint8_t channel){
    digitalWrite(25, channel & 0x01); // on 
    digitalWrite(26, (channel >> 1) & 0x01);// off
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1 ) * ( TickType_t ) 
# 6 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino" 3
              1000 
# 6 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
              ) / ( TickType_t ) 1000U ) )); // switch time on off time
 }

 void enable_mux(int pin){
    digitalWrite(pin,0x0);
 }

 void disable_mux(int pin){
    digitalWrite(pin,0x1);
 }

 void test_mux(){
    for (int device = 0; device < 4; device++) {
        selectMuxChannel(device);
        Serial1 /* uart2 serial port*/.println("Hello device " + String(device));
        vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 21 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino" 3
                  1000 
# 21 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
                  ) / ( TickType_t ) 1000U ) )); // switch time on off time
        if (Serial1 /* uart2 serial port*/.available()) {
          String reply = Serial1 /* uart2 serial port*/.readStringUntil('\n');
          Serial0.print("[DEBUG] "); Serial0.printf("FROM DEVICE :%d,[%s]\n", device, reply.c_str()); Serial0.println();
        }
      }
 }
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
/* 

ota server 

  * author : Trion

    Guided By : Sir Thiha Kyaw

    Copyright : @2025 iih

    ota server & dns reponseder on network 

    using esp ota lib

    file : _fms_main.h

    file : fms_ota_server.ino (rtos task) 

    reference from : mongoose wizard

    description : lite_version ota 

    v 0.1 ota server

*/
# 15 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
// For large uploads - increase buffer size



void fms_info_response() { // mini version show in ota page
  JsonBuilder json;
  json.addString("deviceName", deviceName);
  json.addString("firmwareVersion", firmwareVersion);
  json.addString("ipAddress", WiFi.localIP().toString());
  json.addString("macAddress", WiFi.macAddress());
  json.addInt("rssi", WiFi.RSSI());
  json.addLong("uptime", uptime);
  json.addInt("freeHeap", ESP.getFreeHeap());
  json.addInt("totalHeap", ESP.getHeapSize());
  json.addInt("cpuFreqMHz", ESP.getCpuFreqMHz());
  json.addString("sdkVersion", ESP.getSdkVersion());
  json.addString("status", updateStatus);
  json.addInt("progress", otaProgress);
  json.addBool("otaInProgress", otaInProgress);
  //json.addLong("flashChipSize", ESP.getFlashChipSize());
  cachedInfoResponse = json.toString();
}
void handleDashboard() { // login auth
  if (!isAuthenticated) {
    // Redirect to login if not authenticated
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Redirecting to login...");
    return;
  }
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.sendHeader("Cache-Control", "max-age=86400");
    server.streamFile(file, "text/html");
    file.close();
  }
}

void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");

    if (user == correctUsername && pass == correctPassword) {
      isAuthenticated = true; // Mark as logged in
      server.send(200, "text/plain", "Login Successful!");
    } else {
      server.send(401, "text/plain", "Invalid Username or Password");
    }
  } else {
    server.send(400, "text/plain", "Missing credentials");
  }
}

void handleLogout() {
  isAuthenticated = false;
  server.send(200, "text/plain", "Logged out successfully!");
}

void fms_set_ota_server() {
  fmsLog(FMS_LOG_INFO, "ota server created");
  server.enableCORS(true);
  const char* cacheControl = "max-age=86400";
  server.serveStatic("/", LittleFS, "/login.html", cacheControl);

  server.on("/login", handleLogin);
  server.on("/dashboard", handleDashboard);

  server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);
  server.on("/api/info", HTTP_GET, []() {
    if (millis() - lastInfoRequest < INFO_CACHE_TIME && cachedInfoResponse.length() > 0) {
      server.send(200, "application/json", cachedInfoResponse);
      return;
    }
    fms_info_response();
    lastInfoRequest = millis();
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send(200, "application/json", cachedInfoResponse);
  });
  server.on("/logout", handleLogout); // logout ota server
  server.on(
    "/api/update", HTTP_POST, []() {
      // This handler is called after the upload is complete
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");

      // Re-enable task watchdog if it was disabled
      esp_task_wdt_config_t wdtConfig;
      wdtConfig.timeout_ms = 30 * 1000;
      wdtConfig.idle_core_mask = 0;
      wdtConfig.trigger_panic = true;
      esp_task_wdt_init(&wdtConfig);

      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 108 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
                1000 
# 108 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                ) / ( TickType_t ) 1000U ) ));
      ESP.restart();
    },
    []() {
      // This handler processes the actual upload
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        // Disable task watchdog for update process
        esp_task_wdt_deinit();
        fmsLog(FMS_LOG_INFO, "Update: %s\n", upload.filename.c_str());
        updateStatus = "Update started";
        otaInProgress = true;
        otaProgress = 0;
        contentLength = 0;
        uploadedBytes = 0;

        // Get content length from headers if available
        if (server.hasHeader("Content-Length")) {
          contentLength = server.header("Content-Length").toInt();
          fmsLog(FMS_LOG_INFO, "Content-Length: %d bytes\n", contentLength);
        }
        // Free up as much memory as possible
        fmsLog(FMS_LOG_INFO, "Free heap before update: %d bytes\n", ESP.getFreeHeap());
        // Start the update with the correct partition type
        if (!Update.begin(contentLength > 0 ? contentLength : 0xFFFFFFFF, 0)) {
          Update.printError(Serial0);
          updateStatus = "OTA Error";
          otaInProgress = false;
          // Re-enable task watchdog
          esp_task_wdt_config_t wdtConfig;
          wdtConfig.timeout_ms = 30 * 1000;
          wdtConfig.idle_core_mask = 0;
          wdtConfig.trigger_panic = true;
          esp_task_wdt_init(&wdtConfig);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Process upload chunks in larger batches for better performance
        uploadedBytes += upload.currentSize;
        // Calculate progress
        if (contentLength > 0) {
          otaProgress = (uploadedBytes * 100) / contentLength;
        } else if (Update.size()) {
          otaProgress = (uploadedBytes * 100) / Update.size();
        }
        // Write the received bytes to flash
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial0);
          updateStatus = "OTA Error";
        }
        // Log progress less frequently to reduce overhead
        if (otaProgress % 10 == 0) {
          fmsLog(FMS_LOG_INFO, "Progress: %u%% (%u / %u bytes)\n", otaProgress, uploadedBytes, contentLength > 0 ? contentLength : Update.size())


                                                                         ;
        }
        // Yield to avoid watchdog trigger
        yield();
      } else if (upload.status == UPLOAD_FILE_END) {
        // Finalize the update
        if (Update.end(true)) {
          fmsLog(FMS_LOG_INFO, "Update Success: %u bytes\nRebooting...\n", uploadedBytes);
          updateStatus = "Update successful";
          otaProgress = 100;
        } else {
          Update.printError(Serial0);
          updateStatus = "OTA Error";
        }
        otaInProgress = false;
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        otaInProgress = false;
        updateStatus = "Update aborted";
        fmsLog(FMS_LOG_INFO, "Update aborted");
        // Re-enable task watchdog
        esp_task_wdt_config_t wdtConfig;
        wdtConfig.timeout_ms = 30 * 1000;
        wdtConfig.idle_core_mask = 0;
        wdtConfig.trigger_panic = true;
        esp_task_wdt_init(&wdtConfig);
      }
      yield();
    });

  // Handle restart request
  server.on("/api/restart", HTTP_POST, []() {
    server.send(200, "text/plain", "Restarting...");
    delay(1000);
    ESP.restart();
  });

  // Set up mDNS responder
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
}

static void web_server_task(void* arg) {
  if (!MDNS.begin(deviceName)) { // Set up mDNS responder
    Serial0.println("[DNS] Error setting up MDNS responder!");
  } else {
    Serial0.println("[DNS] mDNS responder started");
    MDNS.addService("esp-ota", "tcp", 80); // Add service to MDNS
    MDNS.addService("http", "tcp", 80); // Add standard HTTP service for better discovery
  }
  deviceName = deviceName + String(WiFi.macAddress()).c_str();
  fms_set_ota_server();
  server.begin();
  while (1) {
    server.handleClient();
    // Update uptime counter (every second)
    if (millis() - lastUptimeUpdate >= 1000) {
      uptime++;
      lastUptimeUpdate = millis();
    }
    // sometime webserver is outoff stack error , fix your stack size in fms_header.h file 
    // pending to upgrade  mongoose wizard ui builder 
    // UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("Stack Remaining: ");
    // Serial.println(stackRemaining);  // Prints remaining stack (in words)
    vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 229 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
                     1000 
# 229 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                     ));
   // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
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

*/
# 35 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
uint8_t addresscount = 1; // Address for the Redstar device
uint8_t nozzlecount = 2; // Number of nozzles
const unsigned long state_query_interval = 2;
unsigned long current_time_seconds = 0; // Current time in seconds
unsigned long last_pump_query_interval = 0; // Interval for querying state
unsigned char buffer[30];

uint8_t server_response_nozzle_id = 0; // Nozzle ID from server response
int preset_price = 0; // Preset price
int preset_liters = 0; // Preset liters

char preset_state[2]; // check preset noz id and amount or liters (0x01,'P')
char charArray[4]; // check nozzle id form server response 
char volume_char[4]; // separate real-time buffer data to get volume (liter) data convert into Decimal data
char amount_char[4]; // separate real-time buffer data to get amount (price) data convert into Decimal data
char mqttdatabuf[50]; // Buffer for MQTT data
char presetArray[13];
char pricechangeArray[7]; // Price change array  

bool pump1_cancel = false; // nozzle one cancel count
bool pump2_cancel = false; // nozzle two cancel count
bool reload_check_1 = false; // control reload function // pump reloading check 
bool reload_check_2 = false; // control reload function
bool presetcount = false; // Preset count flag
bool lastpresetcount = false; // Last preset count flag
bool preset_check = false; // Preset check flag
bool lastpreset_send = false; // Last preset check flag
bool nozzle1_approval = false; // Approval status for nozzle 1
bool nozzle2_approval = false; // Approval status for nozzle 2

String price_state; // to store converted price request data
String total_state; // to store converted  totalizer liter data
String total_amount; // to store converted toatlizer ammount data
String pump2_status = "ide"; // control for nozzle one request final data
String pump1_status = "ide"; // control for nozzle two request final data
String permit_msg = "permit"; // Permit message
String cancel_msg = "cancel"; // Cancel message
String price;
String liter;


// eeprom data hard coding
uint8_t pump1id = 1; // Nozzle ID for pump 1
uint8_t pump2id = 2; // Nozzle ID for pump 2
uint8_t pump3id; // Nozzle ID for pump 3
uint8_t pump4id; // Nozzle ID for pump 4
uint8_t pump5id; // Nozzle ID for pump 5
uint8_t pump6id; // Nozzle ID for pump 6
uint8_t pump7id; // Nozzle ID for pump 7
uint8_t pump8id; // Nozzle ID for pump 8

// device id 
uint8_t device_id = 0; // Device ID
uint8_t nozzle_count = 0; // Nozzle count

unsigned char* response;
uint8_t length = 0;
uint8_t data_count = 0; // Length of the response
// Create an instance of the Redstar class
void red_star_init() {
  redstar.begin(19200, true, 16, 17); // Initialize the Redstar object with the specified baud rate and pins
}


// red start main function (included , pump state, nozzle lifted, fueling)
// check the response from dispenser

void red_star_main() {
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 103 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
            1000 
# 103 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
            ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
  if(redstar.update()){
    response = redstar.parseResponse(length); // Parse the response from the Redstar device
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Data Count: %d", data_count); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Length: %d", length); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Data: "); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("RESPONSE: 0x%02X ",response[data_count]); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();

    data_count++;
  if(addresscount > nozzlecount) addresscount = 1; // Reset address count if it exceeds the number of nozzles
  // normal state -> 0x12 0x57 
  if (response[length - 2] == 0x12 && (response[length - 1] == 0x57 || response[length - 1] == 0x56 || response[length - 1] == 0x53 || response[length - 1] == 0x52)) {
    for (int i = 0; i < 8; i++) {
      buffer[i] = response[i]; // Store the response in the buffer
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 119 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
                1000 
# 119 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
                ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
      Serial0.print(length);
      Serial0.print("/FE/");
      Serial0.print("0x");
      Serial0.print(buffer[i], 16); // Print the buffer contents in hexadecimal format
      Serial0.print(" ");
    }
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Fueling"); Serial0.println();
    data_count = 0; // Reset length for the next response
    length = 0; // Reset length for the next response
    if (buffer[0] == 0x01 && pump1_status == "fuel" || reload_check_1) {
      addresscount = 1; // Check if the first byte is 0x01
      send_fuel_fun(response); // Request fuel data from the dispenser
      Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("------- FINAL-------     1"); Serial0.println();
      send_read_price(response); // Request price data from the dispenser
      send_read_total(); // Request total data from the dispenser
      Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d is in use", addresscount); Serial0.println(); // Log the nozzle in use
    } else {
      Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d is not in use", addresscount); Serial0.println(); // Log the nozzle not in use
    }
  }
  // nozel lifted state -> 0x12 0x77 or 0x76,0x72,0x73 response nozzle lifted condition
  else if(response[data_count - 2] == 0x12 && (response[data_count - 1] == 0x77 || response[data_count - 1] == 0x76 || response[data_count - 1] == 0x72 || response[data_count - 1] == 0x73)) {
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
    Serial0.print("[REDSTAR][ERROR] "); Serial0.printf("No data received"); Serial0.println();
    length = 0;
    data_count = 0; // Reset length and data count for the next response
    check_pump_state_interval(); // Check the pump state at regular intervals
  }

}

void send_read_price(unsigned char* buffer_in){
  while(!redstar.update()) { // Wait for the Redstar device to update
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 183 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 183 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
  }
  data_count = 0; // Reset data count
  while(redstar.update()) { // Wait for the Redstar device to update
    Serial0.print("0x");
    Serial0.print(buffer_in[data_count],16); // Print the response in hexadecimal format
    Serial0.print(" ");
    data_count++;

  }
  data_count = 0; // Reset data count
  if(buffer_in[0] == 0x01) { // Check if the first byte is 0x01
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("READING PRICE 1"); Serial0.println();
  redstar.readPrice(pump1id); // Set the price for pump 1
  } else if(buffer_in[0] == 0x02) { // Check if the first byte is 0x02
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("READING PRICE 2"); Serial0.println();
    redstar.readPrice(pump2id); // Set the price for pump 2
}
vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 10 ) * ( TickType_t ) 
# 201 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
          1000 
# 201 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
          ) / ( TickType_t ) 1000U ) )); // Delay for 10 milliseconds
Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("SENDING READ PRICE FOR ADDRESS: %d", buffer_in[0]); Serial0.println(); // Log the address count
while(!redstar.update()) { // Wait for the Redstar device to update
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 204 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
            1000 
# 204 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
            ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds 
}

data_count = 0; // Reset data count
while(redstar.update()) { // Wait for the Redstar device to update
  Serial0.print("0x");
  Serial0.print(buffer_in[data_count],16); // Print the response in hexadecimal format
  Serial0.print(" ");
  data_count++;
}
data_count = 0; // Reset data count
price_state = (buffer_in[4] << 24 | buffer_in[5] << 16) | (buffer_in[6] << 8) | buffer_in[7]; // convert hex to String
Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Price is => %s", price_state.c_str()); Serial0.println(); // Log the price

send_read_state(); // Request pump state
}

void send_fuel_fun(unsigned char* buffer_in) { // similar send_fuel_fun
  if(buffer[0] == 0x01) {
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("SENDING FULE 1"); Serial0.println();
    redstar.sendFuel(pump1id); // Send fuel command for pump 1
    pump1_cancel = false; // Reset pump 1 cancel flag
  } else if(buffer[0] == 0x02) {
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("SENDING FUEL 2"); Serial0.println();
    redstar.sendFuel(pump2id); // Send fuel command for pump 2
    pump2_cancel = false; // Reset pump 2 cancel flag
  }

  while(redstar.update()) { // Wait for response
    Serial0.print("0x");
    Serial0.print(buffer_in[data_count],16);
    Serial0.print(" ");
    data_count++;
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 237 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 237 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
  }
  data_count = 0; // Reset data count
 for (int i = 0; i < 4; i++) {
  amount_char[i] = buffer_in[4 + i]; // Extract amount data
 }

  for (int i = 0; i < 4; i++) {
    volume_char[i] = buffer_in[8 + i]; // Extract volume data
  }

  price = (amount_char[0] << 24 | amount_char[1] << 16) | (amount_char[2] << 8) | amount_char[3]; // Calculate price
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Price is => %s", price.c_str()); Serial0.println(); // Log the price

  liter = (volume_char[0] << 24 | volume_char[1] << 16) | (volume_char[2] << 8) | volume_char[3]; // Calculate volume
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Liter is => %s", liter.c_str()); Serial0.println(); // Log the volume

  if (!reload_check_2 && !reload_check_1) { // Check reload flags
    //mqttpplive(); // Send MQTT live update
  }

  send_read_state(); // Request pump state




}
// check the dispenser condition
void send_read_state() {
  if(addresscount > nozzlecount) addresscount = 1; // Reset address count if it exceeds 2
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 50 ) * ( TickType_t ) 
# 267 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
            1000 
# 267 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
            ) / ( TickType_t ) 1000U ) )); // Delay for 50 milliseconds
  Serial0.println(redstar.readState(addresscount),16); // Read state for address count
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 10 ) * ( TickType_t ) 
# 269 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
            1000 
# 269 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
            ) / ( TickType_t ) 1000U ) )); // Delay for 50 milliseconds
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("SENDING READ STATE FOR ADDRESS: %d", addresscount); Serial0.println();
  addresscount++; // Increment address count
}

void send_read_total() {
  while(!redstar.update()) { // Wait for the Redstar device to update
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 276 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 276 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
  }

  data_count = 0; // Reset data count
  length = 0; // Reset length
  while(redstar.update()){
    response = redstar.parseResponse(length); // Parse the response from the Redstar device
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5 ) * ( TickType_t ) 
# 283 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 283 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 5 milliseconds
    Serial0.print(length);
    Serial0.print("/");
    Serial0.print(response[data_count], 16);
    Serial0.print(" ");
    data_count++;
  }
  data_count = 0; // Reset data count
}
// send preset state
void send_preset_state(unsigned char* buffer_in) { // similar send_preset_fun
 Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Send preset state"); Serial0.println();

 if(preset_state[1] == 'P') {
  redstar.presetAmount(preset_state[0], preset_price); // Send preset liters
 } else if (preset_state[1] == 'L') {
  redstar.presetLiters(preset_state[0], preset_liters); // Send preset amount
 }
 vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 80 ) * ( TickType_t ) 
# 301 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
           1000 
# 301 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
           ) / ( TickType_t ) 1000U ) )); // Delay for 80 milliseconds
 responsne_buffer(buffer_in,5,"Preset  response");
 if (buffer[1] == 0x04 && buffer[2] == 0x89) { // Check response for expected values
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Preset motor start"); Serial0.println();
  } else {
   send_preset_state(buffer_in); // Retry sending preset
  }
  if (buffer[0] == 0x01) { // Check Buffer for pump1 status
    pump1_status = "fuel";
  } else if (buffer[0] == 0x02) { // Check Buffer for pump2 status
    pump2_status = "fuel";
  }
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 313 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
            1000 
# 313 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
            ) / ( TickType_t ) 1000U ) )); // Wait before the next operation
  send_read_state(); // request pump state
}

void send_approve_state() {
  if (nozzle1_approval) {
    redstar.sendApproval(pump1id); // Send approval for pump 1
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Send approval for pump 1"); Serial0.println();
  } else if (nozzle2_approval) {
    redstar.sendApproval(pump2id); // Send approval for pump 2
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Send approval for pump 2"); Serial0.println();
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
  if (pumpid == 1) { // match Id with initial setup id and append ID from mqtt data arry
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
void check_pump_state_interval() { // check state in every 2 seconds
  current_time_seconds = millis() / 1000; // Get current time in seconds
  if (current_time_seconds - last_pump_query_interval > state_query_interval) {
    //addresscount = 1;                                 // Reset address count
    send_read_state(); // Call the function to send read state command
    last_pump_query_interval = current_time_seconds; // Update last pump query interval
  }
}

// check the serail buffer return from dispenser
void responsne_buffer(unsigned char* buffer_in, int length,const char* logMessage){
for (int i = 0; i < length; i++) {
  buffer[i] = buffer_in[i]; // Store the response in the buffer
    Serial0.print("0x");
    Serial0.print(buffer[i], 16); // Print the buffer contents in hexadecimal format
    Serial0.print(" ");
  }
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("%s", logMessage); Serial0.println(); // Log the message
  data_count = 0; // Reset length for the next response
  length = 0; // Reset length for the next response
}

// mqtt server response buffer (for all control , preset,pricechange,approv)
void redstar_pump_setting(char* topic, String payload) {
  // Permit message reply (e.g., 01permit, 02permit) reply
  if (String(topic) == String(approv_topic)) {
    char pumpapproArray[13]; // Pump approval array
    payload.toCharArray(pumpapproArray, payload.length() + 1); // String to char conversion
    Serial0.print("Approval is ");
    Serial0.println(pumpapproArray);

    // Response is 01approv from MQTT server, hex form is 0x30, 0x31 (01)
    charArray[0] = pumpapproArray[0]; // To check nozzle ID
    charArray[1] = pumpapproArray[1]; // To check nozzle ID

    // Check approval message ID, server response is 01approv, 02approv, 03approv
    check_server_response_nozzle_id(true);

    if (pump1id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 1 ID
      nozzle1_approval = true; // Set approval status for nozzle 1
      Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d approved", server_response_nozzle_id); Serial0.println(); // Log approval status
    } else if (pump2id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 2 ID
      nozzle2_approval = true; // Set approval status for nozzle 2
      Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d approved", server_response_nozzle_id); Serial0.println(); // Log approval status
    }
  }

  if (String(topic) == String(preset_topic)) {
    payload.toCharArray(presetArray, payload.length() + 1); // String to char conversion
    Serial0.print("Preset is ");
    Serial0.println(presetArray);

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
# 503 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
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
    DynamicJsonDocument doc(4096); // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, payload); // check error

    if (error) {
      Serial0.print(((reinterpret_cast<const __FlashStringHelper *>(("JSON parsing failed: ")))));
      Serial0.println(error.c_str());
      return;
    }

    device_id = doc["devicenum"].as<const int>(); // assign device id and nozzle id from JSON data
    nozzle_count = doc["nozzlenum"].as<const int>();
    pump1id = doc["pumpid1"].as<const int>();
    pump2id = doc["pumpid2"].as<const int>();
    pump3id = doc["pumpid3"].as<const int>();
    pump4id = doc["pumpid4"].as<const int>();
    pump5id = doc["pumpid5"].as<const int>();
    pump6id = doc["pumpid6"].as<const int>();
    pump7id = doc["pumpid7"].as<const int>();
    pump8id = doc["pumpid8"].as<const int>();

    if(!fms_nvs_storage.begin("dis_config", false)) { // Open the preferences storage
       ("Failed to open dis config storage");
    } else {
      ("Dis config storage opened");
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
    ("Data Save Done For Dispenser Setting"); // Log the device ID

    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 564 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 564 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 100 milliseconds
    ("Config Loading Testing..."); // Log the device ID
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

  Serial0.println("Loaded config:");
  Serial0.println(devicenum);
  Serial0.println(nozzlenum);
  Serial0.println(pumpid1);
  Serial0.println(pumpid2);
  Serial0.println(pumpid3);
  Serial0.println(pumpid4);
  Serial0.println(pumpid5);
  Serial0.println(pumpid6);
  Serial0.println(pumpid7);
  Serial0.println(pumpid8);
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
    preset_state[1] = 'P'; // Set preset state to price
    for (int i = 0; i < 6; i++) {
      price[i] = presetArray[4+i]; // Store the price in the price array
    }
     preset_price = atoi(price); // Convert price string to integer
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Preset price is %d", preset_price); Serial0.println(); // Log the preset price
  } else if (charArray[3] == 'L') {
    preset_state[1] = 'L'; // Set preset state to liter
    for(int i = 0; i < 3; i++) {
      liter[i] = presetArray[4+i]; // Store the liter in the liter array
    }
     preset_liters = atoi(liter); // Convert liter string to integer
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Preset liter is %d", preset_liters); Serial0.println(); // Log the preset liter
  }
  }

// generate price change data for mqtt server 
void generate_price_change_data(String message) {
  char change_price[4];
  message.toCharArray(pricechangeArray, message.length() + 1); // String to char convert
  Serial0.print("Price change is ");
  Serial0.println(pricechangeArray);

  charArray[0] = pricechangeArray[0]; // to check nozzle id
  charArray[1] = pricechangeArray[1]; // to check nozzle id
   // start check nozzle id form server response 
   check_server_response_nozzle_id(true); // Call the function to check server response nozzle ID
   // end check nozzle id form server response
   bool price1 = false;
   bool price2 = false;
   bool price3 = false;
   bool price4 = false;

  if(pump1id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 1 ID
    price1 = true; // Set price flag for pump 1
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d price change", server_response_nozzle_id); Serial0.println();

     // Log approval status
  } else if(pump2id == server_response_nozzle_id) { // Check if the server response nozzle ID matches pump 2 ID
    price2 = true; // Set price flag for pump 2
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Nozzle %d pirce change", server_response_nozzle_id); Serial0.println(); // Log approval status
  }
 // get price change data fom server response 
  for(int i = 0; i < 4; i++) {
    change_price[i] = pricechangeArray[2+i]; // Store the price in the price array
  }

  int price_change = atoi(change_price); // Convert price string to integer
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Price change is %d", price_change); Serial0.println(); // Log the preset price
  Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
  // price change mean 92 : 3400 MMK , 95 : 3700 MMK
  // send change price to dispenser
  if(price1) {
    redstar.setPrice(1, price_change); // Set price for pump 1
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 659 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 659 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 100 milliseconds
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Price change for pump 1: %d", price_change); Serial0.println(); // Log the price change for pump 1
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
  } else if(price2) {
    redstar.setPrice(2, price_change); // Set price for pump 2
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 664 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino" 3
              1000 
# 664 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Delay for 100 milliseconds
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("Price change for pump 2: %d", price_change); Serial0.println(); // Log the price change for pump 2
    Serial0.print("[REDSTAR][DEBUG] "); Serial0.printf("+--------------------------------------+"); Serial0.println();
  }
}

// check server response nozzle id
void check_server_response_nozzle_id(bool check) {
  if (check) {
  if (charArray[0] == 0x30 && charArray[1] == 0x31) { // checks message ID is 01 and define id is 1
    server_response_nozzle_id = 1;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x32) { // checks message ID is 02 and define id is 2
    server_response_nozzle_id = 2;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x33) { // checks message ID is 03 and define id is 3
    server_response_nozzle_id = 3;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x34) { // checks message ID is 04 and define id is 4
    server_response_nozzle_id = 4;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x35) { // checks message ID is 05 and define id is 5
    server_response_nozzle_id = 5;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x36) { // checks message ID is 06 and define id is 6
    server_response_nozzle_id = 6;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x37) { // checks message ID is 07 and define id is 7
    server_response_nozzle_id = 7;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x38) { // checks message ID is 08 and define id is 8
    server_response_nozzle_id = 8;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x39) { // checks message ID is 09 and define id is 9
    server_response_nozzle_id = 9;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x30) { // checks message ID is 10 and define id is 10
    server_response_nozzle_id = 10;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x31) { // checks message ID is 11 and define id is 11
    server_response_nozzle_id = 11;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x32) { // checks message ID is 12 and define id is 12
    server_response_nozzle_id = 12;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x33) { // checks message ID is 13 and define id is 13
    server_response_nozzle_id = 13;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x34) { // checks message ID is 14 and define id is 14
    server_response_nozzle_id = 14;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x35) { // checks message ID is 15 and define id is 15
    server_response_nozzle_id = 15;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x36) { // checks message ID is 16 and define id is 16
    server_response_nozzle_id = 16;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x37) { // checks message ID is 17 and define id is 17
    server_response_nozzle_id = 17;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x38) { // checks message ID is 18 and define id is 18
    server_response_nozzle_id = 18;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x39) { // checks message ID is 19 and define id is 19
    server_response_nozzle_id = 19;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x30) { // checks message ID is 20 and define id is 20
    server_response_nozzle_id = 20;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x31) { // checks message ID is 21 and define id is 21
    server_response_nozzle_id = 21;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x32) { // checks message ID is 22 and define id is 22
    server_response_nozzle_id = 22;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x33) { // checks message ID is 23 and define id is 23
    server_response_nozzle_id = 23;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x34) { // checks message ID is 24 and define id is 24
    server_response_nozzle_id = 24;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x35) { // checks message ID is 25 and define id is 25
    server_response_nozzle_id = 25;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x36) { // checks message ID is 26 and define id is 26
    server_response_nozzle_id = 26;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x37) { // checks message ID is 27 and define id is 27
    server_response_nozzle_id = 27;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x38) { // checks message ID is 28 and define id is 28
    server_response_nozzle_id = 28;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x39) { // checks message ID is 29 and define id is 29
    server_response_nozzle_id = 29;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x30) { // checks message ID is 30 and define id is 30
    server_response_nozzle_id = 30;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x31) { // checks message ID is 31 and define id is 31
    server_response_nozzle_id = 31;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x32) { // checks message ID is 32 and define id is 32
    server_response_nozzle_id = 32;
  }
}
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
/*

  * fms_sd.cpp

  *  Created on: 2020. 12. 10.

  *   author : thet htar khaing

*/
# 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
bool fms_sd_init() {
  if (!SD.begin(5 /* sd card chip select pin*/)) {
    fmsLog(FMS_LOG_ERROR, "Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    fmsLog(FMS_LOG_ERROR, "No SD card attached");
    ticker.attach(0.6, _led_state);
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 16 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino" 3
              1000 
# 16 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
              ) / ( TickType_t ) 1000U ) ));
    return false;
  }
  fmsLog(FMS_LOG_INFO, "SD CARD MOUNTED");
  return true;
}

void fms_sd_dir(fs::FS& fs, const char* dirname, uint8_t levels) {
  fmsLog(FMS_LOG_INFO, "Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    fmsLog(FMS_LOG_ERROR, "Failed to open directory");
    //return false;
  }
  if (!root.isDirectory()) {
    fmsLog(FMS_LOG_ERROR, "Not a directory");
    //return false;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      fmsLog(FMS_LOG_INFO, "  DIR : ");
      fmsLog(FMS_LOG_INFO, String(file.name()).c_str());
      if (levels) {
        fms_sd_dir(fs, file.name(), levels - 1);
      }
    } else {
      fmsLog(FMS_LOG_INFO, "  FILE: ");
      fmsLog(FMS_LOG_INFO, String(file.name()).c_str());
      fmsLog(FMS_LOG_INFO, "  SIZE: ");
      fmsLog(FMS_LOG_INFO, String(file.size()).c_str());
    }
    file = root.openNextFile();
  }
}

void fms_config_load_sd_test() {
  fms_sd_init();
  fms_sd_dir(SD, "/", 0);
  //return true;
}

bool write_data_sd(char* input) {
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)


  return true;
}

static void sd_task(void* arg) {
  BaseType_t rc;


  while (1) {

    /*

    * Load config data from sd card

    */
# 80 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
    //rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 81 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino" 3
              1000 
# 81 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
              ) / ( TickType_t ) 1000U ) ));
    //write_data_sd("HELLO\n\r");
    //
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool ledState = false;
void _led_state() {
  ledState = !ledState;
  digitalWrite(2, ledState);
}

bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc) {
  rc = xTaskCreatePinnedToCore(
    task_func, // Task function
    name, // Name
    stack_size, // Stack size
    nullptr, // Parameters
    priority, // Priority
    handle, // Handle
    app_cpu // CPU
  );
  
# 17 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino" 3
 ((void)(
# 17 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
 rc == ( ( ( BaseType_t ) 1 ) )
# 17 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino" 3
 ))
# 17 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
                     ;
  if (rc != ( ( ( BaseType_t ) 1 ) )) {
    fmsLog(FMS_LOG_ERROR, "[TASK]%s task created fail", name);
    return false;
  }
  ticker.attach(0.3, _led_state);
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 23 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino" 3
            1000 
# 23 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
            ) / ( TickType_t ) 1000U ) ));
  fmsLog(FMS_LOG_INFO, "[TASK] %s task created done", name);
  return true;
}

bool fms_task_create() {
  BaseType_t sd_rc, wifi_rc, mqtt_rc, cli_rc, uart2_rc, webserver_rc;

  if (!create_task(sd_task, "sdcard", 3000, 2, &hsdCardTask, sd_rc)) return false;
  if (!create_task(wifi_task, "wifi", 3000, 3, &hwifiTask, wifi_rc)) return false;
  if (!create_task(mqtt_task, "mqtt", 3000, 3, &hmqttTask, mqtt_rc)) return false;
  if (!create_task(fms_uart2_task, "uart2", 3000, 1, &huart2Task, uart2_rc)) return false;
  if (!create_task(cli_task, "cli", 3000, 1, &hcliTask, cli_rc)) return false;
  if (!create_task(web_server_task, "webserver", 4096, 4, &hwebServerTask, webserver_rc)) return false;

  return true;
}

bool fm_cli_task_create() {
  BaseType_t cli_rc;
  if (!create_task(cli_task, "cli", 3000, 1, &hcliTask, cli_rc)) return false;
  return true;
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    Serial1 /* uart2 serial port*/.begin(baudrate, SERIAL_8N1, 16, 17); // RXD2 and TXD2 are the GPIO pins for RX and TX
    if (Serial1 /* uart2 serial port*/) {
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 5 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino" 3
                1000 
# 5 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}

void fm_rx_irq_interrupt() { // interrupt RS485/RS232 function
  uint8_t Buffer[30];
  int bytes_received = 0;
  uint16_t size = Serial1 /* uart2 serial port*/.available(); // serial.available
  Serial0.print("[DEBUG] "); Serial0.printf("Got bytes on serial : %d\n", size); Serial0.println();
  while (Serial1 /* uart2 serial port*/.available() && bytes_received < sizeof(Buffer)) {
    yield();
    Buffer[bytes_received] = Serial1 /* uart2 serial port*/.read();
    bytes_received++;
  }
  if(bytes_received > 0) {
    Serial0.print("[DEBUG] "); Serial0.printf("\n uart2 data process \n\r"); Serial0.println();
    Serial0.print("[DEBUG] "); Serial0.printf("uart2 data : %s\n\r", Buffer); Serial0.println();
    Serial0.print("[DEBUG] "); Serial0.printf("uart2 data length : %d\n\r", bytes_received); Serial0.println();
    UART_RECEIVE_STATE = true;
    fms_uart2_decode(Buffer, bytes_received); // decode uart2 data main function
  }

}

void fms_uart2_decode(uint8_t* data, uint32_t len) {


  // Print the raw byte data for debugging
  Serial0.print("[FMSUART2] Received Data: ");
  for (int i = 0; i < len; i++) {
    Serial0.print(data[i], 16); // Print each byte in hex format
    Serial0.print(" ");
  }
  Serial0.println();

  // Example of how to process the data, depending on your protocol
  // FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data); // If data is string
  // Or process the data byte by byte

}

void fms_uart2_task(void* arg) {
  BaseType_t rc;
  while (1) {




    red_star_main(); // redstar protocol

// #ifdef USE_MUX_PC817
// test_mux();
// #endif
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 62 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino" 3
              1000 
# 62 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag) {
  if (flag) {
    // get ssid and password from nvs storage
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    if(ssid_str.length() == 0 || pass_str.length() == 0) {
      gpio_set_level(GPIO_NUM_33, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 9 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 9 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      fmsLog(FMS_LOG_ERROR, "[DEBUG WiFi] wifi .. credential .. value is empty");
      fms_nvs_storage.end();
      return false;
    }
    fms_nvs_storage.end();
    Serial0.print("[DEBUG] "); Serial0.printf("SSID : %s , PASS : %s", ssid_str, pass_str); Serial0.println();
    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);
    if (sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") {
      fmsLog(FMS_LOG_ERROR, "[DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }

    WiFi.mode(WIFI_MODE_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      gpio_set_level(GPIO_NUM_33, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 28 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 28 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      gpio_set_level(GPIO_NUM_33, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      fmsLog(FMS_LOG_INFO, "WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid, sysCfg.wifi_password);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 32 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 32 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
    }
    return true;
  }
}

bool wifi_led_ticker() {
  static bool state = false;
  gpio_set_level(GPIO_NUM_2, state);
  state = !state;
}

uint8_t count = 1;
static void wifi_task(void *arg) {
  BaseType_t rc;
  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      fmsLog(FMS_LOG_WARNING, "Failed to connect to WiFi");
      gpio_set_level(GPIO_NUM_33, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 51 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 51 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      gpio_set_level(GPIO_NUM_33, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 53 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 53 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
    } else {
      fmsLog(FMS_LOG_INFO, "Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(GPIO_NUM_33, 0x1);
      gpio_set_level(GPIO_NUM_32, 0x1);
      gpio_set_level(GPIO_NUM_13, 0x1);
      gpio_set_level(GPIO_NUM_14, 0x0);
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 61 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
              1000 
# 61 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
              ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\test_fms_redstar_fun.ino"
