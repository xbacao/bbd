#ifndef EVCONTROLLER_H
#define EVCONTROLLER_H

#include "valve.h"
//#include <stdbool.h>
#include "Arduino.h"

static bool currentState[N_EV];
static int relayPins[N_EV] = {RP_VALVE_1, RP_VALVE_2, RP_VALVE_3, RP_VALVE_4, RP_VALVE_5};

/* sets the pin as output and sets the valve state to closed (as default) */
void setupEVController();

bool openEVCS(int valveID);

bool closeEVCS(int valveID);

#endif