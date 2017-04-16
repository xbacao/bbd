#include "db.h"

using namespace std;

static uint16_t _time_to_uint16(char* c_time){
	int hour,min,sec;
	sscanf(c_time,"%d:%d:%d",&hour,&min,&sec);
	return hour*60+min;
}

int get_unsent_schedule(ArduinoSchedules* data){
	int n_rows;
	uint16_t schedule_id;
	uint16_t valve_id;
	stringstream ss;
	char* temp_str;
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, MYSQL_URL, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0)) {
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 1;
	}

	ss << "CALL get_unsent_schedule("<<(*data).get_arduino_id()<<")";
	temp_str=(char*)ss.str().c_str();
	/* send SQL query */
	if (mysql_query(conn, temp_str)) {
		mysql_close(conn);
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 2;
	}

	res = mysql_store_result(conn);
	if(!res){
		mysql_close(conn);
		return 3;
	}

	n_rows=mysql_num_rows(res);
	if(n_rows==0){
		mysql_free_result(res);
		mysql_close(conn);
		return 4;
	}

	/* output table name */
	int i=0;
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		valve_id=atoi(row[0]);
		schedule_id=atoi(row[1]);
		schedule_entry se={_time_to_uint16(row[2]), _time_to_uint16(row[3])};
		if((*data).add_cicle(se, valve_id, schedule_id)){
			return 5;
		}
		i++;
	}
	/* close connection */
	mysql_free_result(res);
	mysql_close(conn);
	if(!i){
		return 6;
	}
	return 0;
}

int get_last_schedule(ArduinoSchedules* data){
	int n_rows;
	uint16_t schedule_id;
	uint16_t valve_id;
	stringstream ss;
	char* temp_str;
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, MYSQL_URL, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0)) {
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 1;
	}

	ss << "CALL get_last_schedule("<<(*data).get_arduino_id()<<")";
	temp_str=(char*)ss.str().c_str();
	/* send SQL query */
	if (mysql_query(conn, temp_str)) {
		mysql_close(conn);
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 2;
	}

	res = mysql_store_result(conn);
	if(!res){
		mysql_close(conn);
		return 3;
	}

	n_rows=mysql_num_rows(res);
	if(n_rows==0){
		mysql_free_result(res);
		mysql_close(conn);
		return 4;
	}

	/* output table name */
	int i=0;
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		valve_id=atoi(row[0]);
		schedule_id=atoi(row[1]);
		schedule_entry se={_time_to_uint16(row[2]), _time_to_uint16(row[3])};
		if((*data).add_cicle(se, valve_id, schedule_id)){
			return 5;
		}
		i++;
	}
	/* close connection */
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int set_schedule_sent(int scheduleID){
	MYSQL *conn;
	stringstream ss;
	char* temp_str;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, MYSQL_URL, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0)) {
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 1;
	}

	ss << "CALL set_schedule_sent("<<scheduleID<<")";
	temp_str=(char*)ss.str().c_str();
	/* send SQL query */
	if (mysql_query(conn, temp_str)) {
		mysql_close(conn);
		log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
		return 2;
	}
	/* close connection */
	mysql_close(conn);
	return 0;
}
