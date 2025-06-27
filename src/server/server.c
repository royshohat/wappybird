#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>


#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>
#include <pthread.h>

#include <fcntl.h>

#include "networking/net.h" 
#include "networking/network.h"
#include "utils/util.h"
#include "common/game.h"
#include "common/game_const.h"
#include "networking/net_const.h"


stage game_stage = STAGE_WAIT_FOR_PLAYERS;

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

typedef struct {
    player_t* ptrPlayers; // pointer to array
    player_t* ptrThisPlayer; // pointer to element in the array describing the current client
    size_t* ptrPlayer_count;
} handle_client_args; // accept_clients thread arguments

typedef struct {
    int sockfd;
    struct sockaddr_in* server_addr;
    player_t* ptrPlayers; // pointer to array
    size_t* ptrPlayer_count;
    handle_client_args* ptrHandle_args;
} accept_client_args; // accept_clients thread arguments


bool is_game_stage_equal(stage gs);
void wait_ready(player_t* players, size_t* player_count);
void time_sync(player_t* ptrPlayer);
void main_loop();
void* handle_client(void* args);
void handler(int sigint);
void* ctrlCmech(void* socket_fd);
int accept_clients(int sockfd, struct sockaddr_in* server_addr, client_t* clients, size_t* ptrClient_count); 

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    pthread_t id;
    

    // 1. can you change the array given to select in the middle of execution?
    // 2. can accpet be used in select along side other file descriptors.

    // delete in production
    // terminate the server
    signal(SIGINT, handler);
    signal(SIGPIPE, SIG_IGN); 
    pthread_create(&id, NULL, ctrlCmech, &sockfd);


    int code = init(&sockfd, &server_addr);
    if (code != 0) return code;

    // ****************************TODO**************************************
    //      write main loop with select 
    //      fix accept client
    //      remember to move part of the code from handle client to the main loop when creating it
    //      handle closed sockets in select 
        
        
              

    player_t players[MAX_PLAYER_COUNT] = {0}; 
    // same as memset(players, 0, MAX_PLAYER_COUNT * sizeof(*players));

    size_t client_count = 0;
      
    pthread_create(&id, NULL, accept_clients, (void*)&args);

    // wait for all players to get ready.
    wait_ready(players, &player_count);
    printf("closing server sokcet\n");
    // close(sockfd); // close listening socket.
    
    // print players
    // player_t empty_client;
    // memset(&empty_client, 0, sizeof(player_t));

    // for (int i=0; (memcmp((void*)&players[i], &empty_client, sizeof(player_t)) != 0) && i < MAX_PLAYER_COUNT; i++) {
    //     print_player(players[i]);
    // }
    
    printf("End.\n");
    return;
}

void time_sync(player_t* ptrPlayer) {
    // request timestamp
    char req[SIZE_HEADER + SIZE_REQ_TIMESTAMP];
    // header
    req[0] = TYPE_REQ_TIMESTAMP;
    *(uint32_t*) &req[1] = SIZE_REQ_TIMESTAMP;

    // data
    *(uint64_t*) req[SIZE_HEADER] = get_timestamp_ms();
    send(ptrPlayer->client.fd, req, sizeof(req), 0);
    
    char resp[SIZE_HEADER + SIZE_RESP_TIMESTAMP];
    recv(ptrPlayer->client.fd, resp, sizeof(resp), 0);
    uint64_t recv_client_timestamp = *(uint64_t*) resp[SIZE_HEADER];
    uint64_t recv_server_timestamp = get_timestamp_ms();

    int ms_sum;
    const int COUNT = 5;
    for (int i=0; i < COUNT; i++){
        ms_sum += ping(ptrPlayer->client.fd);
    }
    int latency = ms_sum / COUNT;
    ptrPlayer->client.offset_ms = latency + recv_client_timestamp - recv_server_timestamp;
    return;
}

void wait_ready(player_t* players, size_t* player_count) {
    while (!0) {
        pthread_mutex_lock(&lock_players);
        bool start = true; 
        for (int i=0; i < MAX_PLAYER_COUNT; i++) {
            if (!players[i].client.is_active) continue;
            if (!players[i].is_ready) {
                start = false;
                break;
            }
        }
        if (start && !(*player_count < 2)) {
            pthread_mutex_unlock(&lock_players); 
            break;
        } else {
            pthread_mutex_unlock(&lock_players);
            usleep(2000);
        }
    }
    pthread_mutex_lock(&lock_stage);
    game_stage = STAGE_SYNC_TIME;
    pthread_mutex_unlock(&lock_stage);
}


int accept_clients(int sockfd, struct sockaddr_in* server_addr, client_t* clients, size_t* ptrClient_count) {
    int client_fd;

    // test for max client count..
    if (*ptrClient_count == MAX_CLIENT_COUNT || game_stage != STAGE_GAME) 
        printf("MAX_CLIENT_COUNT=%d REACHED.\n", MAX_CLIENT_COUNT);
        return NULL; // close the connection in the face if lobby is full
    
    // non-blocking accept in case where some client tries to connect but there is no room
    // int flags = fcntl(sockfd, F_GETFL, 0);
    // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    // int fd = accept(sockfd, NULL, NULL);
    // if (fd != -1) close(fd);
    // restore socket flags to blocking again
    // fcntl(sockfd, F_SETFL, flags); 
    
    
    if(((client_fd = accept(sockfd, NULL, NULL)) < 0)){
        printf("client_fd: %d\n", client_fd);
        if (errno == EBADF) {
            // server shutdown via Ctrl - c.
            printf("Everyone is ready!! \nListening socket closed, shutting down accept loop.\n");
        } else {
            // some kind of other error with accept.
            close(sockfd);
            perror("accept failed");
        }
        return NULL; 
    }
    printf("Got A Conncection! (fd: %d)\n",client_fd);
    // printf("accepted client with fd: %d\n", client_fd);

    //construction
    int free_idx = -1;
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        if (!clients[i].is_active) {
            free_idx = i;
            break;
        }
    }
    // assert
    if (free_idx == -1) {
        printf("something bad happend\n");
        return NULL;
    }
    clients[free_idx].fd = client_fd;
    clients[free_idx].offset_ms = 0; // change me
    clients[free_idx].is_active = true;

    
    (*ptrClient_count)++;
    // printf("Creating handle client thread: \nplayer_count: %d\n fd:%d\n", *ptrPlayer_count, client_args->ptrThisClient->fd);

    // TODO: finish arrray sync and test that if finish 
    
    return 0;
}


void* handle_client(void* args) {
    pthread_mutex_lock(&lock_players);
    handle_client_args* t_args = (handle_client_args*)args;
    player_t* ptrPlayers = t_args->ptrPlayers;
    player_t* ptrThisPlayer = t_args->ptrThisPlayer;
    size_t* ptrPlayer_count = t_args->ptrPlayer_count;
    pthread_mutex_unlock(&lock_players);

    packet_fields fields;
    packet_type type;

    // TODO: staging and validating.



    int running = 1;
    while(running) {
        
        if (is_game_stage_equal(STAGE_SYNC_TIME) && ptrThisPlayer->client.offset_ms == INT_MAX) {
            time_sync(ptrThisPlayer);
        }
        printf("Receiving a packet\n");
        type = recv_packet(ptrThisPlayer->client.fd, &fields);
        printf("packet type: %d\n", type);
        fflush(stdout);

        pthread_mutex_lock(&lock_players);
        int res = handle_packet(ptrThisPlayer->client.fd, type, &fields, ptrPlayers, ptrPlayer_count);
        printf("handle result: %d\n", res);
        pthread_mutex_unlock(&lock_players);

        if(type == TYPE_REQ_LEAVE){
            printf("closing file des : %d\n", ptrThisPlayer->client.fd);
            return NULL;
        }

    } 
   return NULL; 

}

void* ctrlCmech(void* socket_fd) {
    int sockfd = *(int*) socket_fd;
    while(!flag);  
    printf("Ctrl-C termination Successful.\n");
    close(sockfd);
    exit(1); // early exit, terminate the process
    return NULL;
}

void handler(int sig){
    flag = 1;
}

bool is_game_stage_equal(stage gs){
    pthread_mutex_lock(&lock_stage);
    if (game_stage == gs) { 
        pthread_mutex_unlock(&lock_stage);
        return true;
    }
    pthread_mutex_unlock(&lock_stage);
    return false;
}   