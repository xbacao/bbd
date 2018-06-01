#include "schedule_manager.h"

#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "log/log.h"

#define SCHEDULER_SLEEP_INTERVAL      100000 //us

struct schedule_status{
  struct schedule sche;
  bool is_active;
};

static struct schedule_status* _ss;
static uint16_t _ss_len;
static bool _schedules_set=false;

static pthread_mutex_t _schedules_mtx;
static pthread_t _scheduler_thread;

static void* _sm_run_scheduler_thread();

static void* _sm_run_scheduler_thread(){
  time_t rawtime;
  struct tm * timeinfo;
  uint16_t curr_mins;

  while(true){
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    curr_mins=timeinfo->tm_hour*60+timeinfo->tm_min;

    for(uint16_t i=0;i<_ss_len;i++){

      //activate schedule
      if(!_ss[i].is_active && _ss[i].sche.start <= curr_mins &&
          _ss[i].sche.stop >= curr_mins){
        pthread_mutex_lock(&_schedules_mtx);
        vm_set_valve_state(_ss[i].sche.valve_id, VALVE_OPEN);
        _ss[i].is_active=true;
        pthread_mutex_unlock(&_schedules_mtx);
      }

      //deactivate schedule
      if(_ss[i].is_active && (curr_mins < _ss[i].sche.start ||
          curr_mins > _ss[i].sche.stop)){
        pthread_mutex_lock(&_schedules_mtx);
        vm_set_valve_state(_ss[i].sche.valve_id, VALVE_CLOSED);
        _ss[i].is_active=false;
        pthread_mutex_unlock(&_schedules_mtx);
      }

      usleep(SCHEDULER_SLEEP_INTERVAL);
    }
  }
  return NULL;
}

//FIXME
void sm_set_new_schedules(struct schedule* sches, uint16_t schedules_len){
  uint16_t ss_bytesize=sizeof(struct schedule_status)*schedules_len;

  pthread_mutex_lock(&_schedules_mtx);

  if(_schedules_set){
    free(_ss);
  }

  _ss=(struct schedule_status*) malloc(ss_bytesize);
  for(uint16_t i=0;i<schedules_len;i++){
    _ss[i].sche=sches[i];
    _ss[i].is_active=false;
  }
  // memcpy(_ss, sches, ss_bytesize);
  _ss_len=schedules_len;
  _schedules_set=true;

  pthread_mutex_unlock(&_schedules_mtx);

  log_set_schedules(sches, schedules_len);
  log_info("schedule manager set new schedules");
}

int sm_init_scheduler(){
  if (pthread_mutex_init(&_schedules_mtx, NULL) != 0){
    return 1;
  }

  log_info("schedule manager initialized");
  return 0;
}

int sm_start_scheduler(){
  if(pthread_create(&_scheduler_thread, NULL, &_sm_run_scheduler_thread, NULL)!=0){
    return 1;
  }

  log_info("schedule manager started");
  return 0;
}


int sm_wait_on_thread(){
  if(pthread_join(_scheduler_thread, NULL)!=0){
    return 1;
  }
  if(pthread_mutex_destroy(&_schedules_mtx)!=0){
    return 2;
  }
  return 0;
}
