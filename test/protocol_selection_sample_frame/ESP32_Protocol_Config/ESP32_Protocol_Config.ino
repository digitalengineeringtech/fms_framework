#include <Arduino.h>
#include <Preferences.h>
#include "Lanfeng.h"
#include "Huangyang.h"
#include "Redstar.h"
#include "Tasuno.h"

// Protocol definitions
#define PROTOCOL_LANFENG 0
#define PROTOCOL_HUANGYANG 1
#define PROTOCOL_REDSTAR 2
#define PROTOCOL_TASUNO 3

// Default settings
#define DEFAULT_PROTOCOL 0
#define DEFAULT_NOZZLES 1
#define DEFAULT_BAUD_RATE 115200

// Global variables
Preferences preferences;
int currentProtocol = DEFAULT_PROTOCOL;
int nozzleCount = DEFAULT_NOZZLES;
long baudRate = DEFAULT_BAUD_RATE;
bool configMode = true;

// Protocol instances
LanfengProtocol lanfengProtocol;
HuangyangProtocol huangyangProtocol;
RedstarProtocol redstarProtocol;
TasunoProtocol tasunoProtocol;

// Protocol names for display
const char* protocolNames[] = {
  "Lanfeng",
  "Huangyang",
  "Redstar",
  "Tasuno"
};

// Available baud rates
const long baudRates[] = { 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 };
const int numBaudRates = 8;

// Function declarations
void loadSettings();
void saveSettings();
void showMenu();
void printCurrentSettings();
void handleCommand(char cmd);
void handlePythonCommand(const char* cmd);
void initializeSelectedProtocol();
void runSelectedProtocol();
void initLanfengProtocol();
void runLanfengProtocol();
void initHuangyangProtocol();
void runHuangyangProtocol();
void initRedstarProtocol();
void runRedstarProtocol();
void initTasunoProtocol();
void runTasunoProtocol();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize preferences
  preferences.begin("protocol-cfg", false);

  // Load saved settings
  loadSettings();

  // Check if we should enter config mode (you could use a button instead)
  delay(1000);
  if (configMode) {

    Serial.println("\n\n*** Configuration Mode ***");
    showMenu();
  } else {
    Serial.println("Running with current settings:");
    printCurrentSettings();
    Serial.println("Send any character within 5 seconds to enter config mode");

    // Give 5 seconds to enter config mode
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
      if (Serial.available()) {
        configMode = true;
        Serial.println("\n\n*** Configuration Mode ***");
        while (Serial.available()) Serial.read();  // Clear buffer
        showMenu();
        break;
      }
      delay(100);
    }

    if (!configMode) {
      Serial.println("Starting normal operation...");
      initializeSelectedProtocol();
    }
  }
}

void loop() {
  if (configMode) {
    // Handle configuration commands
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');

      // Check if it's a Python GUI command (contains ':')
      if (input.indexOf(':') >= 0) {
        handlePythonCommand(input.c_str());
      } else if (input.length() == 1) {
        // Legacy single-character command
        char cmd = input.charAt(0);
        handleCommand(cmd);
      } else {
        // Unknown command format
        Serial.println("Unknown command format");
        showMenu();
      }
    }
  } else {
    // Normal operation - handle the selected protocol
    runSelectedProtocol();
  }
}

void handlePythonCommand(const char* cmd) {
  // Command format: CMD:PARAM
  char command[32];
  char param[32];
  int parsed = sscanf(cmd, "%[^:]:%s", command, param);

  if (parsed < 1) return;

  Serial.print("Received command: ");
  Serial.print(command);

  if (parsed > 1) {
    Serial.print(" with param: ");
    Serial.println(param);
  } else {
    Serial.println();
  }

  // Handle protocol selection
  if (strcmp(command, "PROTOCOL") == 0) {
    int proto = atoi(param);
    if (proto >= 1 && proto <= 4) {
      currentProtocol = proto - 1;
      Serial.print("Protocol set to: ");
      Serial.println(protocolNames[currentProtocol]);
    } else {
      Serial.println("Invalid protocol number. Must be between 1 and 4.");
    }
  }
  // Handle nozzle count
  else if (strcmp(command, "NOZZLE") == 0) {
    int count = atoi(param);
    if (count >= 1 && count <= 10) {
      nozzleCount = count;
      Serial.print("Nozzle count set to: ");
      Serial.println(nozzleCount);
    } else {
      Serial.println("Invalid nozzle count. Must be between 1 and 10.");
    }
  }
  // Handle baud rate
  else if (strcmp(command, "BAUD") == 0) {
    long baud = atol(param);
    bool validBaud = false;
    for (int i = 0; i < numBaudRates; i++) {
      if (baud == baudRates[i]) {
        baudRate = baud;
        Serial.print("Baud rate set to: ");
        Serial.println(baudRate);
        validBaud = true;
        break;
      }
    }
    if (!validBaud) {
      Serial.println("Invalid baud rate. Using default.");
    }
  }
  // Handle save command
  else if (strcmp(command, "SAVE") == 0) {
    saveSettings();
  }
  // Handle status request
  else if (strcmp(command, "STATUS") == 0) {
    printCurrentSettings();
  }
  // Handle exit config mode
  else if (strcmp(command, "EXIT") == 0) {
    configMode = false;
    Serial.println("Exiting config mode and starting normal operation...");
    initializeSelectedProtocol();
  }
  // Handle debug mode
  else if (strcmp(command, "DEBUG") == 0) {
    bool enableDebug = (strcmp(param, "ON") == 0 || strcmp(param, "1") == 0);
    Serial.print("Debug mode: ");
    Serial.println(enableDebug ? "ON" : "OFF");

    // Set debug mode for all protocols
    lanfengProtocol.setDebug(enableDebug);
    huangyangProtocol.setDebug(enableDebug);
    redstarProtocol.setDebug(enableDebug);
    tasunoProtocol.setDebug(enableDebug);
  }
  // Handle help command
  else if (strcmp(command, "HELP") == 0) {
    showMenu();
  } else {
    Serial.println("Unknown command");
    showMenu();
  }
}

void loadSettings() {
  currentProtocol = preferences.getInt("protocol", DEFAULT_PROTOCOL);
  nozzleCount = preferences.getInt("nozzles", DEFAULT_NOZZLES);
  baudRate = preferences.getLong("baudrate", DEFAULT_BAUD_RATE);

  // Validate settings
  if (currentProtocol < 0 || currentProtocol > 3) currentProtocol = DEFAULT_PROTOCOL;
  if (nozzleCount < 1 || nozzleCount > 10) nozzleCount = DEFAULT_NOZZLES;

  // Validate baud rate
  bool validBaud = false;
  for (int i = 0; i < numBaudRates; i++) {
    if (baudRate == baudRates[i]) {
      validBaud = true;
      break;
    }
  }
  if (!validBaud) baudRate = DEFAULT_BAUD_RATE;
}

void saveSettings() {
  preferences.putInt("protocol", currentProtocol);
  preferences.putInt("nozzles", nozzleCount);
  preferences.putLong("baudrate", baudRate);
  Serial.println("Settings saved!");
}

void showMenu() {
  Serial.println("\n--- Protocol Configuration Menu ---");
  Serial.println("1-4: Select Protocol");
  Serial.println("  1: Lanfeng");
  Serial.println("  2: Huangyang");
  Serial.println("  3: Redstar");
  Serial.println("  4: Tasuno");
  Serial.println("n: Set Nozzle Count");
  Serial.println("b: Change Baud Rate");
  Serial.println("s: Save Settings");
  Serial.println("p: Print Current Settings");
  Serial.println("x: Exit Config Mode and Start");
  Serial.println("\nPython GUI Commands:");
  Serial.println("PROTOCOL:n - Set protocol (n=1-4)");
  Serial.println("NOZZLE:n - Set nozzle count (n=1-10)");
  Serial.println("BAUD:n - Set baud rate");
  Serial.println("SAVE: - Save settings");
  Serial.println("STATUS: - Print current settings");
  Serial.println("DEBUG:ON/OFF - Enable/disable debug mode");
  Serial.println("EXIT: - Exit config mode and start");
  Serial.println("HELP: - Show this menu");
  Serial.println("------------------------------------");
  printCurrentSettings();
}

void printCurrentSettings() {
  Serial.println("\nCurrent Settings:");
  Serial.print("Protocol: ");
  Serial.println(protocolNames[currentProtocol]);
  Serial.print("Nozzle Count: ");
  Serial.println(nozzleCount);
  Serial.print("Baud Rate: ");
  Serial.println(baudRate);
  Serial.println();
}

void handleCommand(char cmd) {
  switch (cmd) {
    case '1':
    case '2':
    case '3':
    case '4':
      currentProtocol = cmd - '1';
      Serial.print("Protocol set to: ");
      Serial.println(protocolNames[currentProtocol]);
      break;

    case 'n':
    case 'N':
      Serial.println("Enter nozzle count (1-10):");
      while (!Serial.available()) delay(10);
      if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        int newCount = input.toInt();
        if (newCount >= 1 && newCount <= 10) {
          nozzleCount = newCount;
          Serial.print("Nozzle count set to: ");
          Serial.println(nozzleCount);
        } else {
          Serial.println("Invalid nozzle count. Must be between 1 and 10.");
        }
      }
      break;

    case 'b':
    case 'B':
      Serial.println("Select baud rate:");
      for (int i = 0; i < numBaudRates; i++) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(baudRates[i]);
      }

      while (!Serial.available()) delay(10);
      if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        int selection = input.toInt();
        if (selection >= 1 && selection <= numBaudRates) {
          baudRate = baudRates[selection - 1];
          Serial.print("Baud rate set to: ");
          Serial.println(baudRate);
        } else {
          Serial.println("Invalid selection.");
        }
      }
      break;

    case 's':
    case 'S':
      saveSettings();
      break;

    case 'p':
    case 'P':
      printCurrentSettings();
      break;

    case 'x':
    case 'X':
      configMode = false;
      Serial.println("Exiting config mode and starting normal operation...");
      initializeSelectedProtocol();
      break;

    default:
      if (cmd != '\n' && cmd != '\r') {
        Serial.println("Unknown command");
        showMenu();
      }
      break;
  }
}

void initializeSelectedProtocol() {
  // Close the current serial connection
  Serial.end();

  // Reopen with the selected baud rate
  Serial.begin(baudRate);
  delay(100);

  Serial.println("\n--- Initializing Protocol ---");
  Serial.print("Protocol: ");
  Serial.println(protocolNames[currentProtocol]);
  Serial.print("Nozzle Count: ");
  Serial.println(nozzleCount);
  Serial.print("Baud Rate: ");
  Serial.println(baudRate);

  // Protocol-specific initialization
  switch (currentProtocol) {
    case PROTOCOL_LANFENG:
      initLanfengProtocol();
      break;
    case PROTOCOL_HUANGYANG:
      initHuangyangProtocol();
      break;
    case PROTOCOL_REDSTAR:
      initRedstarProtocol();
      break;
    case PROTOCOL_TASUNO:
      initTasunoProtocol();
      break;
  }
}

void runSelectedProtocol() {
  // Protocol-specific operation
  switch (currentProtocol) {
    case PROTOCOL_LANFENG:
      runLanfengProtocol();
      break;
    case PROTOCOL_HUANGYANG:
      runHuangyangProtocol();
      break;
    case PROTOCOL_REDSTAR:
      runRedstarProtocol();
      break;
    case PROTOCOL_TASUNO:
      runTasunoProtocol();
      break;
  }
}

// Lanfeng protocol implementation
void initLanfengProtocol() {
  Serial.println("Initializing Lanfeng protocol...");

  // Enable debug output
  lanfengProtocol.setDebug(true);

  // Initialize the protocol with the configured nozzle count
  if (lanfengProtocol.begin(nozzleCount, Serial)) {
    Serial.println("Lanfeng protocol initialized successfully");

    // Example: Get initial status
    uint8_t status = lanfengProtocol.getStatus();
    if (status != 0xFF) {
      Serial.print("Initial status: 0x");
      Serial.println(status, HEX);
    } else {
      Serial.print("Failed to get status. Error: ");
      Serial.println(lanfengProtocol.getErrorMessage(lanfengProtocol.getLastError()));
    }
  } else {
    Serial.print("Failed to initialize Lanfeng protocol. Error: ");
    Serial.println(lanfengProtocol.getErrorMessage(lanfengProtocol.getLastError()));
  }
}

void runLanfengProtocol() {
  // Update the protocol handler to process incoming data
  lanfengProtocol.update();

  // Add your application-specific code here
  // For example, you could implement a state machine to control the flow
  // or respond to user commands from a different interface
}

// Huangyang protocol implementation
void initHuangyangProtocol() {
  Serial.println("Initializing Huangyang protocol...");

  // Enable debug output
  huangyangProtocol.setDebug(true);

  // Initialize the protocol with the configured nozzle count
  if (huangyangProtocol.begin(nozzleCount, Serial)) {
    Serial.println("Huangyang protocol initialized successfully");

    // Example: Get initial status
    uint8_t status = huangyangProtocol.getStatus();
    if (status != 0xFF) {
      Serial.print("Initial status: 0x");
      Serial.println(status, HEX);
    } else {
      Serial.print("Failed to get status. Error: ");
      Serial.println(huangyangProtocol.getErrorMessage(huangyangProtocol.getLastError()));
    }
  } else {
    Serial.print("Failed to initialize Huangyang protocol. Error: ");
    Serial.println(huangyangProtocol.getErrorMessage(huangyangProtocol.getLastError()));
  }
}

void runHuangyangProtocol() {
  // Update the protocol handler to process incoming data
  huangyangProtocol.update();

  // Add your application-specific code here
}

// Redstar protocol implementation
void initRedstarProtocol() {
  Serial.println("Initializing Redstar protocol...");

  // Enable debug output
  redstarProtocol.setDebug(true);

  // Initialize the protocol with the configured nozzle count
  if (redstarProtocol.begin(nozzleCount, Serial)) {
    Serial.println("Redstar protocol initialized successfully");

    // Example: Get initial status
    uint8_t status = redstarProtocol.getStatus();
    if (status != 0xFF) {
      Serial.print("Initial status: 0x");
      Serial.println(status, HEX);
    } else {
      Serial.print("Failed to get status. Error: ");
      Serial.println(redstarProtocol.getErrorMessage(redstarProtocol.getLastError()));
    }
  } else {
    Serial.print("Failed to initialize Redstar protocol. Error: ");
    Serial.println(redstarProtocol.getErrorMessage(redstarProtocol.getLastError()));
  }
}

void runRedstarProtocol() {
  // Update the protocol handler to process incoming data
  redstarProtocol.update();

  // Add your application-specific code here
}

// Tasuno protocol implementation
void initTasunoProtocol() {
  Serial.println("Initializing Tasuno protocol...");

  // Enable debug output
  tasunoProtocol.setDebug(true);

  // Initialize the protocol with the configured nozzle count
  if (tasunoProtocol.begin(nozzleCount, Serial)) {
    Serial.println("Tasuno protocol initialized successfully");

    // Example: Get initial status
    uint8_t status = tasunoProtocol.getStatus();
    if (status != 0xFF) {
      Serial.print("Initial status: 0x");
      Serial.println(status, HEX);
    } else {
      Serial.print("Failed to get status. Error: ");
      Serial.println(tasunoProtocol.getErrorMessage(tasunoProtocol.getLastError()));
    }
  } else {
    Serial.print("Failed to initialize Tasuno protocol. Error: ");
    Serial.println(tasunoProtocol.getErrorMessage(tasunoProtocol.getLastError()));
  }
}

void runTasunoProtocol() {
  // Update the protocol handler to process incoming data
  tasunoProtocol.update();

  // Add your application-specific code here
}