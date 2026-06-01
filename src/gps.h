#ifndef GPS_H
#define GPS_H

typedef struct {
    double lat;
    double lon;
} gps_data_t;

int gps_init(void);
gps_data_t get_gps_data(void);

#endif