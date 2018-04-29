#ifndef _DB_H_
#define _DB_H_

#include "schedule.h"

// int get_unsent_schedule(ArduinoSchedules* data);
// int get_last_schedule(ArduinoSchedules* data);
int amount_of_active_schedules(uint16_t arduino_id, uint16_t* n_sches);
int get_active_schedules(uint16_t arduino_id, schedule** sches,
  uint16_t n_sches);

int set_schedule_sent(int scheduleID);

#endif
