#include "schedule.h"

#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE_16 sizeof(uint16_t)

void decode_schedule(char* msg, struct schedule* sche){
  uint16_t schedule_id;
	uint16_t valve_id;
	uint16_t start;
	uint16_t stop;

  memcpy(&schedule_id, msg, BUFFER_SIZE_16);
  schedule_id=ntohs(schedule_id);
  memcpy(&valve_id, msg+BUFFER_SIZE_16, BUFFER_SIZE_16);
  valve_id=ntohs(valve_id);
  memcpy(&start, msg+2*BUFFER_SIZE_16, BUFFER_SIZE_16);
  start=ntohs(start);
  memcpy(&stop, msg+3*BUFFER_SIZE_16, BUFFER_SIZE_16);
  stop=ntohs(stop);

  *sche=(struct schedule) {schedule_id, valve_id, start, stop};
}
