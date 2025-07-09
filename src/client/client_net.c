#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/game.h"
#include "net/net.h"
#include "net/net_util.h"
#include "utils/util.h"

packet_type recv_packet(int fd, packet_fields *fields) {
  if (fd <= 0) {
    perror("Invalid file desc.");
    return 1;
  }

  packet_type type = 0;
  recv(fd, &type, 1, 0);

  switch (type) {
  case TYPE_BROADCAST_LEAVE:
    recv(fd, &(fields->id), sizeof(fields->id), 0);
    break;
  case TYPE_REQ_PING:
    // no data
    break;
  case TYPE_RESP_JOIN:
    recv(fd, &(fields->id), sizeof(fields->id), 0);
    break;
  case TYPE_BROADCAST_JOIN:
    recv(fd, &(fields->id), sizeof(fields->id), 0);
    break;
  case TYPE_BROADCAST_READY:
    recv(fd, &(fields->is_ready), sizeof(fields->is_ready), 0);
    recv(fd, &(fields->id), sizeof(fields->id), 0);
    break;
  case TYPE_REQ_TIMESTAMP:
    recv(fd, &(fields->timestamp), sizeof(fields->timestamp), 0);
    break;
  case TYPE_BROADCAST_START_GAME: // TODO
    break;
  case TYPE_BROADCAST_UPDATE_STATE:
    recv(fd, fields->players_array, SIZE_BROADCAST_UPDATE_STATE, 0);
    break;
  default:
    printf("recv: not implemented\n");
    break;
  }
  return type;
}

int send_packet(int fd, packet_type type, packet_fields *fields) {
  if (fd <= 0) {
    perror("Invalid file desc.");
    return EBADFD;
  }

  char msg_buffer[MAX_DATA_LENGTH];

  *msg_buffer = type;

  switch (type) {
  case TYPE_REQ_LEAVE:
    break;
  case TYPE_REQ_JOIN:
    break;
  case TYPE_RESP_PING:
    break;
  case TYPE_REQ_READY:
    *(msg_buffer + SIZE_HEADER) = fields->is_ready;
    break;
  case TYPE_RESP_TIMESTAMP:
    *(uint64_t *)(msg_buffer + SIZE_HEADER) = fields->timestamp;
    break;
  case TYPE_REQ_UPDATE_STATE: // TODO
    break;
  default:
    printf("send: not implemented.\n");
    return ENOSYS;
  }
  for (uint32_t i = 0; i < SIZE_HEADER + get_packet_size(type); i++) {
    printf("%x ", msg_buffer[i]);
  }
  printf("\n");
  send(fd, msg_buffer, SIZE_HEADER + get_packet_size(type), 0);
  return 0;
}

int init_networking(vars_t *game_vars) {

  // Create socket
  game_vars->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (game_vars->server_fd < 0) {
    perror("Socket creation failed");
    return 1;
  }

  // Set server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr)); // Clear memory
  server_addr.sin_family = AF_INET;             // IPv4
  server_addr.sin_port = htons(SERVER_PORT);    // Server port to big endian
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_port) <= 0) {
    perror("Invalid address / Address not supported");
    close(game_vars->server_fd);
    return 1;
  }

  // Connect to the server
  if (connect(game_vars->server_fd, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(game_vars->server_fd);
    return 1;
  }
  return 0;
  // int n;
  // char buf[SIZE_HEADER];
  // usleep(250 * 1000); // qureter of a sec
  // n = recv(*sockfd, buf, 1, MSG_PEEK | MSG_DONTWAIT);
  // if (n == 0) return 1; // connection closed (refused.)

  // recv clients array

  // if(buf[0] != TYPE_RESP_UPDATE_ARRAY) return -1;
  // n = recv(*sockfd, (void*)players_arr, *(uint32_t*) &buf[1], 0);

  // for (int i = 0; i < n; i++) {
  //     printf("%02X ", (unsigned char)buf[i]);  // Print as hex
  // }
  // printf("\n");

  // // recv my id
  // recv(*sockfd, buf, sizeof(buf), 0);

  // recv(*sockfd, id, *(uint32_t*) &buf[1], 0);
}
