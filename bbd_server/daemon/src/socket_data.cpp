#include "socket_data.h"
#include <arpa/inet.h>
#include <iostream>
#include "log.h"
#include <string.h>

using namespace std;

static void _prep_to_send_8(const void* in_data, void* out_data){
	memcpy(out_data, in_data, BUFFER_SIZE_8);
}

static void _prep_to_send_16(const void* in_data, void* out_data){
  uint16_t temp_16;
  memcpy(&temp_16, in_data, BUFFER_SIZE_16);
  temp_16 = htons(temp_16);
  memcpy(out_data, &temp_16, BUFFER_SIZE_16);
}

static void _prep_to_send_32(const void* in_data, void* out_data){
	uint32_t temp_32;
	memcpy(&temp_32, in_data, BUFFER_SIZE_32);
	temp_32=htonl(temp_32);
	memcpy(out_data, &temp_32, BUFFER_SIZE_32);
}

static int _recv_8(void* data, int sock_fd){
	int n;
	char buffer_8[BUFFER_SIZE_8];

	memset(data, 0, BUFFER_SIZE_8);
	n=recv(sock_fd, buffer_8, BUFFER_SIZE_8, MSG_WAITALL);
	if(n<0){
		return n;
	}

	memcpy(data, &buffer_8, BUFFER_SIZE_8);
	return 0;
}

static int _recv_16(void* data, int sock_fd){
	int n;
	char buffer_16[BUFFER_SIZE_16];
	uint16_t temp_16;

	memset(data, 0, BUFFER_SIZE_16);
	n=recv(sock_fd, buffer_16, BUFFER_SIZE_16, MSG_WAITALL);
	if(n<0){
		return n;
	}

	memcpy(&temp_16, buffer_16, BUFFER_SIZE_16);
	temp_16 = ntohs(temp_16);
	memcpy(data, &temp_16, BUFFER_SIZE_16);
	return 0;
}

int recv_socket_header(int sock_fd, uint8_t* arduino_id, uint8_t* trans_type, uint8_t* trans_size){
  uint16_t temp_16;
  int n;
  n= _recv_16(&temp_16, sock_fd);
  if(n!=0){
    return 1;
  }
  if(temp_16 != SOCKET_HEADER){
		log_file<<time(nullptr)<<"ERROR: bad header:"<<temp_16<<endl;
    return 2;
  }

	n=_recv_8(arduino_id, sock_fd);
	if(n){
		return 3;
	}

	n=_recv_8(trans_type, sock_fd);
	if(n){
		return 4;
	}

  if(*trans_type!=SYNC_TIME_MSG && *trans_type!=ACTIVE_SCHES_MSG && *trans_type!=CHECKIN_MSG && *trans_type!=SCHE_ACT_MSG){
    return 5;
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
	uint16_t total_reply_msg_size = BUFFER_SIZE_16+reply_msg_size+BUFFER_SIZE_8;

	total_reply_msg=new char[total_reply_msg_size];

	_prep_to_send_16(&reply_msg_size, total_reply_msg);

	memcpy(total_reply_msg+BUFFER_SIZE_16, reply_msg, reply_msg_size);

	_prep_to_send_8(&END_TRANS_CHAR, total_reply_msg+BUFFER_SIZE_16
			+reply_msg_size);

	#ifdef DEBUG_SOCKET
	log_file<<"SENDING: [";
	for(uint16_t i=0;i<total_reply_msg_size;i++){
		log_file<<unsigned((uint8_t) total_reply_msg[i])<<" ";
	}
	log_file<<"]"<<endl;
	#endif

	n=send(sock_fd, total_reply_msg, total_reply_msg_size, 0)<0;

	delete[] total_reply_msg;
	return n;
}

int send_time_msg(int sock_fd){
	char* reply_msg;
	int n;
	uint32_t curr_time=time(nullptr);

	reply_msg=new char[TIME_RSP_SIZE];

	_prep_to_send_32(&curr_time, reply_msg);

	n=_send_reply_msg(sock_fd, reply_msg, TIME_RSP_SIZE);

	delete[] reply_msg;
	return n;
}

int send_schedules_msg(int sock_fd, schedule* sches, int sches_size){
	int n;
	uint16_t msg_len=sizeof(schedule)*sches_size;
	char* msg = new char[msg_len];

	_prep_to_send_16(&msg_len, msg);
	for(uint16_t i=0;i<sches_size;i++){
		encode_schedule(msg+sizeof(schedule)*i, sches[i]);
	}

	_prep_to_send_8(&END_TRANS_CHAR, msg+msg_len-BUFFER_SIZE_8);

	#ifdef DEBUG_SOCKET
	log_file << time(nullptr) << ": DB-SOCK [ ";
	for(uint16_t i=0; i<sches_size;i++){
		log_file << unsigned(msg[i]) << " ";
	}
	log_file << "]"<<endl;
	#endif

	n=_send_reply_msg(sock_fd, msg, msg_len);

	delete[] msg;
	return n;
}

int send_empty_msg(int sock_fd){
	return send(sock_fd, &END_TRANS_CHAR, BUFFER_SIZE_8, 0)<0;
}
