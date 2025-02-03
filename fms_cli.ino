static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    printf("cli task started \n");
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
