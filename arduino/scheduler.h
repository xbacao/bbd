#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "schedule.h"

int init_scheduler(ArduinoSchedules a_s);

int process_schedules(ArduinoSchedules a_s);

int scheduler_action();

#endif
