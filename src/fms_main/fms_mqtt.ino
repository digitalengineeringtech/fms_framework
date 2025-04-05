void handleMessage(int ind, String msg){
  FMS_LOG_DEBUG("Topic matched: %s", fms_sub_topics[i]);
  switch (i) {
    case 0: // detpos/local_server/1
      // Handle the first topic here
      break;
    case 1: // detpos/local_server/preset
      // Handle the second topic here
      break;
    case 2: // detpos/local_server/price
      // Handle the third topic here
      break;
    default:
      FMS_LOG_WARNING("Unknown topic index: %d", i);
      break;
  }
}


void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int j = 0; j < length; j++) incommingMessage += (char)payload[j];
  FMS_LOG_DEBUG("Message arrived [%s] : %s", topic, incommingMessage.c_str());
  for (int i = 0; i < fms_sub_topics_count; i++){
    if (strcmp(topic, fms_sub_topics[i]) == 0) {
        handelMessage(i,incommingMessage);
    } else{
      FMS_LOG_DEBUG("Topic not matched: %s", fms_sub_topics[i]);
      FMS_LOG_ERROR("Topic not matched found!");
      // save to log file (logging function)
    }
  }
}

void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    FMS_LOG_DEBUG("Subscribing to topic: %s", fms_sub_topics[i]);
    fms_mqtt_client.subscribe(fms_sub_topics[i]);
    Serial.printf("Subscribing to topic %s", fms_sub_topics[i]); // for testing // please remove this line
  }
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()) {
    FMS_LOG_INFO("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str())) {
      FMS_LOG_INFO("Connected to MQTT server");
      fms_subsbribe_topics();
      // Uncomment the following lines to subscribe to additional topics
      // fms_mqtt_client.subscribe("detpos/#");
      // fms_mqtt_client.subscribe("detpos/local_server/#");
      // fms_mqtt_client.subscribe("detpos/local_server/price");
      // fms_mqtt_client.subscribe("detpos/local_server/preset");
      // Add additional topic subscriptions if necessary
    } else {
      FMS_LOG_WARNING("Failed to connect to MQTT server , rc = %d try again in 5 second", fms_mqtt_client.state());
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
    } else FMS_LOG_INFO("Connected to MQTT server");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}