#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "schedule/schedule.h"
#include "socket_data/socket_data.h"

#define GREEN_C   "\e[0;32m"
#define RED_C     "\033[0;31m"
#define YELLOW_C  "\033[1;33m"
#define BLUE_C    "\033[0;34m"
#define PURPLE_C  "\033[0;35m"
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

void log_error(const char* descr, const char* error){
  fprintf(log_fd, "[%lu] %serror %s%s: %s\n", time(NULL), RED_C, descr, NC, error);
  fflush(log_fd);
}

void log_request(const char* ip, uint16_t device_id, enum request_type msg_type){
  fprintf(log_fd, "[%lu] %sreq%s: [from_ip:%s, dev_id:%u, msg_type:%s]\n",
    time(NULL), YELLOW_C, NC, ip, device_id, sd_req_type_to_str(msg_type));
  fflush(log_fd);
}

void log_response(const char* ip, uint16_t device_id, enum request_type msg_type,
uint16_t msg_len){
  fprintf(log_fd, "[%lu] %srsp%s: [to_ip:%s, dev_id:%u, msg_type:%s, msg_len:%u]\n",
    time(NULL), BLUE_C, NC, ip, device_id, sd_req_type_to_str(msg_type), msg_len);
  fflush(log_fd);
}

template <typename T> void log_db_response(std::vector<T>){
}

template<> void log_db_response<schedule>(std::vector<schedule> vec){
  fprintf(log_fd, "[%lu] %sdb_fetch%s:\n[%11s %11s %11s %11s]\n", time(NULL),
    PURPLE_C, NC, "schedule_id", "valve_id", "start", "stop");
  for(auto itr=vec.begin();itr!=vec.end();itr++){
    fprintf(log_fd, "[%11u %11u %11u %11u]\n", itr->schedule_id, itr->valve_id,
      itr->start, itr->stop);
  }
}

template<> void log_db_response<uint16_t>(std::vector<uint16_t> vec){
  fprintf(log_fd, "[%lu] %sdb_fetch%s:\n[%11s]\n", time(NULL),
    PURPLE_C, NC, "valve_id");
  for(auto itr=vec.begin();itr!=vec.end();itr++){
    fprintf(log_fd, "[%11u]\n",*itr);
  }
}


void close_logs(){
  if(log_fd!=stdout){
    fclose(log_fd);
  }
}
