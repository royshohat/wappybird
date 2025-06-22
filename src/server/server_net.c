#include <stdarg.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>

#include "game.h"
#include "network.h"
#include "net_const.h"

#include "net.h"

int send_packet(int fd, packet_type type, packet_fields* fields) {
    if (fd <= 0) {
        perror("Invalid file desc.");
        return 1;
    }
    char msg_buffer[SIZE_HEADER + MAX_DATA_LENGTH];
    
    // construct the header
    // uint32_t size = get_packet_size(type);
    // *(uint32_t*) (msg_buffer + 1) = size;

    *msg_buffer = type;

    switch (type) {
        case TYPE_BROADCAST_LEAVE:
            *(uint32_t*) (msg_buffer + SIZE_HEADER) = *(fields->id);
            break;
        case TYPE_REQ_PING:
            // no data.
            break;
        case TYPE_RESP_JOIN:
            *(uint32_t*) (msg_buffer + SIZE_HEADER) = *(fields->id);
            break;
        case TYPE_BROADCAST_JOIN:
            *(uint32_t*) (msg_buffer + SIZE_HEADER) = *(fields->id);
            break;
        case TYPE_BROADCAST_READY:
            *(msg_buffer + SIZE_HEADER) = *(fields->is_ready);
            *(uint32_t*) (msg_buffer + SIZE_HEADER + 1) = *(fields->id);
            break;
        case TYPE_REQ_TIMESTAMP:
            *(uint64_t*) (msg_buffer + SIZE_HEADER) = *(fields->timestamp);
            break;
        case TYPE_BROADCAST_START_GAME: // TODO
            break;
        case TYPE_BROADCAST_UPDATE_STATE:
            memcpy(msg_buffer+SIZE_HEADER, fields->players_array, SIZE_BROADCAST_UPDATE_STATE);
            break;
        default: 
            printf("send: not implemented.\n");
            return 1;
    }

    for (int i = 0; i < SIZE_HEADER + get_packet_size(type); i++){
        printf("%x ", msg_buffer[i]);
    }
    printf("\n");
    send(fd, msg_buffer, SIZE_HEADER + get_packet_size(type), 0);
    return 0;
} 
       


packet_type recv_packet(int fd, packet_fields* fields){
    if (fd <= 0) {
        perror("Invalid file desc.");
        return TYPE_INVALID;
    }
    packet_type type = 0;

    char msg_buffer[MAX_DATA_LENGTH];
    recv(fd, type, 1, 0);

    switch (type) {
        case TYPE_RESP_PING:
            // no data.
            break;
        case TYPE_REQ_JOIN:
            recv(fd, *fields->id, sizeof(*fields->id), 0);
            break;
        case TYPE_REQ_READY:
            recv(fd, *fields->is_ready, sizeof(*fields->is_ready), 0);
            break;
        case TYPE_RESP_TIMESTAMP:
            recv(fd, *(fields->timestamp), sizeof(*(fields->timestamp)), 0);
            break;
        case TYPE_REQ_UPDATE_STATE:
            // no data?
            break;
        default: 
            printf("recv: not implemented\n");
            break;
    }
    return type;
} 