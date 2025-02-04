static void mqtt_task(void *arg) {
  BaseType_t rc;
  while(1){
 // low 
    printf("mqtt task started \n");

    rc = xTaskNotify(heventTask, 5, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));

  }
}