#include "log/log.h"
#include "client_controller.h"
#include "schedule_manager/schedule_manager.h"

static int _start(){
  int n;

  init_logs_stdout();
  n=sm_init_scheduler();
  if(n){
    return 1;
  }

  log_info("updating valve info from server");
  n=cc_set_valve_info();
  if(n){
    return 2;
  }

  log_info("updating schedule info from server");
  n=cc_set_active_schedules();
  if(n){
    return 3;
  }

  log_info("starting schedule manager");
  n=sm_start_scheduler();
  if(n){
    return 4;
  }

  return 0;
}

int main(void){
  int n;

  n=_start();
  if(n){
    return 1;
  }

  n=sm_wait_on_thread();
  if(n){
    return 2;
  }

  return 0;
}
