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
const String firmwareVersion = "0.4.0";           /* Current firmware version*/
String deviceName            = "ultm_25505v01_";  /* device ID (for)  change here like this user can change with configpanel*/
#define CLI_PASSWORD         "admin"              /* cli password change this password*/
/* end change note  */

#define FMS_TATSUNO_DEBUG_OPEN

//#define USE_V2_OTA_SERVER
/* #define USE_RESTAR */
#define USE_TATSUNO
// #define USE_REDSTAR

#include "_fms_main.h"
#include "src/_fms_cli.h"
#include "src/_fms_debug.h"
#include "src/_fms_json_helper.h"
#include "src/_fms_lanfeng.h"
#include <src/_fms_redstar.h>           /* test features */
#include <src/_fms_tatsuno.h>
#include <src/_fms_filemanager.h>  /* test features */


// #define DISABLE_MQTT_DEBUG
// #ifdef DISABLE_MQTT_DEBUGP
// #undef FMS_MQTT_DEBUG
// #endif
// #define USE_MQTT_DEBUG

#define USE_CLI
#define DISABLE_LANFENG  // Uncomment this line to disable the library
#ifdef DISABLE_LANFENG
#undef USE_LANFENG        // Undefine USE_LANFENG to disable the library
#endif



FMS_FileManager fileManager;
fms_cli fms_cli(fms_cli_serial, CLI_PASSWORD);      // Use "admin" as the default password change your admin pass here
Redstar redstar(fms_uart2_serial);                  // create redstar object
TatsunoProtocol tatsuno(fms_uart2_serial);          // Create an instance of TatsunoProtocol with the HardwareSerial object
fmsLanfeng lanfeng(22, 22);                         // set re de pin (DTR PIN)s

/* Main function */

void setup() {
  init_staus_leds();  // initialize status LEDs
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
  fms_cli.register_command("protocol_config","Set Protococl Congfig",       handle_protocol_config_command, 11, 11);
  fms_cli.register_command("mqtt_config"   ,"Configure Mqtt settings",     handle_mqtt_command,2,2);
#endif

  fms_pin_mode(BUILTIN_LED, OUTPUT);

  /* test features protocol selection 
 // fms_load_protocol_config();  // load protocol config from nvs storage

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

#ifdef USE_REDSTAR
  red_star_init();                         // redstar init
#endif

#ifdef USE_TATSUNO
  fms_tatsuno_init();                      // tatsuno init
#endif

  // /* test features
  // if (fms_initialize_wifi() && sysCfg.protocol != "0") {  // wifi is connected create all task s
  //   fms_task_create();
  // }
  // */

  if (fms_initialize_wifi()) {             // wifi is connected create all task s
    fms_task_create();
  }
}

void loop() {
}
