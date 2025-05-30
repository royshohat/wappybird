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

#define SIZE_REQ_READY (SIZE_IS_READY)
#define SIZE_BROADCAST_READY (SIZE_IS_READY + SIZE_ID)

// Game Packets


typedef enum  {

    // anytime
    REQ_LEAVE, 
    BROADCAST_LEAVE, 
    REQ_PING,
    RESP_PING,

    // stage 1 - wait for players to join or get ready
    REQ_JOIN,
    BROADCAST_JOIN,
    REQ_READY,
    BROADCAST_READY,

    // stage 2 -  before the game starts, sync time with all clients.
    REQ_TIMESTAMP, 
    RESP_TIMESTAMP,
    
    // stage 3 - broadcast a start game
    BROADCAST_START_GAME, // the server sends at the begining of the game
    
    // stage 4 - IN GAME
    REQ_UPDATE_STATE, // client notifies server of keyboard press for example
    BROADCAST_UPDATE_STATE, // server updates other clients

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

// *REQ_READY* 
// total size = 1 byte
// READY or UNREADY (1 byte)



// *BROADCAST_READY*
// total_size = 5 bytes
// READY or UNREADY (1 byte) 
// client_id (4 byte) 

// *SEND_GAME_START*
// total size = ?? bytes
// the receiver client_id (4 bytes)
// array data (?? bytes)

