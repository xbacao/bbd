#include "socket_data.h"
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include "log/log.h"

using namespace std;

#define SIZE_CH sizeof(char)

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

static void _prep_to_send_8(const void* in_data, void* out_data){
	memcpy(out_data, in_data, SIZE_CH);
}

static void _prep_to_send_16(const void* in_data, void* out_data){
  uint16_t temp_16;
  memcpy(&temp_16, in_data, 2*SIZE_CH);
  temp_16 = htons(temp_16);
  memcpy(out_data, &temp_16, 2*SIZE_CH);
}

// static void _prep_to_send_32(const void* in_data, void* out_data){
// 	uint32_t temp_32;
// 	memcpy(&temp_32, in_data, 4*SIZE_CH);
// 	temp_32=htonl(temp_32);
// 	memcpy(out_data, &temp_32, 4*SIZE_CH);
// }

static int _recv_8(void* data, int sock_fd){
	int n;
	char buffer_8[SIZE_CH];

	memset(data, 0, SIZE_CH);
	n=recv(sock_fd, buffer_8, SIZE_CH, MSG_WAITALL);
	if(n<0){
		return n;
	}

	memcpy(data, &buffer_8, SIZE_CH);
	return 0;
}

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

int recv_socket_header(int sock_fd, uint16_t magic_number, uint8_t* arduino_id,
uint8_t* msg_type, uint8_t* trans_size){
  uint16_t temp_16;
  int n;
  n= _recv_16(&temp_16, sock_fd);
  if(n!=0){
    return 1;
  }
  if(temp_16 != magic_number){
		log_error("magic numbers do not match");
    return 2;
  }

	n=_recv_8(arduino_id, sock_fd);
	if(n){
		return 3;
	}

	n=_recv_8(msg_type, sock_fd);
	if(n){
		return 4;
	}

	n=_recv_8(trans_size, sock_fd);
	if(n){
		return 6;
	}

  if(*trans_size<0 || *trans_size>128){
    return 7;
  }

  return 0;
}

static int _send_reply_msg(int sock_fd, char* reply_msg, uint16_t reply_msg_size){
	int n;
	char* total_reply_msg;
	uint16_t total_reply_msg_size = 2*SIZE_CH+reply_msg_size+SIZE_CH;

	total_reply_msg= (char*) malloc(total_reply_msg_size);

	_prep_to_send_16(&reply_msg_size, total_reply_msg);

	memcpy(total_reply_msg+2*SIZE_CH, reply_msg, reply_msg_size);

	_prep_to_send_8(&END_TRANS_CHAR, total_reply_msg+2*SIZE_CH
			+reply_msg_size);

	n=send(sock_fd, total_reply_msg, total_reply_msg_size, 0)<0;

	free(total_reply_msg);
	return n;
}

// int send_time_msg(int sock_fd){
// 	char* reply_msg;
// 	int n;
// 	uint32_t curr_time=time(nullptr);
//
// 	reply_msg=new char[TIME_RSP_SIZE];
//
// 	_prep_to_send_32(&curr_time, reply_msg);
//
// 	n=_send_reply_msg(sock_fd, reply_msg, TIME_RSP_SIZE);
//
// 	delete[] reply_msg;
// 	return n;
// }

/*TODO AS ESCRITAS TEEM OS TAMANHOS TDS COMIDOS*/
/* the 2 bytes required for sending the message size are not included in message size */
int send_schedules_msg(int sock_fd, vector<schedule> sches){
	int n;
	uint16_t sches_size=sches.size();
	uint16_t msg_len=sizeof(schedule)*sches_size;
	char* msg = (char*) malloc(msg_len);

	for(uint16_t i=0;i<sches_size;i++){
		encode_schedule(msg+sizeof(schedule)*i, sches[i]);
	}

	n=_send_reply_msg(sock_fd, msg, msg_len);

	free(msg);
	return n;
}

int send_empty_msg(int sock_fd){
	return send(sock_fd, &END_TRANS_CHAR, SIZE_CH, 0)<0;
}
