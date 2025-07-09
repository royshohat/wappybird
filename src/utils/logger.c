
#include "utils/logger.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void logger(log_type type, const char *msg) {
  char buffer[MAX_LOG_LENGTH + 1]; // one for the null!
  memset(buffer, 0, sizeof(buffer));
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(buffer, MAX_LOG_LENGTH, "%Y-%m-%d %H:%M:%S", tm_info);
  int len = strlen(buffer);

  const char *type_prefix;
  switch (type) {
  case LOG_INFO:
    type_prefix = " [INFO] ";
    break;
  case LOG_WARNING:
    type_prefix = " [WARN] ";
    break;
  case LOG_ERROR:
    type_prefix = " [ERROR] ";
    break;
  case LOG_DEBUG:
    type_prefix = " [DEBUG] ";
    break;
  }
  memcpy(buffer + len, type_prefix, strlen(type_prefix));
  len += strlen(type_prefix);
  memcpy(buffer + len, msg, strlen(msg));

  printf("%s", buffer);

  // TODO: write to a file log.txt
}
