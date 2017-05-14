#include "gsm_ard.h"
#include "gsm_cmds.h"
#include <avr/pgmspace.h>
#include <Arduino.h>

using namespace std;


const unsigned long int _POSSIBLE_BRS[8] ={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

Gsm_Ard::Gsm_Ard(){
  _gsm_state=GSM_OFF_STATE;
  _clear_recv_buff();
  _clear_rsp_buff();
  _clear_sock_buff();
  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
}

void Gsm_Ard::_change_gsm_state(GSM_STATE new_state){
  _gsm_state=new_state;
  #ifdef DEBUG_STATES
  switch(_gsm_state){
    default:
      break;
    case GSM_OFF_STATE:
      Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
      break;
    case GSM_ON_STATE:
      Serial.println("DB: GSM_STATE=GSM_ON_STATE");
      break;
    case GSM_PIN_STATE:
      Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
      break;
    case GSM_IP_STATE:
      Serial.println("DB: GSM_STATE=GSM_IP_STATE");
      break;
    case GSM_TCP_STATE:
      Serial.println("DB: GSM_STATE=GSM_TCP_STATE");
      break;
  }
  #endif
}

int Gsm_Ard::init_gsm_module(){
  int i,n;
  _change_gsm_state(GSM_OFF_STATE);
  _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
  _ss.begin(DEFAULT_SS_BAUDRATE);

  n=_send_cmd_comp_rsp(AT, AT_LEN, AT_OK, AT_OK_LEN, 5000);
  if(!n){
    _change_gsm_state(GSM_ON_STATE);
  }
  for(i=0;i<8 && _gsm_state!=GSM_ON_STATE;i++){
    _ss.begin(_POSSIBLE_BRS[i]);
    n=_send_cmd_comp_rsp(AT, AT_LEN, AT_OK, AT_OK_LEN, 5000);
    if(!n){
      _change_gsm_state(GSM_ON_STATE);
    }
  }

  if(_gsm_state!=GSM_ON_STATE){
    return 1;
  }

  n=_send_cmd_comp_several_rsp(AT_CPIN_R, AT_CPIN_R_LEN, AT_CPIN_READY, AT_CPIN_READY_LEN, AT_CPIN_SIM_PIN, AT_CPIN_SIM_PIN_LEN, 5000);
  switch(n){
    default:
      return 2+10*n;
      break;
    case 0:
      break;
    case 1:
      n=_send_cmd_comp_rsp(AT_CPIN_SET, AT_CPIN_SET_LEN, AT_OK, AT_OK_LEN, 5000);
      if(n){
        return 3+10*n;
      }
      break;
  }

  n=_send_cmd_comp_several_rsp(AT_CIURC_R, AT_CIURC_R_LEN, AT_CIURC_0, AT_CIURC_0_LEN, AT_CIURC_1, AT_CIURC_1_LEN, 5000);
  switch(n){
    default:
      return 4+10*n;
      break;
    case 0:
      n=_send_cmd_comp_rsp(AT_CIURC_SET, AT_CIURC_SET_LEN, AT_OK, AT_OK_LEN, 5000);
      if(n){
        return 5+10*n;
      }
      break;
    case 1:
      break;
  }

  _change_gsm_state(GSM_PIN_STATE);
  return 0;
}

int Gsm_Ard::attachGPRS(){
  int n;

  if(_gsm_state!=GSM_PIN_STATE){
    return 1;
  }

  n=_send_cmd_comp_several_rsp(AT_CGATT_R, AT_CGATT_R_LEN, AT_CGATT_0, AT_CGATT_0_LEN, AT_CGATT_1, AT_CGATT_1_LEN, 5000);
  switch(n){
    default:
      return 2+n*10;
      break;
    case 0:
      n=_send_cmd_comp_rsp(AT_CGATT_SET_ON, AT_CGATT_SET_ON_LEN, AT_OK, AT_OK_LEN, 10000);
      if(n){
        return 3+n*10;
      }
      break;
    case 1:
      break;
  }

  n=_send_cmd_comp_several_rsp(AT_CIPMODE_R, AT_CIPMODE_R_LEN, AT_CIPMODE_0, AT_CIPMODE_0_LEN, AT_CIPMODE_1, AT_CIPMODE_1_LEN, 5000);
  switch(n){
    default:
      return 4+n*10;
      break;
    case 0:
      break;
    case 1:
      n=_send_cmd_comp_rsp(AT_CIPMODE, AT_CIPMODE_LEN, AT_OK, AT_OK_LEN, 5000);
      if(n){
        return 5+n*10;
      }
      break;
  }

  n=_send_cmd_comp_rsp(AT_CGDCONT, AT_CGDCONT_LEN, AT_OK, AT_OK_LEN, 5000);
  if(n){
    return 6+n*10;
  }

  n=_send_cmd_comp_rsp(AT_CIFSR, AT_CIFSR_LEN, AT_ERROR, AT_ERROR_LEN, 5000);
  switch(n){
    default:
      return 7+n*10;
      break;
    case 1:
      _change_gsm_state(GSM_IP_STATE);
      return 0;
      break;
    case 0:
      break;
  }

  n=_send_cmd_comp_rsp(AT_CIPSERVER_R, AT_CIPSERVER_R_LEN, AT_CIPSERVER_RESP, AT_CIPSERVER_RESP_LEN, 5000);
  switch(n){
    default:
      return 8+n*10;
      break;
    case 1:
      n=_send_cmd_comp_rsp(AT_CIPSERVER_SET, AT_CIPSERVER_SET_LEN, AT_OK, AT_OK_LEN, 5000);
      if(!n){
        return 9+n*10;
      }
      break;
    case 0:
      break;
  }

  n=_send_cmd_comp_rsp(AT_CSTT, AT_CSTT_LEN, AT_OK, AT_OK_LEN, 5000);
  if(n){
    return 10;
  }

  n=_send_cmd_comp_rsp(AT_CIICR, AT_CIICR_LEN, AT_OK, AT_OK_LEN, 5000);
  if(n){
    return 11;
  }

  n=_send_cmd_comp_rsp(AT_CIFSR, AT_CIFSR_LEN, AT_ERROR, AT_ERROR_LEN, 5000);
  if(n!=1){
    return 12;
  }

  _change_gsm_state(GSM_IP_STATE);
  return 0;
}

int Gsm_Ard::dettachGPRS(){
  int n;
  if(_gsm_state!=GSM_IP_STATE){
    return 1;
  }

  n=_send_cmd_comp_rsp(AT_CIPSHUT, AT_CIPSHUT_LEN, AT_SHUT_OK, AT_SHUT_OK_LEN, 10000);
  if(n){
    return 2+10*n;
  }

  n=_send_cmd_comp_several_rsp(AT_CGATT_R, AT_CGATT_R_LEN, AT_CGATT_0, AT_CGATT_0_LEN, AT_CGATT_1, AT_CGATT_1_LEN, 5000);
  switch(n){
    default:
      return 3+10*n;
      break;
    case 0:
      break;
    case 1:
      n=_send_cmd_comp_rsp(AT_CGATT_SET_OFF, AT_CGATT_SET_OFF_LEN, AT_OK, AT_OK_LEN, 10000);
      if(n){
        return 4+10*n;
      }
      break;
  }
  _change_gsm_state(GSM_PIN_STATE);
  return 0;
}

int Gsm_Ard::_connect_tcp_socket(){
  int n;
  if(_gsm_state!=GSM_IP_STATE){
    return 1;
  }

  n=_send_cmd_comp_rsp(AT_CIPSHUT, AT_CIPSHUT_LEN, AT_SHUT_OK, AT_SHUT_OK_LEN, 10000);
  if(n){
    return 2+10*n;
  }

  n=_send_cmd_comp_several_rsp(AT_CIPMUX_R, AT_CIPMUX_R_LEN, AT_CIPMUX_0, AT_CIPMUX_0_LEN, AT_CIPMUX_1, AT_CIPMUX_1_LEN, 5000);
  switch(n){
    default:
      return 3+10*n;
      break;
    case 0:
      break;
    case 1:
      n=_send_cmd_comp_rsp(AT_CIPMUX_SET, AT_CIPMUX_SET_LEN, AT_OK, AT_OK_LEN, 5000);
      if(n){
        return 4+10*n;
      }
      break;
  }

  n=_send_cmd_comp_rsp(AT_CIPSTART_SET, AT_CIPSTART_SET_LEN, AT_OK, AT_OK_LEN, 5000);
  if(n){
    return 5+10*n;
  }

  n=_no_cmd_comp_rsp(AT_CONNECT_OK, AT_CONNECT_OK_LEN, 10000);
  if(n){
    return 6+10*n;
  }

  _change_gsm_state(GSM_TCP_STATE);

  return 0;
}

int Gsm_Ard::_disconnect_tcp_socket(){
  int n;
  if(_gsm_state!=GSM_TCP_STATE){
    return 1;
  }

  n=_send_cmd_comp_rsp(AT_CIPSTATUS, AT_CIPSTATUS_LEN , AT_OK, AT_OK_LEN, 5000);
  if(n){
    return 2+10*n;
  }

  n=_no_cmd_comp_rsp(AT_CONNECT_OK, AT_CONNECT_OK_LEN, 10000);
  if(n){
    n=_send_cmd_comp_rsp(AT_CIPCLOSE, AT_CIPCLOSE_LEN, AT_CIPCLOSE_OK, AT_CIPCLOSE_OK_LEN, 5000);
    if(n){
      return 3+10*n;
    }
  }

  _change_gsm_state(GSM_IP_STATE);
  return 0;
}

int Gsm_Ard::_send_tcp_data(char* data, unsigned int data_len){
  int n,err=0;
  char* cmd;
  char temp;

  if(_gsm_state!=GSM_TCP_STATE){
    return 1;
  }

  bool prompt_received=false;
  cmd=new char[AT_CIPSEND_LEN];

  strcpy_P(cmd, AT_CIPSEND);
  _write_cmd(cmd);

  //wait for prompt
  for(int i=0;i<100 && !prompt_received;i++){
    if(!_ss.available()){
      delay(100);
    }
    else{
      temp=_ss.read();
      if(temp==AT_CIPSEND_PROMPT){
        prompt_received=true;
      }
    }
  }
  if(!prompt_received){
    err=2;
    goto exit_clean;
  }

  #ifdef DEBUG_SOCKET
  Serial.print("SOCK_SND: ");
  #endif
  for(unsigned int i=0;i<data_len;i++){
    _ss.write(data[i]);
    #ifdef DEBUG_SOCKET
    Serial.print((uint8_t)data[i]);
    Serial.print(' ');
    #endif
  }
  #ifdef DEBUG_SOCKET
  Serial.println();
  #endif
  _ss.write(SUB_CHAR);

  n=_no_cmd_comp_rsp(AT_CIPSEND_OK, AT_CIPSEND_OK_LEN, 10000);
  if(n){
    err=3+n*10;
    goto exit_clean;
  }

  err=0;
  goto exit_clean;

exit_clean:
  delete[] cmd;
  return err;
}

int Gsm_Ard::_recv_tcp_data(unsigned int* data_len){
  int n;
  if(_sock_buff_state==BUFF_USED){
    return 1;
  }

  n=_recv_socket(10000);
  if(n){
    return 2+10*n;
  }

  _sock_buff_state=BUFF_USED;
  memcpy(_sock_buff, _recv_buff, _recv_buff_idx);
  _sock_buff_idx=_recv_buff_idx;

  _clear_recv_buff();
  *data_len=_sock_buff_idx;
  return 0;
}


/*
  sends a command, fetches response, compares with desired output
*/
int Gsm_Ard::_send_cmd_comp_rsp(const char* cmd, const uint16_t cmd_size, const char* exp_rsp, const uint16_t exp_rsp_size, int recv_wait_period){
  int n, error=0;
  char* rsp;
  unsigned int rsp_len;

  char* cmd_buffer = new char[cmd_size];
  char* exp_rsp_buffer = new char[exp_rsp_size];
  strcpy_P(cmd_buffer, cmd);
  strcpy_P(exp_rsp_buffer, exp_rsp);

  while(_ss.available()) _ss.read();

  #ifdef DEBUG_SS
  Serial.print("DB-SND: [");
  for(uint16_t i=0;i<cmd_size;i++){
    Serial.print((uint8_t) cmd_buffer[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif
  _write_cmd(cmd_buffer);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd_buffer, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
        error=2+10*n;
        goto leave_func;
      }
      if(!strncmp(rsp, exp_rsp_buffer, rsp_len)){
        error=0;
        goto leave_func;
      }
      else{
        error=1;
        goto leave_func;
      }
    }
    else{
      error=3+10*n;
      goto leave_func;
    }
  }
  else{
    error=4+10*n;
    goto leave_func;
  }

leave_func:
  delete[] cmd_buffer;
  delete[] exp_rsp_buffer;
  delete[] rsp;
  return error;
}

int Gsm_Ard::_send_cmd_comp_several_rsp(const char* cmd, const uint16_t cmd_size, const char* exp_rsp1, const uint16_t exp_rsp1_size,
  const char* exp_rsp2, const uint16_t exp_rsp2_size, int recv_wait_period){
  int n, error=0;
  char* rsp;
  unsigned int rsp_len;

  char* cmd_buffer = new char[cmd_size];
  char* exp_rsp1_buffer = new char[exp_rsp1_size];
  char* exp_rsp2_buffer = new char[exp_rsp2_size];
  strcpy_P(cmd_buffer, cmd);
  strcpy_P(exp_rsp1_buffer, exp_rsp1);
  strcpy_P(exp_rsp2_buffer, exp_rsp2);

  while(_ss.available()) _ss.read();
  #ifdef DEBUG_SS
  Serial.print("DB-SND: [");
  for(uint16_t i=0;i<cmd_size;i++){
    Serial.print((uint8_t) cmd_buffer[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif
  _write_cmd(cmd_buffer);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd_buffer, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
        error=3+n*10;
        goto leave_func;
      }
      if(!strncmp(exp_rsp1_buffer, rsp, exp_rsp1_size)){
        error=0;
        goto leave_func;
      }
      if(!strncmp(exp_rsp2_buffer, rsp, exp_rsp2_size)){
        error=1;
        goto leave_func;
      }
    }
    else{
      error=4+n*10;
      goto leave_func;
    }
  }
  error=2;
  goto leave_func;
leave_func:
  delete[] cmd_buffer;
  delete[] exp_rsp1_buffer;
  delete[] exp_rsp2_buffer;
  delete[] rsp;
  return error;
}

int Gsm_Ard::_no_cmd_comp_rsp(const char* exp_rsp, const uint16_t exp_rsp_size, int recv_wait_period){
  int n, ret;
  unsigned int rsp_len;

  n=_recv_string(recv_wait_period,2);
  if(n){
    return 1+n*10;
  }

  char* rsp;
  n=_fetch_rsp_wo_cmd(&rsp_len);
  if(n){
    return 2+n*10;
  }

  rsp=new char[rsp_len];
  n=_get_rsp(&rsp);
  if(n){
    return 3+n*10;
  }

  char* exp_rsp_buffer = new char[exp_rsp_size];
  strcpy_P(exp_rsp_buffer, exp_rsp);

  if(!strncmp(rsp, exp_rsp_buffer, rsp_len)) ret=0;
  else ret=4;

  delete[] exp_rsp_buffer;
  delete[] rsp;
  return ret;
}

/*
  ret values:
  0 -> success
  1 -> buffer full before transmission ends
  2 -> transmission not fully received
*/
int Gsm_Ard::_recv_string(int wait_period, int max_nl){
  if(_recv_buff_state!=BUFF_READY){
    return 1;
  }
  int nl_counter=0;

  for(int i=0;i<10 && nl_counter<max_nl;i++){
    if(!_ss.available()){
      delay(wait_period/10);
    }
    else{
      while(_ss.available() && _recv_buff_idx<RECV_BUFF_LEN && nl_counter<max_nl){
        _recv_buff[_recv_buff_idx++]=_ss.read();
        if(_recv_buff_idx>1 && _recv_buff[_recv_buff_idx-1]==NL_CHAR && _recv_buff[_recv_buff_idx-2]==CR_CHAR){
          nl_counter++;
        }
      }
    }
  }

  #ifdef DEBUG_SS
  Serial.print("RECV: [");
  for(unsigned int j=0;j<_recv_buff_idx;j++){
    Serial.print((uint8_t)_recv_buff[j]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif

  if(_recv_buff_idx==RECV_BUFF_LEN){
    _clear_recv_buff();
    return 2;
  }
  else if(!_recv_buff_idx){
    _clear_recv_buff();
    return 3;
  }
  else if(nl_counter!=max_nl){
    _clear_recv_buff();
    return 4;
  }
  _recv_buff_state=BUFF_USED;
  return 0;
}

int Gsm_Ard::_recv_socket(int wait_period){
  if(_recv_buff_state!=BUFF_READY){
    return 1;
  }
  if(_gsm_state!=GSM_TCP_STATE){
    return 2;
  }
  bool size_set=false;
  bool ready_for_confirm=false;
  bool done=false;
  uint8_t trans_size=0;
  uint8_t data_read=0;

  for(int i=0;i<10 && !done;i++){
    if(!_ss.available()){
      delay(wait_period/10);
    }
    else{
      if(ready_for_confirm){
        if(_ss.read()!=0xff){
          #ifdef DEBUG_SOCKET
          Serial.print("CONFIRM FAILED SOCK_RECV: [");
          for(unsigned int j=0;j<_recv_buff_idx;j++){
            Serial.print((uint8_t)_recv_buff[j]);
            Serial.print(" ");
          }
          Serial.print("] N:");
          Serial.println(trans_size);
          #endif
          return 3;
        }
        else{
          done=true;
        }
      }
      else{
        if(!size_set){
          trans_size=(uint8_t) _ss.read();
          size_set=true;
        }
        while(_ss.available() && _recv_buff_idx<RECV_BUFF_LEN && data_read<trans_size){
          _recv_buff[_recv_buff_idx++]=_ss.read();
          data_read++;
        }
        if(data_read==trans_size){
          ready_for_confirm=true;
        }
      }
    }
  }

  #ifdef DEBUG_SOCKET
  Serial.print("SOCK_RECV: [");
  for(unsigned int j=0;j<_recv_buff_idx;j++){
    Serial.print((uint8_t)_recv_buff[j]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif

  if(_recv_buff_idx==RECV_BUFF_LEN){
    _clear_recv_buff();
    return 4;
  }
  else if(!_recv_buff_idx){
    _clear_recv_buff();
    return 5;
  }
  else if(!done){
    _clear_recv_buff();
    return 6;
  }
  _recv_buff_state=BUFF_USED;
  return 0;
}

void Gsm_Ard::_clear_recv_buff(){
  _recv_buff_idx=0;
  memset(_recv_buff, 0, RECV_BUFF_LEN);
  _recv_buff_state=BUFF_READY;
}

void Gsm_Ard::_clear_rsp_buff(){
  _rsp_buff_idx=0;
  memset(_rsp_buff, 0, RSP_BUFF_LEN);
  _rsp_buff_state=BUFF_READY;
}

void Gsm_Ard::_clear_sock_buff(){
  _sock_buff_idx=0;
  memset(_sock_buff, 0, SOCK_BUFF_LEN);
  _sock_buff_state=BUFF_READY;
}

int Gsm_Ard::_fetch_rsp_from_recv(const char* cmd, unsigned int cmd_size, unsigned int* rsp_len){
  unsigned int i, start_idx;
  if(_rsp_buff_state!=BUFF_READY || _recv_buff_state!=BUFF_USED){
    Serial.print("THIS ");
    if(_rsp_buff_state!=BUFF_READY){
      Serial.print("1");
    }
    if(_recv_buff_state!=BUFF_USED){
      Serial.print("2");
    }
    // Serial.print(_rsp_buff_state);
    // Serial.println(_recv_buff_state);

    return 1;
  }
  for(i=0;i<cmd_size;i++){
    if(cmd[i]!=_recv_buff[i]){
      _clear_recv_buff();
      return 2;
    }
  }

  if(_recv_buff[i]!=CR_CHAR || _recv_buff[i+1]!=NL_CHAR || _recv_buff[i+2]!=CR_CHAR || _recv_buff[i+3]!=NL_CHAR){
    _clear_recv_buff();
    return 3;
  }

  i+=4;
  start_idx=i;

  while(_recv_buff[i]!=CR_CHAR && _recv_buff[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=_recv_buff_idx-2){
    _clear_recv_buff();
    return 4;
  }

  _rsp_buff_idx=i-start_idx;
  _rsp_buff_state=BUFF_USED;

  strncpy(_rsp_buff, _recv_buff+start_idx, _rsp_buff_idx);

  _clear_recv_buff();

  *rsp_len=_rsp_buff_idx;
  return 0;
}

int Gsm_Ard::_fetch_rsp_wo_cmd(unsigned int* rsp_len){
  unsigned int i, start_idx;
  if(_rsp_buff_state!=BUFF_READY || _recv_buff_state!=BUFF_USED){
    return 1;
  }
  start_idx=0;
  while(_recv_buff[start_idx]!=CR_CHAR || _recv_buff[start_idx+1]!=NL_CHAR){
    start_idx++;
  }

  start_idx+=2;
  i=start_idx;

  while(_recv_buff[i]!=CR_CHAR && _recv_buff[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=_recv_buff_idx-2){
    return 3;
  }

  _rsp_buff_idx=i-start_idx;
  _rsp_buff_state=BUFF_USED;

  strncpy(_rsp_buff, _recv_buff+start_idx, _rsp_buff_idx);

  _clear_recv_buff();

  *rsp_len=_rsp_buff_idx;
  return 0;
}


int Gsm_Ard::_get_rsp(char** rsp){
  if(_rsp_buff_state!=BUFF_USED){
    return 1;
  }
  strncpy(*rsp, _rsp_buff, _rsp_buff_idx);
  _clear_rsp_buff();
  return 0;
}

void Gsm_Ard::_write_cmd(const char* cmd){
  unsigned int cmd_len=strlen(cmd);
  for(unsigned int i=0;i<cmd_len;i++){
    _ss.write(cmd[i]);
  }
  _ss.write(CR_CHAR);
  _ss.write(NL_CHAR);
}

GSM_STATE Gsm_Ard::get_gsm_state(){
  return _gsm_state;
}

int Gsm_Ard::send_socket_msg(char* data, unsigned int data_len, unsigned int* rsp_len){
  int n;
  n=_connect_tcp_socket();
  if(n){
    return 1+10*n;
  }

  n=_send_tcp_data(data, data_len);
  if(n){
    return 2+10*n;
  }

  n=_recv_tcp_data(rsp_len);
  if(n){
    _disconnect_tcp_socket();
    return 3+10*n;
  }

  n=_disconnect_tcp_socket();
  if(n){
    return 4+10*n;
  }

  return 0;
}

int Gsm_Ard::get_socket_rsp(char** data){
  if(_sock_buff_state!=BUFF_USED){
    return 1;
  }

  Serial.print("SOCK BUFF=");
  for(unsigned int i=0;i<_sock_buff_idx;i++){
    Serial.print((unsigned int) _sock_buff[i]);
    Serial.print(' ');
  }
  Serial.println();

  memcpy(*data, _sock_buff, _sock_buff_idx);
  _clear_sock_buff();

  return 0;
}
