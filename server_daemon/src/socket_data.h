#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define SOCKET_HEADER  103361377

#define SCHEDULE_MSG	0x01

#define BUFFER_SIZE_16 sizeof(uint16_t)
#define BUFFER_SIZE_32 sizeof(uint32_t)
#define BUFFER_SIZE_64 sizeof(uint64_t)


typedef struct _schedule_data_{
	uint16_t valve_id;
	uint16_t n_cicles;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
} schedule_data;

int recv_socket_header(int sock_fd, uint* arduino_id, uint* trans_type, uint* trans_size);

int recv_schedule_data(int sock_fd, schedule_data* data);

#endif