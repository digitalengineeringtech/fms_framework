
#ifdef FMS_MQTT_DEBUG
  #define FMS_MQTT_LOG_DEBUG(format, ...) Serial.print("[MQTT][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_MQTT_LOG_ERROR(format, ...) Serial.print("[MQTT][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_MQTT_LOG_DEBUG(format, ...)
  #define FMS_MQTT_LOG_ERROR(format, ...)
#endif

char fms_nmf_tp_prefix[64];

void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
  FMS_MQTT_LOG_DEBUG("INCOMMING TIOPIC [%s] : %s",topic,incommingMessage);
  bool tp_match = false;

  String topic_ = String(topic);
 // get topic last value(for /) deptos/local_server/1 , /1 value or /permit or /price 
  int last = topic_.lastIndexOf('/');
  String topic_value = topic_.substring(last+1);
  // some return topic contain noz id detpos/local_server/1 , check noz id or other 
  int nozzle_num = topic_value.toInt();
  FMS_MQTT_LOG_DEBUG("Topic value : [%s]:%d", topic_value.c_str(),nozzle_num);
  // check if the topic is approved message or not
  if(nozzle_num >=1 && nozzle_num <= MAX_NOZZLES){
    snprintf(approvmsg,sizeof(approvmsg),"%02dappro",nozzle_num);
    FMS_MQTT_LOG_DEBUG("APPROVED MESSAGE GENERTED : %s",approvmsg);
    if (incommingMessage == String(approvmsg)){
      pump_approve[nozzle_num-1] = true;
      tp_match = true;
      FMS_MQTT_LOG_DEBUG("APPROVED MESSAGE for Nozzle %d: %s", nozzle_num, incommingMessage.c_str());
    }
  }

  
  for (int i = 0 ; i < fms_sub_topics_value_count; i++){
      const char* sub_tp_value = fms_sub_topics_value[i]; // declare in main.h file
    if(strcmp(sub_tp_value,topic_value.c_str()) == 0)
    {
      tp_match = true;
      switch (i){
        case 0: {
          FMS_MQTT_LOG_DEBUG("preset topic matched: %s", topic_value.c_str());
          int pumpID = fms_decodePumpId(incommingMessage);
          int presetAmount = fms_decodePresetAmount(incommingMessage);
          presetMessageGet = true; // for preset message get from mqtt broker
          // pump_approve[pumpID-1] = true;
          FMS_MQTT_LOG_DEBUG("Pump ID: %d, Preset Amount: %d", pumpID, presetAmount);
          break;
        }
        case 1: {
          FMS_MQTT_LOG_DEBUG("price topic matched: %s", topic_value.c_str());
          break;
        }
      }
      FMS_MQTT_LOG_DEBUG("MATCH TRUE");
      break;
    } 
    else {
        FMS_MQTT_LOG_DEBUG("not matched : [%s] == %s",topic,fms_sub_topics_value[i]);
    }
  }

  if (!tp_match) {
    FMS_MQTT_LOG_ERROR("Topic not matched : %s", topic);
  }
}


void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    FMS_MQTT_LOG_DEBUG("Subscribing to topic: %s", fms_sub_topics[i]);
    fms_mqtt_client.subscribe(fms_sub_topics[i]);
  }
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()) {
    FMS_MQTT_LOG_DEBUG("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str())) {
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
      fms_subsbribe_topics();
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

static void mqtt_task(void* arg) {
  BaseType_t rc;
  fms_mqtt_client.setServer(MQTT_SERVER, 1883);
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (mqttTask) {
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
    } else {
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}