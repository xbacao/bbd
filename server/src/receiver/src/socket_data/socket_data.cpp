#include "socket_data.h"
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include "log/log.h"

using namespace std;

#define SIZE_CH sizeof(char)
#define HEADER_SIZE 6*SIZE_CH

/*** SOCKET SERIALIZATION ***/
// static void _prep_to_send_8(const void* in_data, void* out_data);
static void _prep_to_send_16(const void* in_data, void* out_data);

static void _prep_recved_8(const void* in_data, void* out_data);
static void _prep_recved_16(const void* in_data, void* out_data);



// static void _prep_to_send_8(const void* in_data, void* out_data){
// 	memcpy(out_data, in_data, SIZE_CH);
// }

static void _prep_to_send_16(const void* in_data, void* out_data){
  uint16_t temp_16;
  memcpy(&temp_16, in_data, 2*SIZE_CH);
  temp_16 = htons(temp_16);
  memcpy(out_data, &temp_16, 2*SIZE_CH);
}

static void _prep_recved_8(const void* in_data, void* out_data){
	memcpy(out_data, in_data, SIZE_CH);
}

static void _prep_recved_16(const void* in_data, void* out_data){
	uint16_t temp_16;
	memcpy(&temp_16, in_data, 2*SIZE_CH);
	temp_16 = ntohs(temp_16);
	memcpy(out_data, &temp_16, 2*SIZE_CH);
}

char* sd_req_type_to_str(enum request_type req_type){
	const char* cst_str;
	char* res=(char*) malloc(sizeof(char)*30);
  switch(req_type){
    case GET_ACTIVE_SCHES_MSG:
      cst_str="GET_ACTIVE_SCHES_MSG";
      break;
    case GET_DEVICE_VALVES_MSG:
      cst_str="GET_DEVICE_VALVES_MSG";
      break;
    default:
      cst_str="UNIDENTIFIED";
      break;
  }
	sprintf(res, "%s", cst_str);
	return res;
}

// static void _prep_to_send_32(const void* in_data, void* out_data){
// 	uint32_t temp_32;
// 	memcpy(&temp_32, in_data, 4*SIZE_CH);
// 	temp_32=htonl(temp_32);
// 	memcpy(out_data, &temp_32, 4*SIZE_CH);
// }


int recv_socket_header(int sockfd, uint16_t* magic_number, uint8_t* device_id,
uint8_t* msg_type, uint16_t* msg_size){
	int n;
	char* msg;

	msg=(char*) malloc(HEADER_SIZE);
	n=recv(sockfd, msg, HEADER_SIZE, MSG_WAITALL);
	if(n<=0){
		return 1;
	}

	_prep_recved_16(msg, magic_number);
	_prep_recved_8(msg+2*SIZE_CH, device_id);
	_prep_recved_8(msg+3*SIZE_CH, msg_type);
	_prep_recved_16(msg+4*SIZE_CH, msg_size);

	free(msg);
  return 0;
}

int send_rsp_msg(int sockfd, char* reply_msg, uint16_t reply_msg_size){
	int n;
	char* total_reply_msg;
	uint16_t total_reply_msg_size = 2*SIZE_CH+reply_msg_size;

	total_reply_msg= (char*) malloc(total_reply_msg_size);

	_prep_to_send_16(&reply_msg_size, total_reply_msg);

	memcpy(total_reply_msg+2*SIZE_CH, reply_msg, reply_msg_size);

	n=send(sockfd, total_reply_msg, total_reply_msg_size, 0)<0;

	free(total_reply_msg);
	return n;
}

char* craft_active_schedules_rsp(vector<schedule> sches){
	uint16_t sches_size=sches.size();
	uint16_t msg_len=sizeof(schedule)*sches_size;
	char* msg = (char*) malloc(msg_len);

	for(uint16_t i=0;i<sches_size;i++){
		encode_schedule(msg+sizeof(schedule)*i, sches[i]);
	}

	return msg;
}

char* craft_device_valves_rsp(vector<uint16_t> valve_ids){
	uint16_t valve_ids_size=valve_ids.size();
	uint16_t msg_len=sizeof(uint16_t)*valve_ids_size;
	char* msg = (char*) malloc(msg_len);

	for(uint16_t i=0;i<valve_ids_size;i++){
		_prep_to_send_16(&(valve_ids[i]), msg+i*(2*SIZE_CH));
	}

	return msg;
}

void encode_schedule(char* msg, struct schedule sche){
	_prep_to_send_16(&(sche.schedule_id), msg);
	_prep_to_send_16(&(sche.valve_id), msg+2*SIZE_CH);
	_prep_to_send_16(&(sche.start), msg+4*SIZE_CH);
	_prep_to_send_16(&(sche.stop), msg+6*SIZE_CH);
}
