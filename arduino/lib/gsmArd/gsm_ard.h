#ifndef _GSM_ARD_H_
#define _GSM_ARD_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

#define _GSM_RXPIN_ 7
#define _GSM_TXPIN_ 8
#define DEFAULT_SS_BAUDRATE 2400
#define CMD_MAX_TRIES 1
#define DEFAULT_RECV_WAIT_PERIOD  100    //ms
#define RECV_MAX_TRIES  1

#define RECV_BUFF_LEN 64
#define PIN "0048"

#define CR_CHAR   0x0d
#define NL_CHAR   0x0a

#define DEBUG_GSM
#define DEBUG_SS

class Gsm_Ard{
  char _recv_buff[RECV_BUFF_LEN];
  int _buff_idx;
  SoftwareSerial _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
public:
  Gsm_Ard();
  int init_gsm_module();
  int send_cmd_get_rsp(const char* cmd, int recv_wait_period, char** rsp_ret, int* rsp_ret_len);
  int send_cmd_comp_rsp(const char* cmd, const char* exp_rsp, int recv_wait_period);
private:
  int _recv_string(char** rsp, int* rsp_len, int wait_period);
  int _fetch_result_from_rsp(const char* cmd, int cmd_size, char* rsp, int rsp_size, char** result, int*res_len);
  void _write_cmd(const char* cmd);

};
#endif
