

#include "Redstar.h"

// Create a Redstar instance using Serial1
Redstar dispenser(Serial1);

// Nozzle ID to use
const uint8_t NOZZLE_ID = 1;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial to be ready
  }
  Serial.println(F("Redstar Fuel Dispenser Example"));
  
  // Initialize the dispenser with a baud rate of 9600 and debug enabled
  dispenser.begin(9600, true);
  
  // Set a timeout of 2 seconds for responses
  dispenser.setTimeout(2000);
}

void loop() {
  // Read the state of the nozzle
  Serial.println(F("\n--- Reading Nozzle State ---"));
  if (dispenser.readState(NOZZLE_ID)) {
    if (dispenser.waitForResponse()) {
      RedstarResponse response = dispenser.getLastResponse();
      printState(response.state);
    } else {
      Serial.println(F("No response received"));
    }
  } else {
    Serial.println(F("Failed to send command"));
  }
  
  delay(1000);
  
  // Read the price
  Serial.println(F("\n--- Reading Price ---"));
  if (dispenser.readPrice(NOZZLE_ID)) {
    if (dispenser.waitForResponse()) {
      RedstarResponse response = dispenser.getLastResponse();
      Serial.print(F("Price: "));
      Serial.println(response.price);
    } else {
      Serial.println(F("No response received"));
    }
  } else {
    Serial.println(F("Failed to send command"));
  }
  
  delay(1000);
  
  // Read the total
  Serial.println(F("\n--- Reading Total ---"));
  if (dispenser.readTotal(NOZZLE_ID)) {
    if (dispenser.waitForResponse()) {
      RedstarResponse response = dispenser.getLastResponse();
      Serial.print(F("Total Volume: "));
      Serial.println(response.totalVolume);
      Serial.print(F("Total Amount: "));
      Serial.println(response.totalAmount);
    } else {
      Serial.println(F("No response received"));
    }
  } else {
    Serial.println(F("Failed to send command"));
  }
  
  delay(1000);
  
  // If nozzle is lifted, send approval
  Serial.println(F("\n--- Checking if approval needed ---"));
  if (dispenser.readState(NOZZLE_ID)) {
    if (dispenser.waitForResponse()) {
      RedstarResponse response = dispenser.getLastResponse();
      if (response.state == REDSTAR_STATE_NOZZLE_LIFTED) {
        Serial.println(F("Nozzle is lifted, sending approval"));
        if (dispenser.sendApproval(NOZZLE_ID)) {
          Serial.println(F("Approval sent"));
        } else {
          Serial.println(F("Failed to send approval"));
        }
      } else {
        Serial.println(F("Nozzle not lifted, no approval needed"));
      }
    }
  }
  
  // Wait 5 seconds before next cycle
  delay(5000);
}

// Helper function to print the state in a human-readable format
void printState(RedstarState state) {
  Serial.print(F("State: "));
  switch (state) {
    case REDSTAR_STATE_IDLE:
      Serial.println(F("Idle"));
      break;
    case REDSTAR_STATE_NOZZLE_LIFTED:
      Serial.println(F("Nozzle Lifted"));
      break;
    case REDSTAR_STATE_FUELING:
      Serial.println(F("Fueling"));
      break;
    case REDSTAR_STATE_FUELING_COMPLETE:
      Serial.println(F("Fueling Complete"));
      break;
    case REDSTAR_STATE_ERROR:
      Serial.println(F("Error"));
      break;
    default:
      Serial.println(F("Unknown"));
  }
}