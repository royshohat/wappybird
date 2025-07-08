#pragma once

#include "net/net_const.h"
#include <netinet/in.h>

typedef struct {
  int server_fd;
  struct sockaddr_in server_addr;
  client_t *clients;
  player_t *players;
} vars_t;

int init_game(vars_t *vars);
