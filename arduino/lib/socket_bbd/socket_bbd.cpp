#include "socket_bbd.h"

// static uint64_t _ntoh64(uint64_t in){
//   uint64_t out=0;
//   // for(int i=0;i<8;i++){
//   //   out |= (uint64_t)((in >> 8*i)&0xff)<<(64-(8*i));
//   // }
//   for(int i=0;i<8;i++){
//     memcpy((&out)+7-i, (&in)+i, 1);
//   }
//   return out;
// }

int get_time_request_msg(char** msg){
  uint8_t msg_len=0;
  uint16_t temp_16=htons(SOCKET_HEADER);
  memcpy(*msg, &temp_16, SH_SIZE);
  memcpy(*msg+SH_SIZE,&ARDUINO_ID, ID_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE, &SYNC_TIME_MSG, TYPE_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE+TYPE_SIZE, &msg_len, T_SIZE_SIZE);;
  return 0;
}

int decode_time_rsp_msg(char* msg, uint8_t msg_len, time_t* res){
  if(msg_len!=4){
    return 1;
  }

  uint32_t curr_time;
  memcpy(&curr_time, msg, msg_len);

  *res=ntohl(curr_time);

  return 0;
}

int get_check_requests_msg(char** msg){
  memcpy(*msg, &SOCKET_HEADER, sizeof(SOCKET_HEADER));
  memcpy(*msg+sizeof(SOCKET_HEADER), &CHECK_REQS, 1);
  return 0;
}

int decode_schedule_msg(char* msg, uint8_t msg_len, schedule_data* res){
  // uint32_t val_size=sizeof(uint16_t);
  //
  // if(msg_len<3*val_size){
  //   return 1;
  // }
  //
  // uint16_t valve_id;
	// uint16_t schedule_id;
	// uint16_t n_cicles;
	// uint16_t* cicle_starts;
	// uint16_t* cicle_stops;
  //
  // memcpy(&valve_id, msg, val_size);
  // memcpy(&schedule_id, msg+val_size, val_size);
  // memcpy(&n_cicles, msg+2*val_size, val_size);
  // if(msg_len != 3*val_size+2*val_size*n_cicles){
  //   return 2;
  // }
  //
  // cicle
  //
  // for(uint32_t i=0;i<msg_len;i++){
  //
  // }
  return 20;
}
