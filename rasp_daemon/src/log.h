#ifndef _LOG_H_
#define _LOG_H_

#define LOG_FILE  "dumb.log"

#define GREEN_C   "\e[0;32m"
#define RED_C        "\033[0;31m"
// #define NC               "\033[0m"


void init_logs();
void log_info(const char* str);
void log_error(const char* str);

#endif
