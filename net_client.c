#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for close()
#include <arpa/inet.h> // for sockaddr_in, inet_pton()

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8080      // Change this to the server's port

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

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

    // Send message to the server
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Send failed");
        close(sockfd);
        return 1;
    }

    printf("Message sent: %s\n", message);

    // Receive response from server
    int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        close(sockfd);
        return 1;
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Response from server: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}