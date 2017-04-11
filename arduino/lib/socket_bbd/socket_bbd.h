#ifndef _SOCKET_BBD_H_
#define _SOCKET_BBD_H_

#include <string.h>
#include <inttypes.h>
#include <time.h>

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)


const uint16_t SOCKET_HEADER=38017;
const uint32_t SH_SIZE=sizeof(SOCKET_HEADER);

const uint8_t ARDUINO_ID=1;
const uint32_t ID_SIZE=sizeof(uint8_t);

const uint32_t TYPE_SIZE=sizeof(uint8_t);
const uint8_t SYNC_TIME_MSG=0x1;
const uint8_t LAST_SCHE_MSG=0x1;
const uint8_t CHECK_IN_MSG=0x3;


const int32_t T_SIZE_SIZE=sizeof(uint8_t);

typedef struct _schedule_data_{
	uint16_t valve_id;
	uint16_t schedule_id;
	uint16_t n_cicles;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
} schedule_data;

const uint32_t REQUEST_TIME_MSG_SIZE = SH_SIZE+ID_SIZE+TYPE_SIZE+T_SIZE_SIZE;
const uint32_t CHECK_REQS_MSG_SIZE = SH_SIZE+ID_SIZE+TYPE_SIZE+T_SIZE_SIZE;

int get_time_request_msg(char** msg);
int decode_time_rsp_msg(char* msg, uint8_t msg_len, time_t* res);
int get_last_sche_msg(char** msg);
int get_check_requests_msg(char** msg);
int decode_schedule_msg(char* msg, uint8_t msg_len, schedule_data* res);

#endif
