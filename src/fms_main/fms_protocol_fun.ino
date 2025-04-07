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
  uint32_t liveData_result = lanfeng.readLiveData(liveDataAddr, l_liter); // get Live Liter
  uint32_t literPerPrice = lanfeng.readSellLiterPerPrice(priceAddr); // get price per liter 
  float l_liter_float = lanfeng.convert_float(l_liter[0], l_liter[1]); // convert to float (live liter ) (modbus returs value is two 16 bit register so we convert to float vlaue
 liveLiterPrice = LivePrice(literPerPrice, l_liter_float);  // optional features // S = P × L // live price = Liter per price * live liter
  if (liveData_result == 0x01) { // check if live data is read successfully
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
    }
  } else if (noz_handle == 0 && permitMessageSent) {
    lanfeng.setPumpState(PUMP_STATE_ADDR, 0x0000);  // stop pump
    FMS_LOG_DEBUG("Pump 1 stopped.");
    startFinalDataPublish();
    pump_approve[0] = false;       // reset after using
    permitMessageSent = false;    // reset permit message sent flag
  }
}

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
