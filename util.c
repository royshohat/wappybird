//server libs

// client libs
#include <sys/time.h>
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>

#include "game.h"
#include "network.h"
#include "util.h"

long long get_timestamp_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert seconds and microseconds to milliseconds
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void print_client(client_t c) {
    printf("Client %d:\nfd:%d\nis_ready%d\noffset_ms:%d\n\n",c.id, c.fd, c.is_ready, (int)c.offset_ms);
}

int ping(int sockfd) {
    // get current timestamp
    char req[SIZE_HEADER + SIZE_REQ_PING];
    long long send_time = get_timestamp_ms();

    req[0] = TYPE_REQ_PING;
    *(uint32_t*) &req[1] = SIZE_REQ_PING;
    send(sockfd, req, sizeof(req), 0);
    // get server time
    
    char resp[SIZE_HEADER + SIZE_RESP_PING];
    resp[1] = TYPE_RESP_PING;
    *(uint32_t*) &resp[1] = SIZE_RESP_PING;
    
    recv(sockfd, resp, sizeof(resp), 0);
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


int init_client(int* sockfd, struct sockaddr_in* server_addr) {

    // Create socket
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set server address
    memset(server_addr, 0, sizeof(*server_addr)); // Clear memory
    server_addr->sin_family = AF_INET;              // IPv4
    server_addr->sin_port = htons(SERVER_PORT);     // Server port to big endian 
    if (inet_pton(AF_INET, SERVER_IP, &server_addr->sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(*sockfd);
        return 1;
    }

    // Connect to the server
    if (connect(*sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Connection failed");
        close(*sockfd);
        return 1;
    }
    int n;
    char buf[1];
    usleep(250 * 1000);
    n = recv(*sockfd, buf, 1, MSG_PEEK | MSG_DONTWAIT);
    if (n == 0) return 1; // connection closed (refused.)
    return 0;
}

void clear_socket_buffer(int sockfd) {
    // set socket flag to non blocking
    long n; 
    int data[1024];
    int flags = fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);
    
    do {
        n = recv(sockfd, data, sizeof(data), 0);
    } while (n > 0);
    // the buffer is now empty :)
    // unset block flag
    fcntl(sockfd, F_SETFL, flags & (O_NONBLOCK ^ INT_MAX));
}
