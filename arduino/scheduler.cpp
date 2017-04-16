#include "scheduler.h"

static ArduinoSchedules _arduino_schedules(ARDUINO_ID);
static bool _started = false;

int init_scheduler(ArduinoSchedules a_s){
  if(_started){
    return 1;
  }
  _arduino_schedules.process_new_schedules(a_s);
  _started=true;
  return 0;
}

int process_schedules(ArduinoSchedules a_s){
  if(!_started){
    return 1;
  }
  _arduino_schedules.process_new_schedules(a_s);
  return 0;
}

int scheduler_action(){
  //TODO
  // time_t cur_time=now()%SECS_PER_DAY;
  // int n;
  //
  // for(uint8_t i=0;i<N_EV;i++){
  //   ValveSchedule v_sche;
  //   n=_arduino_schedules.get_schedule_by_valve(valve_ids[i], &v_sche);
  //   if(!n){
  //     bool v_state=evcs_state(valve_ids[i]);
  //     for(uint8_t j=0;j<v_sche.n_cicles;j++){
  //       if(! && cur_time>v_sche.cicles[i].cicle_start && cur_time<v_sche.cicles[i].cicle_stop){
  //
  //       }
  //     }
  //   }
  // }

  return 0;
}
