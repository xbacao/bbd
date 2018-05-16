#ifndef _SCHEDULE_MANAGER_H_
#define _SCHEDULE_MANAGER_H_

#include <stdint.h>

#include "schedule/schedule.h"

void sm_set_new_schedules(struct schedule* sches, uint16_t schedules_len);

int sm_start_scheduler();
// devera ter 1 thread separado

#endif