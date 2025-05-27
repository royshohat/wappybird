#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <sys/time.h>

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port


long long get_timestamp_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert seconds and microseconds to milliseconds
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

int ping(int sockfd) {
    // get current timestamp
    char buf[10];
    long long send_time = get_timestamp_ms();
    send(sockfd, "Ping!", 6, 0);
    // get server time

    recv(sockfd, buf, sizeof(buf), 0);
    long long recv_time = get_timestamp_ms();
    



    return (recv_time - send_time) / 2;

    //Basic steps for time sync:
        // Client sends request to server, noting the time T1.
        // Server receives, notes T2, sends current server time T3 back.
        // Client receives reply, notes T4.
        // Estimate round-trip time (RTT) and offset.
    // Formulas:
        // RTT ≈ (T4 - T1) - (T3 - T2)
        // Offset ≈ ((T2 - T1) + (T3 - T4)) / 2
}


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char *message = "Hello, Server!";
    char buffer[1024];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr)); // Clear memory
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_port = htons(SERVER_PORT);     // Server port
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    printf("Connected to server! %s:%d\n", SERVER_IP, SERVER_PORT);

    printf("pinging 10 times..\n");
    for (int i = 0; i < 40; i++) {
        int p = ping(sockfd);
        printf("ping: %dms\n", p);
    }
    

    // Close the socket
    close(sockfd);

    return 0;
}