#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "valve.h"
#include "string.h"

#define N_CYCLES 3

static int schedules[N_EV][N_CYCLES*3];

int setValveSchedule(int valve_id, int new_sche[N_CYCLES*3]);

#endif