#ifndef _VALVE_MANAGER_H_
#define _VALVE_MANAGER_H_

#include <stdint.h>

enum valve_state {VALVE_OPEN, VALVE_CLOSED};

void vm_init_valve_manager(uint16_t* valve_ids, uint16_t n_valves);
int vm_set_valve_state(uint16_t valve_id, enum valve_state newstate);
int vm_get_valve_state(uint16_t valve_id, enum valve_state* v_state);

#endif
