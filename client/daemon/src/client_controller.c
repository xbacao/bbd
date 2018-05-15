#include "client_controller.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "server_info/server_info.h"
#include "socket_data/socket_data.h"
#include "log/log.h"

int update_active_schedules(){
  struct schedule* rsp_sches;
  int sockfd, n;
  char* rsp;
  uint16_t sches_len, rsp_len;

  n=send_req_get_active_sches(IP,PORT,MAGIC_NUMBER,DEVICE_ID, &sockfd);
  if(n){
    return 1;
  }

  log_request(IP, PORT, GET_ACTIVE_SCHES_MSG);

  n=recv_rsp_len(sockfd, &rsp_len);
	if(n){
		log_error("receiving response length");
		return 3;
	}

	rsp=(char*) malloc(sizeof(char)*rsp_len);
	n=recv_rsp_msg(sockfd, rsp_len, rsp);
	if(n){
		log_error("receiving response message");
		free(rsp);
		return 4;
	}

  log_response(IP, GET_ACTIVE_SCHES_MSG, rsp_len);

	sches_len=rsp_len/sizeof(struct schedule);

	rsp_sches=(struct schedule*) malloc(rsp_len);
	for(uint16_t i=0;i<sches_len;i++){
		decode_schedule(rsp+i*sizeof(struct schedule), rsp_sches+i);
	}

	free(rsp);


  //do something with schedules

  for(uint16_t i=0;i<sches_len;i++){
    printf("rcved sche:  sche_id:%u valve_id:%u  start:%u  stop:%u\n",
      rsp_sches->schedule_id, rsp_sches->valve_id, rsp_sches->start,
      rsp_sches->stop);
  }

  free(rsp_sches);
  return 0;
}
