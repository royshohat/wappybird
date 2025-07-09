// server libs

// client libs
#include <sys/time.h>

#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/game.h"
#include "utils/util.h"

#include "net/net_const.h"

long long get_timestamp_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  // Convert seconds and microseconds to milliseconds
  return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void print_player(player_t player) {
  printf("Player #%d:\n"
         "    id:%d\n"
         "    is_ready:%d\n"
         "    is_alive:%d\n"
         "    Client:\n"
         "        fd:%d\n"
         "        offset_ms:%d\n"
         "        is_active:%d\n\n",
         player.id, player.id, player.is_ready, player.is_alive,
         player.client->fd, player.client->offset_ms, player.client->is_active);
}

int ping(int sockfd) {
  // get current timestamp
  char req[SIZE_HEADER + SIZE_REQ_PING];
  long long send_time = get_timestamp_ms();

  req[0] = TYPE_REQ_PING;
  *(uint32_t *)&req[1] = SIZE_REQ_PING;
  send(sockfd, req, sizeof(req), 0);
  // get server time

  char resp[SIZE_HEADER + SIZE_RESP_PING];
  resp[1] = TYPE_RESP_PING;
  *(uint32_t *)&resp[1] = SIZE_RESP_PING;

  recv(sockfd, resp, sizeof(resp), 0);
  long long recv_time = get_timestamp_ms();

  return (recv_time - send_time) / 2;
  // Basic steps for time sync:
  //  Client sends request to server, noting the time T1.
  //  Server receives, notes T2, sends current server time T3 back.
  //  Client receives reply, notes T4.
  //  Estimate round-trip time (RTT) and offset.
  // Formulas:
  // RTT ≈ (T4 - T1) - (T3 - T2)
  // Offset ≈ ((T2 - T1) + (T3 - T4)) / 2
}

int sync_timestamp(int fd) {
  // returns offset in ms between
  char header[SIZE_HEADER];
  // TODO
  return 0;
}
