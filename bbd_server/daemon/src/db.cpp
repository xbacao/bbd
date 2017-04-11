#include "db.h"

using namespace std;

static uint16_t _time_to_uint16(char* c_time){
	int hour,min,sec;
	sscanf(c_time,"%d:%d:%d",&hour,&min,&sec);
	return hour*60+min;
}


int get_new_schedule(int valveID, schedule_data* data){
	int n_rows;
	uint16_t scheduleID;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
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

	ss << "CALL get_valve_unsent_schedule("<<valveID<<")";
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
		return -1;
	}

	n_rows=mysql_num_rows(res);
	if(n_rows==0){
		mysql_free_result(res);
		mysql_close(conn);
		return -2;
	}

	cicle_starts=new uint16_t[n_rows];
	cicle_stops=new uint16_t[n_rows];

	/* output table name */
	int i=0;
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		scheduleID=atoi(row[0]);
		cicle_starts[i]=_time_to_uint16(row[1]);
		cicle_stops[i]=_time_to_uint16(row[2]);
		i++;
	}
	*data={.valve_id=(uint16_t)valveID, .schedule_id=scheduleID, .n_cicles=(uint16_t)n_rows, .cicle_starts=cicle_starts, .cicle_stops=cicle_stops};
	/* close connection */
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int get_last_schedule(int valveID, schedule_data* data){
	int n_rows;
	uint16_t scheduleID;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;
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

	ss << "CALL get_valve_last_schedule("<<valveID<<")";
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
		return -1;
	}

	n_rows=mysql_num_rows(res);
	if(n_rows==0){
		mysql_free_result(res);
		mysql_close(conn);
		return -2;
	}

	cicle_starts=new uint16_t[n_rows];
	cicle_stops=new uint16_t[n_rows];

	/* output table name */
	int i=0;
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		scheduleID=atoi(row[0]);
		cicle_starts[i]=_time_to_uint16(row[1]);
		cicle_stops[i]=_time_to_uint16(row[2]);
		i++;
	}
	*data={.valve_id=(uint16_t)valveID, .schedule_id=scheduleID, .n_cicles=(uint16_t)n_rows, .cicle_starts=cicle_starts, .cicle_stops=cicle_stops};
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
