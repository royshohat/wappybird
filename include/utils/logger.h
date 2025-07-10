
typedef enum {
  LOG_INFO,
  LOG_DEBUG,
  LOG_ERROR,
  LOG_WARNING,
} log_type;

#define LOG_PREFIX_BUFFER_LEN 1023 // log single message max length.

void logger(int fd, log_type type, const char *msg, ...);
