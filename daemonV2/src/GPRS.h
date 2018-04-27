#ifndef _GPRS_H_
#define _GPRS_H_


#define DEFAULT_SS_BAUDRATE 2400
#define DEFAULT_RECV_WAIT_PERIOD  100    //ms

#define RECV_BUFF_LEN 128
#define RSP_BUFF_LEN  128
#define SOCK_BUFF_LEN 128
#define CR_CHAR   0x0d
#define NL_CHAR   0x0a
#define SUB_CHAR  0x1a
#define END_TRANS_CHAR 0xff

#define START_COM_TMOUT       500
#define MAX_INTERCHAR_TMOUT   100
#define N_ATTEMPTS            5

#define DEBUG_STATES

enum GPRS_STATE{GPRS_OFF_STATE, GPRS_ON_STATE, GPRS_PIN_STATE};
enum BUFF_STATE{BUFF_READY, BUFF_USED};

int init_gprs();


enum GPRS_STATE get_gsm_state();

#endif
