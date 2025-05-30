#pragma once

#include <stdbool.h>

#define MAX_PLAYER_COUNT 2



typedef struct {
    unsigned int id; 
    int fd;
    double offset_ms;
    bool is_ready;
    bool is_active;
    // another thing yet to come like bird and such...
} client_t; 
