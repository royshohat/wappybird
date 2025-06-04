#define MAX_DATA_LENGTH 1024
#define TCP_PORT 8080
#define UDP_PORT 9000
#define READY 1
#define UNREADY 2

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port

// Headers
#define SIZE_PACKET_TYPE 1
#define SIZE_PACKET_LEN 4 
#define SIZE_HEADER (SIZE_PACKET_TYPE + SIZE_PACKET_LEN)

// Ready Packets
#define SIZE_IS_READY 1
#define SIZE_ID 4

// Ping
#define SIZE_REQ_PING 0
#define SIZE_RESP_PING 0

// Leave
#define SIZE_REQ_LEAVE 0

// Timestamp
#define SIZE_REQ_TIMESTAMP 8
#define SIZE_RESP_TIMESTAMP 8

#define SIZE_REQ_READY (SIZE_IS_READY)
#define SIZE_BROADCAST_READY (SIZE_IS_READY + SIZE_ID)

// Game Packets


typedef enum  {

    // anytime
    TYPE_REQ_LEAVE, 
    TYPE_BROADCAST_LEAVE, 
    TYPE_REQ_PING,
    TYPE_RESP_PING,

    TYPE_REQ_UPDATE_ARRAY, 
    TYPE_RESP_UPDATE_ARRAY, 
    // stage 1 - wait for players to join or get ready

    TYPE_REQ_JOIN,
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

    // POST GAME (?)
} packet_type;

typedef enum {
    STAGE_WAIT_FOR_PLAYERS,
    STAGE_SYNC_TIME,
    STAGE_START_GAME,
    STAGE_GAME, // in game
    
} stage;

// packet structure
// -------------------------------------
// 1 byte                         | packet_type
// 4 byte (size_t)                | packet_length
// packet_length bytes <= 1024    | data



// PACKET DATA STRUCTE

// *TYPE_REQ_READY* 
// total size = 1 byte
// READY or UNREADY (1 byte)



// *TYPE_BROADCAST_READY*
// total_size = 5 bytes
// READY or UNREADY (1 byte) 
// client_id (4 byte) 

// *TYPE_START_GAME*
// total size = ?? bytes
// the receiver client_id (4 bytes)
// array data (?? bytes)

// TYPE_REQ_TIMESTAMP, 
// total size = 8 bytes
// the server timestamp

// TYPE_RESP_TIMESTAMP,
// total size = 8 bytes
// the client timestamp
