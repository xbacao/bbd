#include "schedule_manager.h"

#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

static struct schedule* device_schedules;
static bool schedules_set=false;

static pthread_mutex_t* schedules_mtx;
static pthread_t* scheduler_thread;

static void* _sm_run_scheduler_thread();

static void* _sm_run_scheduler_thread(){
  while(true){
    ;;
  }

  return NULL;
}

void sm_set_new_schedules(struct schedule* sches, uint16_t schedules_len){
  uint16_t sches_bytesize=sizeof(struct schedule)*schedules_len;

  pthread_mutex_lock(schedules_mtx);

  if(schedules_set){
    free(device_schedules);
  }

  device_schedules=(struct schedule*) malloc(sches_bytesize);

  memcpy(device_schedules, sches, sches_bytesize);

  pthread_mutex_unlock(schedules_mtx);
}

int sm_start_scheduler(){
  if (pthread_mutex_init(schedules_mtx, NULL) != 0){
    return 1;
  }
  if(pthread_create(scheduler_thread, NULL, &_sm_run_scheduler_thread, NULL)!=0){
    return 2;
  }
  return 0;
}

// pthread_join(tid[1], NULL);
//    pthread_mutex_destroy(&lock);
