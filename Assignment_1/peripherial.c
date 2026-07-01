#include "src/peripherial.h"
#include <stdio.h>

int peripherial_gpio_init(){
    printf("Initializing GPIO...\n");
    return 1;
}

int get_peripherial_status(){
    printf("Getting ignition status...\n");
    static int ignition_status = 0;
    ignition_status = !ignition_status;
    return ignition_status; 
}