#ifndef PACKET_H
#define PACKET_H  

typedef struct {
    int timestamp;
    double lat;
    double lon;
    int ignition;
    int sensor_data;
} packet_t;

void enqueue(packet_t packet);
int dequeue(packet_t* packet);

#endif