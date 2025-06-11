# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
/*

  FMS main source file

  Author: Trion

  Date: 2025

  Guided By Senior Engineer : Sir Thiha Kyaw

  Description: This file is the main source file of FMS project.

*/
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
/* device login page */
const String correctUsername = "admin"; /* change your login username here*/
const String correctPassword = "admin"; /* change your login pass here*/
const String firmwareVersion = "0.3.0"; /* Current firmware version*/
String deviceName = "ultm_25505v01_"; /* device ID (for)  change here like this user can change with configpanel*/

/* end change note  */


/* #define USE_RESTAR */

// #define USE_REDSTAR

# 24 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 25 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 27 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 28 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 29 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2
# 31 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino" 2


// #define DISABLE_MQTT_DEBUG
// #ifdef DISABLE_MQTT_DEBUGP
// #undef FMS_MQTT_DEBUG
// #endif
// #define USE_MQTT_DEBUG
# 47 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
FMS_FileManager fileManager;
fms_cli fms_cli(Serial0 /* cli serial port*/, "admin" /* cli password change this password*/); // Use "admin" as the default password change your admin pass here
Redstar redstar(Serial1 /* uart2 serial port*/); // create redstar object
TatsunoProtocol tatsuno(Serial1 /* uart2 serial port*/); // Create an instance of TatsunoProtocol with the HardwareSerial object
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
  fms_cli.register_command("protocol", "Set Protocol", handle_protocol_command, 1, 1);
  fms_cli.register_command("protocol_config","Set Protococl Congfig", handle_protocol_config_command, 11, 11);

  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)

  fms_pin_mode(2, 0x03);

  /* test features protocol selection 

 // fms_load_protocol_config();  // load protocol config from nvs storage



  while (sysCfg.protocol == "0") {  // wait for protocol to be set

    FMS_LOG_ERROR("Protocol not set, waiting...");

    vTaskDelay(pdMS_TO_TICKS(1000));  // wait for 1 second

  }

  */
# 83 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
  fms_pin_mode(25, 0x03); // Multiplexer
  fms_pin_mode(26, 0x03);
  fms_pin_mode(27 /* enable input (active LOW) */, 0x03);
  enable_mux(27 /* enable input (active LOW) */); // enable multiplexer (active low)


  fms_run_sd_test(); // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(true); // show serial logging data on Serial Monitor
  fms_boot_count(true); // boot count
# 103 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
  fms_tatsuno_init(); // tatsuno init


  // /* test features
  // if (fms_initialize_wifi() && sysCfg.protocol != "0") {  // wifi is connected create all task s
  //   fms_task_create();
  // }
  // */

  if (fms_initialize_wifi()) { // wifi is connected create all task s
    fms_task_create();
  }
}

void loop() {
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
/*

change note : 

  if (protocol != "tatsuno" && protocol != "gilbarco" && protocol != "redstar" && protocol != "haungyang") {

    fms_cli.respond("protocol_config", "Invalid protocol. Must be tatsuno, gilbarco, redstar, or haungyang", false);

    return;

  }



*/
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
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
  const int MAX_NETWORKS = 5;
  // Start scan
  fms_cli.respond("wifiscan_safe", "Scanning for networks...");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 40 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
            1000 
# 40 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
            ) / ( TickType_t ) 1000U ) ));
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
    Serial0.print("{");
    Serial0.print("\"ssid\": \"");
    Serial0.print(WiFi.SSID());
    Serial0.print("\",");
    Serial0.print("  \"rssi\": ");
    Serial0.print(WiFi.RSSI());
    Serial0.print(",");
    Serial0.print("  \"ip\": \"");
    Serial0.print(WiFi.localIP().toString());
    Serial0.print("\"");
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

void handle_protocol_command(const std::vector<String>& args) {
  if (args.size() != 1) {
    fms_cli.respond("protocol", "Usage: protocol <redstar|tatsuno>", false);
    return;
  }
  String protocol = args[0];
  if (protocol == "redstar") {

    //fms_set_protocol_config("redstar");
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 299 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
              1000 
# 299 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
              ) / ( TickType_t ) 1000U ) )); // Allow time for changes to take effect
    ESP.restart();
    // Set Redstar protocol
    fms_cli.respond("protocol", "Protocol set to Redstar");
    // Add any additional setup for Redstar here
  } else if (protocol == "tatsuno") {

    //fms_set_protocol_config("tatsuno");
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 307 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
              1000 
# 307 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
              ) / ( TickType_t ) 1000U ) )); // Allow time for changes to take effect
    ESP.restart();
    // Set Tatsuno protocol
    fms_cli.respond("protocol", "Protocol set to Tatsuno");
    // Add any additional setup for Tatsuno here
  } else {
    fms_cli.respond("protocol", "Unknown protocol: " + protocol, false);
  }
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

// Protocol config command
void handle_protocol_config_command(const std::vector<String>& args) {
  if (args.size() < 11) {
    fms_cli.respond("protocol_config", "Usage: protocol_config <protocol> <device_id> <nozzle_count> <pump_id1> ... <pump_id8>", false);
    return;
  }

  String protocol = args[0];
  // Validate protocol 
  if (protocol != "tatsuno" && protocol != "gilbarco" && protocol != "redstar" && protocol != "haungyang") {
    fms_cli.respond("protocol_config", "Invalid protocol. Must be tatsuno, gilbarco, redstar, or haungyang", false);
    return;
  }

  // Validate device and nozzle numbers
  uint8_t devn = args[1].toInt();
  uint8_t noz = args[2].toInt();
  if (devn == 0 || devn > 255) {
    fms_cli.respond("protocol_config", "Device ID must be between 1 and 255", false);
    return;
  }
  if (noz == 0 || noz > 8) {
    fms_cli.respond("protocol_config", "Nozzle count must be between 1 and 8", false);
    return;
  }

  // Validate and store pump IDs
  uint8_t pumpids[8] = {0};
  for (int i = 0; i < 8; i++) {
    pumpids[i] = args[i + 3].toInt();
    if (pumpids[i] > 255) {
      fms_cli.respond("protocol_config", "Pump ID " + String(i+1) + " must be between 0 and 255", false);
      return;
    }
  }

  // All validation passed, update configuration
  dcfg.pt = protocol;
  dcfg.devn = devn;
  dcfg.noz = noz;
  memcpy(dcfg.pumpids, pumpids, sizeof(pumpids));

  fms_set_protocol_config(dcfg);
  fms_cli.respond("protocol_config",
    "Protocol configuration saved:\n"
    "Protocol: " + protocol + "\n"
    "Device ID: " + String(devn) + "\n"
    "Nozzle count: " + String(noz), true);
}

static void cli_task(void* arg) {
  BaseType_t rc;
  // cli command
  while (1) {
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 386 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
              1000 
# 386 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_lanfeng_fun.ino"
# 146 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_lanfeng_fun.ino"
/* change note lanfeng mqtt */

/* 

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





*/
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
    fmsLog(FMS_LOG_ERROR, "[fms_main_func.ino:13] Failed to initialize NVS storage");
    return;
  }

  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0) + 1;
  app_cpu = xPortGetCoreID();
  fmsLog(FMS_LOG_INFO, "[fms_main_func.ino:19] CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

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
    fmsLog(FMS_LOG_INFO, "[fms_main_func:45] Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    fmsLog(FMS_LOG_WARNING, "[fms_main_func:48] Failed to connect to WiFi");
    return false;
  }
}

void fms_run_sd_test() {

  fms_config_load_sd_test();
  if (!LittleFS.begin(true)) { // preference storage (1MB)
    Serial0.println("[fms_main_func.ino:57] [STORAGE] Failed to mount file system");
  } else {
    Serial0.println("[fms_main_func.ino:59] [STORAGE] File system mounted");
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

 void fms_load_protocol_config() {

  if (!fms_nvs_storage.begin("fms_d_config", false)) {
    fmsLog(FMS_LOG_ERROR, "[Protocol Config] Failed to initialize NVS storage");
    return;
  }

  // Load protocol type
  sysCfg.protocol = fms_nvs_storage.getString("protocol", "0"); // Default to "redstar"
  fmsLog(FMS_LOG_INFO, "[Protocol Config] Protocol: %s", sysCfg.protocol.c_str());
  // Set protocol defines
  if (sysCfg.protocol == "redstar") {


    fmsLog(FMS_LOG_INFO, "[Protocol Config] Using Redstar protocol");
  } else if (sysCfg.protocol == "tatsuno") {


    fmsLog(FMS_LOG_INFO, "[Protocol Config] Using Tatsuno protocol");
  } else {
    fmsLog(FMS_LOG_ERROR, "[Protocol Config] Unknown protocol, defaulting to Redstar");
    sysCfg.protocol = "0";
  }

  fms_nvs_storage.end();
}

void fms_set_protocol_config(DisConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_d_config", false)) {
    fmsLog(FMS_LOG_ERROR, "[Protocol Config] Failed to initialize NVS storage");
    return;
  }

  // Save configuration to NVS storage
  bool success = true;
  success &= fms_nvs_storage.putString("protocol", cfg.pt);
  success &= fms_nvs_storage.putUChar("devn", cfg.devn);
  success &= fms_nvs_storage.putUChar("noz", cfg.noz);

  // Save pump IDs
  char key[12];
  for (int i = 0; i < 8; i++) {
    snprintf(key, sizeof(key), "pumpid%d", i + 1);
    success &= fms_nvs_storage.putUChar(key, cfg.pumpids[i]);
  }

  if (success) {
    fmsLog(FMS_LOG_INFO, "[Protocol Config] %s configuration saved successfully", cfg.pt.c_str());
    Serial0.printf(
      "Protocol: %s, Device ID: %d, Nozzle count: %d\n"
      "Pump IDs: %d %d %d %d %d %d %d %d\n",
      cfg.pt.c_str(), cfg.devn, cfg.noz,
      cfg.pumpids[0], cfg.pumpids[1], cfg.pumpids[2], cfg.pumpids[3],
      cfg.pumpids[4], cfg.pumpids[5], cfg.pumpids[6], cfg.pumpids[7]
    );

    // Update system configuration
    sysCfg.protocol = cfg.pt;
  } else {
    fmsLog(FMS_LOG_ERROR, "[Protocol Config] Failed to save some configuration values");
  }
  fms_nvs_storage.end();
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
//#define FMS_MQTT_DEBUG
# 11 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
char fms_nmf_tp_prefix[64];
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial0.print("fms_mqtt.ino:13:Message arrived [");
  Serial0.print(topic);
  Serial0.print("] ");
  String message = "";
  for (int j = 0; j < length; j++) message += (char)payload[j];






  // mqtt callback for tatsuno protocol
   tatsuno_pump_setting(topic,message); // call tatsuno pump setting function

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
      /* old feature use below style */
      // Uncomment the following lines to subscribe to additional topics
      // fms_mqtt_client.subscribe("detpos/#");
      // fms_mqtt_client.subscribe("detpos/local_server/#");
      // fms_mqtt_client.subscribe("detpos/local_server/price");
      // fms_mqtt_client.subscribe("detpos/local_server/preset");
      // Add additional topic subscriptions if necessary
    } else {
      ;
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5000 ) * ( TickType_t ) 
# 61 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
                1000 
# 61 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
                ) / ( TickType_t ) 1000U ) ));
    }
  }
}

unsigned long previous_Millis = 0;
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
      if (currentMillis - previous_Millis >= interval) {
        previous_Millis = currentMillis;
        ledState_ = !ledState_;
        gpio_set_level(GPIO_NUM_14, ledState_);
      }
    } else {
      ;
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 88 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
              1000 
# 88 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"






// reference from 74hc lib channel chooser
 void selectMuxChannel(uint8_t channel){
    digitalWrite(25, channel & 0x01); // on 
    digitalWrite(26, (channel >> 1) & 0x01);// off
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1 ) * ( TickType_t ) 
# 11 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino" 3
              1000 
# 11 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
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
# 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino" 3
                  1000 
# 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
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
# 14 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
// For large uploads - increase buffer size
/*

#define HTTP_UPLOAD_BUFLEN 4096         // Increased from default 1460

#define WDT_TIMEOUT_S 30



void fms_info_response() {            // mini version show in ota page

  JsonBuilder json;

  json.addString("deviceName",        deviceName);

  json.addString("firmwareVersion",   firmwareVersion);

  json.addString("ipAddress",         WiFi.localIP().toString());

  json.addString("macAddress",        WiFi.macAddress());

  json.addInt("rssi",                 WiFi.RSSI());

  json.addLong("uptime",              uptime);

  json.addInt("freeHeap",             ESP.getFreeHeap());

  json.addInt("totalHeap",            ESP.getHeapSize());

  json.addInt("cpuFreqMHz",           ESP.getCpuFreqMHz());

  json.addString("sdkVersion",        ESP.getSdkVersion());

  json.addString("status",            updateStatus);

  json.addInt("progress",             otaProgress);

  json.addBool("otaInProgress",       otaInProgress);

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

      isAuthenticated = true;  // Mark as logged in

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

  FMS_LOG_INFO("[fms_ota_server.ino:75] ota server created");

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

  server.on("/logout", handleLogout);  // logout ota server

  server.on(

    "/api/update", HTTP_POST, []() {

      // This handler is called after the upload is complete

      server.sendHeader("Connection", "close");

      server.sendHeader("Access-Control-Allow-Origin", "*");

      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");



      // Re-enable task watchdog if it was disabled

      esp_task_wdt_config_t wdtConfig;

      wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;

      wdtConfig.idle_core_mask = 0;

      wdtConfig.trigger_panic = true;

      esp_task_wdt_init(&wdtConfig);



      vTaskDelay(pdMS_TO_TICKS(1000));

      ESP.restart();

    },

    []() {

      // This handler processes the actual upload

      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {

        // Disable task watchdog for update process

        esp_task_wdt_deinit();

        FMS_LOG_INFO("Update: %s\n", upload.filename.c_str());

        updateStatus = "Update started";

        otaInProgress = true;

        otaProgress = 0;

        contentLength = 0;

        uploadedBytes = 0;



        // Get content length from headers if available

        if (server.hasHeader("Content-Length")) {

          contentLength = server.header("Content-Length").toInt();

          FMS_LOG_INFO("Content-Length: %d bytes\n", contentLength);

        }

        // Free up as much memory as possible

        FMS_LOG_INFO("Free heap before update: %d bytes\n", ESP.getFreeHeap());

        // Start the update with the correct partition type

        if (!Update.begin(contentLength > 0 ? contentLength : UPDATE_SIZE_UNKNOWN, U_FLASH)) {

          Update.printError(Serial);

          updateStatus = "OTA Error";

          otaInProgress = false;

          // Re-enable task watchdog

          esp_task_wdt_config_t wdtConfig;

          wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;

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

          Update.printError(Serial);

          updateStatus = "OTA Error";

        }

        // Log progress less frequently to reduce overhead

        if (otaProgress % 10 == 0) {

          FMS_LOG_INFO("Progress: %u%% (%u / %u bytes)\n",

                       otaProgress,

                       uploadedBytes,

                       contentLength > 0 ? contentLength : Update.size());

        }

        // Yield to avoid watchdog trigger

        yield();

      } else if (upload.status == UPLOAD_FILE_END) {

        // Finalize the update

        if (Update.end(true)) {

          FMS_LOG_INFO("Update Success: %u bytes\nRebooting...\n", uploadedBytes);

          updateStatus = "Update successful";

          otaProgress = 100;

        } else {

          Update.printError(Serial);

          updateStatus = "OTA Error";

        }

        otaInProgress = false;

      } else if (upload.status == UPLOAD_FILE_ABORTED) {

        Update.end();

        otaInProgress = false;

        updateStatus = "Update aborted";

        FMS_LOG_INFO("Update aborted");

        // Re-enable task watchdog

        esp_task_wdt_config_t wdtConfig;

        wdtConfig.timeout_ms = WDT_TIMEOUT_S * 1000;

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

  if (!MDNS.begin(deviceName)) {                // Set up mDNS responder

    Serial.println("[DNS] Error setting up MDNS responder!");

  } else {

    Serial.println("[DNS] mDNS responder started");

    MDNS.addService("esp-ota", "tcp", 80);      // Add service to MDNS

    MDNS.addService("http", "tcp", 80);         // Add standard HTTP service for better discovery

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

   

    vTaskDelay(100 / portTICK_PERIOD_MS);

   // vTaskDelay(pdMS_TO_TICKS(1000));

  }

}



/* change note */
# 233 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
/* user take stack out error */
/* 

sometime webserver is outoff stack error , fix your stack size in fms_header.h file 

pending to upgrade  mongoose wizard ui builder 

UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);

Serial.print("Stack Remaining: ");

Serial.println(stackRemaining);  // Prints remaining stack (in words)

*/
# 242 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
/* version 2*/

// #define HTTP_UPLOAD_BUFLEN 8192         // Increased buffer size for better performance
// #define WDT_TIMEOUT_S 60                // Increased watchdog timeout for large files
// #define PROGRESS_UPDATE_INTERVAL 5      // Update progress every 5%
// #define MAX_RETRY_ATTEMPTS 3

// // Global variables for better progress tracking
// static size_t totalSize = 0;
// static size_t writtenSize = 0;
// static uint8_t lastReportedProgress = 0;
// static unsigned long lastProgressUpdate = 0;
// static unsigned long otaStartTime = 0;

// void fms_info_response() {
//   JsonBuilder json;
//   json.addString("deviceName", deviceName);
//   json.addString("firmwareVersion", firmwareVersion);
//   json.addString("ipAddress", WiFi.localIP().toString());
//   json.addString("macAddress", WiFi.macAddress());
//   json.addInt("rssi", WiFi.RSSI());
//   json.addLong("uptime", uptime);
//   json.addInt("freeHeap", ESP.getFreeHeap());
//   json.addInt("totalHeap", ESP.getHeapSize());
//   json.addInt("cpuFreqMHz", ESP.getCpuFreqMHz());
//   json.addString("sdkVersion", ESP.getSdkVersion());
//   json.addString("status", updateStatus);
//   json.addInt("progress", otaProgress);
//   json.addBool("otaInProgress", otaInProgress);

//   // Add network quality indicators
//   json.addString("wifiMode", WiFi.getMode() == WIFI_STA ? "STA" : "AP");
//   json.addString("channel", String(WiFi.channel()));

//   // Add OTA timing information if in progress
//   if (otaInProgress && otaStartTime > 0) {
//     unsigned long elapsed = (millis() - otaStartTime) / 1000;
//     json.addLong("otaElapsedSeconds", elapsed);
//     if (totalSize > 0 && writtenSize > 0) {
//       // Use addInt or addLong instead of addFloat
//       json.addLong("otaSpeedBps", (long)(writtenSize / (elapsed > 0 ? elapsed : 1)));
//       json.addLong("otaTotalBytes", totalSize);
//       json.addLong("otaWrittenBytes", writtenSize);
//     }
//   }

//   cachedInfoResponse = json.toString();
// }

// void handleDashboard() {
//   if (!isAuthenticated) {
//     server.sendHeader("Location", "/", true);
//     server.send(302, "text/plain", "Redirecting to login...");
//     return;
//   }

//   File file = LittleFS.open("/index.html", "r");
//   if (file) {
//     server.sendHeader("Cache-Control", "max-age=86400");
//     server.sendHeader("Connection", "keep-alive");
//     server.streamFile(file, "text/html");
//     file.close();
//   } else {
//     server.send(404, "text/plain", "Dashboard not found");
//   }
// }

// void handleLogin() {
//   if (server.hasArg("user") && server.hasArg("pass")) {
//     String user = server.arg("user");
//     String pass = server.arg("pass");

//     if (user == correctUsername && pass == correctPassword) {
//       isAuthenticated = true;
//       server.sendHeader("Set-Cookie", "auth=true; Max-Age=3600; Path=/");
//       server.send(200,"text/plain", "Login Successful!");

//       //server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Login successful\"}");
//     } else {
//       server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid credentials\"}");
//     }
//   } else {
//     server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing credentials\"}");
//   }
// }

// void handleLogout() {
//   isAuthenticated = false;
//   server.sendHeader("Set-Cookie", "auth=; Max-Age=0; Path=/");
//   server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Logged out successfully\"}");
// }

// // Improved progress reporting function
// void updateOTAProgress() {
//   if (totalSize > 0) {
//     uint8_t currentProgress = (writtenSize * 100) / totalSize;

//     // Only update if progress changed significantly or enough time has passed
//     if (currentProgress != lastReportedProgress && 
//         (currentProgress - lastReportedProgress >= PROGRESS_UPDATE_INTERVAL || 
//          millis() - lastProgressUpdate > 2000)) {

//       otaProgress = currentProgress;
//       lastReportedProgress = currentProgress;
//       lastProgressUpdate = millis();

//       FMS_LOG_INFO("OTA Progress: %u%% (%u / %u bytes) - Speed: %.2f KB/s\n",
//                    currentProgress,
//                    writtenSize,
//                    totalSize,
//                    (float)(writtenSize * 1000) / (millis() - otaStartTime) / 1024.0);
//     }
//   }
// }

// void fms_set_ota_server() {
//   FMS_LOG_INFO("[fms_ota_server.ino] OTA server created with enhanced performance");

//   // Enable CORS with specific headers for better compatibility
//   server.enableCORS(true);

//   const char* cacheControl = "max-age=86400, public";
//   server.serveStatic("/", LittleFS, "/login.html", cacheControl);

//   server.on("/login", HTTP_POST, handleLogin);
//   server.on("/dashboard", HTTP_GET, handleDashboard);


//   server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);

//   // Enhanced info API with better caching
//   server.on("/api/info", HTTP_GET, []() {
//     server.sendHeader("Access-Control-Allow-Origin", "*");

//     if (otaInProgress || millis() - lastInfoRequest > INFO_CACHE_TIME || cachedInfoResponse.length() == 0) {
//       fms_info_response();
//       lastInfoRequest = millis();
//     }

//     server.sendHeader("Cache-Control", otaInProgress ? "no-cache" : "max-age=5");
//     server.sendHeader("Content-Type", "application/json");
//     server.send(200, "application/json", cachedInfoResponse);
//   });

//   server.on("/logout", HTTP_POST, handleLogout);
//   // Enhanced OTA update handler
//   server.on("/api/update", HTTP_POST, 
//     []() {
//       // Response handler - called after upload completes
//       server.sendHeader("Connection", "close");
//       server.sendHeader("Access-Control-Allow-Origin", "*");
//       server.sendHeader("Content-Type", "application/json");

//       String response;
//       if (Update.hasError()) {
//         response = "{\"status\":\"error\",\"message\":\"Update failed\"}";
//         server.send(500, "application/json", response);
//       } else {
//         response = "{\"status\":\"success\",\"message\":\"Update completed successfully\"}";
//         server.send(200, "application/json", response);
//       }

//       // Re-enable watchdog with original timeout
//       esp_task_wdt_config_t wdtConfig = {
//         .timeout_ms = WDT_TIMEOUT_S * 1000,
//         .idle_core_mask = 0,
//         .trigger_panic = true
//       };
//       esp_task_wdt_init(&wdtConfig);

//       if (!Update.hasError()) {
//         FMS_LOG_INFO("OTA Update successful, restarting in 2 seconds...");
//         vTaskDelay(pdMS_TO_TICKS(2000));
//         ESP.restart();
//       }
//     },
//     []() {
//       // Upload handler - processes the actual file upload
//       HTTPUpload& upload = server.upload();

//       if (upload.status == UPLOAD_FILE_START) {
//         // Disable watchdog for update process
//         esp_task_wdt_deinit();

//         FMS_LOG_INFO("Starting OTA update: %s", upload.filename.c_str());
//         updateStatus = "Initializing update...";
//         otaInProgress = true;
//         otaProgress = 0;
//         lastReportedProgress = 0;
//         writtenSize = 0;
//         otaStartTime = millis();

//         // Get content length from headers
//         totalSize = 0;
//         if (server.hasHeader("Content-Length")) {
//           totalSize = server.header("Content-Length").toInt();
//           FMS_LOG_INFO("Expected file size: %u bytes (%.2f MB)", totalSize, totalSize / 1024.0 / 1024.0);
//         }

//         // Optimize memory before update
//         FMS_LOG_INFO("Free heap before update: %d bytes", ESP.getFreeHeap());

//         // Force garbage collection if available
//         #ifdef ESP_IDF_VERSION_MAJOR
//         if (ESP_IDF_VERSION_MAJOR >= 4) {
//           heap_caps_malloc_extmem_enable(1024);
//         }
//         #endif

//         // Start update with proper size
//         size_t updateSize = totalSize > 0 ? totalSize : UPDATE_SIZE_UNKNOWN;
//         if (!Update.begin(updateSize, U_FLASH)) {
//           Update.printError(Serial);
//           updateStatus = "Failed to start update";
//           otaInProgress = false;

//           // Re-enable watchdog on error
//           esp_task_wdt_config_t wdtConfig = {
//             .timeout_ms = WDT_TIMEOUT_S * 1000,
//             .idle_core_mask = 0,
//             .trigger_panic = true
//           };
//           esp_task_wdt_init(&wdtConfig);
//         } else {
//           updateStatus = "Receiving firmware...";
//           FMS_LOG_INFO("OTA update started successfully");
//         }

//       } else if (upload.status == UPLOAD_FILE_WRITE) {
//         // Write firmware data
//         size_t written = Update.write(upload.buf, upload.currentSize);
//         if (written != upload.currentSize) {
//           Update.printError(Serial);
//           updateStatus = "Write error occurred";
//           FMS_LOG_ERROR("Write error: expected %u bytes, wrote %u bytes", upload.currentSize, written);
//         } else {
//           writtenSize += written;
//           updateOTAProgress();

//           // Fix string concatenation - use snprintf instead
//           char progressBuffer[50];
//           snprintf(progressBuffer, sizeof(progressBuffer), "Writing firmware... %d%%", otaProgress);
//           updateStatus = progressBuffer;
//         }

//         // Yield periodically to prevent watchdog issues
//         if (writtenSize % (32 * 1024) == 0) { // Every 32KB
//           yield();
//           vTaskDelay(1); // Brief delay to allow other tasks
//         }

//       } else if (upload.status == UPLOAD_FILE_END) {
//         // Finalize update
//         updateStatus = "Finalizing update...";

//         if (Update.end(true)) {
//           FMS_LOG_INFO("OTA Update completed successfully: %u bytes written", writtenSize);
//           updateStatus = "Update completed successfully";
//           otaProgress = 100;

//           // Verify the update
//           if (Update.isFinished()) {
//             FMS_LOG_INFO("Update verification successful");
//           } else {
//             FMS_LOG_ERROR("Update verification failed");
//             updateStatus = "Update verification failed";
//           }
//         } else {
//           Update.printError(Serial);
//           updateStatus = "Update finalization failed";
//           FMS_LOG_ERROR("Update end failed");
//         }

//         otaInProgress = false;

//       } else if (upload.status == UPLOAD_FILE_ABORTED) {
//         Update.end();
//         otaInProgress = false;
//         updateStatus = "Update was aborted";
//         FMS_LOG_INFO("OTA Update aborted by user or network error");

//         // Re-enable watchdog
//         esp_task_wdt_config_t wdtConfig = {
//           .timeout_ms = WDT_TIMEOUT_S * 1000,
//           .idle_core_mask = 0,
//           .trigger_panic = true
//         };
//         esp_task_wdt_init(&wdtConfig);
//       }

//       // Yield to prevent blocking
//       yield();
//     }
//   );

//   // Enhanced restart endpoint
//   server.on("/api/restart", HTTP_POST, []() {
//     if (!isAuthenticated) {
//       server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
//       return;
//     }

//     server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Restarting device...\"}");
//     FMS_LOG_INFO("Manual restart requested");
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     ESP.restart();
//   });

//   // Handle preflight OPTIONS requests
//   server.on("/api/update", HTTP_OPTIONS, []() {
//     server.sendHeader("Access-Control-Allow-Origin", "*");
//     server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
//     server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Content-Length");
//     server.send(200);
//   });

//   // 404 handler
//   server.onNotFound([]() {
//     if (server.uri().startsWith("/api/")) {
//       server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"API endpoint not found\"}");
//     } else {
//       server.sendHeader("Location", "/", true);
//       server.send(302, "text/plain", "Redirecting...");
//     }
//   });
// }

// static void web_server_task(void* arg) {
//   // Enhanced mDNS setup
//   String mdnsName = deviceName;
//   mdnsName.replace(":", "");

//   if (!MDNS.begin(mdnsName.c_str())) {
//     Serial.println("[DNS] Error setting up MDNS responder!");
//   } else {
//     Serial.printf("[DNS] mDNS responder started as %s.local\n", mdnsName.c_str());

//     // Add services for better discovery
//     MDNS.addService("http", "tcp", 80);
//     MDNS.addService("esp-ota", "tcp", 80);

//     // Add service properties if supported by your MDNS implementation
//     #ifdef ESP_ARDUINO_VERSION_MAJOR
//     if (ESP_ARDUINO_VERSION_MAJOR >= 2) {
//       MDNS.addServiceTxt("http", "tcp", "version", firmwareVersion);
//       MDNS.addServiceTxt("http", "tcp", "device", deviceName);
//     }
//     #endif
//   }

//   // Removed setTimeout call as it's not supported

//   fms_set_ota_server();
//   server.begin();

//   FMS_LOG_INFO("Web server started on http://%s.local or http://%s", 
//                mdnsName.c_str(), WiFi.localIP().toString().c_str());

//   while (1) {
//     server.handleClient();

//     // Removed MDNS.update() call as it's not supported

//     // Update uptime counter
//     if (millis() - lastUptimeUpdate >= 1000) {
//       uptime++;
//       lastUptimeUpdate = millis();
//     }

//     // Shorter delay for better responsiveness during OTA
//     vTaskDelay(otaInProgress ? pdMS_TO_TICKS(10) : pdMS_TO_TICKS(50));
//   }
// }

// // Optional: Add network optimization function
// void optimizeNetworkSettings() {
//   // Set WiFi power save mode for better performance during OTA
//   WiFi.setSleep(false);

//   // Increase TCP buffer sizes if possible
//   FMS_LOG_INFO("Network optimizations applied");
// }

/* version 3 */

/**

 * ESP32 OTA Update System with Task Management

 * 

 * Features:

 * - Efficient RTOS task management during OTA

 * - Robust error handling and recovery

 * - Progress tracking and performance optimization

 * - Network error resilience

 */
# 637 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
// Configuration constants







// OTA state management
typedef enum {
  OTA_IDLE,
  OTA_STARTING,
  OTA_IN_PROGRESS,
  OTA_FINALIZING,
  OTA_SUCCESS,
  OTA_FAILED,
  OTA_ABORTED
} OtaState;

// Global state variables
static OtaState otaState = OTA_IDLE;
static String otaErrorMessage = "";
static bool otaTasksSuspended = false;
static TaskHandle_t suspendedTasks[10] = {
# 660 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                                         __null
# 660 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                                             };
static int suspendedTaskCount = 0;

// OTA progress tracking
static size_t totalSize = 0;
static size_t writtenSize = 0;
static uint8_t lastReportedProgress = 0;
static unsigned long otaStartTime = 0;
static unsigned long otaTimeoutStart = 0;
static unsigned long lastProgressUpdate = 0;

// Forward declarations
void suspendNonCriticalTasks();
void resumeNonCriticalTasks();
void optimizeSystemForOTA();
void restoreSystemAfterOTA();
void cleanupOTAState();
void handleOTATimeout();
String getOtaStateText();

/**

 * Builds the JSON response for device information

 */
# 683 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void buildInfoResponse() {
  JsonBuilder json;

  // Basic device information
  json.addString("deviceName", deviceName);
  json.addString("firmwareVersion", firmwareVersion);
  json.addString("ipAddress", WiFi.localIP().toString());
  json.addString("macAddress", WiFi.macAddress());
  json.addInt("rssi", WiFi.RSSI());
  json.addLong("uptime", uptime);

  // System resources
  json.addInt("freeHeap", ESP.getFreeHeap());
  json.addInt("totalHeap", ESP.getHeapSize());
  json.addInt("cpuFreqMHz", ESP.getCpuFreqMHz());
  json.addString("sdkVersion", ESP.getSdkVersion());

  // OTA status
  json.addString("status", updateStatus);
  json.addInt("progress", otaProgress);
  json.addBool("otaInProgress", otaInProgress);
  json.addInt("otaState", (int)otaState);
  json.addString("otaStateText", getOtaStateText());

  // Network information
  json.addString("wifiMode", WiFi.getMode() == WIFI_MODE_STA ? "STA" : "AP");
  json.addString("channel", String(WiFi.channel()));

  // OTA performance metrics (only when in progress)
  if (otaInProgress && otaStartTime > 0) {
    unsigned long elapsed = (millis() - otaStartTime) / 1000;
    json.addLong("otaElapsedSeconds", elapsed);

    if (totalSize > 0 && writtenSize > 0) {
      json.addLong("otaSpeedBps", (long)(writtenSize / (elapsed > 0 ? elapsed : 1)));
      json.addLong("otaTotalBytes", totalSize);
      json.addLong("otaWrittenBytes", writtenSize);

      // Timeout information
      unsigned long timeRemaining = (300000 /* OTA timeout (5 minutes)*/ - (millis() - otaTimeoutStart)) / 1000;
      if (timeRemaining > 0) {
        json.addLong("otaTimeoutSeconds", timeRemaining);
      }
    }
  }

  // Task management info
  json.addBool("tasksOptimized", otaTasksSuspended);
  json.addInt("suspendedTaskCount", suspendedTaskCount);

  cachedInfoResponse = json.toString();
}

/**

 * Returns a human-readable text for the current OTA state

 */
# 739 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
String getOtaStateText() {
  switch (otaState) {
    case OTA_IDLE: return "Idle";
    case OTA_STARTING: return "Starting";
    case OTA_IN_PROGRESS: return "In Progress";
    case OTA_FINALIZING: return "Finalizing";
    case OTA_SUCCESS: return "Success";
    case OTA_FAILED: return "Failed";
    case OTA_ABORTED: return "Aborted";
    default: return "Unknown";
  }
}

/**

 * Suspends non-critical tasks to improve OTA performance

 */
# 755 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void suspendNonCriticalTasks() {
  if (otaTasksSuspended) return;

  fmsLog(FMS_LOG_INFO, "Suspending non-critical tasks for OTA");

  // Get list of all tasks
  UBaseType_t taskCount = uxTaskGetNumberOfTasks();
  TaskStatus_t* taskStatusArray = (TaskStatus_t*)malloc(taskCount * sizeof(TaskStatus_t));

  if (taskStatusArray != 
# 764 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                        __null
# 764 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                            ) {
    taskCount = uxTaskGetSystemState(taskStatusArray, taskCount, 
# 765 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                                                                __null
# 765 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                                                                    );
    suspendedTaskCount = 0;

    // Critical task names that should not be suspended
    const char* criticalTasks[] = {
      "IDLE", "esp_timer", "wifi", "tcpip_task", "web_server_task"
    };

    // Current task handle
    TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();

    // Suspend non-critical tasks
    for (UBaseType_t i = 0; i < taskCount && suspendedTaskCount < 10; i++) {
      TaskHandle_t taskHandle = taskStatusArray[i].xHandle;
      const char* taskName = taskStatusArray[i].pcTaskName;
      bool isCritical = false;

      // Check if this is a critical task
      if (taskHandle == currentTask) {
        isCritical = true;
      } else {
        for (size_t j = 0; j < sizeof(criticalTasks)/sizeof(criticalTasks[0]); j++) {
          if (strcmp(taskName, criticalTasks[j]) == 0) {
            isCritical = true;
            break;
          }
        }
      }

      // Suspend if not critical
      if (!isCritical) {
        vTaskSuspend(taskHandle);
        suspendedTasks[suspendedTaskCount++] = taskHandle;
        fmsLog(FMS_LOG_INFO, "Suspended task: %s", taskName);
      }
    }

    free(taskStatusArray);
  }

  otaTasksSuspended = true;
  fmsLog(FMS_LOG_INFO, "Suspended %d non-critical tasks", suspendedTaskCount);
}

/**

 * Resumes previously suspended tasks

 */
# 812 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void resumeNonCriticalTasks() {
  if (!otaTasksSuspended) return;

  fmsLog(FMS_LOG_INFO, "Resuming suspended tasks");

  for (int i = 0; i < suspendedTaskCount; i++) {
    if (suspendedTasks[i] != 
# 818 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                            __null
# 818 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                                ) {
      vTaskResume(suspendedTasks[i]);
      suspendedTasks[i] = 
# 820 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                         __null
# 820 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                             ;
    }
  }

  suspendedTaskCount = 0;
  otaTasksSuspended = false;

  fmsLog(FMS_LOG_INFO, "All tasks resumed");
}

/**

 * Optimizes system settings for OTA update

 */
# 833 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void optimizeSystemForOTA() {
  fmsLog(FMS_LOG_INFO, "Optimizing system for OTA update");

  // Disable WiFi power saving
  WiFi.setSleep(false);

  // Increase CPU frequency
  setCpuFrequencyMhz(240);

  // Suspend non-critical tasks
  suspendNonCriticalTasks();

  // Force garbage collection if available

  if (5 >= 4) {
    heap_caps_malloc_extmem_enable(1024);
  }


  fmsLog(FMS_LOG_INFO, "System optimized. Free heap: %d bytes", ESP.getFreeHeap());
}

/**

 * Restores system settings after OTA completion

 */
# 858 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void restoreSystemAfterOTA() {
  fmsLog(FMS_LOG_INFO, "Restoring system after OTA");

  // Resume suspended tasks
  resumeNonCriticalTasks();

  // Re-enable watchdog
  esp_task_wdt_config_t wdtConfig = {
    .timeout_ms = 60 /* Watchdog timeout in seconds*/ * 1000,
    .idle_core_mask = 0,
    .trigger_panic = true
  };
  esp_task_wdt_init(&wdtConfig);

  fmsLog(FMS_LOG_INFO, "System restored");
}

/**

 * Handles OTA timeout checking

 */
# 878 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleOTATimeout() {
  if (otaState == OTA_IN_PROGRESS && millis() - otaTimeoutStart > 300000 /* OTA timeout (5 minutes)*/) {
    fmsLog(FMS_LOG_ERROR, "OTA update timed out after %lu seconds", 300000 /* OTA timeout (5 minutes)*/ / 1000);

    otaState = OTA_ABORTED;
    updateStatus = "OTA timeout - update aborted";
    otaInProgress = false;

    // Cleanup
    Update.end();
    cleanupOTAState();
  }
}

/**

 * Cleans up OTA state variables

 */
# 895 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void cleanupOTAState() {
  fmsLog(FMS_LOG_INFO, "Cleaning up OTA state");

  // Reset OTA variables
  totalSize = 0;
  writtenSize = 0;
  lastReportedProgress = 0;
  otaProgress = 0;
  otaStartTime = 0;
  otaTimeoutStart = 0;

  // Restore system
  restoreSystemAfterOTA();
}

/**

 * Updates OTA progress tracking

 */
# 913 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void updateOTAProgress() {
  if (totalSize <= 0) return;

  uint8_t currentProgress = (writtenSize * 100) / totalSize;

  // Reset timeout timer on progress
  otaTimeoutStart = millis();

  // Only update if progress changed significantly or enough time has passed
  if (currentProgress != lastReportedProgress &&
      (currentProgress - lastReportedProgress >= 5 /* Update progress percentage interval*/ ||
       millis() - lastProgressUpdate > 2000)) {

    otaProgress = currentProgress;
    lastReportedProgress = currentProgress;
    lastProgressUpdate = millis();

    // Calculate speed in KB/s
    float elapsedSec = (millis() - otaStartTime) / 1000.0f;
    float speedKBps = (elapsedSec > 0) ? (writtenSize / 1024.0f) / elapsedSec : 0;

    fmsLog(FMS_LOG_INFO, "OTA Progress: %u%% (%u / %u bytes) - Speed: %.2f KB/s - Free heap: %d", currentProgress, writtenSize, totalSize, speedKBps, ESP.getFreeHeap())




                                   ;
  }
}

/**

 * Handles dashboard page request

 */
# 946 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleDashboard() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Redirecting to login...");
    return;
  }

  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.sendHeader("Cache-Control", "max-age=86400");
    server.sendHeader("Connection", "keep-alive");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Dashboard not found");
  }
}

/**

 * Handles login request

 */
# 967 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");

    if (user == correctUsername && pass == correctPassword) {
      isAuthenticated = true;
      server.sendHeader("Set-Cookie", "auth=true; Max-Age=3600; Path=/");
      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Login successful\"}");
    } else {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid credentials\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing credentials\"}");
  }
}

/**

 * Handles logout request

 */
# 987 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogout() {
  isAuthenticated = false;
  server.sendHeader("Set-Cookie", "auth=; Max-Age=0; Path=/");
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Logged out successfully\"}");
}

/**

 * Sets up the OTA web server

 */
# 996 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void setupOtaServer() {
  fmsLog(FMS_LOG_INFO, "Setting up OTA server with enhanced performance");

  // Enable CORS
  server.enableCORS(true);

  // Static file serving
  const char* cacheControl = "max-age=86400, public";
  server.serveStatic("/", LittleFS, "/login.html", cacheControl);
  server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);

  // Authentication endpoints
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/dashboard", HTTP_GET, handleDashboard);
  server.on("/logout", HTTP_POST, handleLogout);

  // Device info API
  server.on("/api/info", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    // Update info if needed
    if (otaInProgress || millis() - lastInfoRequest > INFO_CACHE_TIME || cachedInfoResponse.length() == 0) {
      buildInfoResponse();
      lastInfoRequest = millis();
    }

    // Send response
    server.sendHeader("Cache-Control", otaInProgress ? "no-cache" : "max-age=5");
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", cachedInfoResponse);
  });

  // OTA abort endpoint
  server.on("/api/abort", HTTP_POST, []() {
    if (!isAuthenticated) {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
      return;
    }

    if (otaInProgress) {
      fmsLog(FMS_LOG_INFO, "OTA abort requested by user");
      otaState = OTA_ABORTED;
      updateStatus = "OTA aborted by user";
      otaInProgress = false;
      Update.end();
      cleanupOTAState();

      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"OTA update aborted\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No OTA in progress\"}");
    }
  });

  // OTA update endpoint - response handler
  server.on("/api/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Content-Type", "application/json");

    String response;
    int statusCode = 200;

    switch (otaState) {
      case OTA_SUCCESS:
        response = "{\"status\":\"success\",\"message\":\"Update completed successfully\"}";
        break;

      case OTA_ABORTED:
        response = "{\"status\":\"aborted\",\"message\":\"Update was aborted\"}";
        statusCode = 409; // Conflict
        break;

      default:
        response = "{\"status\":\"error\",\"message\":\"Update failed: " + otaErrorMessage + "\"}";
        statusCode = 500; // Internal Server Error
        break;
    }

    server.send(statusCode, "application/json", response);

    // Restart on success after a short delay
    if (otaState == OTA_SUCCESS) {
      fmsLog(FMS_LOG_INFO, "OTA Update successful, restarting in 3 seconds...");
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 3000 ) * ( TickType_t ) 
# 1079 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
                1000 
# 1079 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                ) / ( TickType_t ) 1000U ) ));
      ESP.restart();
    }

    // Cleanup regardless of outcome
    cleanupOTAState();
  },

  // OTA update endpoint - upload handler
  []() {
    HTTPUpload& upload = server.upload();

    switch (upload.status) {
      case UPLOAD_FILE_START:
        handleOtaUploadStart(upload);
        break;

      case UPLOAD_FILE_WRITE:
        handleOtaUploadWrite(upload);
        break;

      case UPLOAD_FILE_END:
        handleOtaUploadEnd(upload);
        break;

      case UPLOAD_FILE_ABORTED:
        handleOtaUploadAbort();
        break;
    }

    // Yield to prevent blocking
    yield();
  });

  // Device restart endpoint
  server.on("/api/restart", HTTP_POST, []() {
    if (!isAuthenticated) {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
      return;
    }

    // Don't allow restart during OTA
    if (otaInProgress) {
      server.send(409, "application/json", "{\"status\":\"error\",\"message\":\"Cannot restart during OTA update\"}");
      return;
    }

    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Restarting device...\"}");
    fmsLog(FMS_LOG_INFO, "Manual restart requested");
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 1128 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
              1000 
# 1128 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
              ) / ( TickType_t ) 1000U ) ));
    ESP.restart();
  });

  // Handle preflight OPTIONS requests
  server.on("/api/update", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Content-Length");
    server.send(200);
  });

  // 404 handler
  server.onNotFound([]() {
    if (server.uri().startsWith("/api/")) {
      server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"API endpoint not found\"}");
    } else {
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "Redirecting...");
    }
  });
}

/**

 * Handles the start of an OTA upload

 */
# 1154 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleOtaUploadStart(HTTPUpload& upload) {
  otaState = OTA_STARTING;
  otaErrorMessage = "";

  // Disable watchdog for update process
  esp_task_wdt_deinit();

  fmsLog(FMS_LOG_INFO, "Starting OTA update: %s", upload.filename.c_str());
  updateStatus = "Initializing update...";
  otaInProgress = true;
  otaProgress = 0;
  lastReportedProgress = 0;
  writtenSize = 0;
  otaStartTime = millis();
  otaTimeoutStart = millis();

  // Get content length from headers
  totalSize = 0;
  if (server.hasHeader("Content-Length")) {
    totalSize = server.header("Content-Length").toInt();
    fmsLog(FMS_LOG_INFO, "Expected file size: %u bytes (%.2f MB)", totalSize, totalSize / 1024.0 / 1024.0)
                                                       ;
  }

  // Optimize system for OTA
  optimizeSystemForOTA();

  // Start update with proper size
  size_t updateSize = totalSize > 0 ? totalSize : 0xFFFFFFFF;
  if (!Update.begin(updateSize, 0)) {
    Update.printError(Serial0);
    otaState = OTA_FAILED;
    otaErrorMessage = "Failed to initialize update";
    updateStatus = otaErrorMessage.c_str();
    otaInProgress = false;
    cleanupOTAState();
  } else {
    otaState = OTA_IN_PROGRESS;
    updateStatus = "Receiving firmware...";
    fmsLog(FMS_LOG_INFO, "OTA update started successfully");
  }
}

/**

 * Handles OTA upload data chunks

 */
# 1200 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleOtaUploadWrite(HTTPUpload& upload) {
  // Check for timeout
  handleOTATimeout();

  if (otaState == OTA_ABORTED) {
    return; // Stop processing if aborted
  }

  // Write firmware data
  size_t written = Update.write(upload.buf, upload.currentSize);
  if (written != upload.currentSize) {
    Update.printError(Serial0);
    otaState = OTA_FAILED;
    otaErrorMessage = "Write error during upload";
    updateStatus = otaErrorMessage.c_str();
    fmsLog(FMS_LOG_ERROR, "Write error: expected %u bytes, wrote %u bytes", upload.currentSize, written)
                                             ;
  } else {
    writtenSize += written;
    updateOTAProgress();

    // Update status with progress
    char progressBuffer[100];
    snprintf(progressBuffer, sizeof(progressBuffer),
             "Writing firmware... %d%% (%u/%u bytes)",
             otaProgress, writtenSize, totalSize);
    updateStatus = progressBuffer;
  }

  // Yield periodically to prevent issues
  if (writtenSize % (16 * 1024) == 0) { // Every 16KB
    yield();
    vTaskDelay(1);
  }
}

/**

 * Handles the end of an OTA upload

 */
# 1239 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleOtaUploadEnd(HTTPUpload& upload) {
  otaState = OTA_FINALIZING;
  updateStatus = "Finalizing update...";

  if (Update.end(true)) {
    fmsLog(FMS_LOG_INFO, "OTA Update completed successfully: %u bytes written", writtenSize);
    otaState = OTA_SUCCESS;
    updateStatus = "Update completed successfully";
    otaProgress = 100;

    // Verify the update
    if (Update.isFinished()) {
      fmsLog(FMS_LOG_INFO, "Update verification successful");
    } else {
      fmsLog(FMS_LOG_ERROR, "Update verification failed");
      otaState = OTA_FAILED;
      otaErrorMessage = "Update verification failed";
      updateStatus = otaErrorMessage.c_str();
    }
  } else {
    Update.printError(Serial0);
    otaState = OTA_FAILED;
    otaErrorMessage = "Update finalization failed";
    updateStatus = otaErrorMessage.c_str();
    fmsLog(FMS_LOG_ERROR, "Update end failed");
  }

  otaInProgress = false;
}

/**

 * Handles OTA upload abort

 */
# 1272 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleOtaUploadAbort() {
  fmsLog(FMS_LOG_INFO, "OTA Upload aborted");
  Update.end();
  otaState = OTA_ABORTED;
  otaInProgress = false;
  updateStatus = "Update was aborted";
  cleanupOTAState();
}

/**

 * Web server task

 */
# 1284 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
static void web_server_task(void* arg) {
  // Set up mDNS responder
  String mdnsName = deviceName;
  mdnsName.replace(":", "");

  if (!MDNS.begin(mdnsName.c_str())) {
    Serial0.println("[DNS] Error setting up MDNS responder!");
  } else {
    Serial0.printf("[DNS] mDNS responder started as %s.local\n", mdnsName.c_str());

    // Add services for better discovery
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("esp-ota", "tcp", 80);

    // Add service properties if supported

    if (3 >= 2) {
      MDNS.addServiceTxt("http", "tcp", "version", firmwareVersion);
      MDNS.addServiceTxt("http", "tcp", "device", deviceName);
    }

  }

  // Set up OTA server
  setupOtaServer();
  server.begin();

  fmsLog(FMS_LOG_INFO, "Web server started on http://%s.local or http://%s", mdnsName.c_str(), WiFi.localIP().toString().c_str())
                                                                   ;

  unsigned long lastHealthCheck = 0;

  // Main server loop
  while (1) {
    server.handleClient();

    // Health check and timeout handling (every second)
    if (millis() - lastHealthCheck > 1000) {
      // Check for OTA timeout
      if (otaInProgress) {
        handleOTATimeout();
      }

      // Update uptime counter
      if (millis() - lastUptimeUpdate >= 1000) {
        uptime++;
        lastUptimeUpdate = millis();
      }

      lastHealthCheck = millis();
    }

    // Adjust delay based on OTA state for better responsiveness
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( otaInProgress ? 5 : 50 ) * ( TickType_t ) 
# 1337 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
              1000 
# 1337 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
              ) / ( TickType_t ) 1000U ) ));
  }
}

/**

 * Optimizes network settings for better OTA performance

 */
# 1344 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void optimizeNetworkSettings() {
  // Disable WiFi power saving
  WiFi.setSleep(false);

  // Set maximum transmit power
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  fmsLog(FMS_LOG_INFO, "Network optimizations applied");
}

/**

 * Checks for recovery from failed OTA on boot

 * Call this in setup()

 */
# 1358 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void checkOtaRecovery() {
  // Check if we're recovering from a failed OTA
  if (otaState == OTA_IN_PROGRESS || otaState == OTA_STARTING) {
    fmsLog(FMS_LOG_INFO, "Detected incomplete OTA on boot, cleaning up...");
    otaState = OTA_FAILED;
    updateStatus = "OTA recovery - previous update failed";
    cleanupOTAState();
  }
}

/**

 * Creates a task to monitor OTA health

 * Call this in setup()

 */
# 1372 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void createOtaMonitorTask() {
  xTaskCreate(
    [](void* parameter) {
      while (1) {
        if (otaInProgress) {
          // Monitor OTA health
          handleOTATimeout();

          // Log memory status during OTA (every 10 seconds)
          if (millis() % 10000 == 0) {
            fmsLog(FMS_LOG_INFO, "OTA Monitor - Free heap: %d, Progress: %d%%, State: %s", ESP.getFreeHeap(), otaProgress, getOtaStateText().c_str())
                                                                                   ;
          }
        }

        vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 1387 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
                  1000 
# 1387 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                  ) / ( TickType_t ) 1000U ) ));
      }
    },
    "ota_monitor",
    2048,
    
# 1392 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
   __null
# 1392 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
       ,
    2,
    
# 1394 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
   __null
  
# 1395 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
 );
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"
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
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
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
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"



/* tatsuno parameter */


// char* Buffer[RESPONSE_BUFFER_SIZE];  // Buffer for incoming data

/*

#define LED_RED                     GPIO_NUM_32

#define LED_GREEN                   GPIO_NUM_14 

#define LED_BLUE                    GPIO_NUM_13

#define LED_YELLOW                  GPIO_NUM_33

*/


// #define RXD2 16
// #define TXD2 17

String incommingMessage;
String incommingmsg1;
String sendMessage;
char ssidtemp[50]; // for WiFi.begin (SSID,        )
char passtemp[50];
// for WiFi.begin (    , pass   )
// const char* ssidtemp = "POS_Server";
// const char* passtemp = "asdffdsa";

char ssidBuf[50];
char passBuf[50];
unsigned char showSSID[6] = { 0X5A, 0XA5, 0X40, 0X82, 0X12, 0x00 };
unsigned char showPASS[6] = { 0X5A, 0XA5, 0X40, 0X82, 0X13, 0x00 };
unsigned char page[9] = { 0X5A, 0XA5, 0X07, 0X82, 0X00, 0X84, 0X5A, 0X01, 0X00 }; // Page change
unsigned char deviceary[8] = { 0x5A, 0XA5, 0x05, 0X82, 0x31, 0x00, 0x00, 0x00 };

int wifitrytime = 0;

// to dispenser
uint8_t enq1[4] = { 0x04, 0x40, 0x51, 0x05 };
uint8_t enq2[4] = { 0x04, 0x41, 0x51, 0x05 };

// unsigned char ACK1[2] = { 0x10, 0x31 };
uint8_t ACK1[2] = { 0x10, 0x31 };

// unsigned char select1[4] = { 0x04, 0x40, 0x41, 0x05 };
// unsigned char select2[4] = { 0x04, 0x41, 0x41, 0x05 };

uint8_t select1[4] = { 0x04, 0x40, 0x41, 0x05 };
uint8_t select2[4] = { 0x04, 0x41, 0x41, 0x05 };

unsigned char EOT[1] = { 0x04 };

unsigned char totalizerstatus1[7] = { 0x02, 0x40, 0x41, 0x32, 0x30, 0x03, 0x00 };
unsigned char totalizerstatus2[7] = { 0x02, 0x41, 0x41, 0x32, 0x30, 0x03, 0x01 };
unsigned char pump1statusary[7] = { 0x02, 0x40, 0x41, 0x31, 0x35, 0x03, 0x06 };
unsigned char pump2statusary[7] = { 0x02, 0x41, 0x41, 0x31, 0x35, 0x03, 0x07 };
// unsigned char Buffer[50];
uint8_t Buffer[50];
int i = 0;
// uint8_t CalculatedCRCdata[9] = { 0x02, 0x00, 0x51, 0x30, 0x30, 0x00, 0x00, 0x03, 0x00 };
uint8_t CalculatedCRCdata[9] = { 0x02, 0x00, 0x51, 0x30, 0x30, 0x00, 0x00, 0x03, 0x00 };
uint8_t CalculatedApprodata[20] = { 0x02, 0x41, 0x41, 0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x03, 0x30 };
uint8_t CalculatedPresetdata[20] = { 0x02, 0x41, 0x41, 0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x03, 0x30 };

// uint8_t unitpriceary1[4] = { 0x32, 0x35, 0x31, 0x30 };
// uint8_t unitpriceary2[4] = { 0x32, 0x35, 0x31, 0x30 };

uint8_t unitpriceary1[4] = { 0x30, 0x30, 0x30, 0x30 };
uint8_t unitpriceary2[4] = { 0x30, 0x30, 0x30, 0x30 };
//mqtt
char pumpapprobuf[22] = "detpos/local_server/1";
char pricechange[26] = "detpos/local_server/price";
char pumppresetbuf[28] = "detpos/local_server/preset";
char pumpfinalreloadbuf[23] = "detpos/device/Reload/1"; // reload add

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
int addresscount = 1; // for pump address in looping
int pumpnum;
bool pumppresetcount = false;
char charArray[3];
char tempcharArray[1];
bool count04 = true;

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
bool pumplivefor1 = false; // for select count
bool pumplivecountfor1 = false; // for select count
bool pumplivefor2 = false; // for select count
bool pumplivecountfor2 = false; // for select count

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
  .devn = 1, // Device number
  .noz = 2, // Nozzle number
  .pumpids = {1, 2, 0, 0, 0, 0, 0, 0} // Pump IDs
};

bool fms_save_tatsuno_config(TatsunoConfig& cfg);
bool fms_load_tatsuno_config(TatsunoConfig& cfg);

/*** end pump setting parameter ****/

void fms_tatsuno_init() {
  pinMode(22, 0x03);
  tatsuno.begin(19200, true, 16, 17);
  //fms_save_tatsuno_config(tatsunoConfig); /* Save Tatsuno configuration to NVS storage*/
  vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 164 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
            1000 
# 164 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
            ) / ( TickType_t ) 1000U ) ));
  fms_load_tatsuno_config(tatsunoConfig); /* Load Tatsuno configuration from NVS storage */
  fms_tatsuno_device_setup(); /* Setup Tatsuno device*/
  enqactivetime1 = millis() / 1000;
}

/* save tatsuno config optional features curently remove this */
bool fms_save_tatsuno_config(TatsunoConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_p_config", false)) {
    fmsLog(FMS_LOG_ERROR, "Failed to initialize NVS storage");
    return false;
  }

  fms_nvs_storage.putUChar("devn", cfg.devn);
  fms_nvs_storage.putUChar("noz", cfg.noz);

  char key[12]; // enough for "pumpidX" + '\0'
  for (int i = 0; i < 8; i++) {
    snprintf(key, sizeof(key), "pumpid%d", i + 1);
    fms_nvs_storage.putUChar(key, cfg.pumpids[i]);
  }

  fms_nvs_storage.end();
  fmsLog(FMS_LOG_INFO, "Tatsuno configuration saved successfully");
  return true;
}

bool fms_load_tatsuno_config(TatsunoConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_d_config", true)) {
    fmsLog(FMS_LOG_ERROR, "Failed to initialize NVS storage");
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

  fmsLog(FMS_LOG_INFO, "Tatsuno configuration loaded successfully");

  Serial0.printf("%d  %d  %d  %d  %d  %d  %d  %d  %d  %d",
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
  gpio_set_level(GPIO_NUM_13, 1);
  // if (!digitalRead(hmi)) hmivalue = true;
 hmivalue = false; // Set to false for testing, change as needed
  if (hmivalue) {
    Serial0.println("[fms_tatsuno_fun.ino]  HMI ");
    hmisetup();
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      if (!fms_mqtt_client.connected()) {
        //  serverConnectionIcon("disconnected");
        Serial0.println("[fms_tatsuno_fun.ino]  Cloud disconnect");
        myfirst = true;
        gpio_set_level(GPIO_NUM_32, 0x0);
      } else {
        // Serial.println("[fms_tatsuno_fun.ino]  Connected to the Cloud");
          gpio_set_level(GPIO_NUM_32, 0x1);
          gpio_set_level(GPIO_NUM_14, 0x1);
        if (myfirst) {
          sendenq(1);
          mainfun();
          myfirst = false;
          pricereqfun();
        }
        // pumpactive();
      }
    } else {
      Serial0.println("[fms_tatsuno_fun.ino]  Not Connected");
     // initWiFi();
    }
    // if (!client.connected()) {
    //   reconnect();
    // }
    // client.loop();

    mainfun();
  }

}

 void mainfun() {
  delay(3);
  if (tatsuno.update()) {
    Buffer[i] = Serial1 /* uart2 serial port*/.read();
    enqactivetime1 = millis() / 1000;
    // delay(10);
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 2 ) * ( TickType_t ) 
# 284 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
              1000 
# 284 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
              ) / ( TickType_t ) 1000U ) )); // Adjusted delay for speed
    //delay(2); //speed change here
   Serial0.printf("Buffer[%d] : 0x%02X", i , Buffer[i]);

    i++;

    if (Buffer[i - 1] == 0x00) i = 0; // might delete later

    if (Buffer[i - 1] == 0x04) {
      // Serial.println(addresscount);
      // Serial.println("[fms_tatsuno_fun.ino]  get04");
      i = 0;

      addresscount++; // enq for another pump
      Serial0.printf("Address Count : %d",addresscount);
      Serial0.println("[fms_tatsuno_fun.ino]  get04");

      // add reload fun

      if (reloadcount) {
        reloadfun();
        reloadcount = false;
      }

      // else
      switch (pumppresetcount) {
        case true:
          Serial0.println("[fms_tatsuno_fun.ino] yep u got");
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
              Serial0.println("[fms_tatsuno_fun.ino]  i gety");
            }


            else {
              Serial0.println("[fms_tatsuno_fun.ino]  yep ");
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

    else if (Buffer[i - 1] == 0x03) { // GetdataFrom dispenser
      Buffer[i] = Serial1 /* uart2 serial port*/.read();
      // delay(20); //speed
      delay(2); //speed
      // Serial.println("[fms_tatsuno_fun.ino]  getCRCdata");
      i = 0;

      //loadoffadd
      // delay(100);  //speed

      messageClassified();

    } else if (Buffer[i - 1] == 0x10) { // Get ACK From dispenser
      Buffer[i] = Serial1 /* uart2 serial port*/.read();
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
    DynamicJsonDocument doc(4096); // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial0.println(((reinterpret_cast<const __FlashStringHelper *>(("JSON parsing failed: ")))));
      Serial0.println(error.c_str());
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

Serial0.printf("devicenum: %d ,", devicenum);
Serial0.printf("nozzlenum: %d ,", nozzlenum);
Serial0.printf("pumpid1: %d ,", pumpid1);
Serial0.printf("pumpid2: %d ,", pumpid2);
Serial0.printf("pumpid3: %d ,", pumpid3);
Serial0.printf("pumpid4: %d ,", pumpid4);
Serial0.printf("pumpid5: %d ,", pumpid5);
Serial0.printf("pumpid6: %d ,", pumpid6);
Serial0.printf("pumpid7: %d ,", pumpid7);
Serial0.printf("pumpid8: %d ,", pumpid8);


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
    Serial0.println("[fms_tatsuno_fun.ino]  yep all save");
  }


  if (String(topic) == String(pumpapprobuf)) { // pump req appro
    // pumapprofun();
    rxledonoff();
    pumpapprocount = true;
    incommingmsg1 = incommingMessage;
  }


  if (String(topic) == "detpos/local_server/whreq") { // whole req
    if (incommingMessage == String(devicenum)) {
      rxledonoff();
      // sendpermission = true;
      fms_mqtt_client.publish(whreqbuf, devicebuf);
      txledonoff();
    }
  }


  if (String(topic) == String(pumppresetbuf)) { // preset change
    rxledonoff();
    incommingMessage.toCharArray(presetmqttary, incommingMessage.length() + 1);
    Serial0.printf("preset is: %s", presetmqttary);


    Serial0.println(presetmqttary[0], 16);
    Serial0.println(presetmqttary[1], 16);

    charArray[0] = presetmqttary[0];
    charArray[1] = presetmqttary[1];

    pumpmqttnumchange();


    if (charArray[2] == 0x40) pumppresetcount = true;
    if (charArray[2] == 0x41) pumppresetcount = true;


    tempcharArray[0] = charArray[2];


    // if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x31) pumppresetcount = true;
    // if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x32) pumppresetcount = true;
  }

  if (String(topic) == String(pricechange)) { // price change

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

  if (String(topic) == String(Reset_topic)) { // esp reset topic
    char resetid[3];
    incommingMessage.toCharArray(resetid, incommingMessage.length() + 1);
    Serial0.println(resetid[1]);
    // delay(4000);
    if (atoi(resetid) == devicenum) {
      Serial0.println("[fms_tatsuno_fun.ino]  yep");
      // ESP.restart();
    }
  }
}

void messageClassified() {
  // if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) {  // Incoming
  //   sendCalculatedCRC();
  //   }

  if (Buffer[1] == 0x41 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) { // Incoming
    // sendCalculatedCRC();
    sendcrcfun();
  } else if (Buffer[1] == 0x40 && Buffer[2] == 0x51 && Buffer[3] == 0x30 && Buffer[4] == 0x30) { // Incoming
    // sendCalculatedCRC();
    sendcrcfun();
  } else if (Buffer[3] == 0x36 && Buffer[4] == 0x30 && Buffer[5] == 0x31 && Buffer[6] == 0x30) { // power on incomming
    sendACK1();
  } else if (Buffer[3] == 0x36 && Buffer[4] == 0x30 && Buffer[5] == 0x32 && Buffer[6] == 0x30) { // power on incomming
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

  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x31) { // pump req 1
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

  else if (Buffer[3] == 0x36 && Buffer[4] == 0x31 && Buffer[5] == 0x30) { // pump req nozzle out
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
      zerocount = false; //add
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

  Serial0.printf("ppbuffer : %s " ,ppbuffer);
  fms_mqtt_client.publish(pplive, ppbuffer);
  txledonoff();
}

void sendCalculatedCRC() {
  uint8_t unCalculatedCRCdata[6];
  for (int y = 0; y < 6; y++) {
    unCalculatedCRCdata[y] = Buffer[y + 3];
  }
  uint16_t polynomial = 0x8408; // CRC-CCITT polynomial

  // uint16_t crc_result = calculate_crc(Buffer, data_length, polynomial);
  uint16_t crc_result = calculate_crc(unCalculatedCRCdata, 6, polynomial);
  uint16_t crc_resultHighbyte = ((uint8_t)((crc_result) >> 8));
  uint16_t crc_resultLowbyte = ((uint8_t)((crc_result) & 0xff));
  uint16_t crc_plus = crc_resultHighbyte + crc_resultLowbyte;

  Serial0.printf("crc final value %d \n",crc_plus);

  if (crc_plus > 255) {
    crc_plus = crc_plus - 256;
    Serial0.println("[fms_tatsuno_fun.ino]  Warnning..................................................CRC is higher255");
  }



  Serial0.println("[fms_tatsuno_fun.ino]  CRC without initial value: 0x");
  Serial0.println(crc_result, 16);

  String str;
  str = String(crc_plus, 16);

  int str_len = str.length() + 1;

  char char_array[str_len];

  str.toCharArray(char_array, str_len);

  Serial0.println(char_array[0], 16);
  Serial0.println(char_array[1], 16);

  if(char_array[1] == 0x00){
    char_array[1] = char_array[0];
    char_array[0] = 0x30;
  }


  // CalculatedCRCdata[1] = Buffer[1];
  if (pumpnum == 1) CalculatedCRCdata[1] = 0x40;
  else if (pumpnum == 2) CalculatedCRCdata[1] = 0x41;

  if (char_array[0] == 0x66) CalculatedCRCdata[5] = 0x46; // string A-F to byte A-F
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

  digitalWrite(22, 0x1);
  // delay(20);
  Serial1 /* uart2 serial port*/.write(CalculatedCRCdata, 9);
  delay(7);
  digitalWrite(22, 0x0);
  Serial0.println("[fms_tatsuno_fun.ino]  sending CRC");
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
    Serial0.println("[fms_tatsuno_fun.ino]  0");
  }
Serial0.printf("LRC Checksum (Hex): 0x%02X", lrc);
Serial0.printf("LRC Checksum (Decimal): %d", lrc);

  return lrc;
}

uint16_t calculate_crc(uint8_t* data, int length, uint16_t polynomial) {
  uint16_t crc = 0; // Initial value (or seed) is 0

  for (int i = 0; i < length; i++) {
    crc ^= data[i]; // XOR the CRC with the next byte

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
      Serial0.println("[fms_tatsuno_fun.ino]  active");
      Serial0.printf("topic : [%s]", activebuf);
      fms_mqtt_client.publish(activebuf, "1");
      txledonoff();
      activecount = false;
    }
    activetime1 = millis() / 1000;
  } else activecount = true;
}

void pumpenqactive() {

  enqactivetime = millis() / 1000;

  if ((enqactivetime - enqactivetime1) > 3) {
    if (enqactivecount) {
      Serial0.println("[fms_tatsuno_fun.ino]  enqactive");
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
  delay(10); //speed
  digitalWrite(22, 0x1);
  // delay(20)
  delay(10); //speed
  if (eq == 1) Serial1 /* uart2 serial port*/.write(enq1, sizeof(enq1));
  else if (eq == 2) Serial1 /* uart2 serial port*/.write(enq2, sizeof(enq2));

  Serial0.printf("[fms_tatsuno_fun.ino] SendEnq : %d \n", eq);


  delay(3.5);
  // delay(6.8);
  // delay(4);
  digitalWrite(22, 0x0);
  //last add
  // delay(20); //speed
}

void sendACK1() {
  digitalWrite(22, 0x1);
  Serial1 /* uart2 serial port*/.write(ACK1, sizeof(ACK1));
  Serial0.println("[fms_tatsuno_fun.ino]  sending ACK");

  delay(2);
  digitalWrite(22, 0x0);
}

void sendEOT() {
  digitalWrite(22, 0x1);
  Serial1 /* uart2 serial port*/.write(EOT, 1); //eot
  Serial0.println("[fms_tatsuno_fun.ino]  sending EOT ");
  delay(4);
  digitalWrite(22, 0x0);

  // sendenq(1);
}

void txledonoff() {
  gpio_set_level(GPIO_NUM_33,0x0);
  vTaskDelay(10 / ( ( TickType_t ) 1000 / 
# 1133 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
                 1000 
# 1133 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
                 )); // delay for 10 ms
  gpio_set_level(GPIO_NUM_33, 0x1);
}

void rxledonoff() {
  gpio_set_level(GPIO_NUM_13, 0x1);
  vTaskDelay(10 / ( ( TickType_t ) 1000 / 
# 1139 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
                 1000 
# 1139 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
                 )); // delay for 10 ms
  gpio_set_level(GPIO_NUM_13, 0x0);
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

  Serial0.printf("ppbuffer : %s" ,ppbuffer);
  fms_mqtt_client.publish(reqcancelbuf, ppbuffer);
  txledonoff();
}

void finalsend() {

  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;


  finalmqtt1();


  sendACK1();

  digitalWrite(22, 0x0);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
  int receivedByte = Serial1 /* uart2 serial port*/.read();
  Serial0.printf("U got: 0x%02X (%d)", receivedByte, receivedByte);
    delay(10);
  }

  if (pumpnum == 1) pump1Select();
  else if (pumpnum == 2) pump2Select();

  int count1;
  char charack0[2];

  digitalWrite(22, 0x0);
  // delay(500);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);

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
  digitalWrite(22, 0x0);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got2 ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);
  }
  // if (charack0[0] == 0x10 && charack0[1] == 0x31) {  //
  sendenq(pumpnum);
  // }


  char totalizerary[33];
  int totalizercount = 0;
  digitalWrite(22, 0x0);
  delay(50);
  while (Serial1 /* uart2 serial port*/.available()) {

    totalizerary[totalizercount] = Serial1 /* uart2 serial port*/.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    Serial0.printf("%d // 0x%02X , ", totalizercount, totalizerary[totalizercount]);
    delay(10);
    if (totalizerary[totalizercount] == 0x03) {
      totalizerary[totalizercount + 1] = Serial1 /* uart2 serial port*/.read();
      delay(10);
      Serial0.println("[fms_tatsuno_fun.ino]  ");
      break;
    }
    totalizercount++;
  }

  // finalmqtt2
  while (totalizerary[4] != 0x35) { // wait for totalizer

    sendACK1();
    digitalWrite(22, 0x0);
    delay(30);
    if (Serial1 /* uart2 serial port*/.available()) {
   char ch = Serial1 /* uart2 serial port*/.read();
    Serial0.printf("U got char: %c (0x%02X)", ch, ch);

    }
    sendenq(pumpnum);

    totalizercount = 0;

    digitalWrite(22, 0x0);
    delay(50);
    while (Serial1 /* uart2 serial port*/.available()) {

      totalizerary[totalizercount] = Serial1 /* uart2 serial port*/.read();
      Serial0.printf("%d // 0x%02X", totalizercount, totalizerary[totalizercount]);
      // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
      delay(10);
      if (totalizerary[totalizercount] == 0x03) {
        totalizerary[totalizercount + 1] = Serial1 /* uart2 serial port*/.read();
        delay(10);
        Serial0.println("[fms_tatsuno_fun.ino]   ");
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

  Serial0.println("[fms_tatsuno_fun.ino]  ");
  zerocount = true;
  Serial0.printf("ppbuffer : %s" ,ppbuffer);
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
      Serial0.println("[fms_tatsuno_fun.ino]  Bye ....................................");
      final_str = "";
      break;
    }


    if (millis() / 1000 - previousMillis >= 13) {

      if (waitcount < 1) {
        fms_mqtt_client.publish(ppfinal, ppbuffer);
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
  Serial0.println("[fms_tatsuno_fun.ino]  get");


  // delay(4);
  delay(20);
  if (Serial1 /* uart2 serial port*/.available()) {
   int receivedByte = Serial1 /* uart2 serial port*/.read();
   Serial0.printf("U got: 0x%02X (%d)", receivedByte, receivedByte);

  }

  delay(50);

  if (pumpnum == 1) pump1Select();
  else if (pumpnum == 2) pump2Select();

  // sendCalculatedCRC();
  digitalWrite(22, 0x0);
  // // delay(500);
  delay(30);
  // delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    // delay(5);  // delay(20)
    Serial0.printf("your name is : 0x%02X 0x%02X", Buffer[0], Buffer[1]);
    delay(2);
  }

  sendCalculatedCRC();

  delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    // delay(5);  // delay(20)
    Serial0.printf("your name1 is : 0x%02X 0x%02X", Buffer[0], Buffer[1]);
    delay(2);
  }

  Serial0.println("[fms_tatsuno_fun.ino]  i am waiting");
  // delay(100);  //speed
  delay(20);

  sendenq(pumpnum);
}

void pricechangefun() {

  incommingMessage.toCharArray(pricechangeary, incommingMessage.length() + 1);

  Serial0.printf("pc is :%s", pricechangeary);



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
  Serial0.printf("ur noz1 price is :0x%02X 0x%02X 0x%02X 0x%02X \n",
                        unitpriceary1[0], unitpriceary1[1], unitpriceary1[2], unitpriceary1[3]);
  Serial0.printf("ur noz2 price is : 0x%02X 0x%02X 0x%02X 0x%02X \n",
                        unitpriceary2[0], unitpriceary2[1], unitpriceary2[2], unitpriceary2[3]);

}

void pumpmqttnumchange() { //change incomming pumpid(0 - 32) from Mqtt to Device pumpid(0 - 2)
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

  digitalWrite(22, 0x0);
  // // delay(500);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);

    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);

    if (charack0[0] == 0x10 && charack0[1] == 0x30) { //
      // sendenq(2);
      // pump2status();
      if (pump == 1) pump1status();
      else if (pump == 2) pump2status();
    }
  }
  Serial0.println("[fms_tatsuno_fun.ino]  shis shi");
  // delay(50);
  // digitalWrite(15, LOW);
  // // delay(500);
  delay(20);

  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got 1221");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);

    // if (charack0[0] == 0x10 && charack0[1] == 0x31) {  //
    sendenq(pump);
    // }
  }
}

void pumppresetfun() {


  if (presetmqttary[2] == 0x4C) {
    CalculatedPresetdata[5] = 0x31; // fueling with volume limitation
    CalculatedPresetdata[6] = 0x31; // volume

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
    Serial0.println("[fms_tatsuno_fun.ino]  yep u changed approved");
    CalculatedPresetdata[5] = 0x33; // fueling with Prest fueling(Preset value cannot be changed at dispenser)
    CalculatedPresetdata[6] = 0x32; // Amount

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
    CalculatedPresetdata[1] = 0x40; // add address
    pump1Select();
    pump1live = true;
    CalculatedPresetdata[14] = unitpriceary1[0];
    CalculatedPresetdata[15] = unitpriceary1[1];
    CalculatedPresetdata[16] = unitpriceary1[2];
    CalculatedPresetdata[17] = unitpriceary1[3];
    pumpnum = 1;

    pumplivefor1 = true; //select count
  }
  // else if (presetmqttary[0] == 0x30 && presetmqttary[1] == 0x32) {
  else if (tempcharArray[0] == 0x41) {
    CalculatedPresetdata[1] = 0x41; // add address
    pump2Select();
    pump2live = true;
    CalculatedPresetdata[14] = unitpriceary2[0];
    CalculatedPresetdata[15] = unitpriceary2[1];
    CalculatedPresetdata[16] = unitpriceary2[2];
    CalculatedPresetdata[17] = unitpriceary2[3];
    pumpnum = 2;

    pumplivefor2 = true; //select count
  }


  int count1;
  char charack0[2];

  digitalWrite(22, 0x0);
  // delay(500);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);

    // if (charack0[0] == 0x10 && charack0[1] == 0x30) {  //
    sendCalculatedPreset();
    // }
  }
}

void reloadfun() {

  Serial0.println("[fms_tatsuno_fun.ino]  reload fun start");

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

  digitalWrite(22, 0x0);

  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);
  }

  if (pumpnum == 1) pump1Totalizerstatus();
  else if (pumpnum == 2) pump2Totalizerstatus();

  digitalWrite(22, 0x0);
  delay(30); // to open

  if (Serial1 /* uart2 serial port*/.available()) { // to change if
    Serial0.println("[fms_tatsuno_fun.ino]  U got2 ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);
  }

  sendenq(pumpnum);

  char totalizerary[33];
  int totalizercount = 0;
  digitalWrite(22, 0x0);
  delay(50);

  while (Serial1 /* uart2 serial port*/.available()) {

    totalizerary[totalizercount] = Serial1 /* uart2 serial port*/.read();
    // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
    Serial0.printf("%d // 0x%02X \n", totalizercount, totalizerary[totalizercount]);

    delay(10);
    if (totalizerary[totalizercount] == 0x03) {
      totalizerary[totalizercount + 1] = Serial1 /* uart2 serial port*/.read();
      delay(10);
      Serial0.println("[fms_tatsuno_fun.ino]   ");
      break;
    }


    totalizercount++;
  }

  // finalmqtt2
  while (totalizerary[4] != 0x35) { // wait for totalizer
    sendACK1();
    digitalWrite(22, 0x0);
    delay(30);
    if (Serial1 /* uart2 serial port*/.available()) {
    int receivedByte = Serial1 /* uart2 serial port*/.read();
    Serial0.printf("U got 4 : 0x%02X (%d) \n", receivedByte, receivedByte);

    }

    sendenq(pumpnum);

    totalizercount = 0;

    digitalWrite(22, 0x0);
    delay(50);

    while (Serial1 /* uart2 serial port*/.available()) {

      totalizerary[totalizercount] = Serial1 /* uart2 serial port*/.read();
      // if (charack0[0] == 0x04) charack0[0] =  fms_uart2_serial.read();
      Serial0.printf("%d // 0x%02X \n", totalizercount, totalizerary[totalizercount]);
      delay(10);

      if (totalizerary[totalizercount] == 0x03) {
        totalizerary[totalizercount + 1] = Serial1 /* uart2 serial port*/.read();
        delay(10);
        Serial0.println("[fms_tatsuno_fun.ino]   ");
        break;
      }

      if (totalizerary[0] == 0x04) {

        Serial0.println("[fms_tatsuno_fun.ino]  first loop break");
        reloadcheck = true;
        break;
      }

      totalizercount++;
    }

    if (reloadcheck) {
      Serial0.println("[fms_tatsuno_fun.ino]  bye reload...................");
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
  Serial0.printf("mqtt count is => %d \n", y);

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

  Serial0.println("[fms_tatsuno_fun.ino]   ");
  zerocount = true;
  Serial0.printf("ppbuffer : %s \n", ppbuffer);


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
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(totalizerstatus1, 7);
  Serial0.println("[fms_tatsuno_fun.ino]  sending 1totalizerstatus");
  delay(5);
  digitalWrite(22, 0x0);
}

void pump2Totalizerstatus() {
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(totalizerstatus2, 7);
  Serial0.println("[fms_tatsuno_fun.ino]  sending 2totalizerstatus");
  delay(5);
  digitalWrite(22, 0x0);
}

void pump2status() {
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(pump2statusary, 7);
  Serial0.println("[fms_tatsuno_fun.ino]  sending pump2status");
  // delay(4.5);
  delay(5);
  digitalWrite(22, 0x0);
}

void pump1status() {
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(pump1statusary, 7);
  Serial0.println("[fms_tatsuno_fun.ino]  sending pump1status");
  delay(5);
  digitalWrite(22, 0x0);
}

void pump1Select() {
  digitalWrite(22, 0x1);
  Serial1 /* uart2 serial port*/.write(select1, sizeof(select1));
  Serial0.println("[fms_tatsuno_fun.ino]  sending select1");
  delay(4);
  digitalWrite(22, 0x0);
}

void pump2Select() {
  digitalWrite(22, 0x1);
  Serial1 /* uart2 serial port*/.write(select2, sizeof(select2));
  Serial0.println("[fms_tatsuno_fun.ino]  sending select2");
  delay(4);
  digitalWrite(22, 0x0);
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
      zerocount = false; //add
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

void pumpidchange() { //change incomming pumpid(0 - 8) from device to mqtt pumpid(0 - 32)
  if (Buffer[1] == 0x40) mqttpumpidchange(pumpid1);
  else if (Buffer[1] == 0x41) mqttpumpidchange(pumpid2);
}

void pumapprofun() {
  // char pumpapproArray[13];
  incommingmsg1.toCharArray(pumpapproArray, incommingmsg1.length() + 1);
  Serial0.printf("Appro  is : %s \n", pumpapproArray);

  charArray[0] = pumpapproArray[0];
  charArray[1] = pumpapproArray[1];

  pumpmqttnumchange();


  pumappproSend();
}

void hmisetup() {

  if (Serial0.available()) {
    for (int j = 0; j <= 50; j++) //this loop will store whole frame in buffer array.
    {
      Buffer[j] = Serial0.read();
      Serial0.println(Buffer[j], 16);
      Serial0.println("[fms_tatsuno_fun.ino]   ");
    }
    Serial0.println("[fms_tatsuno_fun.ino]   ");

    if (Buffer[4] == 0x10 && Buffer[8] == 0x01) {
      ESP.restart();
      Serial0.println("[fms_tatsuno_fun.ino]  restart");
    } else if (Buffer[4] == 0x13) { // wifi ssid
      for (int j = 0; j < 50; j++) ssidBuf[j] = 0;

      for (int j = 0; j < 50; j++) {
        if (Buffer[j + 9] == 0xFF) break;
        ssidBuf[j] = Buffer[j + 9];
      }
      Serial0.printf("ssid : %s ,",ssidBuf);

      Serial0.write(showSSID, 6);
      Serial0.write(ssidBuf, 30);
      Serial0.write(showSSID, 6);
      Serial0.write(ssidBuf, 30);

    } else if (Buffer[4] == 0x14) { // wifi password
      for (int j = 0; j < 50; j++) passBuf[j] = 0;

      for (int j = 0; j < 50; j++) {
        if (Buffer[j + 9] == 0xFF) break;
        passBuf[j] = Buffer[j + 9];
      }
      Serial0.printf ("pass : %s \n" , passBuf);
      Serial0.write(showPASS, 6);
      Serial0.write(passBuf, 50);
      Serial0.write(showPASS, 6);
      Serial0.write(passBuf, 50);
    } else if (Buffer[4] == 0x11) { // wifi connect
      WiFi.hostname("device1");
      WiFi.begin(ssidBuf, passBuf);
      wifitrytime = 0;
      while (WiFi.status() != WL_CONNECTED && wifitrytime != 15) {
      fmsLog(FMS_LOG_INFO, "[fms_tatsuno_fun.ino:2289 [hmi]] WiFi initialized, connecting to %s... wpa:%s", ssidBuf, passBuf);
      gpio_set_level(GPIO_NUM_33, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 2210 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
                1000 
# 2210 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
      gpio_set_level(GPIO_NUM_33, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 2212 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino" 3
                1000 
# 2212 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
                ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
      wifitrytime++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial0.printf(" Wifi Connected : %s \n", WiFi.localIP().toString().c_str());
        writeString(ssidBuf,passBuf); // save to preferences nvs storage ( trion )
        // digitalWrite(wifiled, HIGH);
        Serial0.write(page, 9);
        Serial0.write(0x00);
        Serial0.println("[fms_tatsuno_fun.ino]  home page");
        delay(1000);
      }
    } else if (Buffer[4] == 0x31) { // devnumber
      devicenum = Buffer[8];
      Serial0.printf("%d \n",devicenum);
    } else if (Buffer[4] == 0x41) { // pumpid 1
      pumpid1 = Buffer[8];
      Serial0.printf("%d \n",pumpid1);
    } else if (Buffer[4] == 0x42) { // pumpid 2
      pumpid2 = Buffer[8];
      Serial0.printf("%d \n",pumpid2);
    } else if (Buffer[4] == 0x43) { // pumpid 3
      pumpid3 = Buffer[8];
      Serial0.printf("%d \n",pumpid3);
    } else if (Buffer[4] == 0x44) { // pumpid 4
      pumpid4 = Buffer[8];
      Serial0.printf("%d \n",pumpid4);
    } else if (Buffer[4] == 0x45) { // pumpid 5
      pumpid5 = Buffer[8];
      Serial0.printf("%d \n",pumpid5);
    } else if (Buffer[4] == 0x46) { // pumpid 6
      pumpid6 = Buffer[8];
      Serial0.printf("%d \n",pumpid6);
    } else if (Buffer[4] == 0x47) { // pumpid 7
      pumpid7 = Buffer[8];
      Serial0.printf("%d \n",pumpid7);
    } else if (Buffer[4] == 0x48) { // pumpid 8
      pumpid8 = Buffer[8];
      Serial0.printf("%d \n",pumpid8);
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x00) { // 2noz
      nozzlenum = 2;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x01) { // 4noz
      nozzlenum = 4;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x02) { // 8noz
      nozzlenum = 8;
    } else if (Buffer[4] == 0x40 && Buffer[8] == 0x03) { // save

    /* save data to nvs storage instance of eeprom */
      tatsunoConfig.devn = devicenum; // Set device number
      tatsunoConfig.noz = nozzlenum; // Set nozzle number
      tatsunoConfig.pumpids[0] = pumpid1; // Set pump IDs
      tatsunoConfig.pumpids[1] = pumpid2; // Set pump IDs
      tatsunoConfig.pumpids[2] = pumpid3; // Set pump IDs
      tatsunoConfig.pumpids[3] = pumpid4; // Set pump IDs
      tatsunoConfig.pumpids[4] = pumpid5; // Set pump IDs
      tatsunoConfig.pumpids[5] = pumpid6; // Set pump IDs
      tatsunoConfig.pumpids[6] = pumpid7; // Set pump IDs
      tatsunoConfig.pumpids[7] = pumpid8; // Set pump IDs

      Serial0.println("[fms_tatsuno_fun.ino]  save tatsuno config");
      Serial0.printf("Device Number: %d, Nozzle Number: %d\n", tatsunoConfig.devn, tatsunoConfig.noz);
      fms_save_tatsuno_config(tatsunoConfig); // Save the configuration; 
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

  Serial0.println("[fms_tatsuno_fun.ino]  yep all save");
}

void writeString(String ssid, String password) {
 // Save to preferences
  fms_nvs_storage.begin("fms_config", false);
  fms_nvs_storage.putString("ssid", ssid);
  fms_nvs_storage.putString("pass", password);
  fms_nvs_storage.end();

  Serial0.println("[fms_tatsuno_fun.ino]  Wrtiting ssid and pass to eeprom");
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
    CalculatedApprodata[1] = 0x40; // add address
    pump1live = true;
    CalculatedApprodata[14] = unitpriceary1[0];
    CalculatedApprodata[15] = unitpriceary1[1];
    CalculatedApprodata[16] = unitpriceary1[2];
    CalculatedApprodata[17] = unitpriceary1[3];

    pumplivefor1 = true; //select count
  }
  // else if (pumpapproArray[0] == 0x30 && pumpapproArray[1] == 0x32) {
  else if (charArray[2] == 0x41) {
    pump2Select();
    CalculatedApprodata[1] = 0x41; // add address
    pump2live = true;
    CalculatedApprodata[14] = unitpriceary2[0];
    CalculatedApprodata[15] = unitpriceary2[1];
    CalculatedApprodata[16] = unitpriceary2[2];
    CalculatedApprodata[17] = unitpriceary2[3];

    pumplivefor2 = true; //select count
  }

  // pump1Select();
  int count1;
  char charack0[2];

  digitalWrite(22, 0x0);
  // delay(500);
  delay(30);
  if (Serial1 /* uart2 serial port*/.available()) {
    Serial0.println("[fms_tatsuno_fun.ino]  U got ");
    charack0[0] = Serial1 /* uart2 serial port*/.read();
    if (charack0[0] == 0x04) charack0[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    charack0[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println(charack0[0], 16);
    Serial0.println(charack0[1], 16);

    if (charack0[0] == 0x10 && charack0[1] == 0x30) { //
      sendCalculatedAppro();
    }
  }
}

void sendCalculatedPreset() {

  CalculatedPresetdata[19] = BCCfun2();
  for (int y = 0; y < 20; y++) {
    Serial0.println(CalculatedPresetdata[y], 16);
    Serial0.println("[fms_tatsuno_fun.ino]   ");
  }

  Serial0.println("[fms_tatsuno_fun.ino]  ");

  // delay(100); //speed

  delay(20); //speed

  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(CalculatedPresetdata, 20);
  delay(2);
  Serial1 /* uart2 serial port*/.write(CalculatedPresetdata, 20);
  // delay(2);
  Serial0.println("[fms_tatsuno_fun.ino]  sending preset");
  // delay(12.5);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(22, 0x0);

  //tgi add
  // if (Buffer[1] == 0x40) pumpnum = 1;
  // else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    // latest pumppreset again

    Serial0.println("[fms_tatsuno_fun.ino]  your name1 is");
    Serial0.println(Buffer[0], 16);
    Serial0.println(Buffer[1], 16);
    delay(2);
    //Frist time
    if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1
      // resend
      Serial0.println("[fms_tatsuno_fun.ino]  motor start");
    } else {
      resendpreset();
    }
  }

  // Second time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1
    // resend
    Serial0.println("[fms_tatsuno_fun.ino]  motor start");
  } else {
    resendpreset();
  }
  // Third time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1
    // resend
    Serial0.println("[fms_tatsuno_fun.ino]  motor start");
  } else {
    resendpreset();
  }

  sendenq(pumpnum);


  Serial0.println("[fms_tatsuno_fun.ino]  Uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu");
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
    Serial0.println("[fms_tatsuno_fun.ino]  0");
  }

  Serial0.printf("LRC Checksum (Hex): 0x%02X, Decimal: %d \n", lrc, lrc);


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
    Serial0.println("[fms_tatsuno_fun.ino]  0");
  }
  Serial0.printf("LRC Checksum (Hex): 0x%02X, Decimal: %d \n", lrc, lrc);


  return lrc;
}

void resendpreset() {
  // delay(100);//speed
  delay(20);

  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(CalculatedPresetdata, 20);
  Serial0.println("[fms_tatsuno_fun.ino]  sending preset again");
  // delay(12.5);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(22, 0x0);


  delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    delay(10);
    Serial0.println("[fms_tatsuno_fun.ino]  your resend again is");
    Serial0.println(Buffer[0], 16);
    Serial0.println(Buffer[1], 16);
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
    Serial0.println(CalculatedApprodata[y], 16);
    Serial0.println("[fms_tatsuno_fun.ino]   ");
  }
  Serial0.println("[fms_tatsuno_fun.ino]  ");


  // delay(100); //speed

  delay(20); //speed
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(CalculatedApprodata, 20);
  delay(2);
  Serial1 /* uart2 serial port*/.write(CalculatedApprodata, 20);
  // delay(2);
  Serial0.println("[fms_tatsuno_fun.ino]  sending Appro1");
  // delay(13);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(22, 0x0);

  //tgi add
  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    // latest pumppreset again

    Serial0.println("[fms_tatsuno_fun.ino]  your name1 is");
    Serial0.println(Buffer[0], 16);
    Serial0.println(Buffer[1], 16);
    delay(2);
    //Frist time
    if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1

      Serial0.println("[fms_tatsuno_fun.ino]  app motor start");
    } else {
      resendappro();
    }
  }

  // Second time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1
    // resend
    Serial0.println("[fms_tatsuno_fun.ino]  app motor start");
  } else {
    resendappro();
  }
  // Third time
  if (Buffer[0] == 0x10 || Buffer[0] == 0x30 || Buffer[0] == 0x31 || Buffer[1] == 0x31) { // Incoming ack1
    // resend
    Serial0.println("[fms_tatsuno_fun.ino]  app motor start");
  } else {
    resendappro();
  }

  sendenq(pumpnum);
}

void resendappro() {
  // delay(100); //speed

  delay(20); //speed
  digitalWrite(22, 0x1);
  delay(20);
  Serial1 /* uart2 serial port*/.write(CalculatedApprodata, 20);
  Serial0.println("[fms_tatsuno_fun.ino]  sending resend appro");
  // delay(13);
  // delay(50);
  // delay(34);
  delay(20);
  digitalWrite(22, 0x0);

  //tgi add
  if (Buffer[1] == 0x40) pumpnum = 1;
  else if (Buffer[1] == 0x41) pumpnum = 2;

  // delay(10);
  delay(4);
  if (Serial1 /* uart2 serial port*/.available()) {

    Buffer[0] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    Buffer[1] = Serial1 /* uart2 serial port*/.read();
    delay(20);
    Serial0.println("[fms_tatsuno_fun.ino]  your resend again is");
    Serial0.println(Buffer[0], 16);
    Serial0.println(Buffer[1], 16);
    delay(2);
  }
}


// end tatsuno protocol 
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
# 64 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
            fms_tatsuno_protocol_main(); /* tatsuno protocol */

// #ifdef USE_MUX_PC817
// test_mux();
// #endif
   vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 69 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino" 3
             1000 
# 69 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
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
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      fmsLog(FMS_LOG_ERROR, "[fms_wifi.ino:11] [DEBUG WiFi] wifi .. credential .. value is empty");
      fms_nvs_storage.end();
      return false;
    }

    fms_nvs_storage.end();
    Serial0.print("[DEBUG] "); Serial0.printf("SSID : %s , PASS : %s", ssid_str, pass_str); Serial0.println();
    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);
    if (sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") {
      fmsLog(FMS_LOG_ERROR, "[fms_wifi.ino:21] [DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }

    WiFi.mode(WIFI_MODE_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);

    while (WiFi.status() != WL_CONNECTED) {
      gpio_set_level(GPIO_NUM_32, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 31 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 31 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      gpio_set_level(GPIO_NUM_32, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 33 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 33 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));

      fmsLog(FMS_LOG_INFO, "[fms_wifi.ino:35] WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid, sysCfg.wifi_password);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 36 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 36 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
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
      fmsLog(FMS_LOG_WARNING, "[fms_wifi.ino:53] Failed to connect to WiFi");
      gpio_set_level(GPIO_NUM_32, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 55 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 55 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      gpio_set_level(GPIO_NUM_32, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 57 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 57 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
    } else {
      // FMS_LOG_INFO("[fms_wifi.ino:59] Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(GPIO_NUM_33, 0x1);
      gpio_set_level(GPIO_NUM_32, 0x1);
      gpio_set_level(GPIO_NUM_13, 0x1);
      gpio_set_level(GPIO_NUM_14, 0x0);
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 100 ) * ( TickType_t ) 
# 65 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
              1000 
# 65 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
              ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
  }
}
