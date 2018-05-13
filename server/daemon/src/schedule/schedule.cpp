#include "schedule.h"
#include <string.h>
#include <arpa/inet.h>
#include "socket_data/socket_data.h"

#define SIZE_CH sizeof(char)

void encode_schedule(char* msg, schedule sche){
  uint16_t temp;

  temp=htons(sche.schedule_id);
  memcpy(msg, &temp, 2*SIZE_CH);

  temp=htons(sche.valve_id);
  memcpy(msg+2*SIZE_CH, &temp, 2*SIZE_CH);

  temp=htons(sche.start);
  memcpy(msg+2*2*SIZE_CH, &temp, 2*SIZE_CH);

  temp=htons(sche.start);
  memcpy(msg+3*2*SIZE_CH, &temp, 2*SIZE_CH);
}

std::ostream& operator<< (std::ostream& os, const schedule& sche){
  os << "{id:["<<sche.schedule_id<<"], valve_id:["<<sche.valve_id<<"], start:["<<sche.start/60<<":"
    <<sche.start%60<<"], stop:["<<sche.stop/60<<":"<<sche.stop%60<<"]}";
  return os;
}
