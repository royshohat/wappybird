//server libs

// client libs
#include <sys/time.h>
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>

#include "common/game.h"
#include "networking/network.h"
#include "utils/util.h"

#include "net_const.h"


long long get_timestamp_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert seconds and microseconds to milliseconds
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void print_player(player_t player) {
    printf("Player #%d:\n" \
           "    id:%d\n" \
           "    is_ready:%d\n" \
           "    is_alive:%d\n" \
           "    Client:\n" \
           "        fd:%d\n" \
           "        offset_ms:%d\n" \
           "        is_active:%d\n\n",
         player.id, player.id, player.is_ready, player.is_alive, 
         player.client.fd, player.client.offset_ms,
         player.client.is_active
    );
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

int sync_timestamp(int fd) {
    // returns offset in ms between
    char header[SIZE_HEADER];
    // TODO
}

void send_packet(int count, int fd, packet_type type, ...) {
    va_list ap;
    va_start(ap, type);
    char buf[SIZE_HEADER + MAX_DATA_LENGTH];
    int size;
    buf[0] = type;
    // send_packet(sockfd, TYPE_REQ_TIMESTAMP, READY, 4)
    switch (type) {
    //same order as th enum
        // anytime
        
        case TYPE_REQ_JOIN:
            size = SIZE_REQ_JOIN;
            break;

        case TYPE_REQ_LEAVE:
            size = SIZE_REQ_LEAVE;
            break;
        case TYPE_BROADCAST_LEAVE:
            size = SIZE_BROADCAST_LEAVE;
            *(buf + SIZE_HEADER) = va_arg(ap, int); // id
            break; 
        case TYPE_REQ_PING:
            size = SIZE_REQ_PING;
            break;
        case TYPE_RESP_PING:
            size = SIZE_RESP_PING;
            break;
        case TYPE_RESP_JOIN:
            size = SIZE_RESP_JOIN;
            *(buf + SIZE_HEADER) = va_arg(ap, int); // id
            break;
        case TYPE_RESP_UPDATE_ARRAY:
            size = SIZE_RESP_UPDATE_ARRAY;
            memcpy(buf + SIZE_HEADER, va_arg(ap, void*), MAX_PLAYER_COUNT * sizeof(player_t));
            break;
        // stage 1
        case TYPE_REQ_READY:
            size = SIZE_REQ_READY;
            *(buf + SIZE_HEADER) = va_arg(ap, int); // first field (READY/UNREADY)
            break;

        case TYPE_BROADCAST_READY:
            size = SIZE_BROADCAST_READY;
            *(buf + SIZE_HEADER) = va_arg(ap, int);
            *(uint32_t*) (buf + SIZE_HEADER + 1) = va_arg(ap, int); 
            // ^^^ id
            break;
        default:
            perror("ERROR: No Implementation for Type.\n");
    }
    *(int*) (buf + 1) = size;
    
    send(fd, buf, size + SIZE_HEADER, 0);
    va_end(ap);
}

int recv_packet(int count, int fd, ...) {
    // function return 
    // this function receives a packet, the packet type, 
    // and pointers to all fields.
    // the function will assign values to all
    // fields according to the data in the buffer 
    // returns nothing.
    va_list ap;
    va_start(ap, fd);
    
    char header[SIZE_HEADER];
    char data[MAX_DATA_LENGTH];
    recv(fd, header, SIZE_HEADER, 0);
    recv(fd, data, *(uint32_t*) (header+1), 0);

    switch (header[0]) {
        case TYPE_RESP_JOIN:
            return 0;
        case TYPE_BROADCAST_READY:
            if (count != 4) {
                printf("Invalid argument count for recv_packet\n");
                return 1;
            }
            int* ready = va_arg(ap, int*);
            int* id = va_arg(ap, int*);
            *ready = *data;
            *id = *((int*) data + 1);
            break;
        
    }


    va_end(ap);
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


int init_client(int* sockfd, struct sockaddr_in* server_addr, int* id, player_t* players_arr) {

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
    char buf[SIZE_HEADER];
    usleep(250 * 1000); // qureter of a sec
    n = recv(*sockfd, buf, 1, MSG_PEEK | MSG_DONTWAIT);
    if (n == 0) return 1; // connection closed (refused.)

    // recv clients array

    recv(*sockfd, buf, sizeof(buf), 0);
    if(buf[0] != TYPE_RESP_UPDATE_ARRAY) return -1;
    n = recv(*sockfd, (void*)players_arr, *(uint32_t*) &buf[1], 0);

    for (int i = 0; i < n; i++) {
        printf("%02X ", (unsigned char)buf[i]);  // Print as hex
    }
    printf("\n");

    // recv my id
    recv(*sockfd, buf, sizeof(buf), 0);

    recv(*sockfd, id, *(uint32_t*) &buf[1], 0);

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
