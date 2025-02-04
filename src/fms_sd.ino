void fms_config_load_sd() {
fms_log_printf("config load");
}

bool write_data_sd(String input)
{
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)
  return true;
}

static void sd_task(void *arg) {
  BaseType_t rc;
  while(1) {
    printf("sd task started \n");
    /*
    * Load config data from sd card
    */

    rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}

