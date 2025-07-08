
#include "net/net.h"
#include "common/game.h"
#include <string.h>

int init_networking(vars_t *game_vars) {
  // careful using sizeof, this is allowed cause array.
  memset(game_vars->clients, 0, sizeof(game_vars->clients));
  game_vars->client_count = 0;
  // TODO: init_server (for the fd)
  return 0;
}
