#ifndef _DB_H_
#define _DB_H_

#include "schedule/schedule.h"

#include <vector>

int db_get_device_valves(uint16_t device_id, std::vector<uint16_t> &valve_ids);

int db_get_active_schedules(uint16_t device_id, std::vector<schedule>& sches,
  std::vector<bool>& sents);

int set_schedules_sent(std::vector<schedule> sches, uint16_t device_id);

#endif
