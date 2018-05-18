#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "log/log.h"
#include "client_controller.h"
#include "schedule_manager/schedule_manager.h"



int main(void){
  int n;

  init_logs_stdout();
  n=sm_init_scheduler();
  if(n){
    return 99;
  }

  printf("testing cc_update_active_schedules..\n");
  n=cc_update_active_schedules();
  if(n){
    printf("cc_update_active_schedules failed: %u\n",n);
    return 1;
  }

  printf("testing cc_set_valve_info..\n");
  n=cc_set_valve_info();
  if(n){
    printf("cc_set_valve_info failed: %u\n",n);
    return 2;
  }

  close_logs();
  printf("tests succeeded!\n");
  return 0;
}
