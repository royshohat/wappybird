#pragma once
#include <stdint.h>

#include "net_const.h"

void broadcast(player_t *sender, player_t *clients, packet_type type,
               packet_fields *fields);

uint32_t get_packet_size(packet_type type);
void print_packet(packet_type type, packet_fields *fields);
void clear_socket_buffer(int);

// Draft documentation:

// packet structure
// -------------------------------------
// header:
// 1 byte                         | packet_type
// data
// packet_length bytes <= 1024    | data

// PACKET DATA STRUCTE

// *TYPE_REQ_READY*
// total size = 1 byte
// READY or UNREADY (1 byte)

// *TYPE_BROADCAST_READY*
// total_size = 5 bytes
// READY or UNREADY (1 byte)
// client_id (4 byte)

// TYPE_REQ_TIMESTAMP,
// total size = 8 bytes
// the server timestamp

// TYPE_RESP_TIMESTAMP,
// total size = 8 bytes
// the client timestamp

// TYPE_RESP_ID
// total size = 4
// size_t id

// TYPE_RESP_UPDATE_ARRAY
// total size = MAX_PLAYER_COUNT * sizeof (player_t)
// player_t* clients_array

// TYPE_BROADCAST_JOIN
// total size = sizeof player_t
// player_t player

// TYPE_BROADCAST_LEAVE
//  total size = 4
