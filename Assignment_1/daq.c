#include "src/daq.h"
#include <stdio.h>

int daq_bus_init(void)
{
    printf("Initializing DAQ bus...\n");
    return 1;
}

int daq_read_sensors(void)
{
    printf("Reading sensor data...\n");
    static int value = 42;
    value++;
    return value;
}