
#include "utils/logger.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void logger(int fd, log_type type, const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);

  // simple printf wrapper function.
  char buffer[LOG_PREFIX_BUFFER_LEN]; // one for the null!
  memset(buffer, 0, sizeof(buffer));
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(buffer, LOG_PREFIX_BUFFER_LEN, "%Y-%m-%d %H:%M:%S", tm_info);
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
  if (len > LOG_PREFIX_BUFFER_LEN) {
    len = LOG_PREFIX_BUFFER_LEN;
    buffer[len] = '\0';
  }

  // to stdout
  // printf(buffer); // we are good cause memset
  // vprintf(msg, ap);
  // printf("\n");

  // also write the to a file
  write(fd, buffer, len);
  vdprintf(fd, msg, ap);

  close(fd);
}
