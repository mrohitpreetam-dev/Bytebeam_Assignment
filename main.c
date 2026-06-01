#include "src/daq.h"
#include "src/packet.h"
#include "src/server.h"
#include "src/gps.h"
#include "src/peripherial.h"
#include <stdio.h>
#include <stdbool.h>
#include "test/src/test.h"

#define QUEUE_SIZE 25

packet_t queue[QUEUE_SIZE];

int head = 0;
int tail = 0;
int count = 0;

void enqueue(packet_t packet)
{
    if(count >= QUEUE_SIZE)
    {
        tail = (tail + 1) % QUEUE_SIZE;
        count--;
    }

    queue[head] = packet;
    head = (head + 1) % QUEUE_SIZE;
    count++;
}

int dequeue(packet_t* packet)
{
    if(count == 0) {
        return 0;
    }

    *packet = queue[tail];

    tail = (tail + 1) % QUEUE_SIZE;

    count--;

    return 1;
}

int main() {
    bool daq_bus_status = daq_bus_init();
    bool gps_init_status = gps_init();
    bool peripherial_init_status = peripherial_gpio_init();
    bool server_init_status = server_init();
    int timestamp = 0;
    int iterator = 0;

    // test_buffer_full(count);
    // void test_network_reconnect(void);   
    while(1) {
        packet_t packet;
        gps_data_t gps_data;

        packet.timestamp = timestamp;
        timestamp += 60;

        packet.lat = -1;
        packet.lon = -1;
        packet.ignition = -1;
        packet.sensor_data = -1;

        if (!gps_init_status) {
            printf("GPS Initialization failed. Retrying...\n");
            gps_init_status = gps_init();
        }
        else{
            gps_data = get_gps_data();
            packet.lat = gps_data.lat;
            packet.lon = gps_data.lon;
        }
        if (!peripherial_init_status) {
            printf("Peripherial Initialization failed. Retrying...\n");
            peripherial_init_status = peripherial_gpio_init();
        }
        else{
            packet.ignition = get_peripherial_status();
        }
        if (!daq_bus_status) {
            printf("DAQ Bus Initialization failed. Retrying...\n");
            daq_bus_status = daq_bus_init();
        }
        else{
            packet.sensor_data = daq_read_sensors();
        } 

        enqueue(packet);
        iterator++;

        if(iterator == 5) {
            server_set_connection(0);
        }
        else if(iterator == 15) {
            server_set_connection(1);
        }
        else if(iterator == 20){
            iterator = 0;
        }

        packet_t send_packet;
        if(server_init_status) {
            if(count > 0) {
                send_packet = queue[tail];
                if(server_send(&send_packet)) {
                    printf("Packet sent successfully.\n");
                    printf("Timestamp: %d\n", send_packet.timestamp);
                    printf("Latitude: %f\n", send_packet.lat);
                    printf("Longitude: %f\n", send_packet.lon);
                    printf("Ignition: %d\n", send_packet.ignition);
                    printf("Sensor: %d\n", send_packet.sensor_data);
                    dequeue(&send_packet);
                }
            }
        }
        else {
            printf("Server Initialization failed. Retrying...\n");
            server_init_status = server_init();
        }
    }

    return 0;
}