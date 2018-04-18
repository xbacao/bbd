#ifndef _GPRS_H_
#define _GPRS_H_

#define _GSM_RXPIN_ 7
#define _GSM_TXPIN_ 8
#define DEFAULT_SS_BAUDRATE 2400
#define DEFAULT_RECV_WAIT_PERIOD  100    //ms

#define RECV_BUFF_LEN 128
#define RSP_BUFF_LEN  128
#define SOCK_BUFF_LEN 128
#define CR_CHAR   0x0d
#define NL_CHAR   0x0a
#define SUB_CHAR  0x1a
#define END_TRANS_CHAR 0xff

#define START_COM_TMOUT
#define MAX_INTERCHAR_TMOUT
#define N_ATTEMPTS

#define DEBUG_STATES

enum GPRS_STATE{GSM_OFF_STATE, GSM_ON_STATE, GSM_PIN_STATE, GSM_IP_STATE, GSM_TCP_STATE};
enum BUFF_STATE{BUFF_READY, BUFF_USED};



int init_gprs();
int attachGPRS();
int dettachGPRS();
int connect_tcp_socket();
int disconnect_tcp_socket();



GPRS_STATE get_gsm_state();



};
#endif
