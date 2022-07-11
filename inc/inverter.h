#ifndef INVERTER_H
#define INVERTER_H

#include <stdio.h>
#include <inttypes.h>
#include <iostream>
#include <string.h>

typedef struct __attribute__((__packed__)) {
    uint8_t reg_id;
    uint8_t data_0;
    uint8_t data_1;
    uint8_t data_2;
    uint8_t data_3;
    uint8_t data_4;
    uint8_t data_5;
    uint8_t data_6;
    uint64_t _timestamp;
} inverter_data_t;

#define INVERTER_MSGS 6
#define INVERTER_MSG(idx) (idx==50 || idx==51)

#define CHECK_MOTOR_TEMP_SIDE(id) id==385?0:3
#define CHECK_INV_TEMP_SIDE(id) id==385?0:3
#define CHECK_SPEED_SIDE(id) id==385?0:3

int inverter_fields(FILE *buffer, int j);
int inverter_to_string(void *message, int id, FILE *inverter_files[]);
std::string inverter_filename(int j);

#endif