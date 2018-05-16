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

int recv_socket_header(int sockfd, uint16_t* magic_number, uint8_t* device_id,
  uint8_t* msg_type, uint16_t* msg_size);

int send_rsp_msg(int sock_fd, char* reply_msg, uint16_t reply_msg_size);

char* craft_active_schedules_rsp(std::vector<schedule> sches);

void encode_schedule(char* msg, schedule sche);

#endif
