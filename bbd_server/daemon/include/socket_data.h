#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define BUFFER_SIZE_16	sizeof(uint16_t)
#define BUFFER_SIZE_32	sizeof(uint32_t)
#define BUFFER_SIZE_64	sizeof(uint64_t)

#define SOCKET_HEADER 	102674321

#define CHECKIN_MSG		0x1
#define SCHEDULE_MSG	0x2
#define END_MSG			0x3


typedef struct _schedule_data_{
	uint16_t valve_id;
	uint16_t schedule_id;
	uint16_t n_cicles;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
} schedule_data;

int recv_socket_header(int sock_fd, uint* arduino_id, uint* trans_type, uint* trans_size);

int send_schedule(int sock_fd, schedule_data data);

int wait_schedule_reply(int sock_fd, int schedule_id);

std::ostream& operator<<(std::ostream& os, schedule_data &data);


#endif