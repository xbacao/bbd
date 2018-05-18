#include "schedule_manager.h"

#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "log/log.h"

#define SCHEDULER_SLEEP_INTERVAL      100000 //us

static struct schedule* _device_schedules;
static uint16_t _schedules_len;
static bool _schedules_set=false;

static pthread_mutex_t _schedules_mtx;
static pthread_t* _scheduler_thread;

static void* _sm_run_scheduler_thread();

static void* _sm_run_scheduler_thread(){
  time_t rawtime;
  struct tm * timeinfo;
  uint16_t curr_mins;

  while(true){
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    curr_mins=timeinfo->tm_hour*60+timeinfo->tm_min;

    for(uint16_t i=0;i<_schedules_len;i++){

      //activate schedule
      if(!_device_schedules[i].is_active && _device_schedules[i].start <= curr_mins &&
          _device_schedules[i].stop >= curr_mins){
        pthread_mutex_lock(&_schedules_mtx);
        vm_set_valve_state(_device_schedules[i].valve_id, VALVE_OPEN);
        _device_schedules[i].is_active=true;
        pthread_mutex_unlock(&_schedules_mtx);
      }

      //deactivate schedule
      if(_device_schedules[i].is_active && (curr_mins < _device_schedules[i].start ||
          curr_mins > _device_schedules[i].stop)){
        pthread_mutex_lock(&_schedules_mtx);
        vm_set_valve_state(_device_schedules[i].valve_id, VALVE_CLOSED);
        _device_schedules[i].is_active=false;
        pthread_mutex_unlock(&_schedules_mtx);
      }

      usleep(SCHEDULER_SLEEP_INTERVAL);
    }
  }
  return NULL;
}

//FIXME
void sm_set_new_schedules(struct schedule* sches, uint16_t schedules_len){
  uint16_t sches_bytesize=sizeof(struct schedule)*schedules_len;

  //TODO ADD CHECK IF SCHEDULES ARE VALID

  pthread_mutex_lock(&_schedules_mtx);

  if(_schedules_set){
    free(_device_schedules);
  }

  _device_schedules=(struct schedule*) malloc(sches_bytesize);
  memcpy(_device_schedules, sches, sches_bytesize);
  _schedules_len=schedules_len;

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
  if(pthread_create(_scheduler_thread, NULL, &_sm_run_scheduler_thread, NULL)!=0){
    return 1;
  }

  log_info("schedule manager started");
  return 0;
}

// pthread_join(tid[1], NULL);
//    pthread_mutex_destroy(&lock);
