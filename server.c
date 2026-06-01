#include "src/server.h"
#include <stdio.h>

static int connected = 1;

int server_init(void) {
    printf("Initializing server...\n");
    return 1; 
}

void server_set_connection(int state)
{
    connected = state;
}

int server_send(packet_t* packet)
{
    if(!connected) {
        printf("Network Down\n");
        return 0;
    }
    if(packet == NULL) {
        return 0;
    }

    printf("Sending packet...\n");
    printf("Timestamp: %d\n", packet->timestamp);
    printf("Latitude: %f\n", packet->lat);
    printf("Longitude: %f\n", packet->lon);
    printf("Ignition: %d\n", packet->ignition);
    printf("Sensor: %d\n", packet->sensor_data);

    return 1;
}