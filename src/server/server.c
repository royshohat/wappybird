#include <errno.h>
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
#include "common/player.h"
#include "net/client.h"
#include "net/net.h"
#include "net/net_const.h"

stage game_stage = STAGE_WAIT_FOR_PLAYERS;

// int* ptr
// ptr -> (int*)
// &ptr -> (int**)
// pthread_join(id, (void**)&ptr);

bool is_game_stage_equal(stage gs);
void time_sync(player_t *ptrPlayer);
void main_loop(vars_t *game_vars);
int handle_client(vars_t *game_vars);
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
  FD_ZERO(&sockets);
  FD_SET(game_vars->server_fd, &sockets);
  int nfds = 1; // number of file descriptors in set.
  while (!0) {
    // dont care about write and error
    // wait forever. (the reason this is in a thread)
    int ready_fd = select(nfds, &sockets, NULL, NULL, NULL);
    if (ready_fd == game_vars->server_fd) {
      // new connection
      int fd = accept_client(game_vars->server_fd, game_vars->clients,
                             &game_vars->client_count);
      FD_SET(fd, &sockets); // add to listening set
    } else {
      // client action
      char buf[1];
      // peek at the client scoket buffer to see if it closed.
      ssize_t ret = recv(ready_fd, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT);
      if (ret == 0) {
        // close the client
        FD_CLR(ready_fd, &sockets);
        // cleanup: look for the client in the array and remove the client.
        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
          if (!game_vars->clients[i].is_active)
            continue;
          if (game_vars->clients[i].fd == ready_fd) {
            game_vars->clients[i].is_active = false;
            break;
          }
        }
        continue; // back to select call
      } else if (ret < 0) {
        perror("recv: ");
      }
      handle_client(game_vars); //whats with the hanle client this isnt a function no more
      // i think you meant handle packet or somthing 
    }
  }
}

int accept_client(int sockfd, client_t *clients, size_t *client_count_p) {
  int fd;
  // test for max client count or if the game is already underway
  if (*client_count_p == MAX_CLIENT_COUNT || game_stage != STAGE_WAIT_FOR_PLAYERS) {
    if(*client_count_p == MAX_CLIENT_COUNT)
      printf("MAX_CLIENT_COUNT=%d REACHED.\n", MAX_CLIENT_COUNT);
    else
      printf("Game already in progress!\n", MAX_CLIENT_COUNT);

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
  for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
    if (!clients[i].is_active)
      continue;
    fd = accept(sockfd, (struct sockaddr *)&clients[i].addr,
                &clients[i].addr_len);
    if (fd < 0) {
      perror("accept: ");
      return -1;
    }
    clients[i].fd = fd;
    clients[i].is_active = true;
    clients[i].offset_ms = 0; // default value. change later;

    printf("Got A Conncection! (ip:%s, fd: %d)\n",
           inet_ntoa(clients[i].addr.sin_addr), fd);
  }

  (*client_count_p)++;

  return fd;
}

int handle_client(vars_t *game_vars) {
  return 0;
  // todo : validation of of stages

  while (!0) {
    break;
    /*
        if (is_game_stage_equal(STAGE_SYNC_TIME) &&
            ptrThisPlayer->client.offset_ms == INT_MAX) {
          time_sync(ptrThisPlayer);
        }
        printf("Receiving a packet\n");
        type = recv_packet(ptrThisPlayer->client.fd, &fields);
        printf("packet type: %d\n", type);
        fflush(stdout);

        pthread_mutex_lock(&lock_players);
        int res = handle_packet(ptrThisPlayer->client.fd, type, &fields,
       ptrPlayers, ptrPlayer_count); printf("handle result: %d\n", res);
        pthread_mutex_unlock(&lock_players);

        if (type == TYPE_REQ_LEAVE) {
          printf("closing file des : %d\n", ptrThisPlayer->client.fd);
          return NULL;
        } */
  }
}
