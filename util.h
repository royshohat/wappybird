#ifndef UTIL_H
#define UTIL_H

#include "game.h"

long long get_timestamp_ms();
int ping(int sockfd);
int init_client(int* sockfd, struct sockaddr_in* server_addr);
int init_server(int* sockfd, struct sockaddr_in* server_addr);
void print_client(client_t c);
void clear_socket_buffer(int sockfd);

#endif