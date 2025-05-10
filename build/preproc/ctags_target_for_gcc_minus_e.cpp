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

// Uncomment this line to disable the library





FMS_FileManager fileManager;
fms_cli fms_cli(Serial0, "admin" /* cli password     // change this password*/); // Use "admin" as the default password change your admin pass here


fmsLanfeng lanfeng(22,22);// set re de pin (DTR PIN)

/* Main function */
void setup() {
  fms_cli.begin(115200); // uart
  fms_initialize_uart2(); // uart 2
  fms_pin_mode(2, 0x03);

  // multiplexer pinout
  fms_pin_mode(25,0x03); // Multiplexer
  fms_pin_mode(26 ,0x03);
  fms_pin_mode(27 /* enable input (active LOW) */,0x03);
  enable_mux(27 /* enable input (active LOW) */); // enable multiplexer (active low)

  fms_run_sd_test(); // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(false); // show serial logging data on Serial Monitor
  fms_boot_count(true); // boot count
  // cli command
  fms_cli.register_command("wifi", "Configure WiFi settings", handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect", "Connect to WiFi network", handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart", "Restart the system", handle_restart_command);
  fms_cli.register_command("wifiscan_safe", "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread", "Read current WiFi status", handle_wifi_read_command);
  fms_cli.register_command("wifi_test", "Test WiFi connection", handle_wifi_test_command);
  fms_cli.register_command("uuid_change", "Change Your Device Id unique address", handle_device_id_change_command,1,1);





  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)
  if (fms_initialize_wifi()) { // wifi is connected
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
void handle_device_id_change_command(const std::vector<String>& args){
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
size_t custom_print(const uint8_t *buffer, size_t size) {
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

static void cli_task(void *arg) {
  BaseType_t rc;
  while (1) {
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 310 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino" 3
              1000 
# 310 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
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
# 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
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
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
# 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
char fms_nmf_tp_prefix[64];

void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
  Serial0.print("[MQTT][DEBUG] "); Serial0.printf("INCOMMING TIOPIC [%s] : %s",topic,incommingMessage); Serial0.println();
  bool tp_match = false;

  String topic_ = String(topic);
 // get topic last value(for /) deptos/local_server/1 , /1 value or /permit or /price 
  int last = topic_.lastIndexOf('/');
  String topic_value = topic_.substring(last+1);
  // some return topic contain noz id detpos/local_server/1 , check noz id or other 
  int nozzle_num = topic_value.toInt();
  Serial0.print("[MQTT][DEBUG] "); Serial0.printf("Topic value : [%s]:%d", topic_value.c_str(),nozzle_num); Serial0.println();
  // check if the topic is approved message or not
  if(nozzle_num >=1 && nozzle_num <= 2 /* change your noz count*/){
    snprintf(approvmsg,sizeof(approvmsg),"%02dappro",nozzle_num);
    Serial0.print("[MQTT][DEBUG] "); Serial0.printf("APPROVED MESSAGE GENERTED : %s",approvmsg); Serial0.println();
    if (incommingMessage == String(approvmsg)){
      pump_approve[nozzle_num-1] = true;
      tp_match = true;
      Serial0.print("[MQTT][DEBUG] "); Serial0.printf("APPROVED MESSAGE for Nozzle %d: %s", nozzle_num, incommingMessage.c_str()); Serial0.println();
    }
  }


  for (int i = 0 ; i < fms_sub_topics_value_count; i++){
      const char* sub_tp_value = fms_sub_topics_value[i]; // declare in main.h file
    if(strcmp(sub_tp_value,topic_value.c_str()) == 0)
    {
      tp_match = true;
      switch (i){
        case 0: {
          ("preset topic matched: %s", topic_value.c_str());
          int pumpID = fms_decodePumpId(incommingMessage);
          int presetAmount = fms_decodePresetAmount(incommingMessage);
          presetMessageGet = true; // for preset message get from mqtt broker
          // pump_approve[pumpID-1] = true;
          Serial0.print("[MQTT][DEBUG] "); Serial0.printf("Pump ID: %d, Preset Amount: %d", pumpID, presetAmount); Serial0.println();
          break;
        }
        case 1: {
          ("price topic matched: %s", topic_value.c_str());
          break;
        }
      }
      Serial0.print("[MQTT][DEBUG] "); Serial0.printf("MATCH TRUE"); Serial0.println();
      break;
    }
    else {
        Serial0.print("[MQTT][DEBUG] "); Serial0.printf("not matched : [%s] == %s",topic,fms_sub_topics_value[i]); Serial0.println();
    }
  }

  if (!tp_match) {
    Serial0.print("[MQTT][ERROR] "); Serial0.printf("Topic not matched : %s", topic); Serial0.println();
  }
}


void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    Serial0.print("[MQTT][DEBUG] "); Serial0.printf("Subscribing to topic: %s", fms_sub_topics[i]); Serial0.println();
    fms_mqtt_client.subscribe(fms_sub_topics[i]);
  }
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()) {
    Serial0.print("[MQTT][DEBUG] "); Serial0.printf("MQTT initialized, connecting to %s:%d...", sysCfg.mqtt_server_host /* mqtt server address*/, 1883); Serial0.println();
    String clientId = String(deviceName) + String(random(0xffff), 16);
    if (fms_mqtt_client.connect(clientId.c_str())) {
      Serial0.print("[MQTT][DEBUG] "); Serial0.printf("Connected to MQTT server"); Serial0.println();
      fms_subsbribe_topics();
      // Uncomment the following lines to subscribe to additional topics
      // fms_mqtt_client.subscribe("detpos/#");
      // fms_mqtt_client.subscribe("detpos/local_server/#");
      // fms_mqtt_client.subscribe("detpos/local_server/price");
      // fms_mqtt_client.subscribe("detpos/local_server/preset");
      // Add additional topic subscriptions if necessary
    } else {
      Serial0.print("[MQTT][ERROR] "); Serial0.printf("Failed to connect to MQTT server , rc = %d try again in 5 second", fms_mqtt_client.state()); Serial0.println();
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 5000 ) * ( TickType_t ) 
# 93 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
                1000 
# 93 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
                ) / ( TickType_t ) 1000U ) ));
    }
  }
}

static void mqtt_task(void* arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(sysCfg.mqtt_server_host /* mqtt server address*/, 1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (true) {
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
    } else {
      Serial0.print("[MQTT][DEBUG] "); Serial0.printf("Connected to MQTT server"); Serial0.println();
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 110 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino" 3
              1000 
# 110 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
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
    UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(
# 226 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3 4
                                                            __null
# 226 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                                                                );
    Serial0.print("Stack Remaining: ");
    Serial0.println(stackRemaining); // Prints remaining stack (in words)
    vTaskDelay(1000 / ( ( TickType_t ) 1000 / 
# 229 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino" 3
                     1000 
# 229 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
                     ));
   // vTaskDelay(pdMS_TO_TICKS(1000));
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
  if (!create_task(cli_task, "cli", 3000, 1, &hcliTask, cli_rc)) return false;
  if (!create_task(fms_uart2_task, "uart2", 3000, 1, &huart2Task, uart2_rc)) return false;
  if (!create_task(web_server_task, "webserver", 4096, 4, &hwebServerTask, webserver_rc)) return false;

  return true;
}
# 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    Serial1 /* uart2 serial port*/.begin(baudrate, SERIAL_8N1, 16, 17);
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
  uint8_t Buffer[50];
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




test_mux();

    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 56 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino" 3
              1000 
# 56 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
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
      fmsLog(FMS_LOG_INFO, "WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid, sysCfg.wifi_password);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 20 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 20 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
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
      gpio_set_level(GPIO_NUM_2, 0x1);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 39 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 39 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
      gpio_set_level(GPIO_NUM_2, 0x0);
      vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 500 ) * ( TickType_t ) 
# 41 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
                1000 
# 41 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
                ) / ( TickType_t ) 1000U ) ));
    } else {
      fmsLog(FMS_LOG_INFO, "Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(GPIO_NUM_2, 0x1);
    }
    vTaskDelay(( ( TickType_t ) ( ( ( TickType_t ) ( 1000 ) * ( TickType_t ) 
# 46 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino" 3
              1000 
# 46 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
              ) / ( TickType_t ) 1000U ) )); // Wait for 1 second before repeating
  }
}
