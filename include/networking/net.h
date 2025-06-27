#pragma once

#include "network.h"

int init(int* sockfd, struct sockaddr_in* server_addr);

packet_type recv_packet(int fd, packet_fields* fields);
int send_packet(int fd, packet_type type, packet_fields* fields);
int handle_packet(int fd, packet_type type, packet_fields* fields, player_t* players_arr, size_t* player_count);