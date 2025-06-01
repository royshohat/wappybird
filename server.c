#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>
#include <pthread.h>

#include <fcntl.h>

#include "network.h"
#include "util.h"
#include "game.h"


volatile sig_atomic_t flag = 0;

pthread_mutex_t lock_client; 
pthread_mutex_t lock_stage;
stage game_stage = STAGE_WAIT_FOR_PLAYERS;

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

typedef struct {
    client_t* ptrClients; // pointer to array
    client_t* ptrThisClient; // pointer to element in the array describing the current client
    size_t* ptrPlayer_count;
} handle_client_args; // accept_clients thread arguments

typedef struct {
    int sockfd;
    struct sockaddr_in* server_addr;
    client_t* ptrClients; // pointer to array
    size_t* ptrPlayer_count;
    handle_client_args* ptrHandle_args;
} accept_client_args; // accept_clients thread arguments


void* handle_client(void* arg);
void handler(int sigint);
void* ctrlCmech(void* socket_fd);
void* accept_clients(void* args);


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    pthread_t id;

    
    if (pthread_mutex_init(&lock_client, NULL) != 0 || 
        pthread_mutex_init(&lock_stage, NULL) != 0) {
        printf("Error initializing mutex..\n");
        return 1;
    }

    // delete in production
    // terminate the server
    signal(SIGINT, handler);
    signal(SIGPIPE, SIG_IGN); 
    pthread_create(&id, NULL, ctrlCmech, &sockfd);


    int code = init_server(&sockfd, &server_addr);
    if (code != 0) return code;
    
    accept_client_args args;

    client_t clients[MAX_PLAYER_COUNT]; // mutex
    size_t player_count = 0;            // mutex
    handle_client_args client_args;
    
    args.sockfd = sockfd;
    args.server_addr = &server_addr;
    args.ptrClients = clients;
    args.ptrPlayer_count = &player_count;
    args.ptrHandle_args = &client_args;
    
    pthread_create(&id, NULL, accept_clients, (void*)&args);

    // wait for all clients to get ready.
    while (!0) {
        bool start = true; 
        for (int i=0; i < sizeof(clients); i++) {
            if (!clients[i].is_active) continue;
            if (!clients[i].is_ready) {
                start = false;
                break;
            }
        }
        pthread_mutex_lock(&lock_client);
        if (start && player_count < 2) {
            pthread_mutex_unlock(&lock_client);
            break;
        } else {
            pthread_mutex_unlock(&lock_client);
            usleep(2000);
        }
    }
    pthread_mutex_lock(&lock_stage);
    game_stage = STAGE_SYNC_TIME;
    pthread_mutex_unlock(&lock_stage);

    // while !(everyone ready) { 
    // wait 
    sleep(150); 
    // }
    // kill thread accept_client
    close(sockfd);
    
    // Sync Timestamp
    for (int i=0; i < sizeof(clients); i++) {
        if (!clients[i].is_active) continue;
    }
    
    
    // print clients
    client_t empty_client;
    memset(&empty_client, 0, sizeof(client_t));

    for (int i=0; (memcmp((void*)&clients[i], &empty_client, sizeof(client_t)) != 0) && i < MAX_PLAYER_COUNT; i++) {
        print_client(clients[i]);
    }
    
    printf("End.\n");
    return 0;
}



void* accept_clients(void* args) {
    accept_client_args* t_args = (accept_client_args*) args;

    int sockfd = t_args->sockfd;
    struct sockaddr_in* server_addr = t_args->server_addr;
    client_t* clients = t_args->ptrClients; // assume size of the array is MAX_PLAYER_COUNT
    size_t* ptrPlayer_count = t_args->ptrPlayer_count;
    handle_client_args* client_args = t_args->ptrHandle_args;

    pthread_mutex_lock(&lock_client);
    memset(clients, 0, MAX_PLAYER_COUNT * sizeof(client_t));
    pthread_mutex_unlock(&lock_client);
    
    pthread_t id;
    unsigned int c_id = 1;
    int client_fd;

    struct sockaddr_in client_addr;
    
    int msg_once;
    
    printf("Accepting clients...\n");
    while(!0){
        // test for max players..
        msg_once = 0;
        while (!0) {
            pthread_mutex_lock(&lock_client);
            if (*ptrPlayer_count == MAX_PLAYER_COUNT && game_stage!=STAGE_GAME) {
                pthread_mutex_unlock(&lock_client);
                usleep(2000); // sleep for 2 ms.
            } else {
                pthread_mutex_unlock(&lock_client);
                break;
            }
            // non-blocking accept in case where some client tries to connect but there is no
            int flags = fcntl(sockfd, F_GETFL, 0);
            fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
            int fd = accept(sockfd, NULL, NULL);
            if (fd != -1) close(fd);
            // restore socket flags to blocking again
            fcntl(sockfd, F_SETFL, flags); 
            if (!msg_once) {
                printf("MAX PLAYER REACHED.\n");
                msg_once = 1;
            }
        }

        pthread_mutex_lock(&lock_client);
        // if (*ptrPlayer_count >= MAX_PLAYER_COUNT) {
        //     printf("MAX PLAYER REACHED.\n");
        //     pthread_mutex_unlock(&lock_client);
        //     break;
        // }
        pthread_mutex_unlock(&lock_client);
        // wait for players to connect
        if(((client_fd = accept(sockfd, NULL, NULL)) < 0)){
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
        pthread_mutex_lock(&lock_client);
        //contruction
        int free_idx = -1;
        for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
            if (!clients[i].is_active) {
                free_idx = i;
                break;
            }
        }
        if (free_idx == -1) {
            printf("something bad happend\n");
            return NULL;
        }
        clients[free_idx].fd = client_fd;
        clients[free_idx].id = c_id;
        clients[free_idx].is_active = true;
        printf("Got A Conncection! (fd: %d)\n", client_fd);


        memset(client_args, 0, sizeof(*client_args)); // just in case.
        client_args->ptrClients = clients;
        client_args->ptrPlayer_count = ptrPlayer_count;
        client_args->ptrThisClient = &clients[free_idx];
         
        (*ptrPlayer_count)++;
        // printf("Creating handle client thread: \nplayer_count: %d\n fd:%d\n", *ptrPlayer_count, client_args->ptrThisClient->fd);
        pthread_create(&id, NULL, handle_client, (void*)client_args);

        pthread_mutex_unlock(&lock_client);

        c_id++;
    }
    return NULL;
}


void* handle_client(void* args) {
    pthread_mutex_lock(&lock_client);
    handle_client_args* t_args = (handle_client_args*)args;
    client_t* ptrClients = t_args->ptrClients;
    client_t* ptrThisClient = t_args->ptrThisClient;
    size_t* ptrPlayer_count = t_args->ptrPlayer_count;
    pthread_mutex_unlock(&lock_client);

    char data[MAX_DATA_LENGTH];
    packet_type type = 0;
    uint32_t size;
    long n;
    // TODO: staging and validating.
    printf("fd: %d\n",ptrThisClient->fd);
    int running = 1;
    while(running) {
        // receive the packet, according to protcol

        if (recv(ptrThisClient->fd, (void*) &type, 1, 0) <= 0) goto leave;
        
        // if (n < 0) {
        //     perror("recv: ");
        // } 

        if (recv(ptrThisClient->fd, (void*) &size, 4, 0) <= 0) goto leave;
        if (size > MAX_DATA_LENGTH) {
            printf("packet too large!\n");
            // clear recv buffer.
            clear_socket_buffer(ptrThisClient->fd);
            
            continue;
        }
        if (size > 0) 
            if (recv(ptrThisClient->fd, (void*) data, size, 0) <= 0) goto leave;

        switch (type) {

            case TYPE_REQ_PING:
                data[0] = TYPE_RESP_PING;
                *(uint32_t*)&data[1] = SIZE_RESP_PING;

                if (send(ptrThisClient->fd, data, SIZE_HEADER + SIZE_RESP_PING, 0) <= 0) goto leave;
                break;

            case TYPE_REQ_READY:
                if(data[0] != READY && data[0] != UNREADY) {
                    printf("Unexpected Parsing Error..\n");
                    continue;
                }

                ptrThisClient->is_ready = data[0] == READY ? true : false;

                pthread_mutex_lock(&lock_client);
                // TODO: buffer, size constants
                
                // Send Broadcast Ready
                data[0] = TYPE_BROADCAST_READY;
                *(uint32_t*)&data[1] = SIZE_BROADCAST_READY;
                data[5] = ptrThisClient->is_ready ? READY: UNREADY;
                *(uint32_t*)&data[6] = ptrThisClient->id;

                for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
                    if (!ptrClients[i].is_active || &ptrClients[i] == ptrThisClient) continue;
                    send(ptrClients[i].fd, data, SIZE_HEADER + SIZE_BROADCAST_READY, 0);
                }
                pthread_mutex_unlock(&lock_client);

                break; 
            
            
            
            case TYPE_REQ_LEAVE:
                leave:
                    // printf("fd: %d\n",ptrThisClient->fd);
                    close(ptrThisClient->fd);
                    pthread_mutex_lock(&lock_client);
                    (*ptrPlayer_count)--;
                    pthread_mutex_unlock(&lock_client);
                    ptrThisClient->is_active = false;
                    running = 0;
                    break;
            

            default:
                printf("Not Yet Handled.\n");
                break;
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