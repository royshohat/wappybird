#include <stdarg.h>
#include <stdint.h>

#include "game.h"
#include "constants.h"
#include "network.h"

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
        case TYPE_RESP_UPDATE_ARRAY:
            return SIZE_RESP_UPDATE_ARRAY;
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