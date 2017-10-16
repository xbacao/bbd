#include "socket_bbd.h"
#include <string.h>
#include <Arduino.h>
#include "arduino_cfg.h"

int get_time_request_msg(char** msg){
  uint8_t msg_len=0;
  uint16_t temp_16=htons(SOCKET_HEADER);
  memcpy(*msg, &temp_16, SH_SIZE);
  memcpy(*msg+SH_SIZE,&ARDUINO_ID, ID_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE, &SYNC_TIME_MSG, TYPE_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE+TYPE_SIZE, &msg_len, T_SIZE_SIZE);
  return 0;
}

int decode_time_rsp_msg(char* msg, uint16_t msg_len, time_t* res){
  if(msg_len!=4){
    return 1;
  }

  uint32_t curr_time;
  memcpy(&curr_time, msg, msg_len);

  *res=ntohl(curr_time);

  return 0;
}

int get_active_request_msg(char** msg){
  uint8_t msg_len=0;
  uint16_t temp_16=htons(SOCKET_HEADER);
  memcpy(*msg, &temp_16, SH_SIZE);
  memcpy(*msg+SH_SIZE,&ARDUINO_ID, ID_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE, &ACTIVE_SCHES_MSG, TYPE_SIZE);
  memcpy(*msg+SH_SIZE+ID_SIZE+TYPE_SIZE, &msg_len, T_SIZE_SIZE);
  return 0;
}
