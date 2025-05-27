typedef enum  {
    // PRE GAME
    
    REQ_PING,
    RESP_PING,
    REQ_TIMESTAMP, 
    RESP_TIMESTAMP,
    
    REQ_READY,
    BROADCAST_READY,
    // IN GAME
    
    REQ_UPDATE_STATE, // client notifies server of keyboard press for example
    BROADCAST_UPDATE_STATE, // server updates other clients
    

    // POST GAME (?)
} packet_type;

// packet structure
// -------------------------------------
// 1 byte               | packet_type
// 4 byte (size_t)      | packet_length
// packet_length bytes  | data


