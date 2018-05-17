#include "socket_data.h"
#include "log/log.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SIZE_CH sizeof(char)
#define HEADER_SIZE 6*SIZE_CH

/*** SOCKET SERIALIZATION ***/
static void _sd_prep_to_send_8(const void* in_data, void* out_data);
static void _sd_prep_to_send_16(const void* in_data, void* out_data);

// static void _sd_prep_recved_8(const void* in_data, void* out_data);
static void _sd_prep_recved_16(const void* in_data, void* out_data);

/*** SOCKET USAGE ***/
static int _sd_connect_socket(const char* address, uint16_t port, int* sockfd);
static int _sd_send_socket_header(int sockfd, uint16_t magic_number, uint8_t device_id,
	uint8_t msg_type, uint16_t msg_size);

/*** SOCKET SERIALIZATION ***/
static void _sd_prep_to_send_8(const void* in_data, void* out_data){
	memcpy(out_data, in_data, SIZE_CH);
}

static void _sd_prep_to_send_16(const void* in_data, void* out_data){
  uint16_t temp_16;
  memcpy(&temp_16, in_data, 2*SIZE_CH);
  temp_16 = htons(temp_16);
  memcpy(out_data, &temp_16, 2*SIZE_CH);
}

// static void _sd_prep_recved_8(const void* in_data, void* out_data){
// 	memcpy(out_data, in_data, SIZE_CH);
// }

static void _sd_prep_recved_16(const void* in_data, void* out_data){
	uint16_t temp_16;
	memcpy(&temp_16, in_data, 2*SIZE_CH);
	temp_16 = ntohs(temp_16);
	memcpy(out_data, &temp_16, 2*SIZE_CH);
}

/*** SOCKET USAGE ***/
static int _sd_connect_socket(const char* address, uint16_t port, int* sockfd){
	struct sockaddr_in server;
	*sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if(*sockfd==-1)
	{
		log_error("could not create socket");
		return 1;
	}
	server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(*sockfd,(struct sockaddr *)&server,sizeof(server))< 0){
		log_error("socket connect failed");
		return 2;
	}

	return 0;
}

static int _sd_send_socket_header(int sockfd, uint16_t magic_number, uint8_t device_id,
uint8_t msg_type, uint16_t msg_size){
	int ret;
	char* msg;
	msg=(char*) malloc(HEADER_SIZE);

	_sd_prep_to_send_16(&magic_number, msg);
	_sd_prep_to_send_8(&device_id, msg+2*SIZE_CH);
	_sd_prep_to_send_8(&msg_type, msg+3*SIZE_CH);
	_sd_prep_to_send_16(&msg_size, msg+4*SIZE_CH);

	ret=send(sockfd, msg, HEADER_SIZE, MSG_WAITALL)<0;
	free(msg);
	return ret;
}

char* sd_req_type_to_str(uint8_t req_type){
	const char* cst_str;
	char* res=(char*) malloc(sizeof(char)*30);
  switch(req_type){
    case SYNC_TIME_MSG:
      cst_str="SYNC_TIME_MSG";
      break;
    case GET_ACTIVE_SCHES_MSG:
      cst_str="GET_ACTIVE_SCHES_MSG";
      break;
    case CHECKIN_MSG:
      cst_str="CHECKIN_MSG";
      break;
    case SCHE_ACT_MSG:
      cst_str="SCHE_ACT_MSG";
      break;
    default:
      cst_str="UNIDENTIFIED";
      break;
  }
	sprintf(res, "%s", cst_str);
	return res;
}

int sd_recv_rsp_len(int sockfd, uint16_t* rsp_len){
	int n;
	uint16_t tmp;

	n=recv(sockfd, &tmp, 2*SIZE_CH, MSG_WAITALL);
	if(n<=0){
		return 1;
	}

	_sd_prep_recved_16(&tmp, rsp_len);
	return 0;
}

int sd_recv_rsp_msg(int sockfd, uint16_t rsp_len, char* rsp){
	return recv(sockfd, rsp, rsp_len, MSG_WAITALL)<=0;
}

int sd_send_req_get_active_sches(const char* address, uint16_t port, uint16_t magic_number,
uint8_t device_id,  int* sockfd){
	int n;

	n=_sd_connect_socket(address, port, sockfd);
	if(n){
		log_error("connecting socket");
		return 1;
	}
	n=_sd_send_socket_header(*sockfd, magic_number, device_id, GET_ACTIVE_SCHES_MSG,0);
	if(n){
		log_error("sending message header");
		return 2;
	}

	return 0;
}

void sd_decode_schedule(char* msg, struct schedule* sche){
  uint16_t schedule_id;
	uint16_t valve_id;
	uint16_t start;
	uint16_t stop;

	_sd_prep_recved_16(msg, &schedule_id);
	_sd_prep_recved_16(msg+2*SIZE_CH, &valve_id);
	_sd_prep_recved_16(msg+4*SIZE_CH, &start);
	_sd_prep_recved_16(msg+6*SIZE_CH, &stop);

  *sche=(struct schedule) {schedule_id, valve_id, start, stop, false};
}
