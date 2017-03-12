#ifndef _GSM_ARD_H_
#define _GSM_ARD_H_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include "at.h"
// #include "MemoryFree.h"


#define _GSM_RXPIN_ 7
#define _GSM_TXPIN_ 8
#define DEFAULT_SS_BAUDRATE 2400
#define DEFAULT_RECV_WAIT_PERIOD  100    //ms

#define RECV_BUFF_LEN 128
#define CR_CHAR   0x0d
#define NL_CHAR   0x0a

#define DEBUG_GSM
#define DEBUG_SS
// #define DEBUG_SS_RAW

const unsigned long int _POSSIBLE_BRS[8] ={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

const PROGMEM char NETWORK_APN[] ="internet.vodafone.pt";
const PROGMEM char NETWORK_USER[]="";
const PROGMEM char NETWORK_PASS[]="";

const PROGMEM char SIM_PIN[]="0048";


enum GSM_STATE{GSM_OFF_STATE, GSM_ON_STATE, GSM_PIN_STATE, GSM_GPRS_STATE, GSM_IP_STATE};

class Gsm_Ard{
  char _recv_buff[RECV_BUFF_LEN];
  unsigned int _buff_idx;
  GSM_STATE gsm_state=GSM_OFF_STATE;
  SoftwareSerial _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
public:
  Gsm_Ard();
  int init_gsm_module();
  int attachGPRS();
  int dettachGPRS();
private:
  int send_cmd_comp_rsp(const char* cmd, const char* exp_rsp, int recv_wait_period);
  int send_cmd_get_rsp(const char* cmd, int recv_wait_period, char** rsp_ret, int* rsp_ret_len);

  int _recv_string(char** rsp, int* rsp_len, int wait_period);
  int _fetch_result_from_rsp(const char* cmd, int cmd_size, char* rsp, int rsp_size, char** result, int*res_len);
  void _write_cmd(const char* cmd);

};
#endif
