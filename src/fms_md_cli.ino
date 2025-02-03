static void cli_task(void *arg) {
  BaseType_t rc;
  for (;;) {
    chip_report_printf("  CLI TERMINAL     : %lu cli\n",1 );
    rc = xTaskNotify(heventTask, 4, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
