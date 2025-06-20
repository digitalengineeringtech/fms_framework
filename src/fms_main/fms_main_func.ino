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


