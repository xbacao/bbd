#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define GREEN_C   "\e[0;32m"
#define RED_C     "\033[0;31m"
#define NC        "\033[0m"

static FILE* log_fd;
static bool log_to_stdout=false;

void init_logs_stdout(){
    log_to_stdout=true;
}

void init_logs_file(const char* log_file){
  log_fd=fopen(log_file, "w");
}

void log_info(const char* str){
  if(log_to_stdout){
    printf("[%lu] %s%s\n%s",time(NULL), GREEN_C, str, NC);
  }else {
    fprintf(log_fd, "[%lu] %sinfo:%s\n%s",time(NULL), GREEN_C, str, NC);
  }
}

void log_error(const char* str){
  if(log_to_stdout){
    printf("[%lu] %serror: %s\n%s",time(NULL), RED_C, str, NC);
  } else {
    fprintf(log_fd, "[%lu] %s%s\n%s",time(NULL), RED_C, str, NC);
  }
}

void close_logs(){
  if(!log_to_stdout){
    fclose(log_fd);
  }
}
