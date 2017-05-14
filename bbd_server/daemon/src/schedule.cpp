#include "schedule.h"
#include "socket_data.h"
#include <arpa/inet.h>
#include <string.h>

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
  delete[] cicles;
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
  delete[] schedules;
  schedules=_temp;
  n_schedules++;
  return 0;
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

uint8_t ArduinoSchedules::get_message_size(){
  uint8_t i;
  uint8_t size_16=sizeof(uint16_t);
  uint8_t size_8=sizeof(uint8_t);
  uint8_t size=2*size_8;  //arduino_id n_schedules
  for(i=0;i<n_schedules;i++){
    size+=2*size_16+size_8 + schedules[i].n_cicles*2*size_16; //valveid scheduleid, n_cicles, cicles
  }
  return size;
}

void ArduinoSchedules::make_message(char* msg){
  uint8_t i,j;
  uint8_t size_16=sizeof(uint16_t);
  uint8_t size_8=sizeof(uint8_t);
  uint8_t total_copied=0;
  uint16_t _valve_id, _schedule_id, _cicle_start, _cicle_stop;
  // uint16_t msg_size = get_message_size()-size_16;

  // memcpy(*msg, &msg_size, size_16);
  // total_copied+=size_16;
  memcpy(msg, &arduino_id, size_8);
  total_copied+=size_8;
  memcpy(msg+total_copied, &n_schedules, size_8);
  total_copied+=size_8;
  for(i=0;i<n_schedules;i++){
    _valve_id=htons(schedules[i].valve_id);
    _schedule_id=htons(schedules[i].schedule_id);
    memcpy(msg+total_copied, &(_valve_id), size_16);
    total_copied+=size_16;
    memcpy(msg+total_copied, &(_schedule_id), size_16);
    total_copied+=size_16;
    memcpy(msg+total_copied, &(schedules[i].n_cicles), size_8);
    total_copied+=size_8;
    for(j=0;j<schedules[i].n_cicles;j++){
      _cicle_start=htons(schedules[i].cicles[j].cicle_start);
      _cicle_stop=htons(schedules[i].cicles[j].cicle_stop);
      memcpy(msg+total_copied, &(_cicle_start), size_16);
      total_copied+=size_16;
      memcpy(msg+total_copied, &(_cicle_stop), size_16);
      total_copied+=size_16;
    }
  }
}

int ArduinoSchedules::decode_message(char* msg, uint16_t msg_size){
  uint8_t  i_sche, i_cicle;
  uint16_t _valve_id, _schedule_id, _n_cicles, _cicle_start, _cicle_stop, _n_schedules;
  ValveSchedule _v_s;
  schedule_entry _s_e;

  uint32_t size_16= sizeof(uint16_t);
  uint32_t size_8= sizeof(uint8_t);
  uint16_t total_read=0;

  memcpy(&arduino_id, msg, size_8);
  total_read+=size_8;
  memcpy(&_n_schedules, msg+total_read , size_8);
  total_read+=size_8;
  for(i_sche=0;i_sche<_n_schedules;i_sche++){
    memcpy(&_valve_id, msg+total_read , size_16);
    total_read+=size_16;
    _valve_id=ntohs(_valve_id);
    memcpy(&_schedule_id, msg+total_read, size_16);
    total_read+=size_16;
    _schedule_id=ntohs(_schedule_id);
    memcpy(&_n_cicles, msg+total_read, size_8);
    total_read+=size_8;
    _v_s=ValveSchedule(_valve_id, _schedule_id);
    for(i_cicle=0;i_cicle<_n_cicles;i_cicle++){
      memcpy(&_cicle_start, msg+total_read , size_16);
      total_read+=size_16;
      _cicle_start=ntohs(_cicle_start);
      memcpy(&_cicle_stop, msg+total_read, size_16);
      total_read+=size_16;
      _cicle_stop=ntohs(_cicle_stop);
      _s_e={_cicle_start, _cicle_stop};
      _v_s.add_cicle(_s_e);
    }
    add_schedule(_v_s);
  }
  return total_read != msg_size;
}

std::ostream& operator<< (std::ostream& os, const schedule_entry& s_e){
  os<<"{start:"<<s_e.cicle_start<<", stop:"<<s_e.cicle_stop<<"}";
  return os;
}

std::ostream& operator<< (std::ostream& os, const ValveSchedule& v_s){
  os<<"{valve_id:"<<v_s.valve_id<<", schedule_id:"<<v_s.schedule_id<<", schedules:[";
  for(uint8_t i=0;i<v_s.n_cicles;i++){
    os<<v_s.cicles[i];
    if(i!=v_s.n_cicles-1){
      os<<", ";
    }
  }
  os<<"]}";
  return os;
}


std::ostream& operator<< (std::ostream& os, const ArduinoSchedules& a_s){
  os<<"{arduino_id:"<<a_s.arduino_id<<", schedules:[";
  for(uint8_t i=0;i<a_s.n_schedules;i++){
    os<<a_s.schedules[i];
    if(i!=a_s.n_schedules-1){
      os<<", ";
    }
  }
  os<<"]}";
  return os;
}
