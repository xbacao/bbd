#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define GREEN_C   "\e[0;32m"
#define RED_C     "\033[0;31m"
#define YELLOW_C  "\033[1;33m"
#define NC        "\033[0m"

static FILE* log_fd;

void init_logs_stdout(){
  log_fd=stdout;
}

int init_logs_file(const char* log_file){
  log_fd=fopen(log_file, "a");
  if(log_fd==NULL){
    printf("[%lu]%s Error initializing logs%s: could not open log file\n", time(NULL), RED_C, NC);
    return 1;
  }
  return 0;
}

void log_info(const char* str){
  fprintf(log_fd, "[%lu] %sinfo%s: %s\n", time(NULL), GREEN_C, NC, str);
  fflush(log_fd);
}

void log_error(const char* str){
  fprintf(log_fd, "[%lu] %serror%s: %s\n", time(NULL), RED_C, NC, str);
  fflush(log_fd);
}

void log_request(const char* ip, uint16_t device_id, uint16_t msg_type,
uint16_t msg_size){
  fprintf(log_fd, "[%lu] %sreq: [ip:%s dev_id:%u msg_type:%u msg_size:%u]\n%s",
    time(NULL), YELLOW_C, ip, device_id, msg_type, msg_size, NC);
  fflush(log_fd);
}

void close_logs(){
  if(log_fd!=stdout){
    fclose(log_fd);
  }
}
