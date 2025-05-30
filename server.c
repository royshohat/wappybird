#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "network.h"
#include <fcntl.h>
#include <limits.h>


#include "game.h"

volatile sig_atomic_t flag = 0;

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

typedef struct {
    int sockfd;
    struct sockaddr_in* server_addr;
    client_t* clients; // pointer to array
} thread_args; // accpet_clients arguments


void* handle_client(void* arg);
void handler(int sigint);
void* ctrlCmech(void* socket_fd);
void* accept_clients(void* args);


int main() {

    int sockfd;
    struct sockaddr_in server_addr;
    pthread_t id;

    // delete in production
    // terminate the server
    signal(SIGINT, handler); 
    pthread_create(&id, NULL, ctrlCmech, &sockfd);


    int code = init_server(&sockfd, &server_addr);
    if (code != 0) return code;
    
    thread_args args;

    client_t clients[MAX_PLAYER_COUNT];
    memset(clients, 0, MAX_PLAYER_COUNT * sizeof(client_t));
    // &sockfd
    args.sockfd = sockfd;
    args.server_addr = &server_addr;
    args.clients = clients;
    pthread_create(&id, NULL, accept_clients, (void*)&args);

    // while !(everyone ready) {
    // wait 
    sleep(15); 
    // }

    // kill thread accept_client
    close(sockfd);
    // print clients
    client_t empty_client;
    memset(&empty_client, 0, sizeof(client_t));

    for (int i=0; (memcmp((void*)&clients[i], &empty_client, sizeof(client_t)) != 0) && i < MAX_PLAYER_COUNT; i++) {
        print_client(clients[i]);
    }
    
    printf("End.\n");
}



void* accept_clients(void* args) {
    thread_args* t_args = (thread_args*) args;

    int sockfd = t_args->sockfd;
    struct sockaddr_in* server_addr = t_args->server_addr;
    client_t* clients = t_args->clients; // assume size of the array is MAX_PLAYER_COUNT
    
    int player_count = 0; // index..
    memset(clients, 0, MAX_PLAYER_COUNT * sizeof(client_t));
    
    pthread_t id;
    int c_id = 1;
    int client_fd;
    socklen_t len;
    
    printf("Accepting clients...\n");
    while(!0){
        if (player_count == MAX_PLAYER_COUNT) {
            printf("MAX PLAYER REACHED.\n");
            break;
        }
        // wait for players to connect
        if(((client_fd = accept(sockfd, (struct sockaddr *) server_addr, &len)) < 0)){
            if (errno == EBADF) {
                // server shutdown via Ctrl - c.
                printf("Everyone is ready!! \nListening socket closed, shutting down accept loop.\n");
            } else {
                // some kind of other error with accept.
                perror("accept failed");
            }
           
            return NULL; 
        }
        
        clients[player_count].fd = client_fd;
        clients[player_count].id = c_id;
        printf("Got A Conncection! (fd: %d)\n", client_fd);
        pthread_create(&id, NULL, handle_client, &clients[player_count]);
        player_count++;
        c_id++;
    }
}


void* handle_client(void* args) {
    // args = (int)
    client_t* client = (client_t*)args;
    // time_sync(client_fd);

    char data[MAX_DATA_LENGTH];
    packet_type t;
    uint32_t size;
    ssize_t n;
    // TODO: staging and validating.
    while(!0) {
        recv(client->fd, (void*) &t, 1, 0);
        recv(client->fd, (void*) &size, 4, 0);
        if (size > MAX_DATA_LENGTH) {
            printf("packet too large!\n");
            // clear recv buffer.
            // set socket flag to non blocking
            int flags = fcntl(client->fd, F_GETFL, 0) | O_NONBLOCK;
            fcntl(client->fd, F_SETFL, flags);
            
            do {
                n = recv(client->fd, data, sizeof(data), 0);
            } while (n > 0);
            // the buffer is now empty :)
            // unset block flag
            fcntl(client->fd, F_SETFL, flags & (O_NONBLOCK ^ INT_MAX));
            continue;
        }
        
        recv(client->fd, (void*) data, size, 0);

        switch (t) {
            case REQ_READY:
                if(data[0] != READY && data[0] != UNREADY) {
                    printf("Unexpected Parsing Error..\n");
                    continue;
                }
                
                client->is_ready = data[0] == READY ? true : false;
                break; 
            case BROADCAST_READY:
                break;
        } 
    }
    // for (int i = 0; i < 40; i++) {
    //     // wait for client ping
    //     if (recv(client->fd, buf, sizeof(buf), 0) == -1) {
    //         printf("Client closed!\n");
    //         break;
    //     }
    //     send(client->fd, "Pong!", 6, 0);
    // } 
    close(client->fd);
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