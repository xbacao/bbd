#ifndef _BBDA_H_
#define _BBDA_H_


#include "gsm_ard.h"
#include "socket_bbd.h"
#include "scheduler.h"

int syncTimeWithServer();
int get_last_schedule();
/*******************/
void digitalClockDisplay();
void printDigits(int digits);

#endif
