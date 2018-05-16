#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>

#include "valve_manager/valve_manager.h"
#include "schedule/schedule.h"

void init_logs_stdout();
int init_logs_file(const char* log_file);
void close_logs();

void log_info(const char* str);
void log_error(const char* str);

void log_request(const char* ip, uint16_t port, uint16_t msg_type);
void log_response(const char* ip, uint16_t msg_type, uint16_t rsp_len);

void log_valve_newstate(uint16_t valve_id, enum valve_state v_state);
void log_new_schedules(struct schedule* sches, uint16_t sches_len);

#endif
