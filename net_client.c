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
