#ifndef _GSM_ARD_H_
#define _GSM_ARD_H_

#include <SoftwareSerial.h>
// #include "MemoryFree.h"

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

// #define DEBUG_GSM
#define DEBUG_SS
#define DEBUG_SOCKET

#define DEBUG_STATES

enum GSM_STATE{GSM_OFF_STATE, GSM_ON_STATE, GSM_PIN_STATE, GSM_IP_STATE, GSM_TCP_STATE};
enum BUFF_STATE{BUFF_READY, BUFF_USED};

class Gsm_Ard{
  char _recv_buff[RECV_BUFF_LEN];
  char _rsp_buff[RSP_BUFF_LEN];
  char _sock_buff[SOCK_BUFF_LEN];
  unsigned int _recv_buff_idx;
  unsigned int _rsp_buff_idx;
  unsigned int _sock_buff_idx;
  GSM_STATE _gsm_state;
  BUFF_STATE _recv_buff_state;
  BUFF_STATE _rsp_buff_state;
  BUFF_STATE _sock_buff_state;
  SoftwareSerial _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
public:
  Gsm_Ard();
  int init_gsm_module();
  int attachGPRS();
  int dettachGPRS();
  int send_socket_msg(char* data, unsigned int data_len, unsigned int* rsp_len);
  int get_socket_rsp(char** data);
  GSM_STATE get_gsm_state();
private:
  int _send_cmd_comp_rsp(const char* cmd, const uint16_t cmd_size, const char* exp_rsp, const uint16_t exp_rsp_size, int recv_wait_period);
  int _send_cmd_comp_several_rsp(const char* cmd, const uint16_t cmd_size, const char* exp_rsp1, const uint16_t exp_rsp1_size,
    const char* exp_rsp2, const uint16_t exp_rsp2_size, int recv_wait_period);
  int _no_cmd_comp_rsp(const char* exp_rsp, const uint16_t exp_rsp_size, int recv_wait_period);

  /* tcp */
  int _connect_tcp_socket();
  int _send_tcp_data(char* data, unsigned int data_len);
  int _recv_tcp_data(unsigned int* data_len);
  int _disconnect_tcp_socket();

  void _change_gsm_state(GSM_STATE new_state);

  void _clear_recv_buff();
  void _clear_rsp_buff();
  void _clear_sock_buff();
  int _recv_string(int wait_period, int max_nl=3);
  int _recv_socket(int wait_period);
  int _fetch_rsp_from_recv(const char* cmd, unsigned int cmd_size, unsigned int* rsp_len);
  int _fetch_rsp_wo_cmd(unsigned int* rsp_len);
  int _get_rsp(char** rsp);
  void _write_cmd(const char* cmd);

};
#endif
