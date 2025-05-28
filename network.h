#define MAX_DATA_LENGTH 1024
#define TCP_PORT 8080
#define UDP_PORT 9000


typedef enum  {

    // anytime
    REQ_LEAVE, 
    BROADCAST_LEAVE, 

    // stage 1
    REQ_JOIN,
    BROADCAST_JOIN,

    // stage 2
    REQ_PING,
    RESP_PING,
    REQ_TIMESTAMP, 
    RESP_TIMESTAMP,
    
    // stage 4
    REQ_READY,
    BROADCAST_READY,
    
    // stage 5
    SEND_GAME_START, // the server sends at the begining of the game
    
    // IN GAME
    REQ_UPDATE_STATE, // client notifies server of keyboard press for example
    BROADCAST_UPDATE_STATE, // server updates other clients

    // POST GAME (?)
} packet_type;

# define READY 1
# define UNREADY 2

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

