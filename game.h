#pragma once

#include <stdbool.h>

#define MAX_PLAYER_COUNT 2
#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port


typedef struct {
    unsigned int id; 
    int fd;
    double offset_ms;
    bool is_ready;
    bool is_active;
    // another thing yet to come like bird and such...
} client_t; 
