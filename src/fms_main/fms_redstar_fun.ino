#include <src/Redstar.h>
Redstar redstar(fms_uart2_serial);  // Create an instance of the Redstar class
void red_star_init() {
  redstar.begin(9600, true, RXD2, TXD2);  // Initialize the Redstar object with the specified baud rate and pins
}

void red_star_main() {
   
    // Example usage of the Redstar class
    uint8_t nozzleId = 1;  // Example nozzle ID
    redstar.readState(nozzleId);  // Read the state of the specified nozzle
    redstar.readPrice(nozzleId);   // Read the price of the specified nozzle
    redstar.readTotal(nozzleId);   // Read the total amount for the specified nozzle
    redstar.sendApproval(nozzleId); // Send approval for the specified nozzle
    redstar.sendFuel(nozzleId);     // Send fuel command for the specified nozzle
    redstar.presetAmount(nozzleId, 1000); // Set a preset amount for the specified nozzle
    redstar.setPrice(nozzleId, 1500);     // Set a price for the specified nozzle
}