#include "socket_data.h"
#include "log/log.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SIZE_CH sizeof(char)

static void _prep_to_send_8(const void* in_data, void* out_data);
static void _prep_to_send_16(const void* in_data, void* out_data);
// static void _prep_to_send_32(const void* in_data, void* out_data);

// static int _recv_8(void* data, int sock_fd);
static int _recv_16(void* data, int sock_fd);
// static int _recv_32(void* data, int sock_fd);


static int _connect_socket(const char* address, uint16_t port, int* sockfd);
static int _send_socket_header(int sockfd, uint16_t magic_number, uint8_t device_id,
	uint8_t msg_type, uint16_t msg_size);



static void _prep_to_send_8(const void* in_data, void* out_data){
	memcpy(out_data, in_data, SIZE_CH);
}

static void _prep_to_send_16(const void* in_data, void* out_data){
  uint16_t temp_16;
  memcpy(&temp_16, in_data, 2*SIZE_CH);
  temp_16 = htons(temp_16);
  memcpy(out_data, &temp_16, 2*SIZE_CH);
}


static int _connect_socket(const char* address, uint16_t port, int* sockfd){
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

static int _send_socket_header(int sockfd, uint16_t magic_number, uint8_t device_id,
uint8_t msg_type, uint16_t msg_size){
	int ret;
	char* msg;
	msg=(char*) malloc(6*SIZE_CH);

	_prep_to_send_16(&magic_number, msg);
	_prep_to_send_8(&device_id, msg+2*SIZE_CH);
	_prep_to_send_8(&msg_type, msg+3*SIZE_CH);
	_prep_to_send_16(&msg_size, msg+4*SIZE_CH);

	ret=send(sockfd, msg, 6*SIZE_CH, MSG_WAITALL)<0;
	free(msg);
	return ret;
}


//
// static void _prep_to_send_32(const void* in_data, void* out_data){
// 	uint32_t temp_32;
// 	memcpy(&temp_32, in_data, BUFFER_SIZE_32);
// 	temp_32=htonl(temp_32);
// 	memcpy(out_data, &temp_32, BUFFER_SIZE_32);
// }

// static int _recv_8(void* data, int sock_fd){
// 	int n;
// 	char buffer_8[SIZE_CH];
//
// 	memset(data, 0, SIZE_CH);
// 	n=recv(sock_fd, buffer_8, SIZE_CH, MSG_WAITALL);
// 	if(n<0){
// 		return n;
// 	}
//
// 	memcpy(data, &buffer_8, SIZE_CH);
// 	return 0;
// }

static int _recv_16(void* data, int sock_fd){
	int n;
	char buffer_16[2*SIZE_CH];
	uint16_t temp_16;

	memset(data, 0, 2*SIZE_CH);
	n=recv(sock_fd, buffer_16, 2*SIZE_CH, MSG_WAITALL);
	if(n<0){
		return n;
	}

	memcpy(&temp_16, buffer_16, 2*SIZE_CH);
	temp_16 = ntohs(temp_16);
	memcpy(data, &temp_16, 2*SIZE_CH);
	return 0;
}

char* req_type_to_str(uint8_t req_type){
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

int recv_rsp_len(int sockfd, uint16_t* rsp_len){
	return _recv_16(rsp_len, sockfd);
}

int recv_rsp_msg(int sockfd, uint16_t rsp_len, char* rsp){
	return recv(sockfd, rsp, rsp_len, MSG_WAITALL)<=0;
}

// int send_empty_msg(int sock_fd){
// 	char tmp=END_TRANS_CHAR;
// 	return send(sock_fd, &tmp, SIZE_CH, 0)<0;
// }

int send_req_get_active_sches(const char* address, uint16_t port, uint16_t magic_number,
uint8_t device_id,  int* sockfd){
	int n;

	n=_connect_socket(address, port, sockfd);
	if(n){
		log_error("connecting socket");
		return 1;
	}
	n=_send_socket_header(*sockfd, magic_number, device_id, GET_ACTIVE_SCHES_MSG,0);
	if(n){
		log_error("sending message header");
		return 2;
	}

	return 0;
}
