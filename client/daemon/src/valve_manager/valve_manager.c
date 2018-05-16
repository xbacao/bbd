#include "valve_manager.h"

static valve_state* valve_states;

void vm_init_valve_manager(uint16_t n_valves){
  valve_states=(valve_state*) malloc(sizeof(valve_state)*n_valves);
}

void vm_set_valve_state(uint16_t valve_id, valve_state newstate){
  //TODO
}
