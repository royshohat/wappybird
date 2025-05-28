#define MAX_PLAYER_COUNT 3
#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port

typedef struct {
    int id; 
    int fd;
    double offset_ms;
    bool is_ready;
    // another thing yet to come like bird and such...
} client_t; 
