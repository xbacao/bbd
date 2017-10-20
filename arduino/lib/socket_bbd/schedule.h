#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <inttypes.h>

struct schedule{
	uint16_t schedule_id;
	uint16_t valve_id;
	uint16_t start;
	uint16_t stop;
};

void decode_schedule(schedule& sche, char* msg);


#endif
