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

#define TCP_PORT 8080
#define UDP_PORT 9000

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);


volatile sig_atomic_t flag = 0;
const int MAX_PLAYER_COUNT = 2;

void* handle_client(void* arg);
void cleanup(int sig);
void handler(int sigint);
void* ctrlCmech(void* socket_fd);
int init_socket(int *sockfd, struct sockaddr_in* server_addr);

typedef struct {
    int sockfd;
    struct sockaddr_in* server_addr;
    client_t* clients; // pointer to array
} thread_args;

typedef struct {
    int id; 
    int fd;
    double offset_ms;
    bool is_ready;
    // bird...
} client_t; 

void* accept_clients(void* args) {
    thread_args* t_args = (thread_args*) args;

    int sockfd = t_args->sockfd;
    struct sockaddr_in* server_addr = t_args->server_addr;
    client_t* clients = t_args->clients; // assume size of the array is MAX_PLAYER_COUNT
    
    int player_count = 0; // index..
    memset(clients, 0, MAX_PLAYER_COUNT * sizeof(client_t));
    
    pthread_t id;
    int client_id = 1;
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
        clients[player_count].id = client_id;
        printf("Got A Conncection!\n");
        pthread_create(&id, NULL, handle_client, &clients[player_count].fd);
        player_count++;
        client_id++;
    }
}

int main() {

    int sockfd;
    struct sockaddr_in server_addr;
    pthread_t id;

    // delete in production
    // terminate the server
    signal(SIGINT, handler); 
    pthread_create(&id, NULL, ctrlCmech, &sockfd);


    int code = init_socket(&sockfd, &server_addr);
    if (code != 0) return code;
    
    // pthread_create;
    thread_args args;

    client_t clients[MAX_PLAYER_COUNT];
    // &sockfd
    args.sockfd = sockfd;
    args.server_addr = &server_addr;
    args.clients = clients;
    pthread_create(&id, NULL, accept_clients, (void*)&args);

    
    
    // while !(everyone ready) {
    // wait 
    sleep(10); 
// }
    // kill thread accept_client
    client_t empty_client;
    memset(&empty_client, 0, sizeof(client_t));
    for (int i=0; memcmp((void*)&clients[i], &empty_client, sizeof(client_t)) != 0; i++) {
        print(clients[i]);
    }
    
    printf("End.\n");
}

void print(client_t c) {
    printf("Client %d:\nfd:%d\nis_ready%d\noffset_ms:%d\n\n",c.id, c.fd, c.is_ready, c.offset_ms);
}

void time_sync(int client_fd) {
    
}


void* handle_client(void* args) {
    // args = (int)
    int client_fd = *(int*)args;
    char buf[10];
    // time_sync(client_fd);

    for (int i = 0; i < 40; i++) {
        // wait for client ping
        if (recv(client_fd, buf, sizeof(buf), 0) == -1) {
            printf("Client closed!\n");
            break;
        }
        send(client_fd, "Pong!", 6, 0);
    } 
    close(client_fd);
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

int init_socket(int *sockfd, struct sockaddr_in* server_addr) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        perror("Error creating a socket.");
        return 1;
    }
        /* nice to have, dont care if this fails.*/
  // (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)); 
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