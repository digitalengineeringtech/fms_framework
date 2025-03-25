
// void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
//   char payload_str[length + 1];
//   memcpy(payload_str, payload, length);
//   payload_str[length] = '\0';
//   FMS_LOG_INFO("Message arrived on topic [%s]: %s", topic, payload_str);
// }

// void fms_mqtt_reconnect() {
//   while (!fms_mqtt_client.connected()){
//     FMS_LOG_INFO("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
//     String clientId = String(DEVICE_ID) + String(random(0xffff), HEX);
//     if (fms_mqtt_client.connect(clientId.c_str())){
//       FMS_LOG_INFO("Connected to MQTT server");
//       fms_mqtt_client.subscribe("fms/test/data");
//        // Add additional topic subscriptions if necessary
//     } else {
//       FMS_LOG_WARNING("Failed to connect to MQTT server , rc = %d try again in 5 second",fms_mqtt_client.state());
//       vTaskDelay(pdMS_TO_TICKS(5000));
//     }
//   }
// }

static void mqtt_task(void *arg) {
  BaseType_t rc;
  // fms_mqtt_client.setServer(MQTT_SERVER,1883);
  // fms_mqtt_client.setCallback(fms_mqtt_callback);

  while(mqttTask){
    // fms_mqtt_client.loop();
    // if(!fms_mqtt_client.connected()) {
    //   fms_mqtt_reconnect();
    // }
    // else FMS_LOG_INFO("Connected to MQTT server");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}