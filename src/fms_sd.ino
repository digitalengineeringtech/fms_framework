
/*
  * fms_sd.cpp
  *
  *  Created on: 2020. 12. 10.
  *   author : thet htar khaing
*/
bool fms_config_load_sd_test() {

return true;
}

bool write_data_sd(char* input)
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
  
    /*
    * Load config data from sd card
    */
    
    rc = xTaskNotify(heventTask, 3, eSetBits);
    vTaskDelay(pdMS_TO_TICKS(1000));
    //write_data_sd("HELLO\n\r");
    //
  }
}

