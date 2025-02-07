bool fms_wifi_begin() {
 
  return true;
}

static void wifi_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    fms_log_printf("wifi task started \n");
    rc = xTaskNotify(heventTask, 2, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second before repeating
  }
}
