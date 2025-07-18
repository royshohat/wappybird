#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h> // for close()

#include "common/game.h"
#include "common/player.h"
#include "net/net.h"
#include "utils/logger.h"
#include "utils/util.h"

void *listen_to_server(void *args);
int mainloop(vars_t *game_vars);

typedef struct {
  player_t *players;
  int sockfd;
  int id;
} listen_to_server_args;

int main() {

  vars_t game_vars;

  if (init_networking(&game_vars) != 0)
    return 1;

  logger(1, LOG_INFO, "Connected to server! %s:%d (fd: %d)\n", SERVER_IP,
         SERVER_PORT, game_vars.server_fd);
  printf("mainloop\n");

  mainloop(&game_vars);
  return 0;
}

int mainloop(vars_t *game_vars) {

  char input[101];
  packet_fields fields;
  packet_type type;

  while (!0) {
    printf("Enter packet type: ");
    scanf("%100s", input);

    int len = strlen(input);
    // printf("%s", input);
    if (strncmp(input, "REQ_JOIN", len) == 0) {
      type = TYPE_REQ_JOIN;
    }
    if (strncmp(input, "REQ_LEAVE", len) == 0) {
      type = TYPE_REQ_LEAVE;
    }
    if (strncmp(input, "REQ_PING", len) == 0) {
      type = TYPE_REQ_PING;
    }
    if (strncmp(input, "REQ_READY", len) == 0) {
      fields.is_ready = true;
      type = TYPE_REQ_READY;
    }
    if (strncmp(input, "REQ_TIMESTAMP", len) == 0) {
      fields.timestamp = get_timestamp_ms();
      type = TYPE_REQ_TIMESTAMP;
    }

    send_packet(game_vars->server_fd, type, &fields);
    int flags = fcntl(game_vars->server_fd, F_GETFL);
    fcntl(game_vars->server_fd, F_SETFL, flags | O_NONBLOCK);
    recv_packet(game_vars->server_fd, &fields);
    fcntl(game_vars->server_fd, F_SETFL, flags);

    // if (strncmp(input, "REQ_TIMESTAMP", len) == 0 ||
    // strncmp(input, "REQ_PING", len) == 0) {
    // recv_packet(sockfd, &fields);
    //}
  }

  return 0;
}

void *listen_to_server(void *args) {
  return NULL;
  listen_to_server_args *t_args = (listen_to_server_args *)args;
  player_t *players = t_args->players;
  int sockfd = t_args->sockfd;
  int id = t_args->id;

  bool running = true;
  packet_type type = 0;
  uint32_t size;

  char data[MAX_DATA_LENGTH];
  /*
      while (running) {
          pthread_mutex_lock(&lock_players);
          for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
              if (!players[i].client.is_active) break;
              print_player(players[i]);
          }
          pthread_mutex_unlock(&lock_players);

          if (recv(sockfd, &type, SIZE_PACKET_TYPE, 0) <= 0) goto leave;
          // TODO: if (recv(sockfd, &size, SIZE_PACKET_LEN, 0) <= 0) goto leave;
          if (size > MAX_DATA_LENGTH) { // server shouldnt behave like that.
              // just in case, we handle error so the client doesn't collapse.
              printf("packet too large! discarding...\n");
              // clear recv buffer.
              clear_socket_buffer(sockfd);
              continue; // discard the packet
          }

          switch (type)
          {

          case TYPE_REQ_TIMESTAMP:

              recv(sockfd, data, SIZE_REQ_TIMESTAMP, 0);

              char resp[SIZE_HEADER + SIZE_RESP_TIMESTAMP];
              resp[0] = TYPE_RESP_TIMESTAMP;
              *(uint32_t*) &resp[1] = SIZE_RESP_TIMESTAMP;
              *(uint64_t*) &resp[5] = get_timestamp_ms();
              send(sockfd, resp, sizeof(resp), 0);
              break;

          case TYPE_BROADCAST_JOIN:
              break;

          case TYPE_BROADCAST_LEAVE:
              break;

          case TYPE_BROADCAST_READY:
              break;


          case TYPE_BROADCAST_START_GAME:
              break;

          case TYPE_BROADCAST_UPDATE_STATE:
              break;

          default:
              break;
          }
      }
      // leave
      leave:
          char req[SIZE_HEADER + SIZE_REQ_LEAVE];
          req[0] = TYPE_REQ_LEAVE;
          *(uint32_t*) &req[1] = SIZE_REQ_LEAVE;
          send(sockfd, req, sizeof(req), 0);
          close(sockfd);
  */
}
