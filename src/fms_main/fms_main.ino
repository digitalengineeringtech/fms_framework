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
fmsLanfeng lanfeng(15,15,fms_uart2_serial); 
/* Main function */
void setup() {
  fms_pin_mode(BUILTIN_LED, OUTPUT);
  fms_cli.begin(115200);                    // uart
  fms_initialize_uart2();                   // uart 2
  // fmslanfeng.begin();
  // fmslanfeng.beginModbus(1,fms_uart2_serial); // add slave id
  fms_run_sd_test();                        // demo test fix this load configure data from sd card
  fmsEnableSerialLogging(true);             // show serial logging data on Serial Monitor
  fms_boot_count(true);                     // boot count
  // cli command
  fms_cli.register_command("wifi",              "Configure WiFi settings",        handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect",      "Connect to WiFi network",        handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart",           "Restart the system",             handle_restart_command);
  fms_cli.register_command("wifiscan_safe",     "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread",          "Read current WiFi status",           handle_wifi_read_command);
  fms_cli.register_command("wifi_test",         "Test WiFi connection",               handle_wifi_test_command);
  lanfeng.init(1,fms_uart2_serial); // add slave id

  //fms_cli.register_command("mqtt_connect","Configure Mqtt settings", handle_mqtt_command,)
  if (fms_initialize_wifi()) {  // wifi is connected
    fms_task_create();
  }
}

void loop() {
}
