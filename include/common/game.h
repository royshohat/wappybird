#pragma once

#include "common/player.h"
#include "net/client.h"
#include "net/net_const.h"
#include <netinet/in.h>

typedef struct {
  // network
  int server_fd;
  //client_t clients[MAX_CLIENT_COUNT * sizeof(client_t)];
  // you had a mistake in here by multpining by the sizeof cause your are just creating an array correct me if i am wrong
  client_t clients[MAX_CLIENT_COUNT];
  size_t client_count;

  // game
  //same in here
  //player_t players[MAX_PLAYER_COUNT * sizeof(player_t)];
  player_t players[MAX_PLAYER_COUNT];
  size_t players_count;

  // other
  // int log_fd;
} vars_t;

int init_game(vars_t *vars);
int init_log(vars_t *vars);
