#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "log.h"

#define BUFFER_SIZE_16	sizeof(uint16_t)
#define BUFFER_SIZE_32	sizeof(uint32_t)
#define BUFFER_SIZE_64	sizeof(uint64_t)

const uint16_t SOCKET_HEADER = 38017;
const uint8_t END_TRANS_CHAR = 0xff;
const uint8_t TIME_RSP_SIZE = sizeof(uint32_t);

#define SYNC_TIME_MSG	0x1
#define LAST_SCHE_MSG	0x2
#define CHECKIN_MSG		0x3
#define SCHEDULE_MSG	0x3
#define END_MSG				0x4


typedef struct _schedule_data_{
	uint16_t valve_id;
	uint16_t schedule_id;
	uint16_t n_cicles;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
} schedule_data;

int recv_socket_header(int sock_fd, uint8_t* arduino_id, uint8_t* trans_type, uint8_t* trans_size);

int send_schedule(int sock_fd, schedule_data data);

int send_time(int sock_fd);

int wait_schedule_reply(int sock_fd, int schedule_id);

//JUST FOR TEST
time_t get_time(int sock_fd);

std::ostream& operator<<(std::ostream& os, schedule_data &data);


#endif
