#include "schedule.h"
#include <string.h>
#include <arpa/inet.h>
#include "socket_data/socket_data.h"

void encode_schedule(char* msg, schedule sche){
  uint16_t temp;

  temp=htons(sche.schedule_id);
  memcpy(msg, &temp, BUFFER_SIZE_16);

  temp=htons(sche.valve_id);
  memcpy(msg+BUFFER_SIZE_16, &temp, BUFFER_SIZE_16);

  temp=htons(sche.start);
  memcpy(msg+2*BUFFER_SIZE_16, &temp, BUFFER_SIZE_16);

  temp=htons(sche.start);
  memcpy(msg+3*BUFFER_SIZE_16, &temp, BUFFER_SIZE_16);
}

std::ostream& operator<< (std::ostream& os, const schedule& sche){
  os << "{id:["<<sche.schedule_id<<"], valve_id:["<<sche.valve_id<<"], start:["<<sche.start/60<<":"
    <<sche.start%60<<"], stop:["<<sche.stop/60<<":"<<sche.stop%60<<"]}";
  return os;
}
