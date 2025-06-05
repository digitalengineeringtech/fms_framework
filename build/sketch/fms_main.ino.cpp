#include <Arduino.h>
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
/*
  FMS main source file
  Author: Trion
  Date: 2025
  Guided By Senior Engineer : Sir Thiha Kyaw
  Description: This file is the main source file of FMS project.
*/
 

/* device login page */
const String correctUsername = "admin";           /* change your login username here*/
const String correctPassword = "admin";           /*change your login pass here*/
const String firmwareVersion = "0.1.0";           /* Current firmware version*/
String deviceName            = "ultm_25505v01_";  /*device ID (for)  change here like this user can change with configpanel*/
#define CLI_PASSWORD         "admin"              /*cli password change this password*/
/* end change note  */

#define FMS_TATSUNO_DEBUG_OPEN


#include "_fms_main.h"
#include "src/_fms_cli.h"
#include "src/_fms_debug.h"
#include "src/_fms_json_helper.h"
#include "src/_fms_lanfeng.h"
#include <src/_fms_redstar.h>           /* test features */
#include <src/_fms_tatsuno.h>
#include <src/_fms_filemanager.h>  /* test features */


// #define DISABLE_MQTT_DEBUG
// #ifdef DISABLE_MQTT_DEBUG
// #undef FMS_MQTT_DEBUG
// #endif
// #define USE_MQTT_DEBUG

#define USE_CLI
#define DISABLE_LANFENG  // Uncomment this line to disable the library
#ifdef DISABLE_LANFENG
#undef USE_LANFENG  // Undefine USE_LANFENG to disable the library
#endif

/* #define USE_RESTAR */
#define USE_TATSUNO

FMS_FileManager fileManager;
fms_cli fms_cli(fms_cli_serial, CLI_PASSWORD);      // Use "admin" as the default password change your admin pass here
Redstar redstar(fms_uart2_serial);                  // create redstar object
TatsunoProtocol tatsuno(fms_uart2_serial);          // Create an instance of TatsunoProtocol with the HardwareSerial object
fmsLanfeng lanfeng(22, 22);                         // set re de pin (DTR PIN)s

/* Main function */

#line 54 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup();
#line 116 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void loop();
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_command(const std::vector<String>& args);
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_restart_command(const std::vector<String>& args);
#line 25 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_scan_safe_command(const std::vector<String>& args);
#line 80 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_connect_command(const std::vector<String>& args);
#line 149 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_read_command(const std::vector<String>& args);
#line 169 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_test_command(const std::vector<String>& args);
#line 194 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_test_command(const std::vector<String>& args);
#line 268 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_device_id_change_command(const std::vector<String>& args);
#line 281 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_protocol_command(const std::vector<String>& args);
#line 308 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
size_t custom_print(const uint8_t* buffer, size_t size);
#line 324 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_protocol_config_command(const std::vector<String>& args);
#line 347 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
static void cli_task(void* arg);
#line 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_boot_count(bool bootsave);
#line 25 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_chip_info();
#line 32 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_uart2();
#line 43 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_wifi();
#line 53 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_run_sd_test();
#line 65 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_debug_info();
#line 72 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_pin_mode(int pin, int mode);
#line 76 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_dns_responder_init();
#line 90 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateFinalData(int pump_id,float sell_price_liters,float sell_liters,float price,float totalizer,unsigned long long totalizer_amount);
#line 98 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateLiveData(int pump_id,float price_liters,float live_liters);
#line 108 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePresetAmount(String presetData);
#line 118 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePumpId(String presetData);
#line 131 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void init_staus_leds();
#line 147 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_load_protocol_config();
#line 177 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_set_protocol_config(const String& protocol);
#line 12 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length);
#line 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_subsbribe_topics();
#line 37 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_reconnect();
#line 70 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
static void mqtt_task(void* arg);
#line 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void selectMuxChannel(uint8_t channel);
#line 14 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void enable_mux(int pin);
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void disable_mux(int pin);
#line 22 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void test_mux();
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
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
#line 41 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool fm_cli_task_create();
#line 158 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_init();
#line 236 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_protocol_main();
#line 276 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void mainfun();
#line 381 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void tatsuno_pump_setting(char* topic, String payload);
#line 530 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void messageClassified();
#line 705 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpreqmqtt();
#line 735 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pplivemqtt();
#line 806 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedCRC();
#line 897 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun();
#line 925 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
uint16_t calculate_crc(uint8_t* data, int length, uint16_t polynomial);
#line 943 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void BufferClear();
#line 947 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpactive();
#line 963 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpenqactive();
#line 985 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_device_setup();
#line 1088 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendenq(int eq);
#line 1110 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendACK1();
#line 1119 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendEOT();
#line 1129 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void txledonoff();
#line 1135 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void rxledonoff();
#line 1141 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void cancelfinalsend();
#line 1164 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void finalsend();
#line 1382 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendcrcfun();
#line 1442 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangefun();
#line 1480 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpmqttnumchange();
#line 1576 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendpumpstatus(int pump);
#line 1627 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumppresetfun();
#line 1725 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void reloadfun();
#line 1979 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricereqfun();
#line 1992 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1Totalizerstatus();
#line 2001 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2Totalizerstatus();
#line 2010 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2status();
#line 2020 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1status();
#line 2029 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1Select();
#line 2037 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2Select();
#line 2045 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangeapprove2fun();
#line 2056 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangeapprove1fun();
#line 2067 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void finalmqtt1();
#line 2142 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpidchange();
#line 2147 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumapprofun();
#line 2161 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void hmisetup();
#line 2277 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void saveall();
#line 2303 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void writeString(String ssid, String password);
#line 2313 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void mqttpumpidchange(int pumpid);
#line 2413 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumappproSend();
#line 2463 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedPreset();
#line 2538 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun1();
#line 2571 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun2();
#line 2597 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void resendpreset();
#line 2626 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedAppro();
#line 2703 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void resendappro();
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate);
#line 13 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fm_rx_irq_interrupt();
#line 33 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fms_uart2_decode(uint8_t* data, uint32_t len);
#line 50 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
void fms_uart2_task(void* arg);
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag);
#line 42 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool wifi_led_ticker();
#line 49 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
static void wifi_task(void *arg);
#line 54 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup() {
  init_staus_leds();  // initialize status LEDs
#ifdef USE_CLI
  fms_cli.begin(115200);  // Initialize the CLI with a baud rate of 115200
  fms_cli.register_command("wifi", "Configure WiFi settings", handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect", "Connect to WiFi network", handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart", "Restart the system", handle_restart_command);
  fms_cli.register_command("wifiscan_safe", "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread", "Read current WiFi status", handle_wifi_read_command);
  fms_cli.register_command("wifi_test", "Test WiFi connection", handle_wifi_test_command);
  fms_cli.register_command("uuid_change", "Change Your Device Id unique address", handle_device_id_change_command, 1, 1);
  fms_cli.register_command("protocol", "Set Protocol", handle_protocol_command, 1, 1);
  fms_cli.register_command("protocol_config"," Set Protococl Congfig", handle_protocol_config_command, 11, 11);
  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)
#endif

  fms_pin_mode(BUILTIN_LED, OUTPUT);

  /* test features protocol selection 
  fms_load_protocol_config();  // load protocol config from nvs storage

  while (sysCfg.protocol == "0") {  // wait for protocol to be set
    FMS_LOG_ERROR("Protocol not set, waiting...");
    vTaskDelay(pdMS_TO_TICKS(1000));  // wait for 1 second
  }
  */

#ifdef USE_MUX_PC817
  fms_pin_mode(MUX_S0, OUTPUT);             // Multiplexer
  fms_pin_mode(MUX_S1, OUTPUT);
  fms_pin_mode(MUX_E, OUTPUT);
  enable_mux(MUX_E);                        // enable multiplexer (active low)
#endif

  fms_run_sd_test();                        // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(true);             // show serial logging data on Serial Monitor
  fms_boot_count(true);                     // boot count

#ifdef USE_LANFENG                         // lanfeng Protocol
  FMS_LOG_INFO("[LANFENG] Starting Lanfeng");
  lanfeng.init(1, fms_uart2_serial);       // add slave id
#endif

#ifdef USE_RESTAR
  red_star_init();                         // redstar init
#endif

#ifdef USE_TATSUNO
  fms_tatsuno_init();                      // tatsuno init
#endif

  /* test features
  if (fms_initialize_wifi() && sysCfg.protocol != "0") {  // wifi is connected create all task s
    fms_task_create();
  }
  */

  if (fms_initialize_wifi()) {             // wifi is connected create all task s
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
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  vTaskDelay(pdMS_TO_TICKS(100));
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
    Serial.print("{");
    Serial.print("  \"ssid\": \"");
    Serial.print(WiFi.SSID());
    Serial.print("\",");
    Serial.print("  \"rssi\": ");
    Serial.print(WiFi.RSSI());
    Serial.print(",");
    Serial.print("  \"ip\": \"");
    Serial.print(WiFi.localIP().toString());
    Serial.print("\"");
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
  fms_nvs_storage.begin("fms_config", true);  // Read-only mode
  String ssid = fms_nvs_storage.getString("ssid", "");
  String password = fms_nvs_storage.getString("pass", "");
  fms_nvs_storage.end();

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
    #define USE_RESTAR
    fms_set_protocol_config("redstar");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Allow time for changes to take effect
    ESP.restart();
    // Set Redstar protocol
    fms_cli.respond("protocol", "Protocol set to Redstar");
    // Add any additional setup for Redstar here
  } else if (protocol == "tatsuno") {
    #define USE_TATSUNO
    fms_set_protocol_config("tatsuno");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Allow time for changes to take effect
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
  return Serial.write(buffer, size);
}

// Protocol config command
void handle_protocol_config_command(const std::vector<String>& args) {
  if (args.size() < 11) {
    fms_cli.respond("protocol_config", "Usage: protocol_config <param1> <param2> ... <param11>", false);
    return;
  }

  String protocol = args[0];
  uint8_t devn = args[1].toInt();
  uint8_t noz = args[2].toInt();
  uint8_t pumpids[8];
  for (int i = 0; i < 8; i++) {
    pumpids[i] = args[i + 3].toInt();
  }
  Serial.printf(
    "[INFO] Setting protocol configuration: %s, devn: %d, noz: %d, pumpids: %d %d %d %d %d %d %d %d\n",
    protocol.c_str(), devn, noz,
    pumpids[0], pumpids[1], pumpids[2], pumpids[3],
    pumpids[4], pumpids[5], pumpids[6], pumpids[7]
  );

  fms_cli.respond("protocol_config", "Setting protocol configuration... accepted", true);
}

static void cli_task(void* arg) {
  BaseType_t rc;
  // cli command
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_lanfeng_fun.ino"
#ifdef USE_LANFENG

void sendPumpRequest(uint8_t nozzleNumber) {
  if (!permitMessageSent) {
    snprintf(pumprequest, sizeof(pumprequest), "%s%d", permitTopic, nozzleNumber);
    snprintf(payload, sizeof(payload), "%02dpermit", nozzleNumber);
    Serial.println(String(pumprequest).c_str());  // testing // please remove
    Serial.println(String(payload).c_str());     // testing // please remove
    fms_mqtt_client.publish(pumprequest, payload);
    Serial.println("Permit message sent to MQTT broker.");  // testing // please remove
    FMS_LOG_INFO("Permit message sent to MQTT broker.");
    permitMessageSent = true;
  }
}

bool waitForPumpApproval(int pumpIndex) {
  int wait_time = 0;
  while (!pump_approve[pumpIndex] && wait_time < PUMP_REQUEST_TIMEOUT_MS) {
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_time += 100;
    if (wait_time % 1000 == 0) {
      FMS_LOG_DEBUG("Waiting for pump approval ... %d ms\n", wait_time);
    }
  }
  return pump_approve[pumpIndex];
}

void startPump(uint16_t pumpStateAddr) {
  uint32_t setPumpResult = lanfeng.setPumpState(pumpStateAddr, 0x0001); // pump on & off control 
  if (setPumpResult != 0x01) {
    FMS_LOG_DEBUG("Error starting pump: 0x%X\n", setPumpResult);
    return;
  }
}

void stopPump(uint16_t pumpStateAddr) {
  uint32_t setPumpResult = lanfeng.setPumpState(pumpStateAddr, 0x0000); // pump on & off control 
  if (setPumpResult != 0x01) {
    FMS_LOG_DEBUG("Error stopping pump: 0x%X\n", setPumpResult);
    return;
  }
}

float LivePrice(uint32_t literPerPrice, float l_liter_float) {
  return (literPerPrice) * l_liter_float;  // optional features // S = P × L // live price = Liter per price * live liter
}

void publishPumpData(int pumpIndex, uint16_t liveDataAddr, uint16_t priceAddr) {
  uint32_t liveData_result = lanfeng.readLiveData(liveDataAddr, l_liter);     // get Live Liter
  uint32_t literPerPrice = lanfeng.readSellLiterPerPrice(priceAddr);          // get price per liter 
  float l_liter_float = lanfeng.convert_float(l_liter[0], l_liter[1]);        // convert to float (live liter ) (modbus returs value is two 16 bit register so we convert to float vlaue
 liveLiterPrice = LivePrice(literPerPrice, l_liter_float);                    // optional features // S = P × L // live price = Liter per price * live liter
  if (liveData_result == 0x01) {                                              // check if live data is read successfully
    snprintf(pplive + strlen(pplive), sizeof(pplive) - strlen(pplive), "%d", pumpIndex); // mqtt topic to publish live data
    String l_liter_str = fms_generateLiveData(pumpIndex, literPerPrice, l_liter_float);
    fms_mqtt_client.publish(pplive, l_liter_str.c_str());
    FMS_LOG_DEBUG("Live data sent to MQTT broker.");
  } else {
    FMS_LOG_DEBUG("[LANFENG] Error reading live data: 0x%X\n", liveData_result);
  }
}

void startFinalDataPublish() {
  snprintf(ppfinal, sizeof(ppfinal), "%s%d", ppfinal, 1);
  uint32_t literPerPrice = lanfeng.readSellLiterPerPrice(PRICE_ADDR);
  uint32_t sellLiter = lanfeng.readSellLiter(SELL_LITER_ADDR, s_liter);
  if (sellLiter == 0x01) {
    s_liter_float = lanfeng.convert_float(s_liter[0], s_liter[1]);
  }
  uint32_t finalPrice = sellLiter * literPerPrice;
  uint32_t totalLiter = lanfeng.readTotalizerLiter(TOTALIZER_LITER_ADDR, t_liter);
  if (totalLiter == 0x01) {
    t_liter_float = lanfeng.convert_float(t_liter[0], t_liter[1]);
  }
  uint32_t totalAmount = lanfeng.readTotalizerAmount(TOTALIZER_AMOUNT_ADDR, t_amount);
  if (totalAmount == 0x01) {
    t_amount_float = lanfeng.convert_float(t_amount[0], t_amount[1]);
  }
  String finalMessage = fms_generateFinalData(1, literPerPrice, s_liter_float, finalPrice, t_liter_float, t_amount_float);
  fms_mqtt_client.publish(ppfinal, finalMessage.c_str());
}

void setLivePrice(float price) {
  uint32_t floatAsInt;
  memcpy(&floatAsInt, &price, sizeof(price));  // Safely convert float to 32-bit integer
  uint16_t highWord_ = (floatAsInt >> 16) & 0xFFFF;             // Extract high word
  uint16_t lowWord_ = floatAsInt & 0xFFFF;                      // Extract low word
  lanfeng.setValue_helper(LIVE_PRICE_ADDR, highWord_, lowWord_);  // Set live price in lanfeng class
  FMS_LOG_DEBUG("Setting value: %f, High Word: %04X, Low Word: %04X", price, highWord_, lowWord_);
}

void fms_lanfeng_approval_state() {
  uint32_t noz_handle = lanfeng.readPermit(NOZ_HANDLE_ADDR); // check  nozel handle 
  if (noz_handle == 1) {
    sendPumpRequest(NOZ_ID);  // send permit message to mqtt broker
    if (waitForPumpApproval(0)) {
      startPump(PUMP_STATE_ADDR);  // start pump
      publishPumpData(1, LIVE_DATA_ADDR, PRICE_ADDR);  // publish live data to mqtt broker
      FMS_LOG_DEBUG("LIVEPRICE %f", liveLiterPrice);  // 32 float to 16 bit low, high word check in startPumpAndPublishData
      setLivePrice(liveLiterPrice);  // set live price in lanfeng class
    } else {
      FMS_LOG_DEBUG("Pump approval timed out.");
      sendPumpRequest(NOZ_ID);
    }
  } else if (noz_handle == 0 && permitMessageSent) {
    lanfeng.setPumpState(PUMP_STATE_ADDR, 0x0000);  // stop pump
    FMS_LOG_DEBUG("Pump 1 stopped.");
    startFinalDataPublish();
    pump_approve[0] = false;       // reset after usings
    permitMessageSent = false;    // reset permit message sent flag
  } else {
    FMS_LOG_DEBUG("Modbus Error on 0x%02X : 0x%02X\n",NOZ_HANDLE_ADDR, noz_handle);
  }
}

// start here lanfeng 
void fms_lanfeng_protocol() {
  if(!presetMessageGet) {
   fms_lanfeng_approval_state(); // check lanfeng protocol
  } else if(presetMessageGet) {
    startPump(PUMP_STATE_ADDR);  // start pump
    uint32_t noz_handle = lanfeng.readPermit(NOZ_HANDLE_ADDR); // check  nozel handle 
    while(noz_handle == 0){
      vTaskDelay(pdMS_TO_TICKS(200)); // wait for 200ms
      noz_handle = lanfeng.readPermit(NOZ_HANDLE_ADDR); 
      FMS_LOG_DEBUG("Waiting for pump approval (0)... %d ms\n", 200);
    }
    publishPumpData(1, LIVE_DATA_ADDR, PRICE_ADDR);  // publish live data to mqtt broker
    setLivePrice(liveLiterPrice);  // set live price in lanfeng class
    while(noz_handle == 1) {
      vTaskDelay(pdMS_TO_TICKS(200)); // wait for 200ms
      noz_handle = lanfeng.readPermit(NOZ_HANDLE_ADDR); 
      FMS_LOG_DEBUG("Waiting for pump approval (1)... %d ms\n", 200);
      publishPumpData(1, LIVE_DATA_ADDR, PRICE_ADDR);  // publish live data to mqtt broker
      setLivePrice(liveLiterPrice);  // set live price in lanfeng class
    }
    stopPump(PUMP_STATE_ADDR);  // stop pump
    FMS_LOG_DEBUG("Pump 1 stopped.");
    startFinalDataPublish();
    presetMessageGet = false; // reset after using
  }
}

#endif

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
    FMS_LOG_ERROR("[fms_main_func.ino:13] Failed to initialize NVS storage");
    return;
  }

  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0) + 1;
  app_cpu = xPortGetCoreID();
  FMS_LOG_INFO("[fms_main_func.ino:19] CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);

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
    FMS_LOG_INFO("[fms_main_func:45] Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    FMS_LOG_WARNING("[fms_main_func:48] Failed to connect to WiFi");
    return false;
  }
}

void fms_run_sd_test() {
#if true
  fms_config_load_sd_test();
  if (!LittleFS.begin(true)) {  // preference storage (1MB)
    Serial.println("[fms_main_func.ino:57] [STORAGE] Failed to mount file system");
  } else {
    Serial.println("[fms_main_func.ino:59] [STORAGE] File system mounted");
  }
  // load system config file
#endif
}

void log_debug_info() {
#if SHOW_DEBUG_FMS_CHIP_INFO_LOG
  fms_print_after_setup_info();
  fms_log_task_list();
#endif
}

void fms_pin_mode(int pin, int mode) { // pin out declare
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
  float liveLiterPrice = price_liters * live_liters;  // S = P × L
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

/*
{
  "device": "U8990",
  "payload": "N01L18.90P2000",
  "crc_hex": "A1B2",
  "crc_dec": 41394
}
*/

void init_staus_leds() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_RED) | (1ULL << LED_GREEN) |
                        (1ULL << LED_BLUE) | (1ULL << LED_YELLOW),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_RED, 1);
    gpio_set_level(LED_GREEN, 1);
    gpio_set_level(LED_BLUE, 1);
    gpio_set_level(LED_YELLOW, 1);
}

 void fms_load_protocol_config() {

  if (!fms_nvs_storage.begin("fms_p_config", false)) {
    FMS_LOG_ERROR("Failed to initialize NVS storage");
    return;
  }

  sysCfg.protocol = fms_nvs_storage.getString("protocol", "0");  // Default to "redstar" if not set
  if (sysCfg.protocol != "redstar" && sysCfg.protocol != "tatsuno") {
    FMS_LOG_ERROR("Invalid protocol configuration, defaulting to redstar");
    sysCfg.protocol = "0";  // Default to "redstar" if invalid
  }
  FMS_LOG_INFO("Protocol: %s", sysCfg.protocol.c_str());

  if (sysCfg.protocol == "redstar") {
    #define USE_RESTAR
    #undef USE_TATSUNO  // Ensure Tatsuno is not defined
    FMS_LOG_INFO("Using Redstar protocol");
  } else if (sysCfg.protocol == "tatsuno") {
    #define USE_TATSUNO
    #undef USE_RESTAR  // Ensure Redstar is not defined
    FMS_LOG_INFO("Using Tatsuno protocol");
  } else {
    FMS_LOG_ERROR("Unknown protocol, defaulting to redstar");
    sysCfg.protocol = "0";  // Default to "redstar"
  }

  fms_nvs_storage.end();  // Close NVS storage
}

void fms_set_protocol_config(const String& protocol) {
  if (!fms_nvs_storage.begin("fms_p_config", false)) {
    FMS_LOG_ERROR("Failed to initialize NVS storage");
    return;
  }

  fms_nvs_storage.putString("protocol", protocol);
  sysCfg.protocol = protocol;  // Update the global configuration
  FMS_LOG_INFO("Protocol set to: %s", sysCfg.protocol.c_str());
  fms_nvs_storage.end();  // Close NVS storage
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
//#define FMS_MQTT_DEBUG
#ifdef FMS_MQTT_DEBUG
  #define FMS_MQTT_LOG_DEBUG(format, ...) Serial.print("[fms_mqtt.ino][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_MQTT_LOG_ERROR(format, ...) Serial.print("[fms_mqtt.ino][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_MQTT_LOG_DEBUG(format, ...)
  #define FMS_MQTT_LOG_ERROR(format, ...)
#endif


char fms_nmf_tp_prefix[64];
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("fms_mqtt.ino:13:Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int j = 0; j < length; j++) message += (char)payload[j];

  #ifdef USE_REDSTAR
  // mqtt callback for redstar protocol
   redstar_pump_setting(topic,message); // call redstar pump setting function
  #endif
  #ifdef USE_TATSUNO
  // mqtt callback for tatsuno protocol
   tatsuno_pump_setting(topic,message); // call tatsuno pump setting function
  #endif
}


void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    FMS_MQTT_LOG_DEBUG("Subscribing to topic: %s", fms_sub_topics[i]);
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
    FMS_MQTT_LOG_DEBUG("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str(),sysCfg.mqtt_user,sysCfg.mqtt_password,willTopic,willQos,willRetain,willMessage)) {
       
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
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
      FMS_MQTT_LOG_ERROR("Failed to connect to MQTT server , rc = %d try again in 5 second", fms_mqtt_client.state());
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

unsigned long previous_Millis   = 0;
const long interval             = 1000; // Interval for sending messages
bool ledState_                  = false;

static void mqtt_task(void* arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(MQTT_SERVER, 1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (mqttTask) {
    unsigned long currentMillis = millis();
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
      if (currentMillis - previous_Millis >= interval) {
        previous_Millis = currentMillis;
        ledState_ = !ledState_;
        gpio_set_level(LED_GREEN, ledState_);
      }
    } else {
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
#ifdef USE_MUX_PC817

#define MUX_S0 25
#define MUX_S1 26
#define MUX_E 27 // enable input (active LOW)

// reference from 74hc lib channel chooser
 void selectMuxChannel(uint8_t channel){
    digitalWrite(MUX_S0, channel & 0x01); // on 
    digitalWrite(MUX_S1, (channel >> 1) & 0x01);// off
    vTaskDelay(pdMS_TO_TICKS(1)); // switch time on off time
 }

 void enable_mux(int pin){
    digitalWrite(pin,LOW);
 }

 void disable_mux(int pin){
    digitalWrite(pin,HIGH);
 }

 void test_mux(){
    for (int device = 0; device < 4; device++) {
        selectMuxChannel(device);
        fms_uart2_serial.println("Hello device " + String(device));
        vTaskDelay(pdMS_TO_TICKS(1000)); // switch time on off time
        if (fms_uart2_serial.available()) {
          String reply = fms_uart2_serial.readStringUntil('\n');
          FMS_LOG_DEBUG("FROM DEVICE :%d,[%s]\n", device, reply.c_str());
        }
      }
 }


#endif
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
/* user take stack out error */
/* 
sometime webserver is outoff stack error , fix your stack size in fms_header.h file 
pending to upgrade  mongoose wizard ui builder 
UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);
Serial.print("Stack Remaining: ");
Serial.println(stackRemaining);  // Prints remaining stack (in words)
*/

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"

#ifdef USE_RESTAR

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

uint8_t addresscount                          = 1;    // Address for the Redstar device
uint8_t nozzlecount                           = 2;    // Number of nozzles
const unsigned long state_query_interval      = 2;
unsigned long current_time_seconds            = 0;    // Current time in seconds
unsigned long last_pump_query_interval        = 0;    // Interval for querying state
unsigned char buffer[30];

uint8_t server_response_nozzle_id             = 0;      // Nozzle ID from server response
int preset_price                              = 0;      // Preset price
int preset_liters                             = 0;      // Preset liters

char preset_state[2];                                   // check preset noz id and amount or liters (0x01,'P')
char charArray[4];                                      // check nozzle id form server response 
char volume_char[4];                                    // separate real-time buffer data to get volume (liter) data convert into Decimal data
char amount_char[4];                                    // separate real-time buffer data to get amount (price) data convert into Decimal data
char mqttdatabuf[50];                                   // Buffer for MQTT data
char presetArray[13];
char pricechangeArray[7];                              // Price change array  

bool pump1_cancel = false;                                  // nozzle one cancel count
bool pump2_cancel = false;                                  // nozzle two cancel count
bool reload_check_1                           = false;       // control reload function // pump reloading check 
bool reload_check_2                           = false;       // control reload function
bool presetcount                              = false;  // Preset count flag
bool lastpresetcount                          = false;  // Last preset count flag
bool preset_check                             = false;  // Preset check flag
bool lastpreset_send                          = false;  // Last preset check flag
bool nozzle1_approval                         = false;  // Approval status for nozzle 1
bool nozzle2_approval                         = false;  // Approval status for nozzle 2

String price_state;                                         // to store converted price request data
String total_state;                                         // to store converted  totalizer liter data
String total_amount;                                        // to store converted toatlizer ammount data
String pump2_status                           = "ide";  // control for nozzle one request final data
String pump1_status                           = "ide";  // control for nozzle two request final data
String permit_msg                             = "permit"; // Permit message
String cancel_msg                             = "cancel"; // Cancel message
String price;
String liter;


// eeprom data hard coding
uint8_t pump1id = 1;                                      // Nozzle ID for pump 1
uint8_t pump2id = 2;                                      // Nozzle ID for pump 2
uint8_t pump3id;                                          // Nozzle ID for pump 3
uint8_t pump4id;                                          // Nozzle ID for pump 4
uint8_t pump5id;                                          // Nozzle ID for pump 5
uint8_t pump6id;                                          // Nozzle ID for pump 6
uint8_t pump7id;                                          // Nozzle ID for pump 7
uint8_t pump8id;                                          // Nozzle ID for pump 8

// device id 
uint8_t device_id = 0;                                    // Device ID
uint8_t nozzle_count = 0;                                 // Nozzle count

unsigned char* response;
int length = 0;   
int data_count = 0;                                  // Length of the response
// Create an instance of the Redstar class
void red_star_init() {
  redstar.begin(19200, true, RXD2, TXD2);  // Initialize the Redstar object with the specified baud rate and pins
}


// red start main function (included , pump state, nozzle lifted, fueling)
// check the response from dispenser
// red star main file
void fms_red_star_protocol_main() {
  vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
  if(redstar.update()){
    response = redstar.parseResponse(length);  // Parse the response from the Redstar device
    FMS_RED_LOG_DEBUG("+--------------------------------------+");
    FMS_RED_LOG_DEBUG("Data Count: %d", data_count);
    FMS_RED_LOG_DEBUG("Length: %d", length);
    FMS_RED_LOG_DEBUG("Data: ");
    FMS_RED_LOG_DEBUG("RESPONSE: 0x%02X ",response[data_count]);
    FMS_RED_LOG_DEBUG("+--------------------------------------+");

    data_count++;
  if(addresscount > nozzlecount) addresscount = 1;  // Reset address count if it exceeds the number of nozzles
  // normal state -> 0x12 0x57 
  if (response[length - 2] == 0x12 && (response[length - 1] == 0x57 || response[length - 1] == 0x56 || response[length - 1] == 0x53 || response[length - 1] == 0x52)) { 
    for (int i = 0; i < 8; i++) {
      buffer[i] = response[i];  // Store the response in the buffer
      vTaskDelay(pdMS_TO_TICKS(5));  // Delay for 5 milliseconds
      Serial.print(length);
      Serial.print("/FE/");
      Serial.print("0x");
      Serial.print(buffer[i], HEX);  // Print the buffer contents in hexadecimal format
      Serial.print(" ");
    }
    FMS_RED_LOG_DEBUG("Fueling");
    data_count = 0;                                               // Reset length for the next response
    length = 0;                                                   // Reset length for the next response
    if (buffer[0] == 0x01 && pump1_status == "fuel" || reload_check_1) {
      addresscount = 1;                                           // Check if the first byte is 0x01
      send_fuel_fun(response);                                    // Request fuel data from the dispenser
      FMS_RED_LOG_DEBUG("------- FINAL-------     1");
      send_read_price(response);                                  // Request price data from the dispenser
      send_read_total();                                          // Request total data from the dispenser
      FMS_RED_LOG_DEBUG("Nozzle %d is in use", addresscount);     // Log the nozzle in use
    } else {
      FMS_RED_LOG_DEBUG("Nozzle %d is not in use", addresscount);  // Log the nozzle not in use
    }
  }  
  // nozel lifted state -> 0x12 0x77 or 0x76,0x72,0x73 response nozzle lifted condition
  else if(response[data_count - 2] == 0x12 && (response[data_count - 1] == 0x77 || response[data_count - 1] == 0x76 || response[data_count - 1] == 0x72 || response[data_count - 1] == 0x73)) { 
    responsne_buffer(response,8,"Nozzle lifted");
    if(preset_check){
      if(lastpreset_send) {
        send_preset_state(response);
        lastpreset_send = false;  // Reset the last preset send flag
        preset_check = false;     // Reset the preset check flag
      } else {
        send_read_state();        // Request pump state
        preset_check = false;     // Reset the preset check flag
      }
    } else {
      if(nozzle1_approval == true && buffer[0] == 0x01) {
        send_approve_state();       // Send approval for pump 1
        nozzle1_approval = false;   // Reset the approval status for pump 1
      } else if (nozzle2_approval == true && buffer[0] == 0x02) {
        send_approve_state();       // Send approval for pump 2
        nozzle2_approval = false;   // Reset the approval status for pump 2
      } else {
        memset(mqttdatabuf, '\0', 50);                   // Clean the MQTT data buffer
        if (buffer[0] == 0x01) pump1_cancel = true;      // Set pump 1 cancel flag
        else if (buffer[0] == 0x02) pump2_cancel = true; // Set pump 2 cancel flag
        pumpidchange();                                  // Nozzle ID authentication for sending permit MQTT data
        permit_msg.toCharArray(&mqttdatabuf[2], permit_msg.length() + 1); // Add permit string to MQTT data buffer
        fms_mqtt_client.publish(pumpreqbuf, mqttdatabuf); // Send permit message from MQTT
        preset_check = false;                             // Reset the preset check flag
        send_read_state();                                // Request pump state
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
  
#endif
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
    vTaskDelay(pdMS_TO_TICKS(100));
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

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"

#ifdef USE_TATSUNO
/* tatsuno parameter */
#define RESPONSE_BUFFER_SIZE 50
int length = 0;
// char* Buffer[RESPONSE_BUFFER_SIZE];  // Buffer for incoming data

/*
#define LED_RED                     GPIO_NUM_32
#define LED_GREEN                   GPIO_NUM_14 
#define LED_BLUE                    GPIO_NUM_13
#define LED_YELLOW                  GPIO_NUM_33
*/

#define wifiled 33
#define powerled 32
#define TXled 27
#define RXled 26
#define DIR_PIN 22

// #define RXD2 16
// #define TXD2 17

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
unsigned char showSSID[6] = { 0X5A, 0XA5, 0X40, 0X82, 0X12, 0x00 };
unsigned char showPASS[6] = { 0X5A, 0XA5, 0X40, 0X82, 0X13, 0x00 };
unsigned char page[9] = { 0X5A, 0XA5, 0X07, 0X82, 0X00, 0X84, 0X5A, 0X01, 0X00 };  // Page change
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
  fms_load_tatsuno_config(tatsunoConfig);  /* Load Tatsuno configuration from NVS storage */
  fms_tatsuno_device_setup();  /*Setup Tatsuno device*/
  enqactivetime1 = millis() / 1000;
}

/* save tatsuno config */
bool fms_save_tatsuno_config(TatsunoConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_ts_config", false)) {
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
  if (!fms_nvs_storage.begin("fms_ts_config", true)) {
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
  gpio_set_level(LED_BLUE, 1);
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
        myfirst = true;
        gpio_set_level(LED_RED, LOW);
      } else {
        // Serial.println("[fms_tatsuno_fun.ino]  Connected to the Cloud");
          gpio_set_level(LED_RED, HIGH);
          gpio_set_level(LED_GREEN, HIGH);
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
  gpio_set_level(LED_YELLOW,LOW);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // delay for 10 ms
  gpio_set_level(LED_YELLOW, HIGH);
}

void rxledonoff() {
  gpio_set_level(LED_BLUE, HIGH);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // delay for 10 ms
  gpio_set_level(LED_BLUE, LOW);
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
    Serial.printf("U got char: %c (0x%02X)", ch, ch);

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







#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, RXD2, TXD2);  // RXD2 and TXD2 are the GPIO pins for RX and TX
    if (fms_uart2_serial) {
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}

void fm_rx_irq_interrupt() {  // interrupt RS485/RS232 function
  uint8_t Buffer[30];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available();  // serial.available
  FMS_LOG_DEBUG("Got bytes on serial : %d\n", size);
  while (fms_uart2_serial.available() && bytes_received < sizeof(Buffer)) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++;
  }
  if(bytes_received > 0) {
    FMS_LOG_DEBUG("\n uart2 data process \n\r");
    FMS_LOG_DEBUG("uart2 data : %s\n\r", Buffer);
    FMS_LOG_DEBUG("uart2 data length : %d\n\r", bytes_received);
    UART_RECEIVE_STATE = true;
    fms_uart2_decode(Buffer, bytes_received);  // decode uart2 data main function
  }
 
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  

  // Print the raw byte data for debugging
  Serial.print("[FMSUART2] Received Data: ");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i], HEX);  // Print each byte in hex format
    Serial.print(" ");
  }
  Serial.println();

  // Example of how to process the data, depending on your protocol
  // FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data); // If data is string
  // Or process the data byte by byte

}

void fms_uart2_task(void* arg) {
  BaseType_t rc;
  while (1) {

        #ifdef USE_LANFENG /* developemnt features */
            fms_lanfeng_protocol(); /* lanfeng protocol */
        #endif

        #ifdef USE_RESTAR /* development features */
         FMS_LOG_INFO("[RESTAR] Starting Redstar Protocol");
            fms_red_star_protocol_main();  /* red star protocol */
        #endif

        #ifdef USE_TATSUNO /* pprd report development features */
            fms_tatsuno_protocol_main();     /* tatsuno protocol */
        #endif
// #ifdef USE_MUX_PC817
// test_mux();
// #endif
   vTaskDelay(pdMS_TO_TICKS(100));
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag) {
  if (flag) {
    // get ssid and password from nvs storage
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    
    if(ssid_str.length() == 0 || pass_str.length() == 0) {
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(500));
      FMS_LOG_ERROR("[fms_wifi.ino:11] [DEBUG WiFi] wifi .. credential .. value is empty");
      fms_nvs_storage.end();
      return false;
    }

    fms_nvs_storage.end();
    FMS_LOG_DEBUG("SSID : %s , PASS : %s", ssid_str, pass_str);
    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);
    if (sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") {
      FMS_LOG_ERROR("[fms_wifi.ino:21] [DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);  // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);

    while (WiFi.status() != WL_CONNECTED) {
      gpio_set_level(LED_RED, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));
      gpio_set_level(LED_RED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));

      FMS_LOG_INFO("[fms_wifi.ino:35] WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid, sysCfg.wifi_password);
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
      FMS_LOG_WARNING("[fms_wifi.ino:53] Failed to connect to WiFi");
      gpio_set_level(LED_RED, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));
      gpio_set_level(LED_RED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));
    } else {
      // FMS_LOG_INFO("[fms_wifi.ino:59] Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(LED_YELLOW, HIGH);
      gpio_set_level(LED_RED, HIGH);
      gpio_set_level(LED_BLUE, HIGH);
      gpio_set_level(LED_GREEN, LOW);
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Wait for 1 second before repeating
  }
}

