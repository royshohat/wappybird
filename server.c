#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080
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
    struct sigaction sa;
    
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
    server_addr.sin_port = htons(PORT); 
    
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
    // POSIX
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
            return 1; 
            // this means that the server is closed
            // via Ctrl - c OR some kind of other error with accept.
        }
        printf("Got A Conncection!");
        client_fds[player_count++] = client_fd;
        pthread_create(&id, NULL, handle_client, &client_fd);
    }
}

void* handle_client(void* args) {
    // args = (int)
    int client_fd = *(int*)args;
    printf("%d", client_fd);
    char str[] = "Hellloooo!";
    char buf[1024];
    const char *html_content =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <title>Simple C Server</title>\n"
        "  <style>\n"
        "    body { font-family: sans-serif; background-color: #f0f0f0; }\n"
        "    h1 { color: #333; }\n"
        "    p { color: #555; }\n"
        "  </style>\n"
        "</head>\n"
        "<body>\n"
        "  <h1>Hello from C Server!</h1>\n"
        "  <p>This page was sent by a simple C TCP server.</p>\n"
        "  <p>Current time might not be accurate, but it's fun!</p>\n"
        "</body>\n"
        "</html>";
    char resp[] = "HTTP/1.1 200 OK\r\nDate: Tue, 23 Apr 2025 00:00:00 GMT\r\nServer: Apache/2.4.41 (Ubuntu)\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 137\r\nConnection: close\r\n\r\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Example Page</title>\n</head>\n<body>\n    <h1>Hello, world!</h1>\n    <p>This is a sample HTTP/1.1 response.</p>\n</body>\n</html>";
    // \r\n CRLF <-- windows
    // \n LF <-- Normal operating systems
    memset(&buf, 0, sizeof(buf));
    read(client_fd, buf, sizeof(buf));
    printf("%s\n", buf);
    send(client_fd, resp, sizeof(resp), 0);
    
    close(client_fd);
    
    pthread_exit(0);
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