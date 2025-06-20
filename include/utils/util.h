#ifndef UTIL_H
#define UTIL_H

#include "common/game.h"

long long get_timestamp_ms();
int ping(int sockfd);
int init_client(int* sockfd, struct sockaddr_in* server_addr, int* id, player_t* players_arr);
int init_server(int* sockfd, struct sockaddr_in* server_addr);
void print_player(player_t c);
int sync_timestamp(int fd);
void clear_socket_buffer(int sockfd);

#endif
