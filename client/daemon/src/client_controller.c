#include "client_controller.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "server_info/server_info.h"
#include "socket_data/socket_data.h"
#include "schedule_manager/schedule_manager.h"
#include "log/log.h"

int cc_set_valve_info(){
  int sockfd, n;
  char* rsp;
  uint16_t valve_ids_len, rsp_len;
  uint16_t* valve_ids;

  n=sd_send_request(IP,PORT,MAGIC_NUMBER,DEVICE_ID, GET_DEVICE_VALVES_MSG,
    &sockfd);
  if(n){
    return 1;
  }

  log_request(IP, PORT, GET_DEVICE_VALVES_MSG);

  n=sd_recv_rsp_len(sockfd, &rsp_len);
	if(n){
		log_error("receiving response length");
		return 3;
	}

	rsp=(char*) malloc(sizeof(char)*rsp_len);
	n=sd_recv_rsp_msg(sockfd, rsp_len, rsp);
	if(n){
		log_error("receiving response message");
		free(rsp);
		return 4;
	}

  log_response(IP, GET_DEVICE_VALVES_MSG, rsp_len);

  valve_ids=(uint16_t*) malloc(rsp_len);
  valve_ids_len=rsp_len/sizeof(uint16_t);
  for(uint16_t i=0;i<valve_ids_len;i++){
    sd_decode_valve_id(rsp+sizeof(uint16_t), valve_ids+i);
  }

  vm_init_valve_manager(valve_ids, valve_ids_len);

  free(valve_ids);
  free(rsp);

  return 0;
}

int cc_update_active_schedules(){
  struct schedule* rsp_sches;
  int sockfd, n;
  char* rsp;
  uint16_t sches_len, rsp_len;

  n=sd_send_request(IP,PORT,MAGIC_NUMBER,DEVICE_ID, GET_ACTIVE_SCHES_MSG,
    &sockfd);
  if(n){
    return 1;
  }

  log_request(IP, PORT, GET_ACTIVE_SCHES_MSG);

  n=sd_recv_rsp_len(sockfd, &rsp_len);
	if(n){
		log_error("receiving response length");
		return 3;
	}

	rsp=(char*) malloc(sizeof(char)*rsp_len);
	n=sd_recv_rsp_msg(sockfd, rsp_len, rsp);
	if(n){
		log_error("receiving response message");
		free(rsp);
		return 4;
	}

  log_response(IP, GET_ACTIVE_SCHES_MSG, rsp_len);

	sches_len=rsp_len/sizeof(struct schedule);

	rsp_sches=(struct schedule*) malloc(rsp_len);
	for(uint16_t i=0;i<sches_len;i++){
		sd_decode_schedule(rsp+i*sizeof(struct schedule), rsp_sches+i);
	}

	free(rsp);

  //do something with schedules
  sm_set_new_schedules(rsp_sches, sches_len);
  free(rsp_sches);
  return 0;
}
