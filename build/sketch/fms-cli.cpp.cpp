#include <Arduino.h>
#line 5 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
bool fms_debug_log_printf(const char *line,...);
#line 13 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
bool fms_cli_serial_printf(const char *line,...);
#line 21 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
bool fms_chip_info_log();
#line 26 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
bool fms_print_after_setup_info();
#line 31 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
bool fms_memory_info_log();
#line 40 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
void fms_log_task_list();
#line 18 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main.ino"
void setup();
#line 35 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main.ino"
void loop();
#line 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
void initialize_nvs_storage();
#line 20 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
void log_chip_info();
#line 38 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
bool initialize_uart2();
#line 49 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
bool initialize_wifi();
#line 59 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
void run_sd_test();
#line 69 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
void log_debug_info();
#line 76 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
void fms_pin_mode(int pin, int mode);
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_mqtt.ino"
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length);
#line 10 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_mqtt.ino"
void fms_mqtt_reconnect();
#line 25 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_mqtt.ino"
static void mqtt_task(void *arg);
#line 8 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_sd.ino"
bool fms_config_load_sd_test();
#line 13 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_sd.ino"
bool write_data_sd(char* input);
#line 24 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_sd.ino"
static void sd_task(void *arg);
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc);
#line 19 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_task.ino"
bool fms_task_create();
#line 2 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart2.ino"
bool fms_uart2_begin(bool flag, int baudrate);
#line 15 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart2.ino"
void fm_rx_irq_interrupt();
#line 29 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart2.ino"
void fms_uart2_decode(uint8_t* data, uint32_t len);
#line 36 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart2.ino"
void fms_uart2_task(void *arg);
#line 172 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart_cli.ino"
static void cli_task(void *arg);
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_web_server.ino"
static void web_server_task(void *arg);
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag);
#line 27 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_wifi.ino"
bool wifi_led_ticker();
#line 34 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_wifi.ino"
static void wifi_task(void *arg);
#line 0 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms-cli.cpp"

/*
    * FMS CLI - Command Line Interface for FMS (Firmware Management System)
    * 
    * This library provides a simple command line interface for FMS projects.
    * It allows you to register commands, process input, and respond in JSON format.
    *
    * @copyright 2025 FMS Project
    * @license MIT License
    * @author Sir Thiha Kyaw, Trion
    * original author:
    * @date 2025
    * @version 0.1.0
    * @Innovatic IOT House
*/

#include "fms-cli.h"

// Constructor
fms_cli::fms_cli(HardwareSerial& serial, const char* password) 
    : _serial(serial), 
      _buffer(""),
      _password(password ? password : ""),
      _prompt("FMS> "),
      _authenticated(password ? false : true),
      _authRequired(password ? true : false),
      _echoEnabled(true) {
}

// Initialize CLI
bool fms_cli::begin(unsigned long baudRate) {
    _serial.begin(baudRate);
    
    // Wait for serial to be ready
    vTaskDelay(pdMS_TO_TICKS(100));
    
    if (!_serial) {
        return false;
    }
    
    // Set up interrupt handler
    _serial.onReceive([this]() {
        this->process_input();
    });
    
    // Register built-in commands
    register_built_in_commands();
    
    // Print welcome message
    _serial.println("\n\r+----------------------------------+");
    _serial.println("| FMS Command Line Interface v1.0  |");
    _serial.println("+----------------------------------+");
    
    if (_authRequired && !_authenticated) {
        _serial.println("Please login with 'login <password>'");
    } else {
        _serial.print(_prompt);
    }
    
    return true;
}

// Register a command
void fms_cli::register_command(const String& name, const String& description, 
                            CommandCallback callback, uint8_t minArgs, uint8_t maxArgs) {
    cli_command_t cmd = {
        .name = name,
        .description = description,
        .callback = callback,
        .minArgs = minArgs,
        .maxArgs = maxArgs
    };
    
    _commands[name] = cmd;
}

// Process incoming data
void fms_cli::process_input() {
    while (_serial.available()) {
        char c = _serial.read();
        
        // Handle backspace
        if (c == '\b' || c == 127) {
            if (_buffer.length() > 0) {
                _buffer.remove(_buffer.length() - 1);
                if (_echoEnabled) {
                    _serial.print("\b \b"); // Erase character on terminal
                }
            }
            continue;
        }
        
        // Echo character if enabled
        if (_echoEnabled && c >= 32 && c < 127) {
            _serial.write(c);
        }
        
        // Process on newline
        if (c == '\n' || c == '\r') {
            if (_buffer.length() > 0) {
                _serial.println();
                
                String command;
                std::vector<String> args;
                
                parse_command(_buffer, command, args);
                
                // Handle login if authentication required
                if (_authRequired && !_authenticated) {
                    if (command == "login" && args.size() == 1) {
                        _authenticated = authenticate(args[0]);
                        if (_authenticated) {
                            _serial.println("Login successful");
                        } else {
                            _serial.println("Invalid password");
                        }
                    } else {
                        _serial.println("Please login with 'login <password>'");
                    }
                } else {
                    execute_command(command, args);
                }
                
                _buffer = "";
                _serial.print(_prompt);
            } else if (c == '\r' && _serial.peek() != '\n') {
                // Just a carriage return without newline
                _serial.println();
                _serial.print(_prompt);
            }
        } else if (c >= 32) {
            // Add printable characters to buffer
            _buffer += c;
        }
    }
}

// Parse command line into command and arguments
void fms_cli::parse_command(const String& cmdLine, String& command, std::vector<String>& args) {
    String trimmed = cmdLine;
    trimmed.trim();
    
    int idx = 0;
    int lastIdx = 0;
    bool inQuotes = false;
    
    // Extract command (first word)
    idx = trimmed.indexOf(' ');
    if (idx == -1) {
        command = trimmed;
        return;
    }
    
    command = trimmed.substring(0, idx);
    lastIdx = idx + 1;
    
    // Extract arguments
    while (lastIdx < trimmed.length()) {
        // Skip spaces
        while (lastIdx < trimmed.length() && trimmed[lastIdx] == ' ') {
            lastIdx++;
        }
        
        if (lastIdx >= trimmed.length()) {
            break;
        }
        
        // Check for quoted argument
        if (trimmed[lastIdx] == '"') {
            inQuotes = true;
            lastIdx++; // Skip opening quote
            idx = trimmed.indexOf('"', lastIdx);
            
            if (idx == -1) {
                // No closing quote, take rest of string
                args.push_back(trimmed.substring(lastIdx));
                break;
            }
            
            args.push_back(trimmed.substring(lastIdx, idx));
            lastIdx = idx + 1;
            inQuotes = false;
        } else {
            // Regular argument
            idx = trimmed.indexOf(' ', lastIdx);
            
            if (idx == -1) {
                // Last argument
                args.push_back(trimmed.substring(lastIdx));
                break;
            }
            
            args.push_back(trimmed.substring(lastIdx, idx));
            lastIdx = idx + 1;
        }
    }
}

// Handle authentication
bool fms_cli::authenticate(const String& password) {
    return password == _password;
}

// Execute command
void fms_cli::execute_command(const String& command, const std::vector<String>& args) {
    auto it = _commands.find(command);
    
    if (it == _commands.end()) {
        respond(command, "Command not found", false);
        return;
    }
    
    const cli_command_t& cmd = it->second;
    
    // Check argument count
    if (args.size() < cmd.minArgs) {
        respond(command, "Too few arguments", false);
        return;
    }
    
    if (args.size() > cmd.maxArgs) {
        respond(command, "Too many arguments", false);
        return;
    }
    
    // Execute command callback
    cmd.callback(args);
}

// Escape JSON string values
String fms_cli::escape_json(const String& input) {
    String output;
    output.reserve(input.length() + 10); // Reserve some extra space for potential escapes
    
    for (unsigned int i = 0; i < input.length(); i++) {
        char c = input[i];
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c < 32) {
                    // For control characters, use \uXXXX format
                    char hex[7];
                    snprintf(hex, sizeof(hex), "\\u%04x", c);
                    output += hex;
                } else {
                    output += c;
                }
        }
    }
    
    return output;
}

// Format a JSON string manually
String fms_cli::format_json(const std::map<String, String>& fields) {
    String json = "{";
    bool first = true;
    
    for (const auto& field : fields) {
        if (!first) {
            json += ",";
        }
        first = false;
        
        json += "\"" + field.first + "\":";
        
        // Check if value is a boolean or number
        if (field.second == "true" || field.second == "false" || 
            field.second == "null" || 
            (field.second.length() > 0 && 
             ((field.second[0] >= '0' && field.second[0] <= '9') || 
              field.second[0] == '-'))) {
            json += field.second;
        } else {
            json += "\"" + escape_json(field.second) + "\"";
        }
    }
    
    json += "}";
    return json;
}

// Print response in JSON format
void fms_cli::respond(const String& command, const String& result, bool success) {
    std::map<String, String> fields;
    fields["command"] = command;
    fields["result"] = result;
    fields["success"] = success ? "true" : "false";
    
    String response = format_json(fields);
    _serial.println(response);
}

// Print help
void fms_cli::print_help() {
    _serial.println("+------------------+------------------+");
    _serial.println("| Command          | Description      |");
    _serial.println("+------------------+------------------+");
    
    for (const auto& cmd : _commands) {
        _serial.printf("| %-16s | %-16s |\n", cmd.second.name.c_str(), cmd.second.description.c_str());
    }
    
    _serial.println("+------------------+------------------+");
}

// Set authentication required flag
void fms_cli::set_auth_required(bool required) {
    _authRequired = required;
    if (!required) {
        _authenticated = true;
    }
}

// Set prompt
void fms_cli::set_prompt(const String& prompt) {
    _prompt = prompt;
}

// Enable/disable echo
void fms_cli::set_echo(bool enabled) {
    _echoEnabled = enabled;
}

// Register built-in commands
void fms_cli::register_built_in_commands() {
    // Help command
    register_command("help", "Show available commands", [this](const std::vector<String>&) {
        this->print_help();
    });
    
    // Echo command
    register_command("echo", "Toggle command echo", [this](const std::vector<String>& args) {
        if (args.size() > 0) {
            if (args[0] == "on") {
                this->set_echo(true);
                this->respond("echo", "Echo enabled");
            } else if (args[0] == "off") {
                this->set_echo(false);
                this->respond("echo", "Echo disabled");
            } else {
                this->respond("echo", "Invalid argument. Use 'on' or 'off'", false);
            }
        } else {
            this->set_echo(!_echoEnabled);
            this->respond("echo", _echoEnabled ? "Echo enabled" : "Echo disabled");
        }
    }, 0, 1);
    
    // Exit/logout command
    register_command("logout", "Logout from CLI", [this](const std::vector<String>&) {
        if (_authRequired) {
            _authenticated = false;
            this->respond("logout", "Logged out");
            _serial.println("Please login with 'login <password>'");
        } else {
            this->respond("logout", "Authentication not enabled", false);
        }
    });
}

// Begin a JSON response (prints the opening bracket)
void fms_cli::begin_json_response() {
    _serial.print("{");
}

// Add a part to the JSON response
void fms_cli::add_json_response_part(const String& part) {
    _serial.print(part);
    yield(); // Allow the system to process other tasks
}

// End the JSON response (prints the closing bracket)
void fms_cli::end_json_response() {
    _serial.println("}");
}

// Execute a command directly (for testing)
bool fms_cli::execute_test_command(const String& commandLine) {
    String command;
    std::vector<String> args;
    
    parse_command(commandLine, command, args);
    
    if (command.length() == 0) {
        return false;
    }
    
    auto it = _commands.find(command);
    if (it == _commands.end()) {
        respond(command, "Command not found", false);
        return false;
    }
    
    execute_command(command, args);
    return true;
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_log.ino"
// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

bool fms_debug_log_printf(const char *line,...) { // debug log
  byte loglevel = 1;
  if (SHOW_DEBUG_SYS_LOG) {
    if (loglevel <= seriallog_level) _log_printf(line);
  }
  return true;
}

bool fms_cli_serial_printf(const char *line,...) { // uart log
  byte loglevel = 1;
  if (SHOW_RESP_UART_SYS_LOG) {
    if (loglevel <= seriallog_level) fms_cli_serial.print(line);
  }
  return true;
}

bool  fms_chip_info_log(){
  printBeforeSetupInfo();
  return true;
}

bool fms_print_after_setup_info(){
  printAfterSetupInfo();
  return true;
}

bool fms_memory_info_log(){
// Check free heap size
    size_t freeHeap = esp_get_free_heap_size();
    fms_debug_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    fms_debug_log_printf("Stack High Water Mark: %u bytes\n", stackHighWaterMark);
  return true;
}

void fms_log_task_list() {
  char buffer[512]; // Buffer for task list output
  fms_debug_log_printf("Task List:\n");
  fms_debug_log_printf("Name          State       Prio      Stack        Num \n\r");
  vTaskList(buffer);
  fms_debug_log_printf(buffer);
}


#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main.ino"
/*
  *  FMS main source file
  *  Author: Trion
  *  Date: 2025
  *  Guided By Senior Engineer : Sir Thiha Kyaw
  *  Description: This file is the main source file for FMS project.
*/

#include "fms_header.h"
#include "fms_debug.h"
#include "fms-cli.h"

fms_cli fms_cli_serial(Serial, "admin"); // CLI serial object
bool testModeActive = false;
const unsigned long WIFI_TIMEOUT = 20000;

/* Main function */
void setup() {

   fms_pin_mode(2, OUTPUT); // pinmode
  
   //initialize_uart1(); // for cli terminal
   initialize_uart2(); // Initialize UART2 for rf485 log
   initialize_nvs_storage(); // Save boot count to NVS storage
   fms_debug_log_printf("CPU %d\t: Starting up...\n\r", app_cpu);
   if (initialize_wifi()) {
       fms_debug_log_printf(" [WiFi] wifi .. connected\n\r");
       fms_task_create(); // RTOS task create
   }
   run_sd_test();
 

}

void loop() {
    // user main code here
}


#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_main_func.ino"
/*
    * fms_main_func.ino
    * 
    * This file is part of the ESP32 FMS 
    * main function link file
*/



void initialize_nvs_storage() {
  fms_nvs_storage.begin("fms_config", false);
  sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
  sysCfg.bootcount++;
  app_cpu = xPortGetCoreID();
  FMS_LOG_INFO("CPU %d: Boot count: %lu", app_cpu, sysCfg.bootcount);
  fms_nvs_storage.putUInt("bootcount", sysCfg.bootcount);
  fms_nvs_storage.end(); // close nvs storage
}

void log_chip_info() {
  #if SHOW_FMS_CHIP_INFO_LOG
  fms_chip_info_log();
  fms_memory_info_log();
  #endif
}

// bool initialize_uart1() {
//   if (fms_uart_cli_begin(use_uart_command, 115200)) {
//     fms_cli_serial.onReceive(UART_RX_IRQ); // uart interrupt function 
//     FMS_LOG_INFO("[FMSUART1] UART1 CLI.. DONE");
//     return true;
//   } else {
//     FMS_LOG_ERROR("[FMSUART1] UART1 CLI.. FAIL");
//     return false;
//   }
// }

bool initialize_uart2() {
  if (fms_uart2_begin(use_serial1, 115200)) {
    fms_uart2_serial.onReceive(fm_rx_irq_interrupt); // uart interrupt function
    FMS_LOG_INFO("[FMSUART2] UART2.. DONE"); 
    return true;
  } else {
    FMS_LOG_ERROR("[FMSUART2] UART2.. FAIL"); 
    return false;
  }
}

bool initialize_wifi() {
  if (initialize_fms_wifi(wifi_start_event)) {
    FMS_LOG_INFO("Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  } else {
    FMS_LOG_WARNING("Failed to connect to WiFi");
    return false;
  }
}

void run_sd_test() {
  #if SHOW_SD_TEST_LOG
  if (fms_config_load_sd_test()) {
    FMS_LOG_INFO("==================== sd card test success================");
  } else {
    FMS_LOG_ERROR("sd card test failed");
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

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_mqtt.ino"

void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char payload_str[length + 1];
  memcpy(payload_str, payload, length);
  payload_str[length] = '\0';
  FMS_LOG_INFO("Message arrived on topic [%s]: %s", topic, payload_str);
}


void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()){
    FMS_LOG_INFO("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(DEVICE_ID) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str())){
      FMS_LOG_INFO("Connected to MQTT server");
      fms_mqtt_client.subscribe("fms/test/data");
       // Add additional topic subscriptions if necessary
    } else {
      FMS_LOG_WARNING("Failed to connect to MQTT server , rc = %d try again in 5 second",fms_mqtt_client.state());
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

static void mqtt_task(void *arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(MQTT_SERVER,1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while(mqttTask){
    fms_mqtt_client.loop();
    if(!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
    }
    else FMS_LOG_INFO("Connected to MQTT server");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_sd.ino"

/*
  * fms_sd.cpp
  *
  *  Created on: 2020. 12. 10.
  *   author : thet htar khaing
*/
bool fms_config_load_sd_test() {

return true;
}

bool write_data_sd(char* input)
{
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)


  return true;
}

static void sd_task(void *arg) {
  BaseType_t rc;
  while(1) {
  
    /*
    * Load config data from sd card
    */
    
    //rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}


#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_task.ino"
bool create_task(TaskFunction_t task_func, const char* name, uint32_t stack_size, UBaseType_t priority, TaskHandle_t* handle, BaseType_t& rc) {
  rc = xTaskCreatePinnedToCore(
    task_func,  // Task function
    name,       // Name
    stack_size, // Stack size
    nullptr,    // Parameters
    priority,   // Priority
    handle,     // Handle
    app_cpu     // CPU
  );
  assert(rc == pdPASS);
  if (rc != pdPASS) {
    fms_debug_log_printf("[TASK] %s task created fail", name);
    return false;
  }
  return true;
}

bool fms_task_create() {
  BaseType_t sd_rc, wifi_rc, mqtt_rc, cli_rc, uart2_rc, webserver_rc;

  if (!create_task(sd_task, "sdcard", 3000, 2, &hsdCardTask, sd_rc)) return false;
  if (!create_task(wifi_task, "wifi", 3000, 3, &hwifiTask, wifi_rc)) return false;
  if (!create_task(mqtt_task, "mqtt", 3000, 3, &hmqttTask, mqtt_rc)) return false;
  if (!create_task(cli_task, "cli", 3000, 1, &hcliTask, cli_rc)) return false;
  if (!create_task(fms_uart2_task, "uart2", 3000, 1, &huart2Task, uart2_rc)) return false;
  if (!create_task(web_server_task, "webserver", 3000, 4, &hwebServerTask, webserver_rc)) return false;

  return true;
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart2.ino"

bool fms_uart2_begin(bool flag, int baudrate) {
  if (flag) {
    fms_uart2_serial.begin(baudrate, SERIAL_8N1, 16, 17);
    if(fms_uart2_serial){
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
      return true;
    } else {
      return false;
    }
  }
}


void fm_rx_irq_interrupt() { // interrupt RS485/RS232 function
  uint8_t Buffer[50];
  int bytes_received = 0;
  uint16_t size = fms_uart2_serial.available(); // serial.available  // #define fms_cli_serial Serial
  fms_uart2_serial.printf("Got byes on serial : %d\n",size);
  while(fms_uart2_serial.available()) {
    yield();
    Buffer[bytes_received] = fms_uart2_serial.read();
    bytes_received++; 
  }
  fms_uart2_serial.printf("\n uart2  data process \n\r");
  fms_uart2_decode(Buffer, size); // decode uart2 data main function
}

void fms_uart2_decode(uint8_t* data, uint32_t len) {
  FMS_LOG_DEBUG("[FMSUART2] Received : %s\n\r", data);
}



// free rtos task
void fms_uart2_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_uart_cli.ino"
// Last update: 2021-08-29 20:00:00
// bool fms_uart_cli_begin(bool flag, int baudrate) {
//   if (flag) {
//     fms_cli_serial.begin(baudrate);
//     if(fms_cli_serial){
//       vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
//       return true;
//     } else {
//       return false;
//     }
//   }
// }

// void fms_CmndDebug(){

// }

// void fms_CmndAddDeviceId() {

// }

// void fms_CmndStroagecheck() {
//   nvs_stats_t nvs_stats;
//   nvs_get_stats(NULL,&nvs_stats);
//   size_t freeHeap = esp_get_free_heap_size();
//   UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//   fms_cli_serial.printf("{\"total\":%d,\"used\":%d,\"free\":%d,\"free_heap\":%u,\"stack_high_water_mark\":%u}\n", 
//   nvs_stats.total_entries, nvs_stats.used_entries, nvs_stats.free_entries,freeHeap, stackHighWaterMark);
// }

// void fms_Cmndhelp() {
//     fms_cli_serial.println("+------------------+------------------+");
//     fms_cli_serial.println("| Command         | Description       |");
//     fms_cli_serial.println("+------------------+------------------+");
//     for (const auto& cmd : Commands) {
//       fms_cli_serial.printf("| %-16s | %-16s |\n", cmd.name, "Executes Command");
//     }
//     fms_cli_serial.println("+------------------+------------------+");
// }

// void fms_CmndBootCount() {
//   fms_nvs_storage.begin("fms_config", false);
//   sysCfg.bootcount = fms_nvs_storage.getUInt("bootcount", 0);
//   fms_response_cmnd_handler(String(sysCfg.bootcount).c_str());
//   fms_nvs_storage.end(); // close nvs storage
// }

// void fms_CmndWifi() {
//  char ssid[32] = "ssid";
//  char password[64] = "password";
//   if(sscanf(fmsMailBox.data.c_str(),"ssid:\"%31[^\"]\" password:\"%63[^\"]\"", ssid, password) == 2) {
//     strncpy(sysCfg.wifi_ssid, ssid, sizeof(sysCfg.wifi_ssid)-1);
//     strncpy(sysCfg.wifi_password, password, sizeof(sysCfg.wifi_password)-1);
//     if(SHOW_RESP_UART_SYS_LOG) {
//       fms_cli_serial.printf("[FMICLI] WIFI SSID : %s\n", String(sysCfg.wifi_ssid));
//       fms_cli_serial.printf("[FMICLI] WIFI PASSWORD : %s\n", String(sysCfg.wifi_password));
//     }
//     fms_nvs_storage.begin("fms_config", false);
//     fms_nvs_storage.putString("ssid",sysCfg.wifi_ssid);
//     fms_nvs_storage.putString("pass",sysCfg.wifi_password);
//     fms_nvs_storage.end();
//     fms_response_cmnd_handler("true");
//     fms_CmndRestart();
//   } else {
//     fms_response_cmnd_handler("[FMICLI] Invalid format. Use: wifi \"your_ssid\" \"your_password\"");
//   }
// }

// #define SCAN_COUNT 1                 // Number of scan iterations
// void fms_CmndWifiScan() {
//   WiFi.mode(WIFI_STA);
//  // WiFi.disconnect();                // Disconnect from any network
//     char buffer[512];               // Buffer for JSON output
//     strcpy(buffer, "{\"wifiscan\":true,\"networks\":[");
//     int bufferLen = strlen(buffer);
//     int networkIndex = 0;
//     for (int scanNum = 0; scanNum < SCAN_COUNT; scanNum++) {
//         int numNetworks = WiFi.scanNetworks();
//         for (int i = 0; i < numNetworks; i++) {
//             if (networkIndex > 0) strcat(buffer, ","); // Add comma for JSON formatting
//             char entry[128];
//             snprintf(entry, sizeof(entry),
//                 "{\"Nr\":%d,\"SSID\":\"%s\",\"RSSI\":%d,\"CH\":%d,\"Encryption\":\"%s\"}",
//                 networkIndex + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
//                 WiFi.channel(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "WPA+WPA2"
//             );
//             strcat(buffer, entry);
//             bufferLen += strlen(entry);
//             networkIndex++;
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating // similar delay(1000)
//     }
//     WiFi.scanDelete(); // Free memory
//     strcat(buffer, "]}"); // Close JSON array
//     if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(buffer); // Output JSON result
// }

// void fms_CmndRestart() {
//   vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for 1 second before repeating
//   fms_debug_log_printf("[DEBUG RST] Restarting...\n\r");
//   fms_response_cmnd_handler("true");
//   ESP.restart();
// }

// void fms_CmndWifiRead() {
//   if(WiFi.status() == WL_CONNECTED) {
//     char entry[128];
//     snprintf(entry, sizeof(entry),
//         "{\"SSID\":\"%s\",\"RSSI\":%d,\"IP\":\"%s\"}",
//         WiFi.SSID().c_str(), WiFi.RSSI(),
//         WiFi.localIP().toString().c_str()
//     );
//   if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.println(String(entry).c_str());
//   fms_response_cmnd_handler("Read OK");
//   } else {
//     fms_response_cmnd_handler("false");
//   }
// }

// void fms_CmndMqtt() {
//   fms_response_cmnd_handler("mqttcommand");
// }

// void fms_response_cmnd_handler(const char* result){
//   if(SHOW_RESP_UART_SYS_LOG) {
//     fms_cli_serial.print(F("{\""));
//     fms_cli_serial.print(fmsMailBox.command);
//     fms_cli_serial.print(F("\":\""));
//     fms_cli_serial.print(result);
//     fms_cli_serial.print(F("\"}\n"));
//   }
// }

// void  fms_cli_command_decode(String cmdLine) {  
//   char c;
//   char buffer[32]; // for testing
//     if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
//     cmdLine.trim(); // Remove leading and trailing whitespace from this command line
//     int spaceIndex = cmdLine.indexOf(' ');
//     if(spaceIndex == -1){
//       fmsMailBox.command = cmdLine;
//       if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Received : %s\n\r", cmdLine.c_str());
//       fmsMailBox.data = "";
//     }else {
//       fmsMailBox.command = cmdLine.substring(0, spaceIndex);
//       fmsMailBox.data = cmdLine.substring(spaceIndex + 1);
//       if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] COMMAND : %s , Data : %s \n", fmsMailBox.command.c_str(),fmsMailBox.data.c_str());
//     }
//     fmsMailBox.data_len = fmsMailBox.data.length();
//      for (uint32_t i = 0; i < sizeof(Commands) / sizeof(COMMAND); i++) {
//     if (strcasecmp(fmsMailBox.command.c_str(), Commands[i].name) == 0) {
//       Commands[i].function();
//       return;
//     }
//   }
//     if(SHOW_RESP_UART_SYS_LOG) fms_cli_serial.printf("[FMSCLI] Command not found\n\r");
  
// }

// void UART_RX_IRQ() { // interrupt function
//   String cmd_ ;
//   uint16_t size = fms_cli_serial.available(); // serial.available  // #define fms_cli_serial Serial
//   fms_cli_serial.printf("Got byes on serial : %d\n",size);
//   while(fms_cli_serial.available()) {
//     yield();
//      cmd_ = fms_cli_serial.readStringUntil('\n'); 
//   }
//   fms_cli_serial.printf("\n cli terminal data process \n\r");
//   fms_cli_command_decode(cmd_);
// }

static void cli_task(void *arg) {
  BaseType_t rc;
  while (1) {
  
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_web_server.ino"
static void web_server_task(void *arg) {
  // low 
  BaseType_t rc;
  for (;;) {
   
   
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
#line 1 "d:\\FMS Framework\\development_version\\fms_framework\\src\\fms_wifi.ino"
bool initialize_fms_wifi(bool flag) {
  if(flag) {
    // get ssid and password from nvs storage 
    fms_nvs_storage.begin("fms_config", false);
    String ssid_str = fms_nvs_storage.getString("ssid");
    String pass_str = fms_nvs_storage.getString("pass");
    fms_nvs_storage.end();

    strncpy(sysCfg.wifi_ssid, ssid_str.c_str(), sizeof(sysCfg.wifi_ssid) - 1);
    strncpy(sysCfg.wifi_password, pass_str.c_str(), sizeof(sysCfg.wifi_password) - 1);

    if(sysCfg.wifi_ssid == " " || sysCfg.wifi_password == " ") {
      FMS_LOG_ERROR("[DEBUG WiFi] wifi .. credential .. value is empty");
      return false;
    }
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true); // auto reconnect function
    WiFi.begin(sysCfg.wifi_ssid, sysCfg.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      FMS_LOG_INFO("WiFi initialized, connecting to %s... wpa:%s", sysCfg.wifi_ssid,sysCfg.wifi_password);
      vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
    }
    return true;
  }
}

bool wifi_led_ticker() {
  static bool state = false;
  gpio_set_level(GPIO_NUM_2,state);
  state = !state;
}

uint8_t count = 1;
static void wifi_task(void *arg) {
  BaseType_t rc;
  while(1) {
    if(WiFi.status() != WL_CONNECTED) {
      FMS_LOG_WARNING("Failed to connect to WiFi");
      gpio_set_level(GPIO_NUM_2,HIGH);
      vTaskDelay(pdMS_TO_TICKS(500));
      gpio_set_level(GPIO_NUM_2,LOW);
      vTaskDelay(pdMS_TO_TICKS(500));
    } else {
      FMS_LOG_INFO("Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
      gpio_set_level(GPIO_NUM_2,HIGH);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}

