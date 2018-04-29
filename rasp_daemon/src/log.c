#include "log.h"

#include <time.h>

static FILE* log_fd;

void init_logs(){
  log_fd=open(LOG_FILE);
}

void log_info(const char* str){
  fprintf(log_fd, "[%lu] %s%s\n",time(NULL), GREEN_C, str);
}

void log_error(const char* str){
  fprintf(log_fd, "[%lu] %s%s\n",time(NULL), RED_C, str);
}
