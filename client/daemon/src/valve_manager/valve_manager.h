#ifndef _VALVE_MANAGER_H_
#define _VALVE_MANAGER_H_

enum valve_state { VALVE_OPEN, VALVE_CLOSED};

void vm_init_valve_manager(uint16_t n_valves);

void vm_set_valve_state(uint16_t valve_id, valve_state newstate);

#endif
