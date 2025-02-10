// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

bool fms_log_printf(const char *line,...) { // debug log
  byte loglevel = 1;
  if (SHOW_SYS_LOG) {
    if (loglevel <= seriallog_level) _log_printf(line);
  }
  return true;
}

bool fms_cli_serial_printf(const char *line,...) { // uart log
  byte loglevel = 1;
  if (SHOW_UART_SYS_LOG) {
    if (loglevel <= seriallog_level) fms_cli_serial.print(line);
  }
  return true;
}

bool  fms_chip_info_log(){
  printBeforeSetupInfo();
  return true;
}

bool fms_print_after_setup_info(){
  printAfterSetupInfo();
  return true;
}

bool fms_memory_info_log(){
// Check free heap size
    size_t freeHeap = esp_get_free_heap_size();
    fms_log_printf("Free Heap Size: %u bytes\n", freeHeap);
    // Check stack size of the current task
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    fms_log_printf("Stack High Water Mark: %u bytes\n", stackHighWaterMark);
  return true;
}

void fms_log_task_list() {
  char buffer[512]; // Buffer for task list output
  fms_log_printf("Task List:\n");
  fms_log_printf("Name          State       Prio      Stack        Num \n\r");
  vTaskList(buffer);
  fms_log_printf(buffer);
}

