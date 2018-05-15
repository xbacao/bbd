#ifndef _SOCKET_DATA_H_
#define _SOCKET_DATA_H_

#include "schedule/schedule.h"

#include <stdint.h>

// const uint16_t SOCKET_HEADER = 38017;
#define END_TRANS_CHAR 0xff;
#define TIME_RSP_SIZE sizeof(uint32_t);

#define SYNC_TIME_MSG           0x1
#define GET_ACTIVE_SCHES_MSG    0x2
#define CHECKIN_MSG             0x3
#define SCHE_ACT_MSG            0x4
// #define END_MSG				0x4


// int send_time_msg(int sock_fd);


// int send_empty_msg(int sock_fd);

// int recv_sche_act_msg_size(int sock_fd, uint16_t* size);
//
// int recv_sche_act_msg(int sock_fd, uint16_t size, uint16_t **sche_ids);
char* req_type_to_str(uint8_t req_type);


int recv_rsp_len(int sockfd, uint16_t* rsp_len);
int recv_rsp_msg(int sockfd, uint16_t rsp_len, char* rsp);

int send_req_get_active_sches(const char* address, uint16_t port,
  uint16_t magic_number, uint8_t device_id, int* sockfd);


#endif
