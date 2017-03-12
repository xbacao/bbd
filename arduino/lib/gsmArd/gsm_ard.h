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
#define RSP_BUFF_LEN 128
#define CR_CHAR   0x0d
#define NL_CHAR   0x0a

#define DEBUG_GSM
#define DEBUG_SS
// #define DEBUG_SS_RAW


enum GSM_STATE{GSM_OFF_STATE, GSM_ON_STATE, GSM_PIN_STATE, GSM_GPRS_STATE, GSM_IP_STATE};
enum BUFF_STATE{BUFF_READY, BUFF_USED};

class Gsm_Ard{
  char _recv_buff[RECV_BUFF_LEN];
  char _rsp_buff[RSP_BUFF_LEN];
  unsigned int _recv_buff_idx;
  unsigned int _rsp_buff_idx;
  GSM_STATE _gsm_state;
  BUFF_STATE _recv_buff_state;
  BUFF_STATE _rsp_buff_state;
  SoftwareSerial _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
public:
  Gsm_Ard();
  int init_gsm_module();
  int attachGPRS();
  int dettachGPRS();
private:
  int send_cmd_comp_rsp(const char* cmd, const char* exp_rsp, int recv_wait_period);
  int send_cmd_comp_several_rsp(const char* cmd, char** exp_rsps, unsigned int exp_rsps_len, int recv_wait_period);

  void _clear_recv_buff();
  void _clear_rsp_buff();
  int _recv_string(int wait_period);
  // int _get_recved_string(char** recved_str);
  int _fetch_rsp_from_recv(const char* cmd, unsigned int cmd_size, unsigned int* rsp_len);
  int _get_rsp(char** rsp);
  void _write_cmd(const char* cmd);

};
#endif
