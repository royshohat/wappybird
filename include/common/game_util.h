#pragma once

#include "common/game_const.h"
#include "common/player.h"
#include "net/client.h"
#include <unistd.h>

player_t *client_to_player(client_t *client, player_t *players) {
  player_t *player = NULL;
  for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
    if (!players[i].is_active)
      continue;
    if (players[i].client == client) {
      player = &players[i];
      break;
    }
  }
  return player;
}
