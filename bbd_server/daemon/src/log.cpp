#include "log.h"

using namespace std;

ofstream log_file;

void init_log(){
	stringstream ss_log;
	struct stat st = {0};
	if (stat(LOG_DIR_PATH, &st) == -1) {
		mkdir(LOG_DIR_PATH, 0700);
	}

	time_t curr_ts = time(nullptr);
	ss_log << LOG_DIR_PATH << "log" << curr_ts;
	log_file.open(ss_log.str());
	log_file << time(nullptr)<<": log started!"<<endl;
}

void stop_log(){
	log_file.close();
}
