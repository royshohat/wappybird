
typedef enum {
  LOG_INFO,
  LOG_DEBUG,
  LOG_ERROR,
  LOG_WARNING,
} log_type;

#define MAX_LOG_LENGTH 1023 // log single message max length.

void logger(log_type type, const char *msg);
