#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <sys/time.h>

#include "util.h"
#include "network.h"
#include "game.h"


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    
    bool running = true;
    packet_type type = 0;
    uint32_t size;
    char data[MAX_DATA_LENGTH];
    client_t clients[MAX_PLAYER_COUNT] = {0};
    
    
    if(init_client(&sockfd, &server_addr)!=0) {
        return 1;
    }
    printf("Connected to server! %s:%d (fd: %d)\n", SERVER_IP, SERVER_PORT, sockfd);

    while (running) {
        if (recv(sockfd, &type, SIZE_PACKET_TYPE, 0) <= 0) goto leave;
        if (recv(sockfd, &size, SIZE_PACKET_LEN, 0) <= 0) goto leave;
        if (size > MAX_DATA_LENGTH) { // server shouldnt behave like that. 
            // just in case, we handle error so the client doesn't collapse. 
            printf("packet too large! discarding...\n"); 
            // clear recv buffer.
            clear_socket_buffer(sockfd);
            continue; // discard the packet
        }

        switch (type)
        {

        case TYPE_REQ_TIMESTAMP:
            
            recv(sockfd, data, SIZE_REQ_TIMESTAMP, 0);

            char resp[SIZE_HEADER + SIZE_RESP_TIMESTAMP];
            resp[0] = TYPE_RESP_TIMESTAMP;
            *(uint32_t*) &resp[1] = SIZE_RESP_TIMESTAMP;
            *(uint64_t*) &resp[5] = get_timestamp_ms();
            send(sockfd, resp, sizeof(resp), 0);

              
            
            break;

        case TYPE_BROADCAST_JOIN:
            break;
        case TYPE_BROADCAST_LEAVE:
            break;

            
        case TYPE_BROADCAST_READY:
            break;
            
        
        case TYPE_BROADCAST_START_GAME:
            break;
        
        case TYPE_BROADCAST_UPDATE_STATE:
            break;

        defalt:
            break;
        }
    }
    // leave
    leave:
        char req[SIZE_HEADER + SIZE_REQ_LEAVE];
        req[0] = TYPE_REQ_LEAVE;
        *(uint32_t*) &req[1] = SIZE_REQ_LEAVE;
        send(sockfd, req, sizeof(req), 0);
        close(sockfd);
    return 0;
}
