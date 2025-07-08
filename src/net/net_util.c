#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "net/net_util.h"

uint32_t get_packet_size(packet_type type) {
  switch (type) {
  case TYPE_REQ_LEAVE:
    return SIZE_REQ_LEAVE;
  case TYPE_BROADCAST_LEAVE:
    return SIZE_BROADCAST_JOIN;
  case TYPE_REQ_PING:
    return SIZE_REQ_PING;
  case TYPE_RESP_PING:
    return SIZE_RESP_PING;
  case TYPE_RESP_JOIN:
    return SIZE_RESP_JOIN;
  case TYPE_REQ_JOIN:
    return SIZE_REQ_JOIN;
  case TYPE_BROADCAST_JOIN:
    return SIZE_BROADCAST_JOIN;
  case TYPE_REQ_READY:
    return SIZE_REQ_READY;
  case TYPE_BROADCAST_READY:
    return SIZE_BROADCAST_READY;
  case TYPE_REQ_TIMESTAMP:
    return SIZE_REQ_TIMESTAMP;
  case TYPE_RESP_TIMESTAMP:
    return SIZE_RESP_TIMESTAMP;
  case TYPE_BROADCAST_START_GAME:
    return SIZE_BROADCAST_START_GAME;
  case TYPE_REQ_UPDATE_STATE:
    return SIZE_REQ_UPDATE_STATE;
  case TYPE_BROADCAST_UPDATE_STATE:
    return SIZE_BROADCAST_UPDATE_STATE;
  default:
    printf("in function get_packet_size: No such type. %d\n", type);
    return 0;
  }
}

void clear_socket_buffer(int sockfd) {
  // set socket flag to non blocking
  long n;
  int data[1024];
  int flags = fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);

  do {
    n = recv(sockfd, data, sizeof(data), 0);
  } while (n > 0);
  // the buffer is now empty :)
  // unset block flag
  fcntl(sockfd, F_SETFL, flags & (O_NONBLOCK ^ INT_MAX));
}

void print_packet(packet_type type, packet_fields *fields) {
  printf("TYPE: %d\n", type);
  char data[MAX_DATA_LENGTH];

  switch (type) {
  case TYPE_BROADCAST_LEAVE:
    *(uint32_t *)(data) = fields->id;
    break;
  case TYPE_REQ_PING:
    // no data.
    break;
  case TYPE_RESP_JOIN:
    *(uint32_t *)(data) = fields->id;
    break;
  case TYPE_BROADCAST_JOIN:
    *(uint32_t *)(data) = fields->id;
    break;
  case TYPE_BROADCAST_READY:
    *(data) = fields->is_ready;
    *(uint32_t *)(data + 1) = fields->id;
    break;
  case TYPE_REQ_TIMESTAMP:
    *(uint64_t *)(data) = fields->timestamp;
    break;
  case TYPE_BROADCAST_START_GAME: // TODO
    break;
  case TYPE_BROADCAST_UPDATE_STATE:
    memcpy(data, fields->players_array, SIZE_BROADCAST_UPDATE_STATE);
    break;
  case TYPE_RESP_PING:
    break;
  case TYPE_REQ_READY:
    *(data) = fields->is_ready;
    break;
  case TYPE_RESP_TIMESTAMP:
    *(uint64_t *)(data) = fields->timestamp;
    break;
  case TYPE_REQ_UPDATE_STATE: // TODO
    break;
  default:
    printf("send: not implemented.\n");
    return;
  }

  printf("DATA: ");
  for (uint32_t i = 0; i < get_packet_size(type); i++) {
    printf("%x ", data[i]);
  }
  printf("\n");
}
