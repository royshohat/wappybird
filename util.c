//server libs

// client libs
#include <sys/time.h>
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <unistd.h>
#include "network.h"
#include "game.h"

long long get_timestamp_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert seconds and microseconds to milliseconds
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

int ping(int sockfd) {
    // get current timestamp
    char buf[10];
    long long send_time = get_timestamp_ms();
    send(sockfd, "Ping!", 6, 0);
    // get server time

    recv(sockfd, buf, sizeof(buf), 0);
    long long recv_time = get_timestamp_ms();

    return (recv_time - send_time) / 2;

    //Basic steps for time sync:
        // Client sends request to server, noting the time T1.
        // Server receives, notes T2, sends current server time T3 back.
        // Client receives reply, notes T4.
        // Estimate round-trip time (RTT) and offset.
    // Formulas:
        // RTT ≈ (T4 - T1) - (T3 - T2)
        // Offset ≈ ((T2 - T1) + (T3 - T4)) / 2
}
int init_server(int *sockfd, struct sockaddr_in* server_addr) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        perror("Error creating a socket.");
        return 1;
    }
        /* nice to have, dont care if this fails.*/
    int enabled = 1;
    (void)setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)); 
  // delete in production.

    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET; 
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(TCP_PORT); 
    
    if(bind(*sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr))==-1){
        perror ("Error binding address.");
        close(*sockfd);
        return 1;
    }
    // listen for new connections
    if(listen(*sockfd, MAX_PLAYER_COUNT)==-1){
        perror ("Error Listening to socket.");
        close(*sockfd);
        return 1;
    }
    return 0;
}
