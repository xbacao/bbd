#ifndef _DB_H_
#define _DB_H_

#include <mysql.h>
#include "log.h"
#include "socket_data.h"

#define MYSQL_URL	"localhost"
#define MYSQL_DB	"bbd"
#define MYSQL_USER	"bbduser"
#define MYSQL_PASS	"morcao123"

int get_new_schedule(int valveID, schedule_data* data);
int get_last_schedule(int valveID, schedule_data* data);

int set_schedule_sent(int scheduleID);

#endif
