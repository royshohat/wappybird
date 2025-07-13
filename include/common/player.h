#pragma once

#include "net/client.h"
#include <stdbool.h>
#include <stdint.h>

struct client_t;

typedef struct {
  client_t *client;
  // player_data
  uint32_t id;
  bool is_active;
  bool is_ready;
  bool is_alive;
  // another thing yet to come like bird and such...
} player_t;
