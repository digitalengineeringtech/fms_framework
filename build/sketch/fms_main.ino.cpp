#include <Arduino.h>
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
/*
     FMS main source file
     Author: Trion
     Date: 2025
     Guided By Senior Engineer : Sir Thiha Kyaw
     Description: This file is the main source file of FMS project.
*/
#include "_fms_main.h"
#include "src/_fms_cli.h"
#include "src/_fms_debug.h"
#include "src/_fms_filemanager.h"
#include "src/_fms_json_helper.h"
#include "src/_fms_lanfeng.h"
FMS_FileManager fileManager;
fms_cli fms_cli(Serial, CLI_PASSWORD);      // Use "admin" as the default password change your admin pass here

// Uncomment this line to disable the library
// #define DISABLE_LANFENG
// #ifdef DISABLE_LANFENG
//   #undef USE_LANFENG  // Undefine USE_LANFENG to disable the library
// #endif


fmsLanfeng lanfeng(15,15);

/* Main function */
#line 27 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup();
#line 51 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void loop();
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_command(const std::vector<String>& args);
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_restart_command(const std::vector<String>& args);
#line 25 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_scan_safe_command(const std::vector<String>& args);
#line 88 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_connect_command(const std::vector<String>& args);
#line 157 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_read_command(const std::vector<String>& args);
#line 177 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_test_command(const std::vector<String>& args);
#line 202 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_test_command(const std::vector<String>& args);
#line 276 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
size_t custom_print(const uint8_t *buffer, size_t size);
#line 291 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
static void cli_task(void *arg);
#line 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_boot_count(bool bootsave);
#line 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_chip_info();
#line 33 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_uart2();
#line 44 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_wifi();
#line 54 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_run_sd_test();
#line 65 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_debug_info();
#line 72 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_pin_mode(int pin, int mode);
#line 76 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_dns_responder_init();
#line 89 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateFinalData(int pump_id,float sell_liters,float live_liters,float price,float totalizer,unsigned long long totalizer_amount);
#line 97 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateLiveData(int pump_id,float price_liters,float live_liters);
#line 107 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePresetAmount(String presetData);
#line 117 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePumpId(String presetData);
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length);
#line 14 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_reconnect();
#line 29 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
static void mqtt_task(void* arg);
#line 19 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void fms_info_response();
#line 37 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleDashboard();
#line 52 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogin();
#line 68 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogout();
#line 73 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void fms_set_ota_server();
#line 206 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
static void web_server_task(void* arg);
#line 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
bool fms_sd_init();
#line 23 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
void fms_sd_dir(fs::FS& fs, const char* dirname, uint8_t levels);
#line 54 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
void fms_config_load_sd_test();
#line 60 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
bool write_data_sd(char* input);
#line 70 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
static void sd_task(void* arg);
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
void _led_state();
#line 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc);
#line 28 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool fms_task_create();
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate);
#line 16 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fm_rx_irq_interrupt();
#line 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fms_uart2_decode(uint8_t* data, uint32_t len);
#line 38 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fms_uart2_task(void* arg);
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag);
#line 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool wifi_led_ticker();
#line 33 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
static void wifi_task(void *arg);
#line 27 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup() {
  fms_pin_mode(BUILTIN_LED, OUTPUT);
  fms_cli.begin(115200);                    // uart
  fms_initialize_uart2();                   // uart 2
  fms_run_sd_test();                        // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(false);             // show serial logging data on Serial Monitor
  fms_boot_count(true);                     // boot count
  // cli command
  fms_cli.register_command("wifi",              "Configure WiFi settings",        handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect",      "Connect to WiFi network",        handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart",           "Restart the system",             handle_restart_command);
  fms_cli.register_command("wifiscan_safe",     "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread",          "Read current WiFi status",           handle_wifi_read_command);
  fms_cli.register_command("wifi_test",         "Test WiFi connection",               handle_wifi_test_command);
  #ifdef USE_LANFENG
  FMS_LOG_INFO("[LANFENG] Starting Lanfeng");
  lanfeng.init(1,fms_uart2_serial); // add slave id
  #endif
  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)
  if (fms_initialize_wifi()) {  // wifi is connected
    fms_task_create();
  }
}

void loop() {
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
// cli command function 
void handle_wifi_command(const std::vector<String>& args) {
    if (args.size() != 2) {
        fms_cli.respond("wifi", "Usage: wifi <ssid> <password>", false);
        return;
    }
    String ssid = args[0];
    String password = args[1];
    // Save to preferences
    preferences.begin("fms_config", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", password);
    preferences.end();
    
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
    WiFi.mode(WIFI_STA);
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
        Serial.print("Network ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(WiFi.SSID(i));
        
        // Send RSSI
        Serial.print("  Signal: ");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm");
        
        // Send Channel
        Serial.print("  Channel: ");
        Serial.println(WiFi.channel(i));
        
        // Send Encryption
        Serial.print("  Security: ");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA/WPA2");
        
        Serial.println("---");
        
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
    WiFi.mode(WIFI_STA);
    
    // Begin connection attempt
    fms_cli.respond("wifi_connect", "Connecting to " + ssid + "...");
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    // Check connection result
    if (WiFi.status() == WL_CONNECTED) {
        // Use individual prints instead of building a large JSON string
        Serial.println("{");
        Serial.print("  \"command\": \"wifi_connect\",");
        Serial.print("  \"result\": \"Connected successfully\",");
        Serial.print("  \"success\": true,");
        Serial.print("  \"ip\": \"");
        Serial.print(WiFi.localIP().toString());
        Serial.print("\",");
        Serial.print("  \"ssid\": \"");
        Serial.print(WiFi.SSID());
        Serial.print("\",");
        Serial.print("  \"rssi\": ");
        Serial.print(WiFi.RSSI());
        Serial.println("}");
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
        Serial.println("{");
        Serial.print("  \"ssid\": \"");
        Serial.print(WiFi.SSID());
        Serial.print("\",");
        Serial.print("  \"rssi\": ");
        Serial.print(WiFi.RSSI());
        Serial.print(",");
        Serial.print("  \"ip\": \"");
        Serial.print(WiFi.localIP().toString());
        Serial.println("\"");
        Serial.println("}");
    } else {
        fms_cli.respond("wifiread", "WiFi not connected", false);
    }
}

// Test command to run a series of tests
void handle_test_command(const std::vector<String>& args) {
    fms_cli.respond("test", "Running fms_cli tests...");
    
    // Test help command
    Serial.println("\n--- Testing help command ---");
    fms_cli.execute_test_command("help");
    
    // Test echo command
    Serial.println("\n--- Testing echo command ---");
    fms_cli.execute_test_command("echo on");
    fms_cli.execute_test_command("echo off");
    fms_cli.execute_test_command("echo");
    
    // Test wifi read command
    Serial.println("\n--- Testing wifiread command ---");
    fms_cli.execute_test_command("wifiread");
    
    // Test invalid command
    Serial.println("\n--- Testing invalid command ---");
    fms_cli.execute_test_command("invalidcommand");
    
    fms_cli.respond("test", "Tests completed");
}

// WiFi connection test command
void handle_wifi_test_command(const std::vector<String>& args) {
    fms_cli.respond("wifi_test", "Running WiFi connection tests...");
    
    // Get stored WiFi credentials
    preferences.begin("fms_config", true); // Read-only mode
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("pass", "");
    preferences.end();
    
    if (ssid.length() == 0 || password.length() == 0) {
        fms_cli.respond("wifi_test", "No WiFi credentials stored. Use 'wifi <ssid> <password>' first.", false);
        return;
    }
    
    // Test WiFi connection
    Serial.println("\n--- Testing WiFi connection ---");
    Serial.printf("Connecting to %s...\n", ssid.c_str());
    
    // Disconnect from any current WiFi connection
    WiFi.disconnect();
    delay(100);
    
    // Set WiFi mode to station
    WiFi.mode(WIFI_STA);
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
        Serial.print(".");
    }
    Serial.println();
    
    if (connected) {
        Serial.printf("Connected to %s\n", ssid.c_str());
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Signal strength (RSSI): %d dBm\n", WiFi.RSSI());
        
        // Test ping to gateway
        IPAddress gateway = WiFi.gatewayIP();
        Serial.printf("Gateway IP: %s\n", gateway.toString().c_str());
        
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
    return Serial.write(buffer, size);
}

static void cli_task(void *arg) {
  BaseType_t rc;
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
/*
    * fms_main_func.ino
    * This file is part of the ESP32 FMS 
    * main function link file
*/


void fms_boot_count(bool bootsave) {
  if (!bootsave) {
    return;
  }

  if (!fms_nvs_storage.begin("fms_config", false)) {
    FMS_LOG_ERROR("Failed to initialize NVS storage");
    return;
  }

  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0) + 1;
  app_cpu = xPortGetCoreID();
  FMS_LOG_INFO("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end();  // Close NVS storage
}

void log_chip_info() {
#if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
#endif
}

bool fms_initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 9600)) {
    //fms_uart2_serial.onReceive(fm_rx_irq_interrupt);  // uart interrupt function
    FMS_LOG_INFO("[FMSUART2] UART2.. DONE");
    return true;
  } else {
    FMS_LOG_ERROR("[FMSUART2] UART2.. FAIL");
    return false;
  }
}

bool fms_initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    FMS_LOG_INFO("Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    FMS_LOG_WARNING("Failed to connect to WiFi");
    return false;
  }
}

void fms_run_sd_test() {
#if true
  fms_config_load_sd_test();
  if (!LittleFS.begin(true)) {  // preference storage (1MB)
    Serial.println("[STORAGE] Failed to mount file system");
  } else {
    Serial.println("[STORAGE] File system mounted");
  }
#endif
}

void log_debug_info() {
#if SHOW_DEBUG_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
#endif
}

void fms_pin_mode(int pin, int mode) {
  pinMode(pin, mode);
}

void fms_dns_responder_init() {
  // Set up mDNS responder
  if (!MDNS.begin(deviceName)) {
    FMS_LOG_ERROR("Error setting up MDNS responder!");
  } else {
    FMS_LOG_INFO("mDNS responder started");
    // Add service to MDNS
    MDNS.addService("esp-ota", "tcp", 80);
    MDNS.addService("http", "tcp", 80);  // Add standard HTTP service for better discovery
  }
}

// generate Final Data format 
String fms_generateFinalData(int pump_id,float sell_liters,float live_liters,float price,float totalizer,unsigned long long totalizer_amount){
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "%02dS%.3fL%.3fP%.2fT%.3fA%llu",
           pump_id, sell_liters, live_liters, price, totalizer, totalizer_amount);
  return String(buffer);
}

// generate live data format
String fms_generateLiveData(int pump_id,float price_liters,float live_liters){
  float sell_liter = price_liters * live_liters;  // S = P × L
  char buffer[50];                                // Buffer to store formatted string
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



#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"

void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";

  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
  FMS_LOG_INFO("Message arrived [%s] : %s", topic, incommingMessage);
  // Serial.println("Message arrived [" + String(topic) + "]" + incommingMessage);
  // char payload_str[length + 1];
  // memcpy(payload_str, payload, length);
  // payload_str[length] = '\0';
  // FMS_LOG_INFO("Message arrived on topic [%s]: %s", topic, payload_str);
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()) {
    FMS_LOG_INFO("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str())) {
      FMS_LOG_INFO("Connected to MQTT server");
      fms_mqtt_client.subscribe("detpos/#");
      // Add additional topic subscriptions if necessary
    } else {
      FMS_LOG_WARNING("Failed to connect to MQTT server , rc = %d try again in 5 second", fms_mqtt_client.state());
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

static void mqtt_task(void* arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(MQTT_SERVER, 1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (mqttTask) {
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
    } else FMS_LOG_INFO("Connected to MQTT server");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
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

// For large uploads - increase buffer size
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
  FMS_LOG_INFO("ota server created");
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
    UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("Stack Remaining: ");
    Serial.println(stackRemaining);  // Prints remaining stack (in words)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
   // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
/*
  * fms_sd.cpp
  *  Created on: 2020. 12. 10.
  *   author : thet htar khaing
*/

bool fms_sd_init() {
  if (!SD.begin(SD_CARD_CS_PIN)) {
    FMS_LOG_ERROR("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    FMS_LOG_ERROR("No SD card attached");
    ticker.attach(0.6, _led_state);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return false;
  }
  FMS_LOG_INFO("SD CARD MOUNTED");
  return true;
}

void fms_sd_dir(fs::FS& fs, const char* dirname, uint8_t levels) {
  FMS_LOG_INFO("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    FMS_LOG_ERROR("Failed to open directory");
    //return false;
  }
  if (!root.isDirectory()) {
    FMS_LOG_ERROR("Not a directory");
    //return false;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      FMS_LOG_INFO("  DIR : ");
      FMS_LOG_INFO(String(file.name()).c_str());
      if (levels) {
        fms_sd_dir(fs, file.name(), levels - 1);
      }
    } else {
      FMS_LOG_INFO("  FILE: ");
      FMS_LOG_INFO(String(file.name()).c_str());
      FMS_LOG_INFO("  SIZE: ");
      FMS_LOG_INFO(String(file.size()).c_str());
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

    //rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool ledState = false;
void _led_state() {
  ledState = !ledState;
  digitalWrite(BUILTIN_LED, ledState);
}

bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc) {
  rc = xTaskCreatePinnedToCore(
    task_func,   // Task function
    name,        // Name
    stack_size,  // Stack size
    nullptr,     // Parameters
    priority,    // Priority
    handle,      // Handle
    app_cpu      // CPU
  );
  assert(rc == pdPASS);
  if (rc != pdPASS) {
    FMS_LOG_ERROR("[TASK]%s task created fail", name);
    return false;
  }
  ticker.attach(0.3, _led_state);
  vTaskDelay(pdMS_TO_TICKS(1000));
  FMS_LOG_INFO("[TASK] %s task created done", name);
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

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"

bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, 16, 17);

    if (fms_uart2_serial) {
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}


void fm_rx_irq_interrupt() {  // interrupt RS485/RS232 function
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available();  // serial.available  // #define fms_cli_serial Serial
  fms_uart2_serial.printf("Got byes on serial : %d\n", size);
  while (fms_uart2_serial.available()) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++;
  }
  fms_uart2_serial.printf("\n uart2  data process \n\r");
  fms_uart2_decode(Buffer, size);  // decode uart2 data main function
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data);
}

unsigned long lastUpdate = 0;

uint32_t value[40];
uint32_t s_liter[2];
void fms_uart2_task(void* arg) {
  BaseType_t rc;
  while (1) {
  uint32_t sellLiter = lanfeng.readSellLiter(0x02D4,s_liter);
   vTaskDelay(pdMS_TO_TICKS(1000));
    uint32_t pumpState = lanfeng.readPumpState(0x02DE); // fix send data error when (not included 03 function how to fix this,)
    Serial.print("[LANFENG] PUMP STATE :");
    Serial.println(pumpState,HEX);
    vTaskDelay(pdMS_TO_TICKS(1000));
4
    uint32_t liveData = lanfeng.readLiveData(0x02C4); // fix send data error when (not included 03 function how to fix this,)
    Serial.print("[LANFENG] LIVE DATA :");
    Serial.println(liveData,HEX);
    vTaskDelay(pdMS_TO_TICKS(1000));

  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag) {
  if (flag) {
    // get ssid and password from nvs storage
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    fms_nvs_storage.end();
    FMS_LOG_DEBUG("SSID : %s , PASS : %s", ssid_str, pass_str);
    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);
    if (sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") {
      FMS_LOG_ERROR("[DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);  // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      FMS_LOG_INFO("WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid, sysCfg.wifi_password);
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
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
      FMS_LOG_WARNING("Failed to connect to WiFi");
      gpio_set_level(GPIO_NUM_2, HIGH);
      vTaskDelay(pdMS_TO_TICKS(500));
      gpio_set_level(GPIO_NUM_2, LOW);
      vTaskDelay(pdMS_TO_TICKS(500));
    } else {
      FMS_LOG_INFO("Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(GPIO_NUM_2, HIGH);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}

