#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <sys/time.h>

#include "util.h"
#include "game.h"


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    init_client(&sockfd, &server_addr);

    printf("Connected to server! %s:%d\n", SERVER_IP, SERVER_PORT);
    
    // exit
    close(sockfd);
    return 0;
}

void init_client(int* sockfd, struct sockaddr_in* server_addr) {

    // Create socket
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr)); // Clear memory
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
}