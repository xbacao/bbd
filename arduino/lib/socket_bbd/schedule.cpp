#include "schedule.h"

ValveSchedule::ValveSchedule(){}

ValveSchedule::ValveSchedule(const ValveSchedule &v_sche){
  valve_id=v_sche.valve_id;
  schedule_id=v_sche.schedule_id;
  n_cicles=v_sche.n_cicles;
  cicles= new schedule_entry[n_cicles];
  memcpy(cicles, v_sche.cicles, sizeof(schedule_entry)*n_cicles);
}

ValveSchedule::ValveSchedule(uint16_t _valve_id, uint8_t _schedule_id){
  valve_id=_valve_id;
  schedule_id=_schedule_id;
  n_cicles=0;
}

void ValveSchedule::add_cicle(schedule_entry cicle){
  schedule_entry* _temp=new schedule_entry[n_cicles+1];
  memcpy(_temp, cicles, n_cicles*sizeof(schedule_entry));
  memcpy(_temp+n_cicles, &cicle, sizeof(schedule_entry));
  cicles=_temp;
  n_cicles++;
}

uint16_t ValveSchedule::get_valve_id(){
  return valve_id;
}

uint16_t ValveSchedule::get_schedule_id(){
  return schedule_id;
}

ArduinoSchedules::ArduinoSchedules(uint16_t _arduino_id){
  arduino_id=_arduino_id;
  n_schedules=0;
}

int ArduinoSchedules::add_schedule(ValveSchedule sche){
  //check if theres schedule for valve
  for(uint8_t i=0;i<n_schedules;i++){
    if(schedules[i].valve_id==sche.valve_id){
      return 1;
    }
  }
  ValveSchedule* _temp = new ValveSchedule[n_schedules+1];
  memcpy(_temp, schedules, sizeof(ValveSchedule)*n_schedules);
  memcpy(_temp+n_schedules, &sche, sizeof(ValveSchedule));
  _temp[n_schedules].cicles=new schedule_entry[sche.n_cicles];
  memcpy(_temp[n_schedules].cicles, sche.cicles, sizeof(schedule_entry)*sche.n_cicles);
  schedules=_temp;
  n_schedules++;
  return 0;
}


int ArduinoSchedules::update_schedule(ValveSchedule sche){
  bool found=false;
  for(uint8_t i=0;i<n_schedules;i++){
    if(schedules[i].valve_id==sche.valve_id){
      if(sche.schedule_id>schedules[i].schedule_id){
        schedules[i]=ValveSchedule(sche);
      }
      found=true;
    }
  }
  return !found;  //1
}

int ArduinoSchedules::add_cicle(schedule_entry se, uint16_t valve_id, uint16_t schedule_id){
  uint8_t i;
  bool found=false;
  for(i=0;i<n_schedules;i++){
    if(schedules[i].get_valve_id()==valve_id && schedules[i].get_schedule_id()==schedule_id){
      schedules[i].add_cicle(se);
      found=true;
    }
  }
  if(!found){
    ValveSchedule v=ValveSchedule(valve_id,schedule_id);
    v.add_cicle(se);
    if(add_schedule(v)){
      return 1;
    }
  }
  return 0;
}

uint16_t ArduinoSchedules::get_arduino_id(){
  return arduino_id;
}

int ArduinoSchedules::get_schedule_by_valve(uint16_t valve_id, ValveSchedule *sche){
  uint8_t i;
  for(i=0;i<n_schedules;i++){
    if(schedules[i].valve_id==valve_id){
      *sche=schedules[i];
      return 0;
    }
  }
  return 1;
}

int ArduinoSchedules::decode_message(char* msg, uint16_t msg_size){
  uint8_t  i_sche, i_cicle;
  uint16_t _valve_id, _schedule_id, _n_cicles, _cicle_start, _cicle_stop;
  ValveSchedule _v_s;
  schedule_entry _s_e;

  uint32_t size_16= sizeof(uint16_t);
  uint32_t size_8= sizeof(uint8_t);
  uint16_t total_read=0;

  memcpy(&arduino_id, msg, size_8);
  total_read+=size_8;
  memcpy(&n_schedules, msg+total_read , size_8);
  total_read+=size_8;
  for(i_sche=0;i_sche<n_schedules;i_sche++){
    memcpy(&_valve_id, msg+total_read , size_16);
    total_read+=size_16;
    memcpy(&_schedule_id, msg+total_read, size_16);
    total_read+=size_16;
    memcpy(&_n_cicles, msg+total_read, size_8);
    total_read+=size_8;
    _v_s=ValveSchedule(_valve_id, _schedule_id);
    for(i_cicle=0;i_cicle<_n_cicles;i_cicle++){
      memcpy(&_cicle_start, msg+total_read , size_16);
      total_read+=size_16;
      memcpy(&_cicle_stop, msg+total_read, size_16);
      total_read+=size_16;
      _s_e={_cicle_start, _cicle_stop};
      _v_s.add_cicle(_s_e);
    }
    add_schedule(_v_s);
  }
  return total_read != msg_size;
}

static bool _is_valve_mine(uint16_t valve_id){
  for(uint8_t i=0;i<N_EV;i++){
    if(valve_id==valve_ids[i]){
      return true;
    }
  }
  return false;
}

int ArduinoSchedules::process_new_schedules(ArduinoSchedules _new_schedules){
  uint16_t _new_valve_id;
  for(uint8_t i=0; i<_new_schedules.n_schedules;i++){
    _new_valve_id=_new_schedules.schedules[i].valve_id;
    if(_is_valve_mine(_new_valve_id)){
      bool found=false;
      for(uint8_t j=0;j<n_schedules;j++){
        if(schedules[j].valve_id==_new_valve_id){
          update_schedule(_new_schedules.schedules[i]);
          found=true;
        }
      }
      if(!found){
        if(add_schedule(_new_schedules.schedules[i])){
          return 1;
        }
      }

    }
  }
  return 0;
}
