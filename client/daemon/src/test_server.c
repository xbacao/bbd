#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "socket_data/socket_data.h"
#include "log/log.h"

#define IP            "127.0.0.1"
#define PORT          7777
#define MAGIC_NUMBER  38017
#define DEVICE_ID     1


int test_req_get_active_sches(){
  struct schedule* rsp_sches;
  uint16_t sches_len;
  int n;

  rsp_sches=(struct schedule*) malloc(0);
  n=req_get_active_sches_msg(IP,PORT,MAGIC_NUMBER,DEVICE_ID,rsp_sches,&sches_len);
  if(n){
    return 1;
  }

  for(uint16_t i=0;i<sches_len;i++){
    printf("rcved sche:  sche_id:%u valve_id:%u  start:%u  stop:%u\n",
      rsp_sches->schedule_id, rsp_sches->valve_id, rsp_sches->start,
      rsp_sches->stop);
  }

  free(rsp_sches);
  return 0;
}

int main(void){
  int n;

  init_logs_stdout();

  n=test_req_get_active_sches();
  if(n){
    printf("test_req_get_active_sches failed: %u\n",n);
    return 1;
  }

  close_logs();
  printf("tests succeeded!\n");
  return 0;
}
