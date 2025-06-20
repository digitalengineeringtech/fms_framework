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
const String correctPassword = "admin";           /* change your login pass here*/
const String firmwareVersion = "2.0.0";           /* Current firmware version*/
String deviceName            = "ultm_25505v01_";  /* device ID (for)  change here like this user can change with configpanel*/
#define CLI_PASSWORD         "admin"              /* cli password change this password*/
/* end change note  */


#include "_fms_main.h"
#include "src/_fms_cli.h"
#include "src/_fms_debug.h"
#include "src/_fms_json_helper.h"
#include "src/_fms_lanfeng.h"
#include <src/_fms_redstar.h>           /* test features */
#include <src/_fms_tatsuno.h>
#include <src/_fms_filemanager.h>        /* test features */
#include <src/_fms_touch.h>

 #define USE_TOUCH
// #define FMS_TATSUNO_DEBUG_OPEN
// #define USE_V2_OTA_SERVER
// #define USE_RESTAR 
// #define USE_TATSUNO
// #define USE_REDSTAR
// #define USE_TOUCH
// #define DISABLE_MQTT_DEBUG
// #ifdef DISABLE_MQTT_DEBUGP
// #undef FMS_MQTT_DEBUG
// #endif
// #define USE_MQTT_DEBUG
// #define  USE_LANFENG        // Undefine USE_LANFENG to disable the library

#define USE_CLI
//#define USE_TATSUNO
#define DISABLE_LANFENG
#ifdef DISABLE_LANFENG
#undef USE_LANFENG
#endif

FMS_FileManager fileManager;
fms_cli fms_cli(fms_cli_serial, CLI_PASSWORD);      // Use "admin" as the default password change your admin pass here
Redstar redstar(fms_uart2_serial);                  // create redstar object
TatsunoProtocol tatsuno(fms_uart2_serial);          // Create an instance of TatsunoProtocol with the HardwareSerial object
fmsLanfeng lanfeng(22, 22);                         // set re de pin (DTR PIN)s

/* Main function */

#line 58 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup();
#line 127 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void loop();
#line 11 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_command(const std::vector<String>& args);
#line 27 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_restart_command(const std::vector<String>& args);
#line 34 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_scan_safe_command(const std::vector<String>& args);
#line 89 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_connect_command(const std::vector<String>& args);
#line 158 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_read_command(const std::vector<String>& args);
#line 178 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_test_command(const std::vector<String>& args);
#line 203 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_wifi_test_command(const std::vector<String>& args);
#line 277 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_device_id_change_command(const std::vector<String>& args);
#line 290 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_protocol_command(const std::vector<String>& args);
#line 317 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
size_t custom_print(const uint8_t* buffer, size_t size);
#line 333 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_protocol_config_command(const std::vector<String>& args);
#line 382 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_mqtt_command(const std::vector<String>& args);
#line 405 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void config_writeStringToEEPROM(int address, String data);
#line 417 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void config_eeprom_writeInt(int add, long data);
#line 431 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
void handle_nozzle_command(const std::vector<String>& args);
#line 485 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
static void cli_task(void* arg);
#line 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_boot_count(bool bootsave);
#line 23 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_chip_info();
#line 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_uart2();
#line 41 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
bool fms_initialize_wifi();
#line 51 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_run_sd_test();
#line 63 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void log_debug_info();
#line 70 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_pin_mode(int pin, int mode);
#line 74 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_dns_responder_init();
#line 88 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateFinalData(int pump_id,float sell_price_liters,float sell_liters,float price,float totalizer,unsigned long long totalizer_amount);
#line 96 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
String fms_generateLiveData(int pump_id,float price_liters,float live_liters);
#line 106 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePresetAmount(String presetData);
#line 116 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
int fms_decodePumpId(String presetData);
#line 129 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void init_staus_leds();
#line 145 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_load_protocol_config();
#line 172 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_set_protocol_config(DisConfig& cfg);
#line 209 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main_func.ino"
void fms_load_config();
#line 19 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_led_update();
#line 30 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_set_connected(bool connected);
#line 38 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length);
#line 55 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_subsbribe_topics();
#line 62 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
void fms_mqtt_reconnect();
#line 102 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mqtt.ino"
static void mqtt_task(void* arg);
#line 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void selectMuxChannel(uint8_t channel);
#line 14 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void enable_mux(int pin);
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void disable_mux(int pin);
#line 22 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_mux_pc817.ino"
void test_mux();
#line 413 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void fms_info_response();
#line 433 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleDashboard();
#line 448 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogin();
#line 464 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void handleLogout();
#line 469 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
void fms_set_ota_server();
#line 602 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_ota_server.ino"
static void web_server_task(void* arg);
#line 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
bool fms_sd_init();
#line 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
void fms_sd_dir(fs::FS& fs, const char* dirname, uint8_t levels);
#line 57 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
void fms_config_load_sd_test();
#line 63 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
bool write_data_sd(char* input);
#line 73 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_sd.ino"
static void sd_task(void* arg);
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
void _led_state();
#line 7 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc);
#line 28 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool fms_task_create();
#line 41 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_task.ino"
bool fm_cli_task_create();
#line 140 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_init();
#line 218 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_protocol_main();
#line 256 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void mainfun();
#line 348 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void tatsuno_pump_setting(char* topic, String payload);
#line 497 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void messageClassified();
#line 672 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpreqmqtt();
#line 702 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pplivemqtt();
#line 773 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedCRC();
#line 864 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun();
#line 892 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
uint16_t calculate_crc(uint8_t* data, int length, uint16_t polynomial);
#line 910 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void BufferClear();
#line 914 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpactive();
#line 930 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpenqactive();
#line 952 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void fms_tatsuno_device_setup();
#line 1055 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendenq(int eq);
#line 1073 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendACK1();
#line 1082 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendEOT();
#line 1092 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void txledonoff();
#line 1100 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void rxledonoff();
#line 1107 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void cancelfinalsend();
#line 1130 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void finalsend();
#line 1351 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendcrcfun();
#line 1411 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangefun();
#line 1449 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpmqttnumchange();
#line 1545 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendpumpstatus(int pump);
#line 1596 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumppresetfun();
#line 1694 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void reloadfun();
#line 1948 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricereqfun();
#line 1961 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1Totalizerstatus();
#line 1970 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2Totalizerstatus();
#line 1979 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2status();
#line 1989 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1status();
#line 1998 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump1Select();
#line 2006 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pump2Select();
#line 2014 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangeapprove2fun();
#line 2025 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pricechangeapprove1fun();
#line 2036 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void finalmqtt1();
#line 2111 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumpidchange();
#line 2116 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumapprofun();
#line 2130 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void hmisetup();
#line 2246 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void saveall();
#line 2272 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void writeString(String ssid, String password);
#line 2282 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void mqttpumpidchange(int pumpid);
#line 2382 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void pumappproSend();
#line 2432 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedPreset();
#line 2507 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun1();
#line 2540 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
unsigned char BCCfun2();
#line 2566 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void resendpreset();
#line 2595 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
void sendCalculatedAppro();
#line 2672 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_tatsuno_fun.ino"
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
#line 58 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_main.ino"
void setup() {

  init_staus_leds();                         // initialize status LEDs
  gpio_set_level(LED_RED, LOW);              // turn on red led

#ifdef USE_CLI
  fms_cli.begin(115200);  // Initialize the CLI with a baud rate of 115200
  fms_cli.register_command("wifi",         "Configure WiFi settings",       handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect", "Connect to WiFi network",       handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart",      "Restart the system",            handle_restart_command);
  fms_cli.register_command("wifiscan_safe", "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread",      "Read current WiFi status",     handle_wifi_read_command);
  fms_cli.register_command("wifi_test",     "Test WiFi connection",         handle_wifi_test_command);
  fms_cli.register_command("uuid_change",   "Change Your Device Id unique address", handle_device_id_change_command, 1, 1);
  fms_cli.register_command("protocol",      "Set Protocol",                 handle_protocol_command, 1, 1);
  fms_cli.register_command("protocol_config","Set Protocol Config",       handle_protocol_config_command, 11, 11);
  fms_cli.register_command("mqtt_config"   ,"Configure Mqtt settings",     handle_mqtt_command,2,2);
  fms_cli.register_command("noz_config", "Configure Nozzle settings",   handle_nozzle_command, 16, 16);
#endif
 
  fms_run_sd_test();                        // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(true);             // show serial logging data on Serial Monitor
  fms_boot_count(true);                     // boot count
  fms_load_config();                        // load config from nvs storage
 
/* not included in v1 */
#ifdef USE_MUX_PC817
  fms_pin_mode(MUX_S0, OUTPUT);             // Multiplexer
  fms_pin_mode(MUX_S1, OUTPUT);
  fms_pin_mode(MUX_E, OUTPUT);
  enable_mux(MUX_E);                        // enable multiplexer (active low)
#endif

/* not included in v1 */
#ifdef USE_LANFENG                         // lanfeng Protocol
  FMS_LOG_INFO("[LANFENG] Starting Lanfeng");
  lanfeng.init(1, fms_uart2_serial);       // add slave id
#endif

#ifdef USE_REDSTAR
  red_star_init();                         // redstar init
#endif

#ifdef USE_TATSUNO
  fms_tatsuno_init();                      // tatsuno init
#endif

/* task create */
  if (fms_initialize_wifi()) {             // wifi is connected create all task s
    fms_task_create();
  }



  /* test features protocol selection */
  /*
  fms_load_protocol_config();  // load protocol config from nvs storage
  while (sysCfg.protocol == "0") {  // wait for protocol to be set
    FMS_LOG_ERROR("Protocol not set, waiting...");
    vTaskDelay(pdMS_TO_TICKS(1000));  // wait for 1 second
  }
  // /* test features
  // if (fms_initialize_wifi() && sysCfg.protocol != "0") {  // wifi is connected create all task s
  //   fms_task_create();
  // }
 */

}

void loop() {
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_cli.ino"
/*
change note : 
  if (protocol != "tatsuno" && protocol != "gilbarco" && protocol != "redstar" && protocol != "haungyang") {
    fms_cli.respond("protocol_config", "Invalid protocol. Must be tatsuno, gilbarco, redstar, or haungyang", false);
    return;
  }

*/

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
    Serial.print("\"ssid\": \"");
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
    //fms_set_protocol_config("redstar");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Allow time for changes to take effect
    ESP.restart();
    // Set Redstar protocol
    fms_cli.respond("protocol", "Protocol set to Redstar");
    // Add any additional setup for Redstar here
  } else if (protocol == "tatsuno") {
    #define USE_TATSUNO
    //fms_set_protocol_config("tatsuno");
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

void handle_mqtt_command(const std::vector<String>& args) {

if (args.size() != 2) {
    fms_cli.respond("mqtt_config", "Usage: mqtt_config <host> <port>", false);
    return;
  }
  char mqtt_host[32];
  char mqtt_port[6];

  String host = args[0];
  String port = args[1];

    // Save to preferences
  fms_nvs_storage.begin("fms_config", false);
  fms_nvs_storage.putString("host", host);
  fms_nvs_storage.putString("port", port);
  fms_nvs_storage.end();


  fms_cli.respond("mqtt_config", "config successfully saved", true);
}


void config_writeStringToEEPROM(int address, String data) {  // to store wifi ssid and pass in EEprom
  int data_length = data.length();
  for (int i = 0; i < data_length; i++) {
    EEPROM.write(address + i, data[i]);
    EEPROM.commit();
  }
  EEPROM.write(address + data_length, '\0');
  EEPROM.commit();
  Serial.println("EEPROM write complete");
}


void config_eeprom_writeInt(int add, long data) {
  byte byte0 = (data >> 24) & 0xff;
  byte byte1 = (data >> 16) & 0xff;
  byte byte2 = (data >> 8) & 0xff;
  byte byte3 = data & 0xff;

  EEPROM.write(add, byte0);
  EEPROM.write(add + 1, byte1);
  EEPROM.write(add + 2, byte2);
  EEPROM.write(add + 3, byte3);
  EEPROM.commit();
}


void handle_nozzle_command(const std::vector<String>& args) {
  if (args.size() < 16) {
    fms_cli.respond("nozzle_config", "Usage: nozzle_config <> <>", false);
    return;
  }

  String nozzle_1_fuel_type  = args[0];
  int nozzle_1_fuel_price    = args[1].toInt();

  String nozzle_2_fuel_type  = args[2];
  int nozzle_2_fuel_price    = args[3].toInt();

  String nozzle_3_fuel_type  = args[4];
  int nozzle_3_fuel_price    = args[5].toInt();

  String nozzle_4_fuel_type  = args[6];
  int nozzle_4_fuel_price    = args[7].toInt();

  String nozzle_5_fuel_type  = args[8];
  int nozzle_5_fuel_price    = args[9].toInt();

  String nozzle_6_fuel_type  = args[10];
  int nozzle_6_fuel_price    = args[11].toInt();

  String nozzle_7_fuel_type  = args[12];
  int nozzle_7_fuel_price    = args[13].toInt();

  String nozzle_8_fuel_type  = args[14];
  int nozzle_8_fuel_price    = args[15].toInt();

/* for touch controller eeprom storage */
  config_writeStringToEEPROM(200, nozzle_1_fuel_type);
  config_writeStringToEEPROM(230, nozzle_2_fuel_type);
  config_writeStringToEEPROM(260, nozzle_3_fuel_type);
  config_writeStringToEEPROM(290, nozzle_4_fuel_type);
  config_writeStringToEEPROM(330, nozzle_5_fuel_type);
  config_writeStringToEEPROM(360, nozzle_6_fuel_type);
  config_writeStringToEEPROM(390, nozzle_7_fuel_type);
  config_writeStringToEEPROM(420, nozzle_8_fuel_type);

  config_eeprom_writeInt(114, 2900);
  config_eeprom_writeInt(118, 3000);
  config_eeprom_writeInt(122, 2700);
  config_eeprom_writeInt(81, 3300);
  config_eeprom_writeInt(85, 0);
  config_eeprom_writeInt(132, 0);
  config_eeprom_writeInt(138, 0);
  config_eeprom_writeInt(144, 0);


  fms_cli.respond("nozzle_config" ,"fuel" + String(nozzle_1_fuel_type) , true);
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

  if (!fms_nvs_storage.begin("fms_d_config", false)) {
    FMS_LOG_ERROR("[Protocol Config] Failed to initialize NVS storage");
    return;
  }

  // Load protocol type
  sysCfg.protocol = fms_nvs_storage.getString("protocol", "0");  // Default to "redstar"
  FMS_LOG_INFO("[Protocol Config] Protocol: %s", sysCfg.protocol.c_str());
  // Set protocol defines
  if (sysCfg.protocol == "redstar") {
    #define USE_REDSTAR
    #undef USE_TATSUNO
    FMS_LOG_INFO("[Protocol Config] Using Redstar protocol");
  } else if (sysCfg.protocol == "tatsuno") {
    #define USE_TATSUNO
    #undef USE_REDSTAR
    FMS_LOG_INFO("[Protocol Config] Using Tatsuno protocol");
  } else {
    FMS_LOG_ERROR("[Protocol Config] Unknown protocol, defaulting to Redstar");
    sysCfg.protocol = "0";
  }

  fms_nvs_storage.end();
}

void fms_set_protocol_config(DisConfig& cfg) {
  if (!fms_nvs_storage.begin("fms_d_config", false)) {
    FMS_LOG_ERROR("[Protocol Config] Failed to initialize NVS storage");
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
    FMS_LOG_INFO("[Protocol Config] %s configuration saved successfully", cfg.pt.c_str());
    Serial.printf(
      "Protocol: %s, Device ID: %d, Nozzle count: %d\n"
      "Pump IDs: %d %d %d %d %d %d %d %d\n",
      cfg.pt.c_str(), cfg.devn, cfg.noz,
      cfg.pumpids[0], cfg.pumpids[1], cfg.pumpids[2], cfg.pumpids[3],
      cfg.pumpids[4], cfg.pumpids[5], cfg.pumpids[6], cfg.pumpids[7]
    );
    
    // Update system configuration
    sysCfg.protocol = cfg.pt;
  } else {
    FMS_LOG_ERROR("[Protocol Config] Failed to save some configuration values");
  }
  fms_nvs_storage.end();
}

void fms_load_config() {
  if (!fms_nvs_storage.begin("fms_config", false)) {
    FMS_LOG_ERROR("[fms_main_func:205] Failed to initialize NVS storage");
    return;
  }
  deviceName = fms_nvs_storage.getString("uuid", "ultm_25505v01_");
  FMS_LOG_INFO("[fms_main_func:209] Device UUID: %s", deviceName.c_str());
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
static const unsigned long BLINK_INTERVAL = 500; // 1 second blink interval
static bool mqtt_connected = false;
static bool led_state = false;
static unsigned long previous_millis = 0;

void fms_mqtt_led_update() {
    if (!mqtt_connected) {
        unsigned long current_millis = millis();
        if (current_millis - previous_millis >= BLINK_INTERVAL) {
            previous_millis = current_millis;
            led_state = !led_state;
            gpio_set_level(LED_GREEN, led_state);
        }
    }
}

void fms_mqtt_set_connected(bool connected) {
    mqtt_connected = connected;
    if (connected) {
        gpio_set_level(LED_GREEN, 1); // LED off when connected
    }
}


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
  String willTopicStr = String("device/") + deviceName + "/status";
  const char* willTopic = willTopicStr.c_str();
  const char* willMessage = "offline";
  bool willRetain = true;
  uint8_t willQos = 1;

  while (!fms_mqtt_client.connected()) {
        gpio_set_level(LED_GREEN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_GREEN,1);
        vTaskDelay(pdMS_TO_TICKS(100));
  
    FMS_MQTT_LOG_DEBUG("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str(),sysCfg.mqtt_user,sysCfg.mqtt_password,willTopic,willQos,willRetain,willMessage)) {
       
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
      fms_mqtt_client.publish(willTopic, "online", true);
      fms_subsbribe_topics();
      fms_mqtt_set_connected(true);
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
    fms_nvs_storage.begin("fms_config", false);
    String host = fms_nvs_storage.getString("host");
    String port = fms_nvs_storage.getString("port");
    if(host.length() == 0 || port.length() == 0) {
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(500));
      FMS_LOG_ERROR("[fms_mqtt.ino:79] [DEBUG MQTT] mqtt .. credential .. value is empty");
      fms_nvs_storage.end();
     
    }

  fms_nvs_storage.end();
  FMS_LOG_DEBUG("HOST : %s , PORT : %s", host, port);
  fms_mqtt_client.setServer(host.c_str(), atoi(port.c_str()));
  fms_mqtt_client.setCallback(fms_mqtt_callback);
  while (mqttTask) {
    unsigned long currentMillis = millis();
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
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

/* version 2 ota */
#define USE_V1_OTA_SERVER
#undef  USE_V2_OTA_SERVER
/*

#ifndef USE_V2_OTA_SERVER


#define HTTP_UPLOAD_BUFLEN 8192         // Increased buffer size for better performance
#define WDT_TIMEOUT_S 60                // Increased watchdog timeout for large files
#define PROGRESS_UPDATE_INTERVAL 5      // Update progress every 5%
#define MAX_RETRY_ATTEMPTS 3

// Global variables for better progress tracking
static size_t totalSize = 0;
static size_t writtenSize = 0;
static uint8_t lastReportedProgress = 0;
static unsigned long lastProgressUpdate = 0;
static unsigned long otaStartTime = 0;

void fms_info_response() {
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
  
  // Add network quality indicators
  json.addString("wifiMode", WiFi.getMode() == WIFI_STA ? "STA" : "AP");
  json.addString("channel", String(WiFi.channel()));
  
  // Add OTA timing information if in progress
  if (otaInProgress && otaStartTime > 0) {
    unsigned long elapsed = (millis() - otaStartTime) / 1000;
    json.addLong("otaElapsedSeconds", elapsed);
    if (totalSize > 0 && writtenSize > 0) {
      // Use addInt or addLong instead of addFloat
      json.addLong("otaSpeedBps", (long)(writtenSize / (elapsed > 0 ? elapsed : 1)));
      json.addLong("otaTotalBytes", totalSize);
      json.addLong("otaWrittenBytes", writtenSize);
    }
  }
  
  cachedInfoResponse = json.toString();
}

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

void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");
    
    if (user == correctUsername && pass == correctPassword) {
      isAuthenticated = true;
      server.sendHeader("Set-Cookie", "auth=true; Max-Age=3600; Path=/");
      server.send(200,"text/plain", "Login Successful!");

      //server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Login successful\"}");
    } else {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid credentials\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing credentials\"}");
  }
}

void handleLogout() {
  isAuthenticated = false;
  server.sendHeader("Set-Cookie", "auth=; Max-Age=0; Path=/");
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Logged out successfully\"}");
}

// Improved progress reporting function
void updateOTAProgress() {
  if (totalSize > 0) {
    uint8_t currentProgress = (writtenSize * 100) / totalSize;
    
    // Only update if progress changed significantly or enough time has passed
    if (currentProgress != lastReportedProgress && 
        (currentProgress - lastReportedProgress >= PROGRESS_UPDATE_INTERVAL || 
         millis() - lastProgressUpdate > 2000)) {
      
      otaProgress = currentProgress;
      lastReportedProgress = currentProgress;
      lastProgressUpdate = millis();
      
      FMS_LOG_INFO("OTA Progress: %u%% (%u / %u bytes) - Speed: %.2f KB/s\n",
                   currentProgress,
                   writtenSize,
                   totalSize,
                   (float)(writtenSize * 1000) / (millis() - otaStartTime) / 1024.0);
    }
  }
}

void fms_set_ota_server() {
  FMS_LOG_INFO("[fms_ota_server.ino] OTA server created with enhanced performance");
  
  // Enable CORS with specific headers for better compatibility
  server.enableCORS(true);
  
  const char* cacheControl = "max-age=86400, public";
  server.serveStatic("/", LittleFS, "/login.html", cacheControl);

  server.on("/login", handleLogin);
  server.on("/dashboard",  handleDashboard);


  server.serveStatic("/script.js", LittleFS, "/script.js", cacheControl);

  // Enhanced info API with better caching
  server.on("/api/info", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    if (otaInProgress || millis() - lastInfoRequest > INFO_CACHE_TIME || cachedInfoResponse.length() == 0) {
      fms_info_response();
      lastInfoRequest = millis();
    }
    
    server.sendHeader("Cache-Control", otaInProgress ? "no-cache" : "max-age=5");
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", cachedInfoResponse);
  });

  server.on("/logout", HTTP_POST, handleLogout);
  // Enhanced OTA update handler
  server.on("/api/update", HTTP_POST, 
    []() {
      // Response handler - called after upload completes
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Content-Type", "application/json");
      
      String response;
      if (Update.hasError()) {
        response = "{\"status\":\"error\",\"message\":\"Update failed\"}";
        server.send(500, "application/json", response);
      } else {
        response = "{\"status\":\"success\",\"message\":\"Update completed successfully\"}";
        server.send(200, "application/json", response);
      }

      // Re-enable watchdog with original timeout
      esp_task_wdt_config_t wdtConfig = {
        .timeout_ms = WDT_TIMEOUT_S * 1000,
        .idle_core_mask = 0,
        .trigger_panic = true
      };
      esp_task_wdt_init(&wdtConfig);

      if (!Update.hasError()) {
        FMS_LOG_INFO("OTA Update successful, restarting in 2 seconds...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP.restart();
      }
    },
    []() {
      // Upload handler - processes the actual file upload
      HTTPUpload& upload = server.upload();
      
      if (upload.status == UPLOAD_FILE_START) {
        // Disable watchdog for update process
        esp_task_wdt_deinit();
        
        FMS_LOG_INFO("Starting OTA update: %s", upload.filename.c_str());
        updateStatus = "Initializing update...";
        otaInProgress = true;
        otaProgress = 0;
        lastReportedProgress = 0;
        writtenSize = 0;
        otaStartTime = millis();

        // Get content length from headers
        totalSize = 0;
        if (server.hasHeader("Content-Length")) {
          totalSize = server.header("Content-Length").toInt();
          FMS_LOG_INFO("Expected file size: %u bytes (%.2f MB)", totalSize, totalSize / 1024.0 / 1024.0);
        }

        // Optimize memory before update
        FMS_LOG_INFO("Free heap before update: %d bytes", ESP.getFreeHeap());
        
        // Force garbage collection if available
        #ifdef ESP_IDF_VERSION_MAJOR
        if (ESP_IDF_VERSION_MAJOR >= 4) {
          heap_caps_malloc_extmem_enable(1024);
        }
        #endif
        
        // Start update with proper size
        size_t updateSize = totalSize > 0 ? totalSize : UPDATE_SIZE_UNKNOWN;
        if (!Update.begin(updateSize, U_FLASH)) {
          Update.printError(Serial);
          updateStatus = "Failed to start update";
          otaInProgress = false;
          
          // Re-enable watchdog on error
          esp_task_wdt_config_t wdtConfig = {
            .timeout_ms = WDT_TIMEOUT_S * 1000,
            .idle_core_mask = 0,
            .trigger_panic = true
          };
          esp_task_wdt_init(&wdtConfig);
        } else {
          updateStatus = "Receiving firmware...";
          FMS_LOG_INFO("OTA update started successfully");
        }
        
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Write firmware data
        size_t written = Update.write(upload.buf, upload.currentSize);
        if (written != upload.currentSize) {
          Update.printError(Serial);
          updateStatus = "Write error occurred";
          FMS_LOG_ERROR("Write error: expected %u bytes, wrote %u bytes", upload.currentSize, written);
        } else {
          writtenSize += written;
          updateOTAProgress();
          
          // Fix string concatenation - use snprintf instead
          char progressBuffer[50];
          snprintf(progressBuffer, sizeof(progressBuffer), "Writing firmware... %d%%", otaProgress);
          updateStatus = progressBuffer;
        }
        
        // Yield periodically to prevent watchdog issues
        if (writtenSize % (32 * 1024) == 0) { // Every 32KB
          yield();
          vTaskDelay(1); // Brief delay to allow other tasks
        }
        
      } else if (upload.status == UPLOAD_FILE_END) {
        // Finalize update
        updateStatus = "Finalizing update...";
        
        if (Update.end(true)) {
          FMS_LOG_INFO("OTA Update completed successfully: %u bytes written", writtenSize);
          updateStatus = "Update completed successfully";
          otaProgress = 100;
          
          // Verify the update
          if (Update.isFinished()) {
            FMS_LOG_INFO("Update verification successful");
          } else {
            FMS_LOG_ERROR("Update verification failed");
            updateStatus = "Update verification failed";
          }
        } else {
          Update.printError(Serial);
          updateStatus = "Update finalization failed";
          FMS_LOG_ERROR("Update end failed");
        }
        
        otaInProgress = false;
        
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        otaInProgress = false;
        updateStatus = "Update was aborted";
        FMS_LOG_INFO("OTA Update aborted by user or network error");
        
        // Re-enable watchdog
        esp_task_wdt_config_t wdtConfig = {
          .timeout_ms = WDT_TIMEOUT_S * 1000,
          .idle_core_mask = 0,
          .trigger_panic = true
        };
        esp_task_wdt_init(&wdtConfig);
      }
      
      // Yield to prevent blocking
      yield();
    }
  );

  // Enhanced restart endpoint
  server.on("/api/restart", HTTP_POST, []() {
    if (!isAuthenticated) {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Unauthorized\"}");
      return;
    }
    
    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Restarting device...\"}");
    FMS_LOG_INFO("Manual restart requested");
    vTaskDelay(pdMS_TO_TICKS(1000));
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

static void web_server_task(void* arg) {
  // Enhanced mDNS setup
  String mdnsName = deviceName + String(WiFi.localIP()).c_str();
  mdnsName.replace(":", "");
  
  if (!MDNS.begin(mdnsName.c_str())) {
    Serial.println("[DNS] Error setting up MDNS responder!");
  } else {
    Serial.printf("[DNS] mDNS responder started as %s.local\n", mdnsName.c_str());
    
    // Add services for better discovery
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("esp-ota", "tcp", 80);
    
    // Add service properties if supported by your MDNS implementation
    #ifdef ESP_ARDUINO_VERSION_MAJOR
    if (ESP_ARDUINO_VERSION_MAJOR >= 2) {
      MDNS.addServiceTxt("http", "tcp", "version", firmwareVersion);
      MDNS.addServiceTxt("http", "tcp", "device", deviceName);
    }
    #endif
  }
  
  // Removed setTimeout call as it's not supported
  
  fms_set_ota_server();
  server.begin();
  
  FMS_LOG_INFO("Web server started on http://%s.local or http://%s", 
               mdnsName.c_str(), WiFi.localIP().toString().c_str());

  while (1) {
    server.handleClient();
    
    // Removed MDNS.update() call as it's not supported
    
    // Update uptime counter
    if (millis() - lastUptimeUpdate >= 1000) {
      uptime++;
      lastUptimeUpdate = millis();
    }
    
    // Shorter delay for better responsiveness during OTA
    vTaskDelay(otaInProgress ? pdMS_TO_TICKS(10) : pdMS_TO_TICKS(50));
  }
}

// Optional: Add network optimization function
void optimizeNetworkSettings() {
  // Set WiFi power save mode for better performance during OTA
  WiFi.setSleep(false);
  
  // Increase TCP buffer sizes if possible
  FMS_LOG_INFO("Network optimizations applied");
}
#endif // USE_V2_OTA_SERVER

*/

/* version 1 ota */
#ifdef USE_V1_OTA_SERVER

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

#endif // USE_V1_OTA

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_redstar_fun.ino"

#ifdef USE_REDSTAR

#define FMS_RED_DEBUG_OPEN
#ifdef FMS_RED_DEBUG_OPEN
  #define FMS_RED_LOG_DEBUG(format, ...) Serial.print("[REDSTAR][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_RED_LOG_ERROR(format, ...) Serial.print("[REDSTAR][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_RED_DEBUG(format, ...)
  #define FMS_RED_ERROR(format, ...)
#endif


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
  *  fms_sd.cpp
  *  Created on: 2020. 12. 10.
  *  author : thet htar khaing
*/

/* upgrade in version 2 */
/* not included in version 1 */

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







#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main\\fms_touch_controller.ino"
#ifndef USE_TOUCH

void fms_touch_init() {
eeprom_init();
default_setup_all();
mqtt_topic_change();
screen_brightness_in();

}

/* old version */
void fms_touch_main_loop() {
  if (brightness_check) screen_brightness_out();
    hmi_two_main_fun();
}

void hmi_two_main_fun() {  // for hmi two main function
  Hmi_Two_setup();
  if (rfid_check_two) rfid_setup_two();
  if (rfid_auth_check_two) rfid_auth_two();
}

void Hmi_Two_setup() {

  if (Serial2.available() > 0) {

    Hmi_time = millis() / 1000;  // for brightness time

    for (int i = 0; i < 40; i++) {
      Hmi_Buffer[i] = Serial2.read();
      delay(2);
      // Serial.print(Hmi_Buffer[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println();

    if (Hmi_Buffer[4] == 0x12) {
      rfid_card_check_two = false;

      switch (Hmi_Buffer[8]) {
        case 0x01:
          buzzer_song_two();
          common_pump_Id_two = pumpid1;
          dispenser_ID_two = 1;
          break;

        case 0x02:
          buzzer_song_two();
          common_pump_Id_two = pumpid2;
          dispenser_ID_two = 2;
          break;

        case 0x03:
          buzzer_song_two();
          common_pump_Id_two = pumpid3;
          dispenser_ID_two = 3;

          break;
        case 0x04:
          buzzer_song_two();
          common_pump_Id_two = pumpid4;
          dispenser_ID_two = 4;
          break;

        case 0x05:
          buzzer_song_two();
          common_pump_Id_two = pumpid5;
          dispenser_ID_two = 5;
          break;

        case 0x06:
          buzzer_song_two();
          common_pump_Id_two = pumpid6;
          dispenser_ID_two = 6;
          break;

        case 0x07:
          buzzer_song_two();
          common_pump_Id_two = pumpid7;
          dispenser_ID_two = 7;
          break;

        case 0x08:
          buzzer_song_two();
          common_pump_Id_two = pumpid8;
          dispenser_ID_two = 8;
          break;
      }

      if (casher_lock_check == 1) {
        master_http_login_post_two(master_rfid);

        if (master_check) {
          vehicle_setup_two();
          master_check = false;

        } else {
          Page_change[9] = 0x05;
          Serial2.write(Page_change, 10);
          Serial2.write(Page_change, 10);
        }

      } else rfid_check_two = true;

    } else if (Hmi_Buffer[4] == 0x23 && Hmi_Buffer[5] == 0x00) {  // to read car no
      // Serial.println(" read car no");
      for (int j = 0; j < 15; j++) Car_no_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Car_no_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Car number is => ");
      Serial.println(Car_no_two);

    } else if (Hmi_Buffer[4] == 0x24 && Hmi_Buffer[5] == 0x00) {  // to read member id
      for (int j = 0; j < 20; j++) Member_id_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Member_id_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Member id is => ");
      Serial.println(Member_id_two);

    } else if (Hmi_Buffer[4] == 0x25 && Hmi_Buffer[5] == 0x00) {  // to read member name
      for (int j = 0; j < 20; j++) Member_name_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Member_name_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Member name is => ");
      Serial.println(Member_name_two);

    } else if (Hmi_Buffer[4] == 0x22 && Hmi_Buffer[5] == 0x00) {  // to read vehicle type
      for (int j = 0; j < 4; j++) vehicle_type_two[j] = 0;

      for (int i = 0; i < 4; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        vehicle_type_two[i] = Hmi_Buffer[8 + i];
      }
      // Serial.println(vehicle_type_two);
      vehicle_value_two = vehicle_type_two[0] + 1;
      Serial.print("Hmi two vehicle type is => ");
      Serial.println(vehicle_value_two);

      // vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

    } else if (Hmi_Buffer[4] == 0x21 && Hmi_Buffer[5] == 0x00) {  // to read cash type id
      for (int j = 0; j < 4; j++) cash_type_two[j] = 0;

      for (int i = 0; i < 4; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        cash_type_two[i] = Hmi_Buffer[8 + i];
      }

      cash_value_two = cash_type_two[0] + 1;
      Serial.print("Hmi two Cash type id => ");
      Serial.println(cash_value_two);

    } else if (Hmi_Buffer[4] == 0x32 && Hmi_Buffer[5] == 0x00) {  // for liter calculation and read liter
      Serial.println(" liter count in hmi two");

      Total_check_two = false;
      over_all_li_price_check_two = true;
      liter_check_two = true;
      price_check_two = false;
      preset_check_two = false;

      for (int j = 0; j < 10; j++) Liter_count_two[j] = 0;

      for (int i = 0; i < 10; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        Liter_count_two[i] = Hmi_Buffer[9 + i];
        Serial.print(Liter_count_two[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      combine_Liter_two = (Liter_count_two[0] << 8) | Liter_count_two[1];
      Serial.print("Hmi two Liter count is => ");
      Serial.println(combine_Liter_two);

      liter_calculation_check_two(combine_Liter_two);  // for calculation

    } else if (Hmi_Buffer[4] == 0x33 && Hmi_Buffer[5] == 0x00) {  // to read price data

      over_all_li_price_check_two = true;
      liter_check_two = false;
      price_check_two = true;
      Total_check_two = false;
      preset_check_two = false;

      for (int j = 0; j < 5; j++) Price_count_two[j] = 0;

      for (int i = 0; i < 5; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        Price_count_two[i] = Hmi_Buffer[8 + i];
        Serial.print(Price_count_two[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      combine_price_two = (Price_count_two[0] << 16) | (Price_count_two[1] << 8) | Price_count_two[2];
      Serial.print("Hmi two price count is => ");
      Serial.println(combine_price_two);

      price_calculation_check_two(combine_price_two);

    } else if (Hmi_Buffer[4] == 0x4A && Hmi_Buffer[5] == 0x11) {  // to read F1 liter value
      // Serial.println("f1 read liter in hmi two");
      F1_liter_check_two = true;
      F1_price_check_two = false;
      for (int j = 0; j < 5; j++) F1_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F1_liter_two[i] = Hmi_Buffer[9 + i];
      }
      // Serial.println();

      F1_liter_value_two = (F1_liter_two[0] << 8) | F1_liter_two[1];
      Serial.print("Hmi two F1 liter value is => ");
      Serial.println(F1_liter_value_two);

      Default_preset_price[4] = 0x4A;
      Default_preset_price[5] = 0x21;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4A && Hmi_Buffer[5] == 0x21) {  // to read F1 price
      // Serial.println("f1 read price in hmi two");
      F1_liter_check_two = false;
      F1_price_check_two = true;
      for (int j = 0; j < 8; j++) F1_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F1_price_two[i] = Hmi_Buffer[8 + i];
      }

      F1_price_value_two = (F1_price_two[0] << 16) | (F1_price_two[1] << 8) | F1_price_two[2];
      Serial.print("Hmi two F1 price value is => ");
      Serial.println(F1_price_value_two);

      Default_preset_liter[4] = 0x4A;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4B && Hmi_Buffer[5] == 0x11) {  // to read F2 liter value
      F2_liter_check_two = true;
      F2_price_check_two = false;
      for (int j = 0; j < 5; j++) F2_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F2_liter_two[i] = Hmi_Buffer[9 + i];
      }

      F2_liter_value_two = (F2_liter_two[0] << 8) | F2_liter_two[1];
      Serial.print("Hmi two F2 liter value is => ");
      Serial.println(F2_liter_value_two);

      Default_preset_price[4] = 0x4B;
      Default_preset_price[5] = 0x21;
      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4B && Hmi_Buffer[5] == 0x21) {  // to read F2 price
      Serial.println("f2 read price in hmi two");
      F2_liter_check_two = false;
      F2_price_check_two = true;
      for (int j = 0; j < 8; j++) F2_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F2_price_two[i] = Hmi_Buffer[8 + i];
      }

      F2_price_value_two = (F2_price_two[0] << 16) | (F2_price_two[1] << 8) | F2_price_two[2];

      Serial.print("Hmi two F2 price value is => ");
      Serial.println(F2_price_value_two);

      Default_preset_liter[4] = 0x4B;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4C && Hmi_Buffer[5] == 0x11) {  // to read F3 liter value
      Serial.println("f3 read liter in hmi two");
      F3_liter_check_two = true;
      F3_price_check_two = false;
      for (int j = 0; j < 5; j++) F3_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F3_liter_two[i] = Hmi_Buffer[9 + i];
      }

      F3_liter_value_two = (F3_liter_two[0] << 8) | F3_liter_two[1];
      Serial.print("Hmi two F3 liter value is => ");
      Serial.println(F3_liter_value_two);

      Default_preset_price[4] = 0x4C;
      Default_preset_price[5] = 0x31;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4C && Hmi_Buffer[5] == 0x31) {  // to read F3 price
      Serial.println("f3 read price in hmi two");
      F3_liter_check_two = false;
      F3_price_check_two = true;
      for (int j = 0; j < 8; j++) F3_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F3_price_two[i] = Hmi_Buffer[8 + i];
      }

      F3_price_value_two = (F3_price_two[0] << 16) | (F3_price_two[1] << 8) | F3_price_two[2];
      Serial.print("Hmi two F3 price value is => ");
      Serial.println(F3_price_value_two);

      Default_preset_liter[4] = 0x4C;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x4D && Hmi_Buffer[5] == 0x11) {  // to read F4 liter value
      Serial.println("f4 read liter in hmi two");
      F4_liter_check_two = true;
      F4_price_check_two = false;
      for (int j = 0; j < 5; j++) F4_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F4_liter_two[i] = Hmi_Buffer[9 + i];
        // Serial.print(F4_liter_two[i], HEX);
        // Serial.print(" ");
      }
      // Serial.println();

      F4_liter_value_two = (F4_liter_two[0] << 8) | F4_liter_two[1];
      Serial.print("Hmi two F4 liter value is => ");
      Serial.println(F4_liter_value_two);

      Default_preset_price[4] = 0x4D;
      Default_preset_price[5] = 0x41;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4D && Hmi_Buffer[5] == 0x41) {  // to read F4 price
      Serial.println("f4 read price in hmi two");
      F4_liter_check_two = false;
      F4_price_check_two = true;
      for (int j = 0; j < 8; j++) F4_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F4_price_two[i] = Hmi_Buffer[8 + i];
      }

      F4_price_value_two = (F4_price_two[0] << 16) | (F4_price_two[1] << 8) | F4_price_two[2];
      Serial.print("Hmi two F4 price value is => ");
      Serial.println(F4_price_value_two);

      Default_preset_liter[4] = 0x4D;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x31) {  // to store eeprom F1 value
      Serial.println("F1 save function in hmi two");
      buzzer_song_two();
      if (F1_liter_check_two) {
        F1_price_value_two = 0;
        eeprom_writeInt(150, F1_liter_value_two);
        eeprom_writeInt(155, F1_price_value_two);

        EEPROM.write(154, 1);
        EEPROM.commit();

        F1_liter_check_two = false;
        F1_price_check_two = false;
      }

      if (F1_price_check_two) {
        F1_liter_value_two = 0;
        eeprom_writeInt(150, F1_liter_value_two);
        eeprom_writeInt(155, F1_price_value_two);
        EEPROM.write(154, 2);
        EEPROM.commit();

        F1_price_check_two = false;
        F1_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x32) {  // to store eeprom F2 value
      Serial.println("F2 save function in hmi two");
      buzzer_song_two();
      if (F2_liter_check_two) {
        F2_price_value_two = 0;
        eeprom_writeInt(160, F2_liter_value_two);
        eeprom_writeInt(165, F2_price_value_two);
        EEPROM.write(184, 1);
        EEPROM.commit();

        F2_liter_check_two = false;
        F2_price_check_two = false;
      }

      if (F2_price_check_two) {
        F2_liter_value_two = 0;
        eeprom_writeInt(160, F2_liter_value_two);
        eeprom_writeInt(165, F2_price_value_two);
        EEPROM.write(164, 2);
        EEPROM.commit();

        F2_price_check_two = false;
        F2_liter_check_two = false;
      }
    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x33) {  // to store eeprom F3 value
      Serial.println("F3 save function in hmi two");
      buzzer_song_two();
      if (F3_liter_check_two) {
        F3_price_value_two = 0;
        eeprom_writeInt(170, F3_liter_value_two);
        eeprom_writeInt(175, F3_price_value_two);
        EEPROM.write(174, 1);
        EEPROM.commit();

        F3_liter_check_two = false;
        F3_price_check_two = false;
      }

      if (F3_price_check_two) {
        F3_liter_value_two = 0;
        eeprom_writeInt(170, F3_liter_value_two);
        eeprom_writeInt(175, F3_price_value_two);
        EEPROM.write(174, 2);
        EEPROM.commit();

        F3_price_check_two = false;
        F3_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x34) {  // to store eeprom F4 value
      Serial.println("F4 save function in hmi two");
      buzzer_song_two();
      if (F4_liter_check_two) {
        F4_price_value_two = 0;
        eeprom_writeInt(180, F4_liter_value_two);
        eeprom_writeInt(185, F4_price_value_two);
        EEPROM.write(184, 1);
        EEPROM.commit();

        F4_liter_check_two = false;
        F4_price_check_two = false;
      }

      if (F4_price_check_two) {
        F4_liter_value_two = 0;
        eeprom_writeInt(180, F4_liter_value_two);
        eeprom_writeInt(185, F4_price_value_two);
        EEPROM.write(184, 2);
        EEPROM.commit();

        F4_price_check_two = false;
        F4_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // F1 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F1_return_liter_two = eeprom_read_data(150);
      int F1_return_price_two = eeprom_read_data(155);
      Preset_total_eep_check_two = EEPROM.read(154);

      Serial.print("Hmi two F1 liter or price is ===> ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F1 return liter value is => ");
      Serial.println(F1_return_liter_two);

      Serial.print("Hmi two F1 return price value is => ");
      Serial.println(F1_return_price_two);

      Total_check_two = true;
      if (F1_return_liter_two > 0) {
        liter_calculation_check_two(F1_return_liter_two);
      }

      if (F1_return_price_two > 1) {
        price_calculation_check_two(F1_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x02) {  // F2 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F2_return_liter_two = eeprom_read_data(160);
      int F2_return_price_two = eeprom_read_data(165);
      Preset_total_eep_check_two = EEPROM.read(164);

      Serial.print("Hmi two F2 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F2 return liter value is => ");
      Serial.println(F2_return_liter_two);

      Serial.print("Hmi twoF2 return price value is => ");
      Serial.println(F2_return_price_two);

      Total_check_two = true;
      if (F2_return_liter_two > 0) {
        liter_calculation_check_two(F2_return_liter_two);
      }

      if (F2_return_price_two > 1) {
        price_calculation_check_two(F2_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x03) {  // F3 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F3_return_liter_two = eeprom_read_data(170);
      int F3_return_price_two = eeprom_read_data(175);
      Preset_total_eep_check_two = EEPROM.read(174);

      Serial.print("Hmi two F3 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F3 return liter value is => ");
      Serial.println(F3_return_liter_two);

      Serial.print("Hmi two F3 return price value is => ");
      Serial.println(F3_return_price_two);

      Total_check_two = true;
      if (F3_return_liter_two > 0) {
        liter_calculation_check_two(F3_return_liter_two);
      }

      if (F3_return_price_two > 1) {
        price_calculation_check_two(F3_return_price_two);
      }
    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x04) {  // F4 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F4_return_liter_two = eeprom_read_data(180);
      int F4_return_price_two = eeprom_read_data(185);
      Preset_total_eep_check_two = EEPROM.read(184);

      Serial.print("Hmi two F4 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F4 return liter value is => ");
      Serial.println(F4_return_liter_two);

      Serial.print("Hmi two F4 return price value is => ");
      Serial.println(F4_return_price_two);

      Total_check_two = true;
      if (F4_return_liter_two > 0) {
        liter_calculation_check_two(F4_return_liter_two);
      }

      if (F4_return_price_two > 1) {
        price_calculation_check_two(F4_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x61 && Hmi_Buffer[5] == 0x00) {  // pin code auth two

      for (int j = 0; j < 5; j++) PIN_code_two[j] = 0;
      for (int i = 0; i < 5; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        PIN_code_two[i] = Hmi_Buffer[8 + i];
      }

      PIN_CODE_two = (PIN_code_two[0] << 16) | (PIN_code_two[1] << 8) | PIN_code_two[2];
      Serial.print("pin code two is => ");
      Serial.println(PIN_CODE_two);

      PIN_Auth_two();

    } else if (Hmi_Buffer[4] == 0x53 && Hmi_Buffer[5] == 0x00) {  // to read wifi password in hmi two
      Serial.println("Wifi pass read in hmi two");
      for (int j = 0; j < 40; j++) PASS_buffer[j] = 0;
      for (int i = 0; i < 40; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        PASS_buffer[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Wifi pass is => ");
      Serial.println(PASS_buffer);

    } else if (Hmi_Buffer[4] == 0x52 && Hmi_Buffer[5] == 0x00) {  // to read wifi ssid
      Serial.println("wifi ssid read in hmi two");
      for (int j = 0; j < 40; j++) SSID_buffer[j] = 0;

      for (int i = 0; i < 40; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        SSID_buffer[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      // Serial.println();

      Serial.print("Hmi two Wifi ssid is => ");
      Serial.println(SSID_buffer);

    } else if (Hmi_Buffer[4] == 0x50 && Hmi_Buffer[6] == 0x01) {  // wifi Connect
      Serial.println("wifi connect function in Hmi two");
      buzzer_song_two();

      Page_change[9] = 0X10;

      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

      isconnected = true;

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x05) {  // when press reset vehicle info default
      buzzer_song_two();
      vehicle_setup_two();

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x06) {  // when press clear liter and price default
      buzzer_song_two();
      clear_setup_two();

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x07 && route_protect) {  // when press start check liter or price press
      Serial.println("STart in Hmi two");
      buzzer_song_two();

      Fuel_type_name_two(dispenser_ID_two);

      cash_type_name_two(cash_value_two);
      vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

      if (over_all_li_price_check_two) {
        if (liter_check_two) {
          Serial.println("Hmi two Liter function Active.....");
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total liter is => ");
          Serial.println(combine_Liter_two);

          server_post_vehicle_info_two();

          price_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (price_check_two) {
          Serial.println("Hmi two Price function Active ......");
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total price is => ");
          Serial.println(combine_price_two);

          server_post_vehicle_info_two();

          liter_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 1) {
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total liter is => ");
          Serial.println(Preset_eep_total_liter_two);

          server_post_vehicle_info_two();

          Preset_total_eep_check_two = 0;
          Preset_eep_total_liter_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 2) {
          Serial.println("Preset price function  Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(Preset_eep_total_price_two);

          server_post_vehicle_info_two();

          Preset_total_eep_check_two = 0;
          Preset_eep_total_price_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

      } else {
        Serial.println("Price and Liter function is InActive in start..!");

        server_post_vehicle_info_two();

        Preset_total_eep_check_two = 0;
        Preset_eep_total_price_two = 0;
        Preset_eep_total_liter_two = 0;

        liter_check_two = false;
        price_check_two = false;
        preset_check_two = false;
        over_all_li_price_check_two = false;

        Information_page_show_two();
      }

      route_protect = false;

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x08 && route_protect) {  // when press Preset check liter or price press
      Serial.println("Preset");
      buzzer_song_two();

      Fuel_type_name_two(dispenser_ID_two);

      cash_type_name_two(cash_value_two);
      vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

      if (over_all_li_price_check_two) {
        if (liter_check_two) {
          Serial.println("Liter function Active.....");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print(" press liter is => ");
          Serial.println(combine_Liter_two);

          server_post_preset_two("liter", combine_Liter_two);

          price_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (price_check_two) {
          Serial.println("Price Funtion Active......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(combine_price_two);

          server_post_preset_two("price", combine_price_two);

          liter_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 1) {
          Serial.println("Preset liter function Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total liter is => ");
          Serial.println(Preset_eep_total_liter_two);

          server_post_preset_two("liter", Preset_eep_total_liter_two);

          Preset_total_eep_check_two = 0;
          Preset_eep_total_liter_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 2) {
          Serial.println("Preset price function  Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(Preset_eep_total_price_two);

          server_post_preset_two("price", Preset_eep_total_price_two);

          Preset_total_eep_check_two = 0;
          Preset_eep_total_price_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

      } else {
        Serial.println("Price and Liter function is InActive..!");

        int ab = 0;
        server_post_preset_two("liter", ab);

        Preset_total_eep_check_two = 0;
        Preset_eep_total_price_two = 0;
        Preset_eep_total_liter_two = 0;

        liter_check_two = false;
        price_check_two = false;
        preset_check_two = false;
        over_all_li_price_check_two = false;

        Information_page_show_two();
      }
      Information_page_show_two();

      route_protect = false;

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x10) {  // screen brightness
      Serial.println("Screen brightness");
      buzzer_song_two();
      screen_brightness_in();

    } else if (Hmi_Buffer[4] == 0x41 && Hmi_Buffer[6] == 0x01) {  // page chane in Done key
      Serial.println("Done key");
      buzzer_song_two();

      Page_change[9] = 0x10;

      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

    } else if (Hmi_Buffer[4] == 0x10 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x07) {
      Serial.println("Reset Button");
      buzzer_song_two();
      String dvc_id = "";
      if (devicenum < 10) dvc_id = "0" + String(devicenum);
      else dvc_id = String(devicenum);
      client.publish(Reset_topic, dvc_id.c_str());
      delay(5);
      ESP.restart();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // security key
      Serial.println("security preset key in hmi two");
      buzzer_song_two();
      casher_lock_check = EEPROM.read(90);
      Serial.print("security key is => ");
      Serial.println(casher_lock_check);

      if (casher_lock_check == 1) {
        Show_casher_lock[7] = 0x06;
        Serial2.write(Show_casher_lock, 8);
        Serial2.write(Show_casher_lock, 8);

      } else if (casher_lock_check == 0) {
        Show_casher_lock[7] = 0x05;
        Serial2.write(Show_casher_lock, 8);
        Serial2.write(Show_casher_lock, 8);
      }

    } else if (Hmi_Buffer[4] == 0x42 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x06) {  // casher unlock
      Serial.println("chasher is unlcok in hmi two");
      buzzer_song_two();
      casher_lock_check = 1;
      EEPROM.write(90, casher_lock_check);
      EEPROM.commit();
      http_login_post_two(master_rfid);

    } else if (Hmi_Buffer[4] == 0x42 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x05) {  // casher lock
      Serial.println("chasher is lcok in hmi two");
      buzzer_song_two();
      casher_lock_check = 0;
      EEPROM.write(90, casher_lock_check);
      EEPROM.commit();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // setting key
      Serial.println("setting key in hmi two");
      buzzer_song_two();

      // Page_change[9] = 0x18;
      // Serial2.write(Page_change, 10);
      // Serial2.write(Page_change, 10);

      Serial2.write(Default_pin_code, 10);
      Serial2.write(Default_pin_code, 10);

      Serial2.write(Clean_wifi_pass, 8);
      Serial2.write(Clean_wifi_pass, 8);

      Serial2.write(Default_wifi_ssid, 18);
      delay(5);
      Serial2.write(Default_wifi_ssid, 18);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x20 && Hmi_Buffer[8] == 0x01) {  // member card
      Serial.println("member card in hmi two");
      buzzer_song_two();
      rfid_card_check_two = true;
      rfid_check_two = true;

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // lock dispenser
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x04) {  // price setting
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x01) {  // member info page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x02) {  // casher info
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x03) {  // credit balance page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x04) {  // Device info page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[6] == 0x01 && Hmi_Buffer[8] == 0x02) {  // to write fun preset data in eeprom
      Serial.println("funtion preset pressed for hmi two");
      F1_liter_check_two = false;
      F1_price_check_two = false;
      buzzer_song_two();

      preset_eeprom_value();
      preset_rewrite_price_two();
      preset_rewrite_liter_two();

      Serial2.write(Clean_wifi_pass, 8);
      Serial2.write(Clean_wifi_pass, 8);

      Serial2.write(Default_wifi_ssid, 18);
      delay(5);
      Serial2.write(Default_wifi_ssid, 18);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x41 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01) {  // function preset page done key
      Serial.println("Done key in");
      buzzer_song_two();
      Information_page_show_two();

    } else if (Hmi_Buffer[4] == 0x43 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01) {  // done key in security
      Serial.println("Done key in security");
      buzzer_song_two();
      Information_page_show_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x01) {  // for to reset wifi default
      buzzer_song_two();
      Serial.println("wifi setting");

    } else if (Hmi_Buffer[4] == 0x9F && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // page change in OK key
      Serial.println("ok key in hmi two");
      route_protect = true;
      rfid_check_two = false;
      rfid_auth_check_two = false;

      buzzer_song_two();
      clear_setup_two();
      page_check_fun_two(nozzlenum);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x01) {  // page change in Cancel key
      Serial.println("cancle key");
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();
      page_check_fun_two(nozzlenum);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // page change in next key in vehicle info page
      Serial.println("next presssed in hmi two");
      buzzer_song_two();
      Preset_setup_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x04) {  // member card page back key
      Serial.println("back key in member card");
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();

      Page_change[9] = 0X04;
      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01 && Hmi_Buffer[8] == 0x00) {  // page change in back key
      Serial.println("back key in hmi two");
      route_protect = true;
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();
      clear_setup_two();
      page_check_fun_two(nozzlenum);
    }
  }
}

void master_http_login_post_two(String rfid_data) {  // when access rfid roken is receive

  HTTPClient http;

  Serial.print("rfid Id is => ");
  Serial.println(rfid_data);
  String serverAddress = "http://192.168.0.100:9000/api/user/cardAuth";

  http.begin(serverAddress);
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cardId\"\r\n\r\n";
  formData += rfid_data + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);


  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();

    // Parse the JSON response
    // Serial.print("Response is => ");
    // Serial.println(response);
    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    master_check = doc["con"];
    String token = doc["result"]["token"];

    accessToken_two = "";
    accessToken_two = token;

    // Print the token
    Serial.print("rfid condition : ");
    Serial.println(rfid_condition);

    Serial.print("Token: ");
    Serial.println(accessToken_two);

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void http_login_post_two(String rfid_data) {  // when access rfid roken is receive

  HTTPClient http;

  Serial.print("rfid Id is => ");
  Serial.println(rfid_data);
  String serverAddress = "http://192.168.0.100:9000/api/user/cardAuth";

  http.begin(serverAddress);
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cardId\"\r\n\r\n";
  formData += rfid_data + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();

    // Parse the JSON response
    // Serial.print("Response is => ");
    // Serial.println(response);
    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String token = doc["result"]["token"];

    accessToken_two = "";
    accessToken_two = token;

    // Print the token
    Serial.print("rfid condition : ");
    Serial.println(rfid_condition);

    Serial.print("Token: ");
    Serial.println(accessToken_two);

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void server_post_vehicle_info_two() {  /// for http post for vehicle info

  String nozzleNo = "";
  if (common_pump_Id_two < 10) nozzleNo = "0" + String(common_pump_Id_two);
  else nozzleNo = String(common_pump_Id_two);

  String Dev_ID = String(devicenum);


  String serverAddress = "http://192.168.0.100:9000/api/detail-sale?depNo=" + Dev_ID + "&nozzleNo=" + nozzleNo;

  // Serial.print("server Address two => ");
  // Serial.println(serverAddress);

  String carNo;
  if (strlen(Car_no_two) == 0) carNo = "-";
  else carNo = String(Car_no_two);

  String member_id;
  if (strlen(Member_id_two) == 0) member_id = "-";
  else member_id = String(Member_id_two);

  String member_name;
  if (strlen(Member_name_two) == 0) member_name = "-";
  else member_name = String(Member_name_two);

  String vehicleType = vehicle_name_two;
  String fuelType = Nozzle_name_two;
  String cashType = cash_name_two;
  String device = "HMI";

  HTTPClient http;

  http.begin(serverAddress);

  http.addHeader("Authorization", "Bearer " + String(accessToken_two));

  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"nozzleNo\"\r\n\r\n";
  formData += nozzleNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"vehicleType\"\r\n\r\n";
  formData += vehicleType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"carNo\"\r\n\r\n";
  formData += carNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cashType\"\r\n\r\n";
  formData += cashType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"fuelType\"\r\n\r\n";
  formData += fuelType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  formData += device + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String Server_reply = doc["msg"];

    Serial.print("server condi  is => ");
    Serial.println(rfid_condition);

    Serial.print("server reply from start is => ");
    Serial.println(Server_reply);

    if (rfid_condition) {
      char my_chr[30];
      Server_reply.toCharArray(my_chr, Server_reply.length() + 1);

      Serial2.write(Show_server_reply, 6);
      Serial2.write(Show_server_data, 20);

      Serial2.write(Show_infor, 6);
      Serial2.write(my_chr, 30);

    } else {

      Serial2.write(Show_server_Error, 21);
      Serial2.write(Show_server_Error, 21);
    }

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  Serial.println("http post is complete");
  accessToken_two = "";
}

void server_post_preset_two(String check_data, int main_data) {

  String nozzleNo = "";
  if (common_pump_Id_two < 10) nozzleNo = "0" + String(common_pump_Id_two);
  else nozzleNo = String(common_pump_Id_two);

  String Dev_ID = String(devicenum);
  String serverAddress = "http://192.168.0.100:9000/api/detail-sale/preset?depNo=" + Dev_ID + "&nozzleNo=" + nozzleNo;

  String carNo;
  if (strlen(Car_no_two) == 0) carNo = "-";
  else carNo = String(Car_no_two);

  String member_id;
  if (strlen(Member_id_two) == 0) member_id = "-";
  else member_id = String(Member_id_two);

  String member_name;
  if (strlen(Member_name_two) == 0) member_name = "-";
  else member_name = String(Member_name_two);

  String price_dd = "";
  String liter_data = "";
  if (check_data == "price") {
    price_dd = String(main_data);

  } else if (check_data == "liter") {
    float liter_dd = main_data + 0.0;
    liter_data = String(liter_dd);
  }

  String fuelType = Nozzle_name_two;
  String salePrice = String(common_dispenser_price_two);
  String vehicleType = vehicle_name_two;
  String cashType = cash_name_two;
  String device = "HMI";

  Serial.print("Server address is => ");
  Serial.println(serverAddress);

  Serial.print("price or liter => ");
  Serial.println(check_data);

  Serial.print("float vlaue is => ");
  Serial.println(liter_data);

  Serial.print("Price vlaue is => ");
  Serial.println(price_dd);

  Serial.print("sale price => ");
  Serial.println(salePrice);

  HTTPClient http;

  http.begin(serverAddress);

  // Set authorization header with the token
  http.addHeader("Authorization", "Bearer " + String(accessToken_two));

  // Create the multipart/form-data payload
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"nozzleNo\"\r\n\r\n";
  formData += nozzleNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"vehicleType\"\r\n\r\n";
  formData += vehicleType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"salePrice\"\r\n\r\n";
  formData += salePrice + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"carNo\"\r\n\r\n";
  formData += carNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cashType\"\r\n\r\n";
  formData += cashType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"fuelType\"\r\n\r\n";
  formData += fuelType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  if (check_data == "price") {
    formData += "Content-Disposition: form-data; name=\"kyat\"\r\n\r\n";
    formData += price_dd + "\r\n";
    formData += "--" + formDataBoundary + "\r\n";
  } else if (check_data == "liter") {
    formData += "Content-Disposition: form-data; name=\"liter\"\r\n\r\n";
    formData += liter_data + "\r\n";
    formData += "--" + formDataBoundary + "\r\n";
  }
  formData += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  formData += device + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  // Set content type for form data
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print the response payload
    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String Server_reply = doc["msg"];

    Serial.print("server condi  is => ");
    Serial.println(rfid_condition);

    Serial.print("server reply is => ");
    Serial.println(Server_reply);

    if (rfid_condition) {
      char my_chr[30];
      Server_reply.toCharArray(my_chr, Server_reply.length() + 1);

      Serial2.write(Show_server_reply, 6);
      Serial2.write(Show_server_data, 20);

      Serial2.write(Show_infor, 6);
      Serial2.write(my_chr, 30);

    } else {

      Serial2.write(Show_server_Error, 21);
      Serial2.write(Show_server_Error, 21);
    }

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  // Close the connection
  http.end();
  Serial.println("http post is complete");
  accessToken_two = "";
}

void Fuel_type_name_two(int Fu_Id) {  // to get nozzle name and send server

  Nozzle_name_two = "";
  switch (Fu_Id) {
    case 1:
      Nozzle_name_two = String(Fuel_Type_1);
      Serial.print("fuel type 1 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 2:
      Nozzle_name_two = String(Fuel_Type_2);
      Serial.print("fuel type 2 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 3:
      Nozzle_name_two = String(Fuel_Type_3);
      Serial.print("fuel type 3 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 4:
      Nozzle_name_two = String(Fuel_Type_4);
      Serial.print("fuel type 4 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 5:
      Nozzle_name_two = String(Fuel_Type_5);
      break;
    case 6:
      Nozzle_name_two = String(Fuel_Type_6);
      break;
    case 7:
      Nozzle_name_two = String(Fuel_Type_7);
      break;
    case 8:
      Nozzle_name_two = String(Fuel_Type_8);
      break;
    default:
      Nozzle_name_two = "-";
      break;
  }
}

void vehicle_type_name_two(int type_id) {  // to get vehicle name and send server
  Serial.println("Vehicle type choose in Hmi two");
  vehicle_name_two = "";
  switch (type_id) {
    case 1:
      vehicle_name_two = "Cycle";
      break;
    case 2:
      vehicle_name_two = "Cycle ( 3 Wheel )";
      break;
    case 3:
      vehicle_name_two = "Car";
      break;
    case 4:
      vehicle_name_two = "Bus ( City )";
      break;
    case 5:
      vehicle_name_two = "Light Truck ( City )";
      break;
    case 6:
      vehicle_name_two = "Light Truck ( High way )";
      break;
    case 7:
      vehicle_name_two = "Heavy Truck ( City )";
      break;
    case 8:
      vehicle_name_two = "Heavy Truck ( High way )";
      break;
    case 9:
      vehicle_name_two = "Trailer ( City )";
      break;
    case 10:
      vehicle_name_two = "Trailer ( High way )";
      break;
    case 11:
      vehicle_name_two = "Htawlargyi";
      break;
    case 12:
      vehicle_name_two = "Tractor";
      break;
    case 13:
      vehicle_name_two = "Small Tractor";
      break;
    case 14:
      vehicle_name_two = "Heavy Machinery";
      break;
    case 15:
      vehicle_name_two = "Commercial Vehicle";
      break;
    case 16:
      vehicle_name_two = "Phone Tower";
      break;
    case 17:
      vehicle_name_two = "Industrial Zone";
      break;
    case 18:
      vehicle_name_two = "Generator Industry";
      break;
    case 19:
      vehicle_name_two = "Agriculture Machine";
      break;
    case 20:
      vehicle_name_two = "Generator ( Home Use )";
      break;
    case 21:
      vehicle_name_two = "Hospital";
      break;
    case 22:
      vehicle_name_two = "School";
      break;
    case 23:
      vehicle_name_two = "Super Market";
      break;
    case 24:
      vehicle_name_two = "Hotel";
      break;
    case 25:
      vehicle_name_two = "Housing";
      break;
    case 26:
      vehicle_name_two = "Boat";
      break;
    case 27:
      vehicle_name_two = "Pump Test";
      break;
    case 28:
      vehicle_name_two = "Office Use ( Bowser Car )";
      break;
    case 29:
      vehicle_name_two = "Station Use";
      break;
    default:
      vehicle_name_two = "-";
      break;
  }
}

void cash_type_name_two(int type_id) {  // to get cash name and send server
  cash_name_two = "";
  // Serial.println("cash name in Hmi two");
  switch (type_id) {
    case 1:
      cash_name_two = "Cash";
      break;
    case 2:
      cash_name_two = "KBZ Pay";
      break;
    case 3:
      cash_name_two = "Credit Card";
      break;
    case 4:
      cash_name_two = "Member Card";
      break;
    case 5:
      cash_name_two = "KBZ Banking";
      break;
    case 6:
      cash_name_two = "AYA Pay";
      break;
    case 7:
      cash_name_two = "AYA Banking";
      break;
    case 8:
      cash_name_two = "CB Banking";
      break;
    case 9:
      cash_name_two = "Other";
      break;
    default:
      cash_name_two = "-";
      break;
  }
}

void price_calculation_check_two(int price_data) {  // if price data input liter calculation
  Serial.println("price calculation check function in Hmi two");
  Preset_eep_total_price_two = price_data;  // for return when press reset and start

  switch (dispenser_ID_two) {
    case 1:
      Serial.print("dispendser one price is => ");
      Serial.println(dispenser_1_price);
      price_calculation_two(price_data, dispenser_1_price);
      break;

    case 2:
      price_calculation_two(price_data, dispenser_2_price);
      break;

    case 3:
      price_calculation_two(price_data, dispenser_3_price);
      break;

    case 4:
      price_calculation_two(price_data, dispenser_4_price);
      break;

    case 5:
      price_calculation_two(price_data, dispenser_5_price);
      break;

    case 6:
      price_calculation_two(price_data, dispenser_6_price);
      break;

    case 7:
      price_calculation_two(price_data, dispenser_7_price);
      break;

    case 8:
      price_calculation_two(price_data, dispenser_8_price);
      break;
  }
}

void price_calculation_two(int price_total_data, int price_data_dispenser) {
  common_dispenser_price_two = price_data_dispenser;

  if (price_data_dispenser > 0 && !Total_check_two) {
    Serial.println("price total check is false in Hmi two");
    int total_liter_two = price_total_data / price_data_dispenser;

    Total_check_two = false;

    Show_liter[7] = (total_liter_two >> 16) & 0xff;
    Show_liter[8] = (total_liter_two >> 8) & 0xff;
    Show_liter[9] = total_liter_two & 0xff;


    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);
  }

  if (price_data_dispenser > 0 && Total_check_two) {

    Serial.println("price total check is true in Hmi two");
    int total_liter_two = price_total_data / price_data_dispenser;

    Total_check_two = false;

    Show_liter[7] = (total_liter_two >> 16) & 0xff;
    Show_liter[8] = (total_liter_two >> 8) & 0xff;
    Show_liter[9] = total_liter_two & 0xff;

    Show_price[7] = (price_total_data >> 16) & 0xff;
    Show_price[8] = (price_total_data >> 8) & 0xff;
    Show_price[9] = price_total_data & 0xff;

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);

    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);
  }
}

void liter_calculation_check_two(int liter_data) {  //if liter input each dispenser price calculation
  Serial.println("liter calculation check function in Hmi two");
  Preset_eep_total_liter_two = liter_data;

  switch (dispenser_ID_two) {
    case 1:
      liter_calculation_two(liter_data, dispenser_1_price);
      break;

    case 2:
      liter_calculation_two(liter_data, dispenser_2_price);
      break;

    case 3:
      liter_calculation_two(liter_data, dispenser_3_price);
      break;

    case 4:
      liter_calculation_two(liter_data, dispenser_4_price);
      break;

    case 5:
      liter_calculation_two(liter_data, dispenser_5_price);
      break;

    case 6:
      liter_calculation_two(liter_data, dispenser_6_price);
      break;

    case 7:
      liter_calculation_two(liter_data, dispenser_7_price);
      break;

    case 8:
      liter_calculation_two(liter_data, dispenser_8_price);
      break;
  }
}

void liter_calculation_two(int liter_total_data, int price_data) {  // to show total price if input liter

  common_dispenser_price_two = price_data;

  if (price_data > 0 && !Total_check_two) {
    Serial.println("liter total check is false in Hmi two");
    int total_price_two = liter_total_data * price_data;

    Total_check_two = false;

    Show_price[7] = (total_price_two >> 16) & 0xff;
    Show_price[8] = (total_price_two >> 8) & 0xff;
    Show_price[9] = total_price_two & 0xff;

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);
  }

  if (price_data > 0 && Total_check_two) {
    Serial.println("liter total check is true in Hmi two");

    int total_price_two = liter_total_data * price_data;

    Total_check_two = false;

    Show_price[7] = (total_price_two >> 16) & 0xff;
    Show_price[8] = (total_price_two >> 8) & 0xff;
    Show_price[9] = total_price_two & 0xff;

    Show_liter[7] = (liter_total_data >> 16) & 0xff;
    Show_liter[8] = (liter_total_data >> 8) & 0xff;
    Show_liter[9] = liter_total_data & 0xff;

    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);
  }
}

void preset_rewrite_price_two() {

  Show_F1_price_two[7] = (F1_eep_price_value >> 16) & 0xff;
  Show_F1_price_two[8] = (F1_eep_price_value >> 8) & 0xff;
  Show_F1_price_two[9] = F1_eep_price_value & 0xff;

  Serial2.write(Show_F1_price_two, 10);
  delay(10);
  Serial2.write(Show_F1_price_two, 10);
  delay(10);

  Show_F2_price_two[7] = (F2_eep_price_value >> 16) & 0xff;
  Show_F2_price_two[8] = (F2_eep_price_value >> 8) & 0xff;
  Show_F2_price_two[9] = F2_eep_price_value & 0xff;

  Serial2.write(Show_F2_price_two, 10);
  delay(10);
  Serial2.write(Show_F2_price_two, 10);
  delay(10);

  Show_F3_price_two[7] = (F3_eep_price_value >> 16) & 0xff;
  Show_F3_price_two[8] = (F3_eep_price_value >> 8) & 0xff;
  Show_F3_price_two[9] = F3_eep_price_value & 0xff;

  Serial2.write(Show_F3_price_two, 10);
  delay(10);
  Serial2.write(Show_F3_price_two, 10);
  delay(10);

  Show_F4_price_two[7] = (F4_eep_price_value >> 16) & 0xff;
  Show_F4_price_two[8] = (F4_eep_price_value >> 8) & 0xff;
  Show_F4_price_two[9] = F4_eep_price_value & 0xff;

  Serial2.write(Show_F4_price_two, 10);
  delay(10);
  Serial2.write(Show_F4_price_two, 10);
  delay(10);

  Serial.println("Rewrite price fun complete in Hmi two....!");
}

void preset_rewrite_liter_two() {
  Serial.println("rewrite reset function liter in Hmi two");
  Show_F1_liter_two[7] = (F1_eep_liter_value >> 16) & 0xff;
  Show_F1_liter_two[8] = (F1_eep_liter_value >> 8) & 0xff;
  Show_F1_liter_two[9] = F1_eep_liter_value & 0xff;

  Serial2.write(Show_F1_liter_two, 10);
  Serial2.write(Show_F1_liter_two, 10);

  Show_F2_liter_two[7] = (F2_eep_liter_value >> 16) & 0xff;
  Show_F2_liter_two[8] = (F2_eep_liter_value >> 8) & 0xff;
  Show_F2_liter_two[9] = F2_eep_liter_value & 0xff;

  Serial2.write(Show_F2_liter_two, 10);
  delay(50);
  Serial2.write(Show_F2_liter_two, 10);
  delay(50);

  Show_F3_liter_two[7] = (F3_eep_liter_value >> 16) & 0xff;
  Show_F3_liter_two[8] = (F3_eep_liter_value >> 8) & 0xff;
  Show_F3_liter_two[9] = F3_eep_liter_value & 0xff;

  Serial2.write(Show_F3_liter_two, 10);
  delay(50);
  Serial2.write(Show_F3_liter_two, 10);
  delay(50);

  Show_F4_liter_two[7] = (F4_eep_liter_value >> 16) & 0xff;
  Show_F4_liter_two[8] = (F4_eep_liter_value >> 8) & 0xff;
  Show_F4_liter_two[9] = F4_eep_liter_value & 0xff;

  Serial2.write(Show_F4_liter_two, 10);
  delay(50);
  Serial2.write(Show_F4_liter_two, 10);
  delay(50);

  Serial.println("Rewrite liter fun complete in Hmi two....!");
}

void clear_setup_two() {  // to clear data in preset
  route_protect = true;
  combine_Liter_two = 0;
  combine_price_two = 0;
  Serial2.write(clean_infor, 8);
  Serial2.write(clean_infor, 8);
  Serial2.write(clean_infor_one, 8);
  Serial2.write(clean_infor_one, 8);
  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_price, 10);
  Serial2.write(Show_default_price, 10);
}

void Preset_setup_two() {  // to reset all default in preset page
  Total_check_two = false;
  over_all_li_price_check_two = false;
  liter_check_two = false;
  price_check_two = false;
  preset_check_two = false;
  rfid_card_check_two = false;

  Preset_total_eep_check_two = 0;
  Preset_eep_total_price_two = 0;
  Preset_eep_total_liter_two = 0;
  combine_Liter_two = 0;
  combine_price_two = 0;

  Serial.print("Hmi two vehicle value => ");
  Serial.println(vehicle_value_two);
  Serial.print("Hmi two cash value => ");
  Serial.println(cash_value_two);
  Serial.print("Hmi two car no => ");
  Serial.println(Car_no_two);
  Serial.print("Hmi two member id => ");
  Serial.println(Member_id_two);
  Serial.print("Hmi two member name => ");
  Serial.println(Member_name_two);

  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_liter, 10);

  Serial2.write(Show_default_price, 10);
  Serial2.write(Show_default_price, 10);

  Page_change[9] = 0X07;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);
}

void vehicle_setup_two() {  // to reset all default in vehicle page
  // Serial.println(" vehicle defaultin Hmi two ");

  route_protect = true;

  for (int j = 0; j < 20; j++) Member_id_two[j] = 0;
  for (int j = 0; j < 20; j++) Car_no_two[j] = 0;
  for (int j = 0; j < 20; j++) Member_name_two[j] = 0;
  for (int j = 0; j < 4; j++) vehicle_type_two[j] = 0;
  for (int j = 0; j < 4; j++) cash_type_two[j] = 0;

  vehicle_value_two = 1;
  cash_value_two = 1;

  Serial2.write(Show_car_no, 15);
  // delay(5);
  Serial2.write(Show_car_no, 15);
  // delay(5);

  Serial2.write(Show_member_id, 17);
  // delay(5);
  Serial2.write(Show_member_id, 17);
  // delay(5);

  Serial2.write(Show_member_name, 19);
  // delay(5);
  Serial2.write(Show_member_name, 19);
  // delay(5);
  Serial2.write(Default_cash, 8);
  // delay(5);
  Serial2.write(Default_cash, 8);
  // delay(5);

  Serial2.write(Default_vehicle, 8);
  Serial2.write(Default_vehicle, 8);

  Page_change[9] = 0X04;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);
}

void rfid_setup_two() {  // rfid auth
  Serial.println(" rfid setup in Hmi two");
  if (Hmi_mfrc522.PICC_IsNewCardPresent() && Hmi_mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID tag :");

    rfid_content_two = "";

    for (byte i = 0; i < Hmi_mfrc522.uid.size; i++) {
      Serial.print(Hmi_mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(Hmi_mfrc522.uid.uidByte[i], HEX);
      rfid_content_two.concat(String(Hmi_mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      rfid_content_two.concat(String(Hmi_mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    rfid_content_two.toUpperCase();

    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    Hmi_mfrc522.PICC_HaltA();
    Hmi_mfrc522.PCD_StopCrypto1();

    http_login_post_two(rfid_content_two);  // when get server api to open

    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    rfid_auth_check_two = true;
    rfid_check_two = false;
  }
}

void rfid_auth_two() {  // rfid auth one
  Serial.println("rfid auth function Hmi two");

  if (rfid_condition && !rfid_card_check_two) {
    Serial.println("rfid auth in rfid card check false");

    tone(buzzer_pin_two, 5000);
    delay(50);

    rfid_condition = false;
    rfid_check_two = false;
    rfid_card_check_two = false;

    Page_change[9] = 0X04;
    Serial2.write(Page_change, 10);
    vehicle_setup_two();

    Serial2.write(Page_change, 10);
    vehicle_setup_two();

    noTone(buzzer_pin_two);
    rfid_auth_check_two = false;

  } else if (rfid_condition && rfid_card_check_two) {  // when card no not access
    Serial.println("rfid auth in rfid card check true line");

    tone(buzzer_pin_two, 5000);
    delay(50);

    rfid_condition = false;
    rfid_check_two = false;
    rfid_card_check_two = false;

    Page_change[9] = 0X07;
    Serial2.write(Page_change, 10);
    Preset_setup_two();

    Serial2.write(Page_change, 10);
    Preset_setup_two();

    noTone(buzzer_pin_two);
    rfid_auth_check_two = false;

  } else if (!rfid_condition) {

    Page_change[9] = 0x05;
    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    delay(50);
    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);
    delay(50);
    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    rfid_auth_check_two = false;
    rfid_condition = false;
  }
}

void Information_page_show_two() {  // to show information page

  Page_change[9] = 0x10;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);

  Serial.println("information page done");
}

void page_check_fun_two(int page_data) {
  Serial.println("page check fun in hmi two");

  if (page_data == 0 || page_data != nozzlenum) {
    page_data = nozzlenum;
  }

  switch (page_data) {
    case 1:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 2:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 3:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 4:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 5:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 6:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 7:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
    case 8:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
  }
}

void PIN_Auth_two() {  // pin code auth two
  if (PIN_CODE_two == eep_PIN_CODE) {
    Page_change[9] = 0x0D;

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(Default_pin_code, 10);
  } else {

    Page_change[9] = 0x17;

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(Default_pin_code, 10);
  }
}

void buzzer_song_two() {
  tone(buzzer_pin_two, 5000);
  delay(50);
  noTone(buzzer_pin_two);
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$----common function----$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void wifi_init() {

  Serial.println("wifi init function");
  wifi_try = 0;

  WiFi.begin("POS_Server", "asdffdsa");  ///akkkkk
  // WiFi.begin(SSID_buffer, PASS_buffer);
  while (WiFi.status() != WL_CONNECTED && wifi_try++ != 15) {

    Serial2.write(Hide_wifi_icon, 8);
    delay(250);
    Serial2.write(Show_wifi_icon, 8);
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Wifi is connected..!");

    Serial2.write(Show_wifi_icon, 8);
    Serial2.write(Show_wifi_icon, 8);

    Icon_fun();  // icon fun

    Serial.println(WiFi.localIP());
  }
}

void wifi_save_connect() {
  Serial.println("wifi save connect function");
  wifi_try = 0;
  Serial.println(SSID_buffer);
  Serial.println(PASS_buffer);
  Serial.print("Wifi is connecting to ");
  Serial.println(SSID_buffer);

  WiFi.begin(SSID_buffer, PASS_buffer);
  while (WiFi.status() != WL_CONNECTED && wifi_try++ != 15) {
    Serial2.write(Hide_wifi_icon, 8);
    delay(250);
    Serial2.write(Show_wifi_icon, 8);
    delay(250);
  }

  common_page_check_fun(nozzlenum);  // to check page for screen brightness

  isconnected = false;

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Wifi is connected from ");
    Serial.println(SSID_buffer);
    Serial.println(WiFi.localIP());

    writeStringToEEPROM(0, SSID_buffer);  // for eeprom write
    writeStringToEEPROM(40, PASS_buffer);

    Serial2.write(Show_wifi_icon, 8);
    Serial2.write(Show_wifi_icon, 8);

    Icon_fun();

    isconnected = false;
  }
}

void preset_eeprom_value() {
  F1_eep_liter_value = eeprom_read_data(150);
  F1_eep_price_value = eeprom_read_data(155);

  F2_eep_liter_value = eeprom_read_data(160);
  F2_eep_price_value = eeprom_read_data(165);

  F3_eep_liter_value = eeprom_read_data(170);
  F3_eep_price_value = eeprom_read_data(175);

  F4_eep_liter_value = eeprom_read_data(180);
  F4_eep_price_value = eeprom_read_data(185);

  Serial.println(F1_eep_liter_value);
  Serial.println(F1_eep_price_value);
  Serial.println(F2_eep_liter_value);
  Serial.println(F2_eep_price_value);
  Serial.println(F3_eep_liter_value);
  Serial.println(F3_eep_price_value);
  Serial.println(F4_eep_liter_value);
  Serial.println(F4_eep_price_value);
}

void eeprom_init() {  // to read wifi data and pump id from eeprom

  // readStringFromEEPROM(0, SSID_buffer, sizeof(SSID_buffer));
  // readStringFromEEPROM(40, PASS_buffer, sizeof(SSID_buffer));
  char eep_ssid;
  char eep_pass;

  for (int i = 0; i < 40; i++) {
    eep_ssid = EEPROM.read(i);
    if (eep_ssid == '\0') break;
    SSID_buffer[i] = eep_ssid;
  }

  for (int j = 40; j < 80; j++) {
    eep_pass = EEPROM.read(j);
    if (eep_pass == '\0') break;
    PASS_buffer[j - 40] = eep_pass;
  }

  casher_lock_check = EEPROM.read(90);
  eep_PIN_CODE = eeprom_read_data(92);

  pumpid1 = EEPROM.read(101);
  pumpid2 = EEPROM.read(102);
  pumpid3 = EEPROM.read(103);
  pumpid4 = EEPROM.read(104);
  pumpid5 = EEPROM.read(105);
  pumpid6 = EEPROM.read(106);
  pumpid7 = EEPROM.read(107);
  pumpid8 = EEPROM.read(108);

  devicenum = EEPROM.read(109);
  nozzlenum = EEPROM.read(110);

  dispenser_1_price = eeprom_read_data(114);
  dispenser_2_price = eeprom_read_data(118);
  dispenser_3_price = eeprom_read_data(122);
  dispenser_4_price = eeprom_read_data(81);
  dispenser_5_price = eeprom_read_data(85);
  dispenser_6_price = eeprom_read_data(132);
  dispenser_7_price = eeprom_read_data(138);
  dispenser_8_price = eeprom_read_data(144);

  Serial.println("dispenser price is =>");
  Serial.println(dispenser_1_price);
  Serial.println(dispenser_2_price);
  Serial.println(dispenser_3_price);
  Serial.println(dispenser_4_price);
  Serial.println(dispenser_5_price);
  Serial.println(dispenser_6_price);

  F1_eep_liter_value = eeprom_read_data(150);
  F1_eep_price_value = eeprom_read_data(155);

  F2_eep_liter_value = eeprom_read_data(160);
  F2_eep_price_value = eeprom_read_data(165);

  F3_eep_liter_value = eeprom_read_data(170);
  F3_eep_price_value = eeprom_read_data(175);

  F4_eep_liter_value = eeprom_read_data(180);
  F4_eep_price_value = eeprom_read_data(185);

  Fuel_Type_1 = read_Fuel_type(200, 220);
  Fuel_Type_2 = read_Fuel_type(230, 260);
  Fuel_Type_3 = read_Fuel_type(260, 290);
  Fuel_Type_4 = read_Fuel_type(290, 320);
  Fuel_Type_5 = read_Fuel_type(330, 360);
  Fuel_Type_6 = read_Fuel_type(360, 390);
  Fuel_Type_7 = read_Fuel_type(390, 420);
  Fuel_Type_8 = read_Fuel_type(420, 450);

  // Serial.print("EEPROM read fuel types=> ");
  // Serial.println(Fuel_Type_1);
  // Serial.println(Fuel_Type_2);
  // Serial.println(Fuel_Type_3);
  // Serial.println(Fuel_Type_4);
  // Serial.println(Fuel_Type_5);
  // Serial.println(Fuel_Type_6);
  // Serial.println(Fuel_Type_7);
  // Serial.println(Fuel_Type_8);

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Serial.print("EEPROM cut fuel types=> ");
  Serial.println(cut_Fuel_Type_1);
  Serial.println(cut_Fuel_Type_2);
  Serial.println(cut_Fuel_Type_3);
  Serial.println(cut_Fuel_Type_4);
  Serial.println(cut_Fuel_Type_5);
  Serial.println(cut_Fuel_Type_6);
  Serial.println(cut_Fuel_Type_7);
  Serial.println(cut_Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  Serial.print("Pin Code is => ");
  Serial.println(eep_PIN_CODE);

  Serial.print("Casher lock Id 1 is => ");
  Serial.println(casher_lock_check);

  Serial.print("Pump Id 1 is => ");
  Serial.println(pumpid1);
  Serial.print("Pump Id 2 is => ");
  Serial.println(pumpid2);
  Serial.print("Pump Id 3 is => ");
  Serial.println(pumpid3);
  Serial.print("Pump Id 4 is => ");
  Serial.println(pumpid4);
  Serial.print("Pump Id 5 is => ");
  Serial.println(pumpid5);
  Serial.print("Pump Id 6 is => ");
  Serial.println(pumpid6);
  Serial.print("Pump Id 7 is => ");
  Serial.println(pumpid7);
  Serial.print("Pump Id 8 is => ");
  Serial.println(pumpid8);

  Serial.print("device Id is => ");
  Serial.println(devicenum);
  Serial.print("nozzlenum is => ");
  Serial.println(nozzlenum);


  Serial.print("EEPROM read ssid is => ");
  Serial.println(SSID_buffer);
  Serial.print("EEPROM read pass is => ");
  Serial.println(PASS_buffer);

  Serial.println("EEprom init is Done ......!");
}

// void initial_setup() {  // initial setup for page swith and show device name

//   if (pumpid1 == 0 && pumpid2 == 0) {
//     Serial.println("One nozzle page is called.");
//     page_check = 1;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 == 0 && pumpid3 == 0) {
//     Serial.println("One nozzle page is called.");
//     page_check = 1;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 == 0 && pumpid4 == 0) {
//     Serial.println("Two nozzle page is called.");
//     page_check = 2;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 == 0 && pumpid5 == 0) {
//     Serial.println("Three nozzle page is called.");
//     page_check = 3;
//     Serial2.write(four_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 == 0 && pumpid6 == 0) {
//     Serial.println("Four nozzle page is called.");
//     page_check = 4;
//     Serial2.write(four_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 == 0 && pumpid7 == 0) {
//     Serial.println("Five nozzle page is called.");
//     page_check = 5;
//     Serial2.write(six_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 == 0 && pumpid8 == 0) {
//     Serial.println("Six nozzle page is called.");
//     page_check = 6;
//     Serial2.write(six_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 > 0 && pumpid8 == 0) {
//     Serial.println("Seven nozzle page is called.");
//     page_check = 7;
//     Serial2.write(eight_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 > 0 && pumpid8 > 0) {  // eight page change
//     Serial.println("eight nozzle page is called.");
//     page_check = 8;
//     Serial2.write(eight_Page_change, 10);
//   }
// }

void mqtt_topic_change() {

  if (devicenum == 1) {
    Show_device_id[6] = highByte(1);
    Show_device_id[7] = lowByte(1);
    pump_permit[21] = '1';
    final_data[20] = '1';
    pin_Code[19] = '1';
    device_Id_topic[38] = '1';
    FuelType_topic[38] = '1';

  } else if (devicenum == 2) {
    Show_device_id[6] = highByte(2);
    Show_device_id[7] = lowByte(2);
    pump_permit[21] = '2';
    final_data[20] = '2';
    pin_Code[19] = '2';
    device_Id_topic[38] = '2';
    FuelType_topic[38] = '2';

  } else if (devicenum == 3) {
    Show_device_id[6] = highByte(3);
    Show_device_id[7] = lowByte(3);
    pump_permit[21] = '3';
    final_data[20] = '3';
    pin_Code[19] = '3';
    device_Id_topic[38] = '3';
    FuelType_topic[38] = '3';

  } else if (devicenum == 4) {
    Show_device_id[6] = highByte(4);
    Show_device_id[7] = lowByte(4);
    pump_permit[21] = '4';
    final_data[20] = '4';
    pin_Code[19] = '4';
    device_Id_topic[38] = '4';
    FuelType_topic[38] = '4';

  } else if (devicenum == 5) {
    Show_device_id[6] = highByte(5);
    Show_device_id[7] = lowByte(5);
    pump_permit[21] = '5';
    final_data[20] = '5';
    pin_Code[19] = '5';
    device_Id_topic[38] = '5';
    FuelType_topic[38] = '5';

  } else if (devicenum == 6) {
    Show_device_id[6] = highByte(6);
    Show_device_id[7] = lowByte(6);
    pump_permit[21] = '6';
    final_data[20] = '6';
    pin_Code[19] = '6';
    device_Id_topic[38] = '6';
    FuelType_topic[38] = '6';

  } else if (devicenum == 7) {
    Show_device_id[6] = highByte(7);
    Show_device_id[7] = lowByte(7);
    pump_permit[21] = '7';
    final_data[20] = '7';
    pin_Code[19] = '7';
    device_Id_topic[38] = '7';
    FuelType_topic[38] = '7';

  } else if (devicenum == 8) {
    Show_device_id[6] = highByte(8);
    Show_device_id[7] = lowByte(8);
    pump_permit[21] = '8';
    final_data[20] = '8';
    pin_Code[19] = '8';
    device_Id_topic[38] = '8';
    FuelType_topic[38] = '8';
  }

  delay(5);

  Serial2.write(Show_device_id, 8);
}

void show_nozzle_no_fun(int noz_data, int noz_num) {

  if (noz_data == 0) {
    Show_Nozzle_NO[6] = 0x00;
    Show_Nozzle_NO[7] = 0x00;

  } else {
    Show_Nozzle_NO[6] = highByte(noz_data);
    Show_Nozzle_NO[7] = lowByte(noz_data);
  }

  if (noz_num == 1) Show_Nozzle_NO[5] = 0x11;
  else if (noz_num == 2) Show_Nozzle_NO[5] = 0x12;
  else if (noz_num == 3) Show_Nozzle_NO[5] = 0x13;
  else if (noz_num == 4) Show_Nozzle_NO[5] = 0x14;
  else if (noz_num == 5) Show_Nozzle_NO[5] = 0x15;
  else if (noz_num == 6) Show_Nozzle_NO[5] = 0x16;
  else if (noz_num == 7) Show_Nozzle_NO[5] = 0x17;
  else if (noz_num == 8) Show_Nozzle_NO[5] = 0x18;

  Serial2.write(Show_Nozzle_NO, 8);
}

void reconnect() {
  while (!client.connected()) {

    Serial.println("MQtt is connecting ");
    String clientId = "Rfid_client-";
    clientId += String(random(0xffff), HEX);
    // Serial.println(clientId);
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Mqtt connected");
      cloud_icon_check = true;

      client.subscribe("Connect");                    // subscribe the topics here
      client.publish("connect", "Device1Connected");  // subscribe the topics here
      client.subscribe("detpos/#");

    } else {
      Serial.print("mqtt connection failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  incommingMessage = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }

  Serial.println(topic);
  Serial.println(incommingMessage);

  if (String(topic) == String("detpos/device/req")) {
    Serial.println("cancle data from dispenser");
    incommingMessage.toCharArray(finaldataArry, incommingMessage.length() + 1);

    charArray[0] = finaldataArry[0];
    charArray[1] = finaldataArry[1];

    pumpmqttnumchange();
    show_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(pump_permit)) {  // pump permit

    incommingMessage.toCharArray(permitArry, incommingMessage.length() + 1);
    Serial.print("Permit number is => ");
    Serial.println(permitArry);

    charArray[0] = permitArry[0];
    charArray[1] = permitArry[1];

    pumpmqttnumchange();
    hide_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(final_data)) {

    Serial.println("Final data response");
    incommingMessage.toCharArray(finaldataArry, incommingMessage.length() + 1);
    Serial.print("Final Data is => ");
    Serial.println(finaldataArry);

    charArray[0] = finaldataArry[0];
    charArray[1] = finaldataArry[1];

    pumpmqttnumchange();
    show_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }


  if (String(topic) == String(pin_Code)) {  // pin code topic
    Serial.println("pin code topic");

    incommingMessage.toCharArray(pinCodeArry, incommingMessage.length() + 1);
    char PIN_Arry[6];
    for (int j = 0; j < 6; j++) PIN_Arry[j] = 0;

    for (int i = 0; i < 6; i++) {
      PIN_Arry[i] = pinCodeArry[i];
    }

    String pin_str = String(PIN_Arry);
    int pin_Int = pin_str.toInt();
    Serial.print("Pin code form mqtt is => ");
    Serial.println(pin_Int);

    eeprom_writeInt(92, pin_Int);
    Serial.println("Done");

    eep_PIN_CODE = eeprom_read_data(92);
    Serial.print("pin code return is => ");
    Serial.println(eep_PIN_CODE);
  }

  if (String(topic) == String(device_Id_topic)) {
    DynamicJsonDocument doc(4096);  // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    int pumpid_1 = 0;
    int pumpid_2 = 0;
    int pumpid_3 = 0;
    int pumpid_4 = 0;
    int pumpid_5 = 0;
    int pumpid_6 = 0;
    int pumpid_7 = 0;
    int pumpid_8 = 0;

    devicenum = doc["devicenum"].as<const int>();
    nozzlenum = doc["nozzlenum"].as<const int>();
    pumpid_1 = doc["pumpid1"].as<const int>();
    pumpid_2 = doc["pumpid2"].as<const int>();
    pumpid_3 = doc["pumpid3"].as<const int>();
    pumpid_4 = doc["pumpid4"].as<const int>();
    pumpid_5 = doc["pumpid5"].as<const int>();
    pumpid_6 = doc["pumpid6"].as<const int>();
    pumpid_7 = doc["pumpid7"].as<const int>();
    pumpid_8 = doc["pumpid8"].as<const int>();

    Serial.println(devicenum);
    Serial.println(nozzlenum);
    Serial.println(pumpid_1);
    Serial.println(pumpid_2);
    Serial.println(pumpid_3);
    Serial.println(pumpid_4);
    Serial.println(pumpid_5);
    Serial.println(pumpid_6);
    Serial.println(pumpid_7);
    Serial.println(pumpid_8);

    EEPROM.write(101, pumpid_1);
    EEPROM.commit();
    EEPROM.write(102, pumpid_2);
    EEPROM.commit();
    EEPROM.write(103, pumpid_3);
    EEPROM.commit();
    EEPROM.write(104, pumpid_4);
    EEPROM.commit();
    EEPROM.write(105, pumpid_5);
    EEPROM.commit();
    EEPROM.write(106, pumpid_6);
    EEPROM.commit();
    EEPROM.write(107, pumpid_7);
    EEPROM.commit();
    EEPROM.write(108, pumpid_8);
    EEPROM.commit();
    EEPROM.write(109, devicenum);  // device id
    EEPROM.commit();
    EEPROM.write(110, nozzlenum);  // nozzle count
    EEPROM.commit();
    Serial.println("yep all save");

    pumpid1 = EEPROM.read(101);
    pumpid2 = EEPROM.read(102);
    pumpid3 = EEPROM.read(103);
    pumpid4 = EEPROM.read(104);
    pumpid5 = EEPROM.read(105);
    pumpid6 = EEPROM.read(106);
    pumpid7 = EEPROM.read(107);
    pumpid8 = EEPROM.read(108);

    devicenum = EEPROM.read(109);
    nozzlenum = EEPROM.read(110);

    Serial.print("Pump Id 1 is => ");
    Serial.println(pumpid1);
    Serial.print("Pump Id 2 is => ");
    Serial.println(pumpid2);
    Serial.print("Pump Id 3 is => ");
    Serial.println(pumpid3);
    Serial.print("Pump Id 4 is => ");
    Serial.println(pumpid4);
    Serial.print("Pump Id 5 is => ");
    Serial.println(pumpid5);
    Serial.print("Pump Id 6 is => ");
    Serial.println(pumpid6);
    Serial.print("Pump Id 7 is => ");
    Serial.println(pumpid7);
    Serial.print("Pump Id 8 is => ");
    Serial.println(pumpid8);

    Serial.print("device id is => ");
    Serial.println(devicenum);
    Serial.print("nozz no id is => ");
    Serial.println(nozzlenum);

    // initial_setup(); //ak ch

    Show_device_id[6] = highByte(devicenum);
    Show_device_id[7] = lowByte(devicenum);
    Serial2.write(Show_device_id, 8);
    delay(2);
    Serial2.write(Show_device_id, 8);

    show_nozzle_no_fun(pumpid1, 1);
    show_nozzle_no_fun(pumpid2, 2);
    show_nozzle_no_fun(pumpid3, 3);
    show_nozzle_no_fun(pumpid4, 4);
    show_nozzle_no_fun(pumpid5, 5);
    show_nozzle_no_fun(pumpid6, 6);
    show_nozzle_no_fun(pumpid7, 7);
    show_nozzle_no_fun(pumpid8, 8);

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(FuelType_topic)) {
    Serial.println("this is fuel type change");

    DynamicJsonDocument doc(1024);  // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    String fuelType_1 = "";
    String fuelType_2 = "";
    String fuelType_3 = "";
    String fuelType_4 = "";
    String fuelType_5 = "";
    String fuelType_6 = "";
    String fuelType_7 = "";
    String fuelType_8 = "";

    fuelType_1 = doc["fuelid1"].as<const char*>();
    fuelType_2 = doc["fuelid2"].as<const char*>();
    fuelType_3 = doc["fuelid3"].as<const char*>();
    fuelType_4 = doc["fuelid4"].as<const char*>();
    fuelType_5 = doc["fuelid5"].as<const char*>();
    fuelType_6 = doc["fuelid6"].as<const char*>();
    fuelType_7 = doc["fuelid7"].as<const char*>();
    fuelType_8 = doc["fuelid8"].as<const char*>();

    Serial.println("mqtt fuel type");
    Serial.println(fuelType_1);
    Serial.println(fuelType_2);
    Serial.println(fuelType_3);
    Serial.println(fuelType_4);
    Serial.println(fuelType_5);
    Serial.println(fuelType_6);
    Serial.println(fuelType_7);
    Serial.println(fuelType_8);

    writeStringToEEPROM(200, fuelType_1);
    writeStringToEEPROM(230, fuelType_2);
    writeStringToEEPROM(260, fuelType_3);
    writeStringToEEPROM(290, fuelType_4);
    writeStringToEEPROM(330, fuelType_5);
    writeStringToEEPROM(360, fuelType_6);
    writeStringToEEPROM(390, fuelType_7);
    writeStringToEEPROM(420, fuelType_8);

    Serial.println("yep all save");

    Fuel_Type_1 = read_Fuel_type(200, 220);
    Fuel_Type_2 = read_Fuel_type(230, 260);
    Fuel_Type_3 = read_Fuel_type(260, 290);
    Fuel_Type_4 = read_Fuel_type(290, 320);
    Fuel_Type_5 = read_Fuel_type(330, 360);
    Fuel_Type_6 = read_Fuel_type(360, 390);
    Fuel_Type_7 = read_Fuel_type(390, 420);
    Fuel_Type_8 = read_Fuel_type(420, 450);

    Serial.println("from mqtt fuel type");
    Serial.println(Fuel_Type_1);
    Serial.println(Fuel_Type_2);
    Serial.println(Fuel_Type_3);
    Serial.println(Fuel_Type_4);
    Serial.println(Fuel_Type_5);
    Serial.println(Fuel_Type_6);
    Serial.println(Fuel_Type_7);
    Serial.println(Fuel_Type_8);

    show_fuel_type_name();
    mqtt_topic_change();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(pricechange)) {
    incommingMessage.toCharArray(pricechangeary, incommingMessage.length() + 1);

    Serial.print("price change is => ");
    Serial.println(pricechangeary);

    charArray[0] = pricechangeary[0];
    charArray[1] = pricechangeary[1];

    pumpmqttnumchange();

    price_show_hmi();

    if (!brightness_check) screen_brightness_in();
  }
}

void pumpmqttnumchange() {
  Serial.println("pumpmqttnumchange function");

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
}

void price_show_hmi() {  // to show price input from server
  // Serial.println("price show hmi function");

  if (pumpid1 == pumpmqttnum) {  // Pump Id 1
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }

    String uniquePrice = String(priceArray);
    dispenser_1_price = uniquePrice.toInt();

    eeprom_writeInt(114, dispenser_1_price);

    Serial.print("pumpid 1 price is => ");
    Serial.println(dispenser_1_price);

    int Pump_Id = 1;

    ShowDisplay(Pump_Id, dispenser_1_price);

  } else if (pumpid2 == pumpmqttnum) {  // Pump Id 2
    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 2 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_2_price = uniquePrice.toInt();

    eeprom_writeInt(118, dispenser_2_price);

    int Pump_Id = 2;

    ShowDisplay(Pump_Id, dispenser_2_price);

  } else if (pumpid3 == pumpmqttnum) {  // pump Id 3

    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 3 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_3_price = uniquePrice.toInt();

    eeprom_writeInt(122, dispenser_3_price);

    int Pump_Id = 3;

    ShowDisplay(Pump_Id, dispenser_3_price);

  } else if (pumpid4 == pumpmqttnum) {  // pump Id 4
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 4 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_4_price = uniquePrice.toInt();

    eeprom_writeInt(81, dispenser_4_price);

    int Pump_Id = 4;

    ShowDisplay(Pump_Id, dispenser_4_price);

  } else if (pumpid5 == pumpmqttnum) {  // pump Id 5
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 5 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_5_price = uniquePrice.toInt();
    eeprom_writeInt(85, dispenser_5_price);
    int Pump_Id = 5;

    ShowDisplay(Pump_Id, dispenser_5_price);

  } else if (pumpid6 == pumpmqttnum) {  // pump Id 6
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 6 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_6_price = uniquePrice.toInt();
    eeprom_writeInt(132, dispenser_6_price);
    int Pump_Id = 6;

    ShowDisplay(Pump_Id, dispenser_6_price);

  } else if (pumpid7 == pumpmqttnum) {  // pump id 7
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 7 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_7_price = uniquePrice.toInt();
    eeprom_writeInt(138, dispenser_7_price);
    int Pump_Id = 7;

    ShowDisplay(Pump_Id, dispenser_7_price);

  } else if (pumpid8 == pumpmqttnum) {  // pump Id 8

    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 8 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_8_price = uniquePrice.toInt();
    eeprom_writeInt(144, dispenser_8_price);
    int Pump_Id = 8;

    ShowDisplay(Pump_Id, dispenser_8_price);
  }
}

void ShowDisplay(int dispenser, int decimal_price) {  // to show price in
  // Serial.println("show display function");

  Show_dispenser_price[6] = highByte(decimal_price);
  Show_dispenser_price[7] = lowByte(decimal_price);

  if (dispenser == 1) Show_dispenser_price[5] = 0x11;
  else if (dispenser == 2) Show_dispenser_price[5] = 0x12;
  else if (dispenser == 3) Show_dispenser_price[5] = 0x13;
  else if (dispenser == 4) Show_dispenser_price[5] = 0x14;
  else if (dispenser == 5) Show_dispenser_price[5] = 0x15;
  else if (dispenser == 6) Show_dispenser_price[5] = 0x16;
  else if (dispenser == 7) Show_dispenser_price[5] = 0x17;
  else if (dispenser == 8) Show_dispenser_price[5] = 0x18;

  Serial2.write(Show_dispenser_price, 8);
  Serial2.write(Show_dispenser_price, 8);
}

void show_dispenser_icon_fun() {  // dispenser icon main function
  if (pumpid1 == pumpmqttnum) Show_dispenser_icon[5] = 0x11;
  else if (pumpid2 == pumpmqttnum) Show_dispenser_icon[5] = 0x12;
  else if (pumpid3 == pumpmqttnum) Show_dispenser_icon[5] = 0x13;
  else if (pumpid4 == pumpmqttnum) Show_dispenser_icon[5] = 0x14;
  else if (pumpid5 == pumpmqttnum) Show_dispenser_icon[5] = 0x15;
  else if (pumpid6 == pumpmqttnum) Show_dispenser_icon[5] = 0x16;
  else if (pumpid7 == pumpmqttnum) Show_dispenser_icon[5] = 0x17;
  else if (pumpid8 == pumpmqttnum) Show_dispenser_icon[5] = 0x18;

  Serial2.write(Show_dispenser_icon, 8);
  Serial2.write(Show_dispenser_icon, 8);
}

void hide_dispenser_icon_fun() {  // dispenser icon main function

  if (pumpid1 == pumpmqttnum) Hide_dispenser_icon[5] = 0x11;
  else if (pumpid2 == pumpmqttnum) Hide_dispenser_icon[5] = 0x12;
  else if (pumpid3 == pumpmqttnum) Hide_dispenser_icon[5] = 0x13;
  else if (pumpid4 == pumpmqttnum) Hide_dispenser_icon[5] = 0x14;
  else if (pumpid5 == pumpmqttnum) Hide_dispenser_icon[5] = 0x15;
  else if (pumpid6 == pumpmqttnum) Hide_dispenser_icon[5] = 0x16;
  else if (pumpid7 == pumpmqttnum) Hide_dispenser_icon[5] = 0x17;
  else if (pumpid8 == pumpmqttnum) Hide_dispenser_icon[5] = 0x18;

  Serial2.write(Hide_dispenser_icon, 8);
  Serial2.write(Hide_dispenser_icon, 8);
}

void show_dispenser_icon_initial(int icon_int) {  // dispenser icon main function
  if (icon_int == 1) Show_dispenser_icon[5] = 0x11;
  else if (icon_int == 2) Show_dispenser_icon[5] = 0x12;
  else if (icon_int == 3) Show_dispenser_icon[5] = 0x13;
  else if (icon_int == 4) Show_dispenser_icon[5] = 0x14;
  else if (icon_int == 5) Show_dispenser_icon[5] = 0x15;
  else if (icon_int == 6) Show_dispenser_icon[5] = 0x16;
  else if (icon_int == 7) Show_dispenser_icon[5] = 0x17;
  else if (icon_int == 8) Show_dispenser_icon[5] = 0x18;

  Serial2.write(Show_dispenser_icon, 8);
  Serial2.write(Show_dispenser_icon, 8);
}

void Show_Fueltype(char* Fuel_data, int fuelid) {  //show Fuel type

  if (fuelid == 1) Show_fuel[4] = 0x91;
  else if (fuelid == 2) Show_fuel[4] = 0x92;
  else if (fuelid == 3) Show_fuel[4] = 0x93;
  else if (fuelid == 4) Show_fuel[4] = 0x94;
  else if (fuelid == 5) Show_fuel[4] = 0x95;
  else if (fuelid == 6) Show_fuel[4] = 0x96;
  else if (fuelid == 7) Show_fuel[4] = 0x97;
  else if (fuelid == 8) Show_fuel[4] = 0x98;

  Serial2.write(Show_fuel, 6);
  Serial2.write(Fuel_data, 20);
}

// void Default_Show_Fueltype(int fuelid) {  //show Fuel type

//   if (fuelid == 1) Default_Show_fuel[4] = 0x91;
//   else if (fuelid == 2) Default_Show_fuel[4] = 0x92;
//   else if (fuelid == 3) Default_Show_fuel[4] = 0x93;
//   else if (fuelid == 4) Default_Show_fuel[4] = 0x94;
//   else if (fuelid == 5) Default_Show_fuel[4] = 0x95;
//   else if (fuelid == 6) Default_Show_fuel[4] = 0x96;
//   else if (fuelid == 7) Default_Show_fuel[4] = 0x97;
//   else if (fuelid == 8) Default_Show_fuel[4] = 0x98;

//   Serial2.write(Default_Show_fuel, 8);
//   Serial2.write(Default_Show_fuel, 8);
// }

void Icon_fun() {  // to hide wifi icon

  // Serial2.write(Hide_wifi_icon, 8);
  Serial2.write(Show_icon_one, 8);
  Serial2.write(Show_icon_three, 8);
  Serial2.write(Hide_cloud_icon, 8);

  Serial.println("icon function done");
}

void screen_brightness_out() {  // screen brightness out

  if (millis() / 1000 - Hmi_time > 300) {

    Page_change[9] = 0x11;


    Serial.println("hmi one brightness out");

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(brightout, 8);
    Serial2.write(brightout, 8);

    rfid_check_two = false;
    rfid_auth_check_two = false;
    brightness_check = false;
    Serial.println("brightness out");
  }
}

void screen_brightness_in() {  // screen brightness


  Serial2.write(brightin, 8);
  Serial2.write(brightin, 8);

  common_page_check_fun(nozzlenum);  // to check page for screen brightness

  brightness_check = true;

  rfid_check_two = false;
  rfid_auth_check_two = false;
}

void common_page_check_fun(int page_data) {
  Serial.println("coommon page check");

  if (page_data == 0 || page_data != nozzlenum) {
    page_data = nozzlenum;
  }

  switch (page_data) {
    case 1:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 2:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 3:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 4:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 5:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 6:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 7:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
    case 8:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
  }
}

void default_setup_all() {  // power on setup all

  for (int i = 1; i < 9; i++) {
    ShowDisplay(i, 0);
  }

  for (int i = 1; i < 9; i++) {
    show_dispenser_icon_initial(i);
  }

  // initial_setup();  // intitial setup in hmi ak ch
  Icon_fun();

  ShowDisplay(1, dispenser_1_price);
  ShowDisplay(2, dispenser_2_price);
  ShowDisplay(3, dispenser_3_price);
  ShowDisplay(4, dispenser_4_price);
  ShowDisplay(5, dispenser_5_price);
  ShowDisplay(6, dispenser_6_price);
  ShowDisplay(7, dispenser_7_price);
  ShowDisplay(8, dispenser_8_price);

  show_nozzle_no_fun(pumpid1, 1);
  show_nozzle_no_fun(pumpid2, 2);
  show_nozzle_no_fun(pumpid3, 3);
  show_nozzle_no_fun(pumpid4, 4);
  show_nozzle_no_fun(pumpid5, 5);
  show_nozzle_no_fun(pumpid6, 6);
  show_nozzle_no_fun(pumpid7, 7);
  show_nozzle_no_fun(pumpid8, 8);

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  // Serial.print("EEPROM cut fuel type one is => ");
  // Serial.println(cut_Fuel_Type_1);
  // Serial.println(cut_Fuel_Type_2);
  // Serial.println(cut_Fuel_Type_3);
  // Serial.println(cut_Fuel_Type_4);
  // Serial.println(cut_Fuel_Type_5);
  // Serial.println(cut_Fuel_Type_6);
  // Serial.println(cut_Fuel_Type_7);
  // Serial.println(cut_Fuel_Type_8);
}

void show_fuel_type_name() {  // to show fuel type setup

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  // Serial.print("EEPROM cut fuel type one is => ");
  // Serial.println(cut_Fuel_Type_1);
  // Serial.println(cut_Fuel_Type_2);
  // Serial.println(cut_Fuel_Type_3);
  // Serial.println(cut_Fuel_Type_4);
  // Serial.println(cut_Fuel_Type_5);
  // Serial.println(cut_Fuel_Type_6);
  // Serial.println(cut_Fuel_Type_7);
  // Serial.println(cut_Fuel_Type_8);
}


////////// EEPROm functions ////////


char* cut_fuel_name(char* cut_data) {  // cut fuel type name

  char* common_data = (char*)malloc(20);  // Dynamically allocate memory

  if (common_data == NULL) {
    return NULL;
  }

  for (int i = 0; i < 20; i++) common_data[i] = 0;

  for (int j = 0; j < 20; j++) {
    if (cut_data[4 + j] == '\0') break;
    common_data[j] = cut_data[4 + j];
  }

  // Serial.print("common cut data is => ");
  // Serial.println(common_data);

  return common_data;
}

void eeprom_writeInt(int add, long data) {
  byte byte0 = (data >> 24) & 0xff;
  byte byte1 = (data >> 16) & 0xff;
  byte byte2 = (data >> 8) & 0xff;
  byte byte3 = data & 0xff;

  EEPROM.write(add, byte0);
  EEPROM.write(add + 1, byte1);
  EEPROM.write(add + 2, byte2);
  EEPROM.write(add + 3, byte3);
  EEPROM.commit();
}

int eeprom_read_data(int address) {
  return ((long)EEPROM.read(address) << 24) + ((long)EEPROM.read(address + 1) << 16) + ((long)EEPROM.read(address + 2) << 8) + (long)EEPROM.read(address + 3);
}


void writeStringToEEPROM(int address, String data) {  // to store wifi ssid and pass in EEprom
  int data_length = data.length();
  for (int i = 0; i < data_length; i++) {
    EEPROM.write(address + i, data[i]);
    EEPROM.commit();
  }
  EEPROM.write(address + data_length, '\0');
  EEPROM.commit();
  Serial.println("EEPROM write complete");
}

char* read_Fuel_type(int fuel_start_address, int fuel_end_address) {  // to save fuel type in eeprom
  char tem_data;
  char* common_data = (char*)malloc(20);  // Dynamically allocate memory

  if (common_data == NULL) {
    return NULL;
  }

  for (int i = 0; i < 20; i++) common_data[i] = 0;

  for (int j = fuel_start_address; j < fuel_end_address; j++) {
    tem_data = EEPROM.read(j);
    if (tem_data == '\0') break;
    common_data[j - fuel_start_address] = tem_data;
  }

  // Serial.print("common data is => ");
  // Serial.println(common_data);

  return common_data;
}

// void eeprom_writeInt(int add, int data) {

//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(add >> 8));    // MSB
//   Wire.write((uint8_t)(add & 0xFF));  // LSB

//   // Write the value bytes
//   Wire.write((uint8_t)(data >> 24));   // Byte 3
//   Wire.write((uint8_t)(data >> 16));   // Byte 2
//   Wire.write((uint8_t)(data >> 8));    // Byte 1
//   Wire.write((uint8_t)(data & 0xFF));  // Byte 0

//   Wire.endTransmission();

//   delay(5);
// }


// int eeprom_read_data(int address) {
//   int value = 0;

//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));  // LSB
//   Wire.endTransmission(false);            // Send repeated start condition

//   Wire.requestFrom(EEPROM_I2C_ADDRESS, 4);

//   if (Wire.available() >= 4) {
//     value = Wire.read() << 24;   // Byte 3
//     value |= Wire.read() << 16;  // Byte 2
//     value |= Wire.read() << 8;   // Byte 1
//     value |= Wire.read();        // Byte 0
//   }

//   return value;
// }


// void writeStringToEEPROM(int address, String data) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));

//   int i = 0;
//   while (data[i]) {
//     Wire.write(data[i]);
//     i++;
//   }
//   Wire.write('\0');  // Null-terminate the string
//   Wire.endTransmission();

//   delay(5);  // Delay for EEPROM write cycle
// }

// void readStringFromEEPROM(int address, char* buffer, size_t bufferSize) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));
//   Wire.endTransmission(false);

//   Wire.requestFrom(EEPROM_I2C_ADDRESS, bufferSize);

//   size_t i = 0;
//   while (Wire.available() && i < bufferSize - 1) {
//     buffer[i++] = Wire.read();
//   }
//   buffer[i] = '\0';
// }

// void setDataEEPROM(int alamat, byte data) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);
//   Wire.write((int)(alamat >> 8));    //MSB
//   Wire.write((int)(alamat & 0xFF));  //LSB
//   Wire.write(data);
//   Wire.endTransmission();
//   delay(5);
// }

// byte getDataEEPROM(int alamat) {
//   byte data = 0xFF;
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);
//   Wire.write((int)(alamat >> 8));
//   Wire.write((int)(alamat & 0xFF));
//   Wire.endTransmission();
//   Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
//   data = Wire.read();
//   return data;
// }

#endif
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

        #ifdef USE_REDSTAR /* development features */
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
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));
      gpio_set_level(LED_YELLOW, HIGH);
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
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));
      gpio_set_level(LED_YELLOW, HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));
    } else {
      // FMS_LOG_INFO("[fms_wifi.ino:59] Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(LED_YELLOW, LOW);
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Wait for 1 second before repeating
  }
}

