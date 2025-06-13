//#define FMS_MQTT_DEBUG
#ifdef FMS_MQTT_DEBUG
  #define FMS_MQTT_LOG_DEBUG(format, ...) Serial.print("[fms_mqtt.ino][DEBUG] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
  #define FMS_MQTT_LOG_ERROR(format, ...) Serial.print("[fms_mqtt.ino][ERROR] "); Serial.printf(format, ##__VA_ARGS__); Serial.println()
#else
  #define FMS_MQTT_LOG_DEBUG(format, ...)
  #define FMS_MQTT_LOG_ERROR(format, ...)
#endif


char fms_nmf_tp_prefix[64];
void fms_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("fms_mqtt.ino:13:Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int j = 0; j < length; j++) message += (char)payload[j];

  #ifdef USE_REDSTAR
  // mqtt callback for redstar protocol
   redstar_pump_setting(topic,message); // call redstar pump setting function
  #endif
  #ifdef USE_TATSUNO
  // mqtt callback for tatsuno protocol
   tatsuno_pump_setting(topic,message); // call tatsuno pump setting function
  #endif
}


void fms_subsbribe_topics() {
  for (uint8_t i = 0; i < fms_sub_topics_count; i++) {
    FMS_MQTT_LOG_DEBUG("Subscribing to topic: %s", fms_sub_topics[i]);
    fms_mqtt_client.subscribe(fms_sub_topics[i]);
  }
}

void fms_mqtt_reconnect() {
  // for check client connection online or offline

  String willTopicStr = String("device/") + deviceName + "/status";
  const char* willTopic = willTopicStr.c_str();
  const char* willMessage = "offline";
  bool willRetain = true;
  uint8_t willQos = 1;

  while (!fms_mqtt_client.connected()) {
    FMS_MQTT_LOG_DEBUG("MQTT initialized, connecting to %s:%d...", MQTT_SERVER, 1883);
    String clientId = String(deviceName) + String(random(0xffff), HEX);
    if (fms_mqtt_client.connect(clientId.c_str(),sysCfg.mqtt_user,sysCfg.mqtt_password,willTopic,willQos,willRetain,willMessage)) {
       
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
      fms_mqtt_client.publish(willTopic, "online", true);
      fms_subsbribe_topics();
      /* old feature use below style */
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

unsigned long previous_Millis   = 0;
const long interval             = 1000; // Interval for sending messages
bool ledState_                  = false;

static void mqtt_task(void* arg) {
  BaseType_t rc;
   fms_nvs_storage.begin("fms_config", false);
    String host = fms_nvs_storage.getString("host");
    String port = fms_nvs_storage.getString("port");
    
    if(host.length() == 0 || port.length() == 0) {
      gpio_set_level(LED_YELLOW, LOW);
      vTaskDelay(pdMS_TO_TICKS(500));
      FMS_LOG_ERROR("[fms_mqtt.ino:79] [DEBUG MQTT] mqtt .. credential .. value is empty");
      fms_nvs_storage.end();
     
    }

    fms_nvs_storage.end();
    FMS_LOG_DEBUG("HOST : %s , PORT : %s", host, port);

  fms_mqtt_client.setServer(host.c_str(), atoi(port.c_str()));
  fms_mqtt_client.setCallback(fms_mqtt_callback);

  while (mqttTask) {
    unsigned long currentMillis = millis();
    fms_mqtt_client.loop();
    if (!fms_mqtt_client.connected()) {
      fms_mqtt_reconnect();
      if (currentMillis - previous_Millis >= interval) {
        previous_Millis = currentMillis;
        ledState_ = !ledState_;
        gpio_set_level(LED_GREEN, ledState_);
      }
    } else {
      FMS_MQTT_LOG_DEBUG("Connected to MQTT server");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}