#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>

void init_logs_stdout();
int init_logs_file(const char* log_file);
void log_info(const char* str);
void log_request(const char* ip, uint16_t device_id, uint16_t msg_type,
  uint16_t msg_size);
void log_error(const char* str);
void log_error(const char* descr, const char* error);
void close_logs();

#endif
