#ifndef _DB_H_
#define _DB_H_

#include "schedule.h"

#define MYSQL_URL	"localhost"
#define MYSQL_DB	"bbd"
#define MYSQL_USER	"bbduser"
#define MYSQL_PASS	"morcao123"

int get_unsent_schedule(ArduinoSchedules* data);
int get_last_schedule(ArduinoSchedules* data);

int set_schedule_sent(int scheduleID);

#endif
