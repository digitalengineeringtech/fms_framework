

bool test_sd_init()
{
  fms_log_printf("initializing sd card\n\r");
  if (!LittleFS.begin(true)) {
    fms_log_printf("SD Card Mount Failed\n\r");
    return false;
  }
  fms_log_printf("SD Card Mount Success\n\r");
  return true;
}


bool test_read() {
  fms_log_printf("testing read\n\r");
  test_sd_init();

  File file = LittleFS.open(SD_CARD_CONFIG_FILE_NAME,FILE_READ);
  if (!file) {
    fms_log_printf("Failed to open file for reading\n\r");
    return false;
  }
  fms_log_printf("File Content:");
  while (file.available()) {
    fms_log_printf("%c", file.read());
  }
  file.close();
  return true;
}

bool fms_config_load_sd() {
  fms_log_printf("loading config data from sd cardn\n\r");
  if (!test_read()) {
    fms_log_printf("failed to read data from sd card\n\r");
    return false;
  }
  return true;
}



bool write_data_sd(String input)
{
  //to write code to save data to sd.
  //step 1. simple write
  //step 2. encrypt and write
  //setp 3. sd formarting (clicommand)

  fms_log_printf("writing data to sd card\n\r");
  test_sd_init();
  File file = LittleFS.open(SD_CARD_CONFIG_FILE_NAME, FILE_WRITE);
  if (!file) {
    fms_log_printf("Failed to open file for writing\n\r");
    return false;
  }
  if (file.print(input)) {
    fms_log_printf("File written\n\r");
    return true;
  } else {
    fms_log_printf("Write failed\n\r");
    return false;
  }

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

