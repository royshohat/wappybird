#include "net/net.h"
#include "net/net_util.h"

#include <arpa/inet.h> // for sockaddr_in, inet_pton()
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int send_packet(int fd, packet_type type, packet_fields *fields) {
  if (fd <= 0) {
    perror("Invalid file desc.");
    return 1;
  }
  char msg_buffer[SIZE_HEADER + MAX_DATA_LENGTH];

  // construct the header
  // uint32_t size = get_packet_size(type);
  // *(uint32_t*) (msg_buffer + 1) = size;

  *msg_buffer = type;
  // 1. transform project to use select
  // 2. implement ping in server: (in handle client recv packet as usual and add
  // a case for ping in the switch statment)
  //
  switch (type) {
  case TYPE_BROADCAST_LEAVE:
    *(uint32_t *)(msg_buffer + SIZE_HEADER) = fields->id;
    break;
  case TYPE_RESP_PING:
    // no data.
    break;
  case TYPE_RESP_JOIN:
    *(uint32_t *)(msg_buffer + SIZE_HEADER) = fields->id;
    break;
  case TYPE_BROADCAST_JOIN:
    *(uint32_t *)(msg_buffer + SIZE_HEADER) = fields->id;
    break;
  case TYPE_BROADCAST_READY:
    *(msg_buffer + SIZE_HEADER) = fields->is_ready;
    *(uint32_t *)(msg_buffer + SIZE_HEADER + 1) = fields->id;
    break;
  case TYPE_REQ_TIMESTAMP:
    *(uint64_t *)(msg_buffer + SIZE_HEADER) = fields->timestamp;
    break;
  case TYPE_BROADCAST_START_GAME: // TODO
    break;
  case TYPE_BROADCAST_UPDATE_STATE:
    memcpy(msg_buffer + SIZE_HEADER, fields->players_array,
           SIZE_BROADCAST_UPDATE_STATE);
    break;
  default:
    printf("send: not implemented.\n");
    return 1;
  }
  send(fd, msg_buffer, SIZE_HEADER + get_packet_size(type), 0);
  return 0;
}

packet_type recv_packet(int fd, packet_fields *fields) {
  if (fd <= 0) {
    perror("Invalid file desc.");
    return TYPE_INVALID;
  }
  packet_type type = TYPE_REQ_LEAVE;

  char msg_buffer[MAX_DATA_LENGTH];
  recv(fd, &type, 1, 0);

  switch (type) {
  case TYPE_REQ_LEAVE:
    // if fd is closed
    return TYPE_REQ_LEAVE;
  case TYPE_REQ_PING:
    // no data.
    break;
  case TYPE_REQ_JOIN:
    break;
  case TYPE_REQ_READY:
    recv(fd, &(fields->is_ready), sizeof(fields->is_ready), 0);
    break;
  case TYPE_RESP_TIMESTAMP:
    recv(fd, &(fields->timestamp), sizeof(fields->timestamp), 0);
    break;
  case TYPE_REQ_UPDATE_STATE:
    // no data?
    break;
  default:
    printf("recv: not implemented\n");
    return TYPE_INVALID;
  }
  return type;
}

int handle_packet(int fd, packet_type type, packet_fields *fields,
                  vars_t *game_vars) {
  // not thread safe
  // destroys fields.
  switch (type) {
  case TYPE_REQ_PING:

    break;
  case TYPE_REQ_LEAVE:
    // player_p = client_to_player(this_client, game_vars->players);
    // if (player_p == NULL)
    //   return 1; // didnt join so cant leave.
    // fields.id = player_p->id;
    // broadcast(player_p, game_vars->players, TYPE_BROADCAST_LEAVE, &fields);
    break;
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
      // set the player with the current fd to is_active = false.
      if (!game_vars->players[i].client->is_active)
        continue;
      if (game_vars->players[i].client->fd == fd) {
        game_vars->players[i].client->is_active = false;
        close(fd);
        break;
      }
    }
    // decrement player count
    (game_vars->players_count)--;

    break;
  case TYPE_REQ_JOIN:
    // find the current player object (stored in player struct)
    player_t *player = game_vars->players; // default value
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
      if (!game_vars->players[i].client->is_active)
        continue;
      if (game_vars->players[i].client->fd == fd) {
        if (game_vars->players[i].is_alive)
          return 1; // already joined
        player = (game_vars->players + i);
        break;
      }
    }
    player->id = fd;
    player->is_ready = false;
    player->is_alive = true;

    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
      if (!game_vars->players[i].client->is_active ||
          (game_vars->players + i) == player)
        continue;
      fields->id = player->id;
      send_packet(player->client->fd, TYPE_BROADCAST_JOIN, fields);
    }

    break;
  case TYPE_REQ_READY:
    // Todo: integrate
    // if (game_vars->players_count == MAX_PLAYER_COUNT)
    //   return 1;
    // for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
    //   if (game_vars->players[i].is_active)
    //     continue; // skip filled slots
    //   player_p = &game_vars->players[i];
    //   // TODO: seperate into a function.
    //   player_p->is_active = true;
    //   player_p->client = this_client;
    //   player_p->id = id_count;
    //   id_count++;
    //   player_p->is_alive = true;
    //   player_p->is_ready = false;
    // }
    // broadcast(player_p, game_vars->players, TYPE_BROADCAST_LEAVE, &fields);
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
      // set the player with the current fd to is_active = false.
      if (!game_vars->players[i].client->is_active)
        continue;
      if (game_vars->players[i].client->fd == fd) {
        game_vars->players[i].is_ready = true;
        break;
      }
    }
    wait_ready(game_vars->players, &(game_vars->players_count));
    break;
  case TYPE_RESP_TIMESTAMP:
    break;
  case TYPE_REQ_UPDATE_STATE:
    break;
  default:
    printf("recv: not implemented\n");
    break;
  }
  return 0;
}

int init_networking(vars_t *vars) {
  vars->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (vars->server_fd == -1) {
    perror("Error creating a socket.");
    return 1;
  }

  int enabled = 1;
  (void)setsockopt(vars->server_fd, SOL_SOCKET, SO_REUSEADDR, &enabled,
                   sizeof(enabled));
  // nice to have, let us run server file a few times in a row without binding
  // issues.
  // delete in production.

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(TCP_PORT);

  if (bind(vars->server_fd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("Error binding address.");
    close(vars->server_fd);
    return 1;
  }
  // listen for new connections
  if (listen(vars->server_fd, MAX_PLAYER_COUNT) == -1) {
    perror("Error Listening to socket.");
    close(vars->server_fd);
    return 1;
  }

  // clients array init
  memset(vars->clients, 0, MAX_CLIENT_COUNT * sizeof(client_t));

  return 0;
}
