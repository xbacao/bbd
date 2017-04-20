#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <inttypes.h>
#include <iostream>

struct schedule_entry{
	uint16_t cicle_start;
	uint16_t cicle_stop;
};

std::ostream& operator<< (std::ostream& os, const schedule_entry& s_e);


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
	friend std::ostream& operator<< (std::ostream& os, const ValveSchedule& v_s);
};

class ArduinoSchedules{
private:
	uint16_t arduino_id;
	uint8_t n_schedules;
	ValveSchedule* schedules;
public:
	ArduinoSchedules(uint16_t _arduino_id);
	int add_schedule(ValveSchedule sche);
	int add_cicle(schedule_entry se, uint16_t valve_id, uint16_t schedule_id);
	uint16_t get_arduino_id();
	uint16_t get_message_size();
	void make_message(char* msg);
	//test
	int decode_message(char* msg, uint16_t msg_size);
  friend std::ostream& operator<< (std::ostream& os, const ArduinoSchedules& a_s);
};



#endif
