#pragma once

#define MAX_DATA_LENGTH 1024
#define TCP_PORT 8080
#define UDP_PORT 9000
#define READY 1
#define UNREADY 2

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port
// Headers
#define SIZE_PACKET_TYPE 1
// #define SIZE_PACKET_LEN 4 
#define SIZE_HEADER SIZE_PACKET_TYPE

// Ready Packets
#define SIZE_IS_READY 1
#define SIZE_ID 4

// Ping
#define SIZE_REQ_PING 0
#define SIZE_RESP_PING 0

// Leave
#define SIZE_REQ_LEAVE 0
#define SIZE_BROADCAST_LEAVE 4

// Join
#define SIZE_REQ_JOIN 0
#define SIZE_RESP_JOIN 4
#define SIZE_BROADCAST_JOIN 4

// Timestamp
#define SIZE_REQ_TIMESTAMP 8
#define SIZE_RESP_TIMESTAMP 8

// Ready
#define SIZE_REQ_READY (SIZE_IS_READY)
#define SIZE_BROADCAST_READY (SIZE_IS_READY + SIZE_ID)

// Update Array
#define SIZE_BROADCAST_UPDATE_STATE (MAX_PLAYER_COUNT * sizeof(player_t))

// Update
#define SIZE_BROADCAST_START_GAME 0 // TODO
#define SIZE_REQ_UPDATE_STATE 0 // TODO