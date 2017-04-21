#ifndef EVCONTROLLER_H
#define EVCONTROLLER_H

/* sets the pin as output and sets the valve state to closed (as default) */
void setup_ev_controller();

bool evcs_state(int valve_id);

bool open_evcs(int valve_id);

bool close_evcs(int valve_id);

#endif
