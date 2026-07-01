#include "src/gps.h"
#include <stdio.h>

int gps_init(){
    printf("Initializing GPS...\n");
    return 1;
}

gps_data_t get_gps_data(){
    
    static double lat = 35.1234;
    static double lon = 15.1234;

    lat += 0.01;
    lon += 0.01;

    gps_data_t gps_data;

    printf("Getting GPS data...\n");
    
    gps_data.lat = lat; 
    gps_data.lon = lon; 
    
    return gps_data;
}