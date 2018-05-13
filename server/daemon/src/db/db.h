#ifndef _DB_H_
#define _DB_H_

#include "schedule/schedule.h"

#include <vector>

// int get_unsent_schedule(ArduinoSchedules* data);
// int get_last_schedule(ArduinoSchedules* data);
int amount_of_active_schedules(uint16_t arduino_id, uint16_t* n_sches);
int db_get_active_schedules(uint16_t arduino_id, std::vector<schedule>& sches);

int set_schedule_sent(int scheduleID);

#endif
