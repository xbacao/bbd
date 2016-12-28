#include "scheduler.h"

int setValveSchedule(int valve_id, int new_sche[N_CYCLES*3]){
	if(valve_id < N_EV){				//change this on this file and on evController.ino in future, because valveIDs may not start at 0
		return -1;
	}
	memcpy(schedules[valve_id], new_sche, sizeof(int)*N_CYCLES*3);
	return 1;
}