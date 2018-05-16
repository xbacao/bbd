#ifndef _SOCKET_DATA_H_
#define _SOCKET_DATA_H_

#include "schedule/schedule.h"

#include <stdint.h>

#define SYNC_TIME_MSG           0x1
#define GET_ACTIVE_SCHES_MSG    0x2
#define CHECKIN_MSG             0x3
#define SCHE_ACT_MSG            0x4

char* sd_req_type_to_str(uint8_t req_type);

int sd_recv_rsp_len(int sockfd, uint16_t* rsp_len);
int sd_recv_rsp_msg(int sockfd, uint16_t rsp_len, char* rsp);

int sd_send_req_get_active_sches(const char* address, uint16_t port,
  uint16_t magic_number, uint8_t device_id, int* sockfd);

void sd_decode_schedule(char* msg, struct schedule* sche);

#endif
