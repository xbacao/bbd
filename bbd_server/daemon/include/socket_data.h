#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "log.h"
#include "schedule.h"

#define BUFFER_SIZE_16	sizeof(uint16_t)
#define BUFFER_SIZE_32	sizeof(uint32_t)
#define BUFFER_SIZE_64	sizeof(uint64_t)

const uint16_t SOCKET_HEADER = 38017;
const uint8_t END_TRANS_CHAR = 0xff;
const uint8_t TIME_RSP_SIZE = sizeof(uint32_t);

#define SYNC_TIME_MSG	0x1
#define LAST_SCHE_MSG	0x2
#define CHECKIN_MSG		0x3
#define SCHE_ACT_MSG  0x4
// #define END_MSG				0x4

int recv_socket_header(int sock_fd, uint8_t* arduino_id, uint8_t* trans_type, uint8_t* trans_size);

int send_time(int sock_fd);

int send_msg(char* msg, uint32_t size, int sock_fd);

int send_empty_msg(int sock_fd);

int recv_sche_act_msg_size(int sock_fd, uint16_t* size);

int recv_sche_act_msg(int sock_fd, uint16_t size, uint16_t **sche_ids);

//JUST FOR TEST
time_t get_time(int sock_fd);

#endif
