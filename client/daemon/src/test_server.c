#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "log/log.h"
#include "client_controller.h"



int main(void){
  int n;

  init_logs_stdout();

  n=cc_update_active_schedules();
  if(n){
    printf("test_req_get_active_sches failed: %u\n",n);
    return 1;
  }

  close_logs();
  printf("tests succeeded!\n");
  return 0;
}
