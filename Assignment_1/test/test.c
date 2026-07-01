#include <stdio.h>
#include "../src/packet.h"
#include "../src/server.h"

void test_buffer_full(const int count)
{
    packet_t p;

    for(int i = 0; i < 30; i++)
    {
        p.timestamp = i;
        enqueue(p);
    }

    printf("Buffer count = %d\n", count);
}

void test_network_reconnect(void)
{
    server_set_connection(0);

    packet_t p;

    for(int i = 0; i < 5; i++)
    {
        p.timestamp = i;
        enqueue(p);
    }

    printf("Packets buffered\n");

    server_set_connection(1);

    while(dequeue(&p))
    {
        server_send(&p);
    }

    printf("Reconnect test complete\n");
}