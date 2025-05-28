long long get_timestamp_ms();
int ping(int sockfd);
void init_client(int* sockfd, struct sockaddr_in* server_addr);
void init_server(int* sockfd, struct sockaddr_in* server_addr);
