#include <stdarg.h>
#include <stdint.h>
#include <sys/socket.h>
#include <errno.h>

#include "game.h"
#include "network.h"
#include "net_const.h"

#include "net.h"

packet_type recv_packet(int fd, packet_fields* fields) {
    if (fd <= 0) {
        perror("Invalid file desc.");
        return 1;
    }

    packet_type type = 0;
    recv(fd, type, 1, 0);


    switch (type) {
        case TYPE_BROADCAST_LEAVE:
            recv(fd, *fields->id, sizeof(*fields->id), 0);
            break;
        case TYPE_REQ_PING:
            // no data
            break;
        case TYPE_RESP_JOIN:
            recv(fd, *fields->id, sizeof(*fields->id), 0);
            break;
        case TYPE_BROADCAST_JOIN:
            recv(fd, *fields->id, sizeof(*fields->id), 0);
            break;
        case TYPE_BROADCAST_READY:
            recv(fd, *fields->is_ready, sizeof(*fields->is_ready), 0);
            recv(fd, *fields->id, sizeof(*fields->id), 0);
            break;
        case TYPE_REQ_TIMESTAMP:
            recv(fd, *fields->timestamp, sizeof(*fields->timestamp), 0);
            break;
        case TYPE_BROADCAST_START_GAME: // TODO
            break;
        case TYPE_BROADCAST_UPDATE_STATE: 
            recv(fd, fields->players_array, sizeof(fields->players_array), 0);
            break;
        default: 
            printf("recv: not implemented\n");
            break;
    }
    return type;
} 
       


int send_packet(int fd, packet_type type, packet_fields* fields){
    if (fd <= 0) {
        perror("Invalid file desc.");
        return EBADFD;
    }

    char msg_buffer[MAX_DATA_LENGTH];

    *msg_buffer = type;

    switch (type) {
        case TYPE_RESP_PING:
            break;
        case TYPE_RESP_JOIN:
            *(uint32_t*) (msg_buffer + SIZE_HEADER) = *(fields->id);
            break;
        case TYPE_REQ_READY:
            *(msg_buffer + SIZE_HEADER) = *(fields->is_ready);
            break;
        case TYPE_RESP_TIMESTAMP:
            *(uint64_t*) (msg_buffer + SIZE_HEADER) = *(fields->timestamp);
            break;
        case TYPE_REQ_UPDATE_STATE: // TODO
            break;
        default: 
            printf("send: not implemented.\n");
            return ENOSYS;
    }
    for (int i = 0; i < SIZE_HEADER + get_packet_size(type); i++){
        printf("%x ", msg_buffer[i]);
    }
    printf("\n");
    send(fd, msg_buffer, SIZE_HEADER + get_packet_size(type), 0);
    return 0;
} 