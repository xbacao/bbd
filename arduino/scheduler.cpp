#include "scheduler.h"
#include "arduino_cfg.h"

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

  return 0;
}
