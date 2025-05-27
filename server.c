#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define TCP_PORT 8080
#define UDP_PORT 9000

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

int flag = 0;
const int MAX_PLAYER_COUNT = 2;

void* handle_client(void* arg);
void cleanup(int sig);
void handler(int sigint);
void* ctrlCmech(void* socket_fd);

int main() {
    // listen for new connections
    //struct sigaction sa;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating a socket.");
        return 1;
    }
    
    /* nice to have, dont care if this fails.*/
  // (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)); 
  // delete in production.
    
    signal(SIGINT, handler);
    pthread_t id;
    pthread_create(&id, NULL, ctrlCmech, &sockfd);

    struct sockaddr_in server_addr;
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(TCP_PORT); 
    
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))==-1){
        perror ("Error binding address.");
        close(sockfd);
        return 1;
    }
    
    if(listen(sockfd, MAX_PLAYER_COUNT)==-1){
        perror ("Error Listening to socket.");
        close(sockfd);
        return 1;
    }
    int player_count = 0;
    // int* client_fds = malloc(sizeof(int) * MAX_PLAYER_COUNT);
    int client_fds[sizeof(int) * MAX_PLAYER_COUNT];
    int client_fd;
    struct sockaddr client;
    socklen_t len;
    printf("Accepting clients...\n");
    while(!0){
        // wait for players to connect
        if(((client_fd = accept(sockfd, (struct sockaddr *) &server_addr, &len)) == -1)){
            printf("OhNo!!\n");
            return 1; 
            // this means that the server is closed
            // via Ctrl - c OR some kind of other error with accept.
        }
        printf("Got A Conncection!\n");
        //fflush(stdout);
        if (player_count == MAX_PLAYER_COUNT) {
            printf("MAX PLAYER REACHED.\n");
            continue;
        }
        client_fds[player_count++] = client_fd;
        pthread_create(&id, NULL, handle_client, &client_fd);
    }
    printf("End..\n");
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
    exit(1); // terminate the process
    return NULL;
}

void handler(int sig){
    flag = 1;
}