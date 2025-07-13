#pragma once

#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>

typedef struct {
  // client_data
  int fd;
  int id;
  int offset_ms;
  struct sockaddr_in addr;
  socklen_t addr_len;
  bool is_active;
} client_t;
