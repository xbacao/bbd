#include "db.h"
#include <pqxx/pqxx>
#include "log/log.h"
#include <sstream>

using namespace std;

#define CONN_INFO           "user=root password=morcao123 dbname=bbd host=bbd_db.bbd_network port=5432"

int db_get_device_valves(uint16_t device_id, std::vector<uint16_t> &valve_ids){
	log_info("requesting get_device_valves from db");

	try{
		pqxx::connection c(CONN_INFO);
		pqxx::result res;
		pqxx::work txn(c);

		c.prepare("get_device_valves", "SELECT * FROM get_device_valves ($1)");
		res=txn.prepared("get_device_valves")(device_id).exec();

		for (pqxx::result::const_iterator row=res.begin();row != res.end();++row){
			valve_ids.push_back(row[0].as<uint16_t>());
		}
	}	catch(const std::exception &e) {
			log_error(__func__, e.what());
			return 1;
	}

	log_db_response<uint16_t>(valve_ids);
	return 0;
}

int db_get_active_schedules(uint16_t device_id, vector<schedule>& sches){
	log_info("requesting get_active_schedules from db");

	try{
		pqxx::connection c(CONN_INFO);
		pqxx::result res;
		pqxx::work txn(c);

		c.prepare("get_active_schedules", "SELECT * FROM get_active_schedules ($1)");
		res=txn.prepared("get_active_schedules")(device_id).exec();

		for (pqxx::result::const_iterator row=res.begin();row != res.end();++row){
			sches.push_back({row[0].as<uint16_t>(), row[1].as<uint16_t>(),
				row[2].as<uint16_t>(), row[3].as<uint16_t>()});
	  }
	}	catch(const std::exception &e) {
			log_error(__func__, e.what());
			return 1;
	}

	log_db_response<schedule>(sches);
	return 0;
}



/*TODO*/
int set_schedule_sent(int scheduleID){
	// MYSQL *conn;
	// stringstream ss;
	// char* temp_str;
	//
	// conn = mysql_init(NULL);
	//
	// /* Connect to database */
	// if (!mysql_real_connect(conn, MYSQL_URL, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0)) {
	// 	log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
	// 	return 1;
	// }
	//
	// ss << "CALL set_schedule_sent("<<scheduleID<<")";
	// temp_str=(char*)ss.str().c_str();
	// /* send SQL query */
	// if (mysql_query(conn, temp_str)) {
	// 	mysql_close(conn);
	// 	log_file << time(nullptr) <<": DB:"<<mysql_error(conn)<<endl;
	// 	return 2;
	// }
	// /* close connection */
	// mysql_close(conn);
	return 1;
}