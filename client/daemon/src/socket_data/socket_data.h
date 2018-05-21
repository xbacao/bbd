#ifndef _SOCKET_DATA_H_
#define _SOCKET_DATA_H_

#include "schedule/schedule.h"

#include <stdint.h>

enum request_type {
  GET_DEVICE_VALVES_MSG =         0x1,
  GET_ACTIVE_SCHES_MSG =           0x2,
  CHECK_NEW_SCHEDULES_MSG= 0x3,
};

char* sd_req_type_to_str(enum request_type req_type);

int sd_recv_rsp_len(int sockfd, uint16_t* rsp_len);
int sd_recv_rsp_msg(int sockfd, uint16_t rsp_len, char* rsp);

int sd_send_request(const char* address, uint16_t port,
  uint16_t magic_number, uint8_t device_id, enum request_type request, int* sockfd);

void sd_decode_schedule(char* msg, struct schedule* sche);
void sd_decode_valve_id(char* msg, uint16_t* valve_id);

#endif
