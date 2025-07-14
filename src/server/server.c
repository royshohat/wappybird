#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>
#include <signal.h>
#include <sys/select.h>

#include <fcntl.h>

#include "common/game.h"
#include "common/game_const.h"
#include "common/game_util.h"
#include "common/player.h"
#include "net/client.h"
#include "net/net.h"
#include "net/net_const.h"
#include "net/net_util.h"
#include "utils/logger.h"

stage game_stage = STAGE_WAIT_FOR_PLAYERS;

static int id_count = 0;

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

bool is_game_stage_equal(stage gs);
void time_sync(player_t *ptrPlayer);
void main_loop(vars_t *game_vars);
int handle_client(client_t *this_client, vars_t *game_vars);
int accept_client(int sockfd, client_t *clients, size_t *client_count_p);

int main() {
  vars_t game_vars;

  if (init_networking(&game_vars) != 0)
    return 1;

  // TODO: Thread this call.
  main_loop(&game_vars);

  printf("End.\n");
  return 0;
}

void main_loop(vars_t *game_vars) {
  fd_set sockets;
  packet_fields fields;
  packet_type type;

  while (!0) {
    // construct a new set of fds each time.
    FD_ZERO(&sockets);
    FD_SET(game_vars->server_fd, &sockets);
    int max_fd = game_vars->server_fd; // max file descriptor value in set.
    // loop and add all active clients
    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
      if (!game_vars->clients[i].is_active)
        continue;
      FD_SET(game_vars->clients[i].fd, &sockets);
      if (game_vars->clients[i].fd > max_fd)
        max_fd = game_vars->clients[i].fd;
    }

    // dont care about write and error fds
    // wait forever. (the reason this is in a thread)
    logger(1, LOG_INFO, "Listening for requests.. server_fd=%d\n",
           game_vars->server_fd);
    int n_ready = select(max_fd + 1, &sockets, NULL, NULL, NULL);
    if (n_ready < 0) {
      perror("select: ");
    }
    int ready_fd = -1;
    for (int i = 3; i < max_fd + 1; i++) {
      if (FD_ISSET(i, &sockets)) {
        ready_fd = i;
      }
    }
    if (ready_fd == -1) {
      logger(1, LOG_ERROR, "No fd is ready!");
    }
    printf("%d\n", ready_fd);
    // logger(1, LOG_INFO, "ready_fd=%d", n_ready);
    if (game_vars->server_fd == ready_fd) {
      // new connection
      int fd = accept_client(game_vars->server_fd, game_vars->clients,
                             &game_vars->client_count);
      printf("%d\n\n", fd);
    } else {
      // client action
      char buf[1];
      // peek at the client scoket buffer to see if it closed.
      ssize_t ret = recv(ready_fd, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT);

      client_t *this_client;
      // find the client object with the ready fd
      for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        if (!game_vars->clients[i].is_active)
          continue;
        if (game_vars->clients[i].fd == ready_fd) {
          this_client = &game_vars->clients[i];
          break;
        }
      }

      if (ret == 0) {
        logger(1, LOG_INFO, "closing client with fd=%d", this_client->fd);
        // close client : remove client from array.
        this_client->is_active = false;
        close(this_client->fd);
        continue; // back to select call
      } else if (ret < 0) {
        perror("recv: ");
      }
      //logger(1, LOG_INFO, "handling client request fd=%d", this_client->fd);
      printf("hi\n");
      fflush(stdout);
      type = recv_packet(this_client->fd, &fields);
      handle_packet(this_client->fd, type, &fields, game_vars);
    }
  }
}

int accept_client(int sockfd, client_t *clients, size_t *client_count_p) {
  int fd;
  // test for max client count or if the game is already underway
  if (*client_count_p == MAX_CLIENT_COUNT ||
      game_stage != STAGE_WAIT_FOR_PLAYERS) {
    if (*client_count_p == MAX_CLIENT_COUNT)
      printf("MAX_CLIENT_COUNT=%d REACHED.\n", MAX_CLIENT_COUNT);
    else
      printf("Game already in progress!\n");

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    fd = accept(sockfd, NULL, NULL);
    if (fd != -1)
      close(fd); // close the connection in the face cause server is full
    // restore server socket flags to blocking again
    fcntl(sockfd, F_SETFL, flags);
    return -1;
  }

  // look where the free space is and accept.
  int i=0;
  for (;i < MAX_CLIENT_COUNT; i++) 
    if (!clients[i].is_active)
      break;
  assert(i!=MAX_CLIENT_COUNT);
  fd = accept(sockfd, (struct sockaddr *)&clients[i].addr,
              &clients[i].addr_len);
  printf("in accept:%d\n", fd);
  if (fd < 0) {
    perror("accept: ");
    return -1;
  }
  clients[i].fd = fd;
  clients[i].is_active = true;
  clients[i].offset_ms = 0; // default value. change later;

  printf("Got A Conncection! (ip:%s, fd: %d)\n",
          inet_ntoa(clients[i].addr.sin_addr), fd);


  (*client_count_p)++;

  return fd;
}
