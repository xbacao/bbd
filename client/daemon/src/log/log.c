#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "socket_data/socket_data.h"

#define GREEN_C      "\033[0;32m"
#define RED_C           "\033[0;31m"
#define YELLOW_C    "\033[1;33m"
#define BLUE_C          "\033[0;34m"
#define NC                  "\033[0m"

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

void log_request(const char* ip, uint16_t port, uint16_t msg_type){
  fprintf(log_fd, "[%lu] %sreq%s: [to:%s, port:%u, msg_type:%s]\n",
    time(NULL), YELLOW_C, NC, ip, port, req_type_to_str(msg_type));
  fflush(log_fd);
}

void log_response(const char* ip, uint16_t msg_type, uint16_t rsp_len){
  fprintf(log_fd, "[%lu] %srsp%s: [from_ip:%s, msg_type:%s, rsp_len:%u]\n",
    time(NULL), BLUE_C, NC, ip, req_type_to_str(msg_type), rsp_len);
  fflush(log_fd);
}

void close_logs(){
  if(log_fd!=stdout){
    fclose(log_fd);
  }
}
