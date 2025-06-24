#pragma once
#include <stdint.h>

typedef struct {
    // client_data
    int fd;
    int offset_ms;
    bool is_active;
} client_t; 

typedef struct 
{
    client_t client;
    // player_data
    uint32_t id; 
    bool is_ready;
    bool is_alive;
    // another thing yet to come like bird and such...
} player_t;

typedef struct {
    // data
    char is_ready;
    int id;
    uint64_t timestamp;
    player_t* players_array;
    player_t player;
} packet_fields;

// Game Packets
typedef enum  {

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

uint32_t get_packet_size(packet_type type); 
void print_packet(packet_type type, packet_fields* fields);

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
// total size = MAX_PLAYER_COUNT * sizeof (client_t)
// client_t* clients_array


// TYPE_BROADCAST_JOIN
// total size = sizeof player_t
// player_t player


//TYPE_BROADCAST_LEAVE
// total size = 4
