#ifndef SERVER_H
#define SERVER_H

#include "packet.h"

int server_init(void);
void server_set_connection(int state);
int server_send(packet_t* packet);

#endif