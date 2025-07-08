#pragma once

#include "net/net_const.h"
#include <netinet/in.h>

typedef struct {
  // network
  int server_fd;
  struct sockaddr_in server_addr;
  client_t clients[MAX_CLIENT_COUNT * sizeof(client_t)];
  size_t client_count;

  // game
  player_t players[MAX_PLAYER_COUNT * sizeof(player_t)];
  size_t players_count;
} vars_t;

int init_game(vars_t *vars);
