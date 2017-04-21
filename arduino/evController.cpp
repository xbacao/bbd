#include "evController.h"
#include <inttypes.h>
#include "arduino_cfg.h"
#include <Arduino.h>

static bool current_state[N_EV];

static int _find_valve_idx(uint16_t valve_id){
  uint8_t valve_idx;
  for(uint8_t i=0;i<N_EV;i++){
    if(valve_ids[i]==valve_id){
      return i;
    }
  }
  return -1;
}

void setupEVController(){
  int i;
  for(i = 0; i < N_EV; i++){
  	pinMode(relay_pins[i], OUTPUT);
  	digitalWrite(relay_pins[i], LOW);
  	current_state[i] = false;
  }
}

bool evcs_state(int valve_id){
  int valve_idx=_find_valve_idx(valve_id);
  if(valve_idx<0){
    return false;
  }
  return current_state[valve_idx];
}

bool open_evcs(int valve_id){
  int valve_idx =_find_valve_idx(valve_id);
  if(valve_idx<0){
    return false;
  }

	digitalWrite(relay_pins[valve_idx], HIGH);
	current_state[valve_idx] = true;
	return true;
}

bool close_evcs(int valve_id){
  int valve_idx =_find_valve_idx(valve_id);
  if(valve_idx<0){
    return false;
  }

 	digitalWrite(relay_pins[valve_idx], LOW);
	current_state[valve_idx] = false;
	return true;
}
