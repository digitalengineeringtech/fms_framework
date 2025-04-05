
#define FMS_MQTT_DEBUG
#ifdef FMS_MQTT_DEBUG
  #define FMS_MQTT_LOG_DEBUG(format, ...) Serial.print("[MQTT][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_MQTT_LOG_ERROR(format, ...) Serial.print("[MQTT][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_MQTT_LOG_DEBUG(format, ...)
  #define FMS_MQTT_LOG_ERROR(format, ...)
#endif

char fms_nmf_tp_prefix[64];

void handleMessage(char* tp,int ind, String msg){
  FMS_MQTT_LOG_DEBUG("Topic matched: %s", fms_sub_topics[ind]);
  switch (ind) {
    case 0: //  "detpos/local_server/#",
      String approv_tp = String(tp);
      FMS_MQTT_LOG_DEBUG("Wild card topic matched: %s", fms_sub_topics[ind]);
      int last   = approv_tp.lastIndexOf('/'); // detpos/local_server/1 
      int noz_id = approv_tp.substring(last+1).toInt(); // get 1 or 2
      FMS_MQTT_LOG_DEBUG("Nozzle ID: %d", noz_id);
      FMS_MQTT_LOG_DEBUG("Message : %s",msg);
      // add main code here
      break;

    case 1: //"detpos/local_server/price",
      FMS_MQTT_LOG_DEBUG("Topic matched: %s", fms_sub_topics[ind]);
      break;

    case 2: //"detpos/local_server/preset"
      FMS_MQTT_LOG_DEBUG("Topic matched: %s", fms_sub_topics[ind]);
    
      // add main code here
      break;

    default:
      FMS_MQTT_LOG_ERROR("Unknown topic index: %d", ind);
      break;
  }
}


void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
  //FMS_MQTT_LOG_DEBUG("Message arrived [%s] : %s", topic, incommingMessage.c_str());
  bool tp_match = false;

  for (int i = 0; i < fms_sub_topics_count; i++){
  const char* sub_tp = fms_sub_topics[i];
    if (strcmp(topic,sub_tp) == 0) {
      tp_match = true;
      handleMessage(topic,i,incommingMessage);
      break;
    }
    int len = strlen(sub_tp);
    if(len > 0 && sub_tp[len-1] == '#' ){
     strncpy(fms_nmf_tp_prefix,sub_tp, len-1); // copy topic prefix to fms_nmf_tp_prefix
     fms_nmf_tp_prefix[len-1]='\0';
     FMS_MQTT_LOG_DEBUG("Wild Card Topic : %s",fms_nmf_tp_prefix);
     tp_match = true;
     handleMessage(topic,0,incommingMessage);
   }
  } // end for loop
  if(!tp_match){
    FMS_MQTT_LOG_ERROR("Topic not matched : %s",topic);
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
    } else FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}