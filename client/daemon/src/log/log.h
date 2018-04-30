#ifndef _LOG_H_
#define _LOG_H_

void init_logs_stdout();
void init_logs_file(const char* log_file);
void log_info(const char* str);
void log_error(const char* str);
void close_logs();

#endif
