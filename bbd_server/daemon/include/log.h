#ifndef _LOG_H_
#define _LOG_H_

#include <fstream>

#define LOG_DIR_PATH   "/opt/bbd_server/logs/"

extern std::ofstream log_file;

void init_log();
void stop_log();

#endif
