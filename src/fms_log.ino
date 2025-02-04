// Created: 2019-04-10 15:00:00

int seriallog_level = 1;

void fms_log_print(const char *line) {
  byte loglevel = 1;
  if (SHOW_SYS_LOG) {
    char mxtime[9];
    struct tm rtcTime;
    //if (getLocalTime(&rtcTime)) snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d:%02d:%02d"), rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);
    if (loglevel <= seriallog_level) fms_log_printf("%s\n", line);
  }
}

void  fms_chip_info_log(){
  printChipInfo(); // build in chip-debug-report.cpp
}
