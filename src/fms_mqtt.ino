
void fms_mqtt_callback(char* topic,byte* payload,unsigned int length){
  fms_debug_log_printf("Message arrived [%c]",topic);
  for (int i = 0; i < length; i++) {
    fms_cli_serial.print((char)payload[i]);
  }
  fms_cli_serial.println();
}

void fms_mqtt_reconnect() {
  while (!fms_mqtt_client.connected()){
    fms_debug_log_printf("[Mqtt] connection .. fail\n\r");
    DEVICE_ID += String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(DEVICE_ID)){
        fms_debug_log_printf("[Mqtt] connected ..");
        fms_mqtt_client.subscribe("fms/test/data");
        /*
        user mqtt topic here
        */
    } else {
      fms_debug_log_printf("[Mqtt] connection .. failed, rc=%d try again in 5 second\n\r",fms_mqtt_client.state());
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
    else fms_debug_log_printf("[Mqtt] mqtt .. connected\n\r");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}