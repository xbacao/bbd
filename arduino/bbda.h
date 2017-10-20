#ifndef _BBDA_H_
#define _BBDA_H_

#define SYNC_TIME 60*30
#define CI_TIME   60*15

int sync_time_with_server();
int get_last_schedule();
int get_active_sches();
int checkin_server();
/*******************/
void digitalClockDisplay();
void printDigits(int digits);

#endif
