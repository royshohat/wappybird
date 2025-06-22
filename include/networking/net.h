#include "network.h"

packet_type recv_packet(int fd, packet_fields* fields);
int send_packet(int fd, packet_type type, packet_fields* fields);