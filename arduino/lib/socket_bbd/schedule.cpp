#include <schedule.h>
#include <socket_bbd.h>
#include <string.h>

void decode_schedule(schedule& sche, char* msg){
  uint16_t schedule_id, valve_id, start, stop;

  memcpy(&schedule_id, msg, BUFFER_SIZE_16);
  schedule_id=ntohs(schedule_id);

  memcpy(&valve_id, msg+BUFFER_SIZE_16, BUFFER_SIZE_16);
  valve_id=ntohs(valve_id);

  memcpy(&start, msg+2*BUFFER_SIZE_16, BUFFER_SIZE_16);
  start=ntohs(start);

  memcpy(&stop, msg+3*BUFFER_SIZE_16, BUFFER_SIZE_16);
  stop=ntohs(stop);

  sche={schedule_id, valve_id, start, stop};
}
