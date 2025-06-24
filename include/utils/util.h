#pragma once
#define UTIL_H

#include "common/game.h"

long long get_timestamp_ms();
int ping(int sockfd);
void print_player(player_t c);
int sync_timestamp(int fd);
void clear_socket_buffer(int sockfd);

