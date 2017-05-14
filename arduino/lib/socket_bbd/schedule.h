#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <inttypes.h>

struct schedule_entry{
	uint16_t cicle_start;
	uint16_t cicle_stop;
};

class ArduinoSchedules;
class ValveSchedule{
private:
	friend int scheduler_action();
	friend class ArduinoSchedules;
	uint16_t valve_id;
	uint16_t schedule_id;
	uint8_t n_cicles;
	schedule_entry* cicles;
public:
	ValveSchedule();
	~ValveSchedule();
	ValveSchedule(const ValveSchedule &v_sche);
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
	~ArduinoSchedules();
	int add_schedule(ValveSchedule sche);
	int update_schedule(ValveSchedule sche);
	int add_cicle(schedule_entry se, uint16_t valve_id, uint16_t schedule_id);
	uint16_t get_arduino_id();
	int get_schedule_by_valve(uint16_t valve_id, ValveSchedule *sche);
	int decode_message(char* msg, uint16_t msg_size);
	int process_new_schedules(ArduinoSchedules _new_schedules);
};

#endif
