#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "socket_data.h"

struct schedule_entry{
	uint16_t cicle_start;
	uint16_t cicle_stop;
};

class ArduinoSchedules;
class ValveSchedule{
private:
	friend class ArduinoSchedules;
	uint16_t valve_id;
	uint16_t schedule_id;
	uint8_t n_cicles;
	schedule_entry* cicles;
public:
	ValveSchedule();
	ValveSchedule(uint16_t _valve_id, uint8_t _schedule_id);
	void add_cicle(schedule_entry cicle);
	uint16_t get_valve_id();
	uint16_t get_schedule_id();
};

class ArduinoSchedules{
private:
	uint16_t arduino_id;
	uint8_t n_schedules;
	ValveSchedule* schedules;
public:
	ArduinoSchedules(uint16_t _arduino_id);
	void add_schedule(ValveSchedule sche);
	void add_cicle(schedule_entry se, uint16_t valve_id, uint16_t schedule_id);
	uint16_t get_arduino_id();
	uint16_t get_message_size();
	void make_message(char** msg);
};

#endif
