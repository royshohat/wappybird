#pragma once

// both client_net.c and server_net.c
// implement this header.

#include "common/game.h"
#include "net/net_const.h"
#include <netinet/in.h>
#include <unistd.h>

int init_networking(vars_t *game_vars);

packet_type recv_packet(int fd, packet_fields *fields);
int send_packet(int fd, packet_type type, packet_fields *fields);
int handle_packet(int fd, packet_type type, packet_fields *fields,
                  vars_t *game_vars);
