#ifndef _VALVE_MANAGER_H_
#define _VALVE_MANAGER_H_

#include <stdint.h>

enum valve_state {VALVE_CLOSED=0, VALVE_OPEN=1};

void vm_init_set_valves(uint16_t* valve_ids, uint16_t n_valves);
int vm_set_valve_state(uint16_t valve_id, enum valve_state newstate);
int vm_get_valve_state(uint16_t valve_id, enum valve_state* v_state);

#endif
