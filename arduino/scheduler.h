#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "string.h"
#include "socket_bbd.h"
#include "arduino_cfg.h"
#include "Time.h"

int init_scheduler(ArduinoSchedules a_s);

int process_schedules(ArduinoSchedules a_s);

int scheduler_action();

#endif
