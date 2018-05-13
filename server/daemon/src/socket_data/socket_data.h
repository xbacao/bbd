#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include "schedule/schedule.h"

#include <vector>

const uint8_t END_TRANS_CHAR = 0xff;
const uint16_t TIME_RSP_SIZE = sizeof(uint32_t);

#define SYNC_TIME_MSG           0x1
#define GET_ACTIVE_SCHES_MSG    0x2
#define CHECKIN_MSG             0x3
#define SCHE_ACT_MSG            0x4
// #define END_MSG				0x4

char* req_type_to_str(uint8_t req_type);

int recv_socket_header(int sock_fd, uint16_t magic_number, uint8_t* arduino_id,
  uint8_t* trans_type, uint8_t* trans_size);

int send_time_msg(int sock_fd);

// int send_schedule_msg(ArduinoSchedules a_s, int sock_fd);
int send_schedules_msg(int sock_fd, std::vector<schedule> sches);

int send_empty_msg(int sock_fd);

// int recv_sche_act_msg_size(int sock_fd, uint16_t* size);
//
// int recv_sche_act_msg(int sock_fd, uint16_t size, uint16_t **sche_ids);

#endif
