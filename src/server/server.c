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
#include <sys/select.h>


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


bool is_game_stage_equal(stage gs);
void wait_ready(player_t* players, size_t* player_count);
void time_sync(player_t* ptrPlayer);
void main_loop();
void* handle_client(void* args);
int accept_client(int sockfd, struct sockaddr_in* server_addr, client_t* clients, size_t* ptrClient_count); 

int main() {
    vars_t game_vars;


    if (init_networking(&game_vars)!= 0) return -1;
    if (init_game(&game_vars)!= 0) return -1;

    // ****************************TODO**************************************
    //      - write main loop with select 
    //      remember to move part of the code from handle client to the main loop when creating it
    //      handle closed sockets in select 
        
    main_loop();
    
        
    printf("End.\n");
    return;
}

void main_loop() {
    fd_set sockets;
    FD_ZERO(&sockets);
    
    while (!0) {

    }
}

void wait_ready(player_t* players, size_t* player_count) {

    for (int i=0; i < MAX_PLAYER_COUNT; i++) {
        if (!players[i].client.is_active) continue;
        if (!players[i].is_ready) {
            return;
        }
    }
    
    // everyone is ready
    // checks for less then 2 players
    // as it's the bare minimum
    if ((*player_count < 2)) return;
    // if all conditions are met, update game stage.
    game_stage = STAGE_SYNC_TIME;
}


int accept_client(int sockfd, struct sockaddr_in* server_addr, 
                   client_t* clients, size_t* ptrClient_count) {
    
    int client_fd;

    // test for max client count..
    if (*ptrClient_count == MAX_CLIENT_COUNT || game_stage != STAGE_GAME) {
        printf("MAX_CLIENT_COUNT=%d REACHED.\n", MAX_CLIENT_COUNT);

        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        int fd = accept(sockfd, NULL, NULL);
        if (fd != -1) close(fd); // close the connection in the face cause server is full
        // restore server socket flags to blocking again
        fcntl(sockfd, F_SETFL, flags); 
        return NULL; 
    }
        
    
    
    if(((client_fd = accept(sockfd, NULL, NULL)) < 0)){
        perror("accept: ");
        return NULL; 
    }
    printf("Got A Conncection! (fd: %d)\n",client_fd);
    
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
        printf("Something bad happend! possible severe memory leak????? \n");
        return NULL;
    }
    clients[free_idx].fd = client_fd;
    clients[free_idx].offset_ms = INT_MAX; // change me
    clients[free_idx].is_active = true;

    
    (*ptrClient_count)++;
    
    return 0;
}


void* handle_client(void* args) {
    // todo : validation of of stages 

    while(!0) {
        
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
