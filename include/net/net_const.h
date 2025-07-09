#pragma once

#include "common/game_const.h"
#include "common/player.h"
#include "netinet/in.h"
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_DATA_LENGTH 1024
#define MAX_CLIENT_COUNT 5
#define TCP_PORT 8080
#define UDP_PORT 9000 // not in use.
#define READY 1
#define UNREADY 2

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port

// **HEADER**
// header fields
#define SIZE_PACKET_TYPE 1
// header size
#define SIZE_HEADER SIZE_PACKET_TYPE

// **DATA**
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
#define SIZE_REQ_UPDATE_STATE 0     // TODO

typedef struct {
  // client_data
  int fd;
  int offset_ms;
  struct sockaddr_in addr;
  socklen_t addr_len;
  bool is_active;
} client_t;

typedef struct {
  // data
  char is_ready;
  int id;
  uint64_t timestamp;
  player_t *players_array;
  player_t *player; // use static variable instead?
} packet_fields;

// Game Packets
typedef enum {

  // anytime
  TYPE_REQ_LEAVE,
  TYPE_BROADCAST_LEAVE,
  TYPE_REQ_PING,
  TYPE_RESP_PING,

  TYPE_RESP_UPDATE_ARRAY,
  // stage 1 - wait for players to join or get ready

  TYPE_REQ_JOIN,
  TYPE_RESP_JOIN,
  TYPE_BROADCAST_JOIN,
  TYPE_REQ_READY,
  TYPE_BROADCAST_READY,

  // stage 2 -  before the game starts, sync time with all clients.
  TYPE_REQ_TIMESTAMP,
  TYPE_RESP_TIMESTAMP,
  // stage 3 - broadcast a start game
  TYPE_BROADCAST_START_GAME, // the server sends at the begining of the game
  // stage 4 - IN GAME
  TYPE_REQ_UPDATE_STATE, // client notifies server of keyboard press for example
  TYPE_BROADCAST_UPDATE_STATE, // server updates other clients

  TYPE_INVALID
  // POST GAME (?)
} packet_type;

typedef enum {
  STAGE_WAIT_FOR_PLAYERS,
  STAGE_SYNC_TIME,
  STAGE_START_GAME,
  STAGE_GAME, // in game
} stage;
