#include "schedule.h"

ValveSchedule::ValveSchedule(){}

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

void ArduinoSchedules::add_schedule(ValveSchedule sche){
  ValveSchedule* _temp = new ValveSchedule[n_schedules+1];
  memcpy(_temp, schedules, sizeof(ValveSchedule)*n_schedules);
  memcpy(_temp+n_schedules, &sche, sizeof(ValveSchedule));
  schedules=_temp;
  n_schedules++;
}

void ArduinoSchedules::add_cicle(schedule_entry se, uint16_t valve_id, uint16_t schedule_id){
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
    add_schedule(v);
  }
}

uint16_t ArduinoSchedules::get_arduino_id(){
  return arduino_id;
}

uint16_t ArduinoSchedules::get_message_size(){
  uint8_t i;
  uint32_t size_16=sizeof(uint16_t);
  uint32_t size_8=sizeof(uint8_t);
  uint16_t size=size_16 + size_8;  //arduino_id n_schedules
  for(i=0;i<n_schedules;i++){
    size+=2*size_16+size_8 + schedules[i].n_cicles*2*size_16; //valveid scheduleid, n_cicles, cicles
  }
  return size+1;    //endchar
}

void ArduinoSchedules::make_message(char** msg){
  uint8_t i,j;
  uint32_t size_16=sizeof(uint16_t);
  uint32_t size_8=sizeof(uint8_t);
  uint32_t total_copied;
  uint16_t _valve_id, _schedule_id, _cicle_start, _cicle_stop;
  uint16_t _arduino_id = htons(arduino_id);
  memcpy(*msg, &_arduino_id, size_16);
  memcpy((*msg)+size_16, &n_schedules, size_8);
  total_copied=size_16+size_8;
  for(i=0;i<n_schedules;i++){
    _valve_id=htons(schedules[i].valve_id);
    _schedule_id=htons(schedules[i].schedule_id);
    memcpy((*msg)+total_copied, &(_valve_id), size_16);
    total_copied+=size_16;
    memcpy((*msg)+total_copied, &(_schedule_id), size_16);
    total_copied+=size_16;
    memcpy((*msg)+total_copied, &(schedules[i].n_cicles), size_8);
    total_copied+=size_8;
    for(j=0;j<schedules[i].n_cicles;j++){
      _cicle_start=htons(schedules[i].cicles[j].cicle_start);
      _cicle_stop=htons(schedules[i].cicles[j].cicle_stop);
      memcpy((*msg)+total_copied, &(_cicle_start), size_16);
      total_copied+=size_16;
      memcpy((*msg)+total_copied, &(_cicle_stop), size_16);
      total_copied+=size_16;
    }
  }
  memcpy((*msg), &END_TRANS_CHAR, size_8);
}
