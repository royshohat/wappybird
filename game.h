#pragma once

#include <stdbool.h>

#define MAX_PLAYER_COUNT 2



typedef struct {
    // client_data
    int fd;
    int offset_ms;
    bool is_active;
} client_t; 

typedef struct 
{
    client_t client;
    // player_data
    unsigned int id; 
    bool is_ready;
    bool is_alive;
    // another thing yet to come like bird and such...
} player_t;

