#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <sys/time.h>
#include <pthread.h>

#include "utils/util.h"
#include "networking/network.h"
#include "common/game.h"


pthread_mutex_t lock_players; 

void* listen_to_server(void* args);



typedef struct {
    player_t* players;
    int sockfd;
    int id;
} listen_to_server_args;

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    
    player_t players[MAX_PLAYER_COUNT] = {0};
    int id;
    
    if (pthread_mutex_init(&lock_players, NULL) != 0) {
        printf("Error initializing mutex..\n");
        return 1;
    }

    
    if(init_client(&sockfd, &server_addr, &id, players)!=0) {
        return 1;
    }

    printf("Connected to server! %s:%d (fd: %d)\n", SERVER_IP, SERVER_PORT, sockfd);
    
    listen_to_server_args t_args = {
        .players = players,
        .sockfd = sockfd,
        .id = id
    };

    pthread_t t_id;    
    pthread_create(&t_id, NULL, listen_to_server, NULL);
    char input[101];
    char resp[MAX_DATA_LENGTH + SIZE_HEADER];
    
    while (!0) {
        scanf("%100s", input);
        
        int len = strlen(input);
        if (memcmp(input, "REQ_JOIN", len) == 0) {
            send_packet(3, sockfd, TYPE_REQ_JOIN);
        }
        if (memcmp(input, "REQ_LEAVE", len) == 0) {
            send_packet(3, sockfd, TYPE_REQ_LEAVE);
            
        }
        if (memcmp(input, "REQ_PING", len) == 0) {
            ping(sockfd);
            // send_packet(3, sockfd, TYPE_REQ_PING);
        }
        if (memcmp(input, "REQ_READY", len) == 0) {
            send_packet(4, sockfd, TYPE_REQ_READY, READY);
        }
        if (memcmp(input, "REQ_TIMESTAMP", len) == 0) {
            send_packet(3, sockfd, TYPE_REQ_TIMESTAMP, get_timestamp_ms());
            
        }
     
        sleep(1);

    }
    
}

void* listen_to_server(void* args){
    listen_to_server_args* t_args = (listen_to_server_args*)args;
    player_t* players = t_args->players;
    int sockfd = t_args->sockfd;
    int id = t_args->id;

    bool running = true;
    packet_type type = 0;
    uint32_t size;
    
    char data[MAX_DATA_LENGTH];

    while (running) {
        pthread_mutex_lock(&lock_players);
        for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
            if (!players[i].client.is_active) break;
            print_player(players[i]);
        }
        pthread_mutex_unlock(&lock_players);

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

        default:
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
