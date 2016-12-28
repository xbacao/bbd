#ifndef _BBDA_H_
#define _BBDA_H_

void requestSync();
int setNewTime();
int syncTimeWithServer();
int checkRequests();
int handleRequest();
int getRequestMsg();
int getTimeMsg();
int markRequestServed(int requestID);
int decodeTime(uint64_t *time_64);
int decodeRequest(int *requestID, int *valveID, int *action);
int confGPRS();
int dettachGPRS();
int attachGPRS();
/*******************/
void digitalClockDisplay();
void printDigits(int digits);

#endif
