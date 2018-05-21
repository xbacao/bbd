#ifndef _SOCKET_DATA_H
#define _SOCKET_DATA_H

#include "schedule/schedule.h"

#include <vector>

enum request_type {
  GET_DEVICE_VALVES_MSG =         0x1,
  GET_ACTIVE_SCHES_MSG =           0x2,
  CHECK_NEW_SCHEDULES_MSG= 0x3,
};

char* sd_req_type_to_str(enum request_type req_type);

int recv_socket_header(int sockfd, uint16_t* magic_number, uint8_t* device_id,
  uint8_t* msg_type, uint16_t* msg_size);

int send_rsp_msg(int sock_fd, char* reply_msg, uint16_t reply_msg_size);

char* craft_active_schedules_rsp(std::vector<schedule> sches);
char* craft_device_valves_rsp(std::vector<uint16_t> valve_ids);

void encode_schedule(char* msg, schedule sche);

#endif
