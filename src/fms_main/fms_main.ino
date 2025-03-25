/*
     FMS main source file
     Author: Trion
     Date: 2025
     Guided By Senior Engineer : Sir Thiha Kyaw
     Description: This file is the main source file for FMS project.
*/

#include "_fms_main.h"
#include "_fms_cli.h"
#include "_fms_debug.h"
#include "_fms_filemanager.h"
#include "_fms_json_helper.h"
FMS_FileManager fileManager;

fms_cli fms_cli(Serial, "admin");  // Use "admin" as the default password change your admin pass here
WebSocketsServer webSocket(81);

/* Main function */
void setup() {
  fms_pin_mode(BUILTIN_LED, OUTPUT);
  fms_cli.begin(115200);
  
  initialize_nvs_storage();
  
  if (!LittleFS.begin(true)) {
    Serial.println("Failed to mount file system");
  } else {
    Serial.println("File system mounted");
  }

  fms_cli.register_command("wifi", "Configure WiFi settings", handle_wifi_command, 2, 2);
  fms_cli.register_command("wifi_connect", "Connect to WiFi network", handle_wifi_connect_command, 2, 2);
  fms_cli.register_command("restart", "Restart the system", handle_restart_command);
  fms_cli.register_command("wifiscan_safe", "Scan for WiFi networks (safe mode)", handle_wifi_scan_safe_command);
  fms_cli.register_command("wifiread", "Read current WiFi status", handle_wifi_read_command);
  fms_cli.register_command("wifi_test", "Test WiFi connection", handle_wifi_test_command);
  fms_cli.register_command("test", "Run fms_cli tests", handle_test_command);

  initialize_uart2();

  if (initialize_wifi()) {
    fms_task_create();
    ticker.detach();
  }

  // FMS_LOG_INFO("Device IP address: ");
  // FMS_LOG_INFO(String(WiFi.localIP()).c_str());
  // FMS_LOG_INFO("Firmware version: ");
  // Serial.println(_firmware_version);
  run_sd_test();
}

void loop() {
}
