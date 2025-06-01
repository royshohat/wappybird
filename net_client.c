#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <sys/time.h>

#include "util.h"
#include "network.h"
#include "game.h"


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    if(init_client(&sockfd, &server_addr)!=0) {
        return 1;
    }
    
    printf("Connected to server! %s:%d (fd: %d)\n", SERVER_IP, SERVER_PORT, sockfd);
    printf("Pinging..\n");
    int pings[3];
    for (int i=0 ; i<3; i++) {
        sleep(1);
        int latency = ping(sockfd);
        printf("ping: %d\n", latency);
        pings[i] = latency;
    }
    int sum = 0;
    for (int i=0 ; i<3; i++) {
        sum += pings[i];
    }
    printf("Average ping: %d\n", sum/3);
    printf("Time taken: %d\n", sum); 

    char req[SIZE_HEADER + SIZE_REQ_LEAVE];
    req[0] = TYPE_REQ_LEAVE;
    *(uint32_t*) &req[1] = SIZE_REQ_LEAVE;
    send(sockfd, req, sizeof(req), 0);
    // exit
    close(sockfd);
    return 0;
}
