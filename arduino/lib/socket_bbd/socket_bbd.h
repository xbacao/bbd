#ifndef _SOCKET_BBD_H_
#define _SOCKET_BBD_H_

#include <string.h>
#include <inttypes.h>

const uint64_t SOCKET_HEADER =102674321;
const uint32_t SYNC_TIME_MSG=0x1;

// #define SYNC_TIME_MSG	0x1

#define REQUEST_TIME_MSG_SIZE   sizeof(SOCKET_HEADER)+sizeof(char)

int get_time_request_msg(char** msg);

#endif
