#ifndef _ARDUINO_CFG_H_
#define _ARDUINO_CFG_H_

#include <inttypes.h>

const uint8_t ARDUINO_ID=1;
#define N_EV          5

const uint16_t valve_ids[N_EV] = {1,2,3,4,5};
const uint8_t relay_pins[N_EV] = {4,5,9,10,11};

#endif
