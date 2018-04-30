#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define GREEN_C   "\e[0;32m"
#define RED_C     "\033[0;31m"
#define YELLOW_C  "\033[1;33m"
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
    printf("[%lu] %sinfo:%s\n%s",time(NULL), GREEN_C, str, NC);
  }else {
    fprintf(log_fd, "[%lu] %sinfo:%s\n%s",time(NULL), GREEN_C, str, NC);
  }
}

void log_error(const char* str){
  if(log_to_stdout){
    printf("[%lu] %serror: %s\n%s",time(NULL), RED_C, str, NC);
  } else {
    fprintf(log_fd, "[%lu] %serror: %s\n%s",time(NULL), RED_C, str, NC);
  }
}

void log_request(const char* ip, uint16_t device_id, uint16_t msg_type,
uint16_t msg_size){
  if(log_to_stdout) {
    printf("[%lu] %sreq: [ip:%s dev_id:%u msg_type:%u msg_size:%u]\n%s",time(NULL),
      YELLOW_C, ip, device_id, msg_type, msg_size, NC);
  } else {
    fprintf(log_fd, "[%lu] %sreq: [ip:%s dev_id:%u msg_type:%u msg_size:%u]\n%s",
      time(NULL), YELLOW_C, ip, device_id, msg_type, msg_size, NC);
  }
}

void close_logs(){
  if(!log_to_stdout){
    fclose(log_fd);
  }
}
