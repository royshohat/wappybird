#pragma once

#include <stdbool.h>

#include "network.h"
#include "game_const.h"


typedef struct 
{
    client_t client;
    // player_data
    uint32_t id; 
    bool is_ready;
    bool is_alive;
    // another thing yet to come like bird and such...
} player_t;

