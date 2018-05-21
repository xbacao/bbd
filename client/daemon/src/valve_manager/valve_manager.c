#include "valve_manager.h"

#include <stdlib.h>

#include "log/log.h"

static struct valve_map_entry* _valves_map;
static uint16_t _n_valves;

struct valve_map_entry{
    uint16_t v_id;
    enum valve_state v_state;
};

void vm_init_set_valves(uint16_t* valve_ids, uint16_t n_valves){
  _n_valves=n_valves;
  _valves_map=(struct valve_map_entry*) malloc(sizeof(struct valve_map_entry)*n_valves);

  for(uint16_t i=0;i<_n_valves;i++){
    _valves_map[i]=(struct valve_map_entry) {valve_ids[i], VALVE_CLOSED};
  }

  log_set_valves(valve_ids, n_valves);
  log_info("valve manager initialized; new valves set");
}

int vm_set_valve_state(uint16_t valve_id, enum valve_state newstate){
  for(uint16_t i=0;i<_n_valves;i++){
    if(_valves_map[i].v_id==valve_id){
      //TODO

      _valves_map[i].v_state=newstate;
      log_valve_newstate(valve_id, newstate);
      return 0;
    }
  }
  log_error("attempting set_valve_state but valve_id not found");
  return 1;
}

int vm_get_valve_state(uint16_t valve_id, enum valve_state* v_state){
  for(uint16_t i=0;i<_n_valves;i++){
    if(_valves_map[i].v_id==valve_id){
      *v_state=_valves_map[i].v_state;
      return 0;
    }
  }
  log_error("attempting get_valve_state but valve_id not found");
  return 1;
}
