#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <vector>

void init_logs_stdout();
int init_logs_file(const char* log_file);
void log_info(const char* str);
void log_request(const char* ip, uint16_t device_id, const char* msg_type,
  uint16_t msg_size);
void log_response(const char* ip, uint16_t device_id, const char* msg_type);
template <typename T> void log_db_response(std::vector<T>);
void log_error(const char* str);
void log_error(const char* descr, const char* error);
void close_logs();

#endif
