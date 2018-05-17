#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <vector>
#include "socket_data/socket_data.h"

void init_logs_stdout();
int init_logs_file(const char* log_file);
void log_info(const char* str);
void log_request(const char* ip, uint16_t device_id, enum request_type msg_type);
void log_response(const char* ip, uint16_t device_id, enum request_type msg_type,
  uint16_t msg_len);
template <typename T> void log_db_response(std::vector<T>);
void log_error(const char* str);
void log_error(const char* descr, const char* error);
void close_logs();

#endif
