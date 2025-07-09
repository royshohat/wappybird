#pragma once

#include <stdbool.h>
#include <stdint.h>

struct client_t;

typedef struct {
  struct client_t *client; // forward definition
  // player_data
  uint32_t id;
  bool is_ready;
  bool is_alive;
  // another thing yet to come like bird and such...
} player_t;
