
#include "common/game.h"
#include "string.h"

int init_game(vars_t *game_vars) {
  // carefully using sizeof, note that players is an array and not a pointer
  memset(&game_vars->players, 0, sizeof(game_vars->players));
  game_vars->players_count = 0;
  return 0;
}

int init_log(vars_t *game_vars) { return 0; }
