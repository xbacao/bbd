#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>

void init_logs_stdout();
int init_logs_file(const char* log_file);
void log_info(const char* str);
void log_request(const char* ip, uint16_t port, uint16_t msg_type);
void log_response(const char* ip, uint16_t msg_type, uint16_t rsp_len);
void log_error(const char* str);
void close_logs();

#endif
