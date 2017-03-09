#include "gsm_ard.h"

using namespace std;

static unsigned long int _possible_brs[8]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

Gsm_Ard::Gsm_Ard(){
  _buff_idx=0;
}

int Gsm_Ard::init_gsm_module(){
  int i,n;
  bool suc=false;
  #ifdef DEBUG_GSM
  Serial.println("DB: Setting baudrate");
  #endif
  #ifdef DEBUG_GSM
  Serial.println("DB: Trying  default baudrate "+String(DEFAULT_SS_BAUDRATE));
  #endif
  _ss.begin(DEFAULT_SS_BAUDRATE);
  n=send_cmd_comp_rsp("AT","OK",5000);
  suc=!n;
  for(i=0;i<8 && !suc;i++){
    #ifdef DEBUG_GSM
    Serial.println("DB: Trying baudrate "+String(_possible_brs[i]));
    #endif
    _ss.begin(_possible_brs[i]);
    n=send_cmd_comp_rsp("AT", "OK",5000);
    if(!n){
      suc=true;
      break;
    }
  }
  if(!suc){
    return 1;
  }
  #ifdef DEBUG_GSM
  Serial.println("DB: Setting PIN");
  #endif
  n=send_cmd_comp_rsp("AT+CPIN="+String(PIN), "OK",5000);
  if(n){
    return 2;
  }
  n=send_cmd_comp_rsp("AT+CGATT=1", "OK", 500);
  if(n){
    return 3;
  }
  return 0;
}


/*
  sends a command, fetches and returns response
*/
// int Gsm_Ard::send_cmd_get_rsp(String cmd, String& rsp, int recv_wait_period){
//   /*int i,n;
//   for(i=0;i<CMD_MAX_TRIES;i++){
//     _ss.println(cmd);
//     n=_recv_string(rsp,recv_wait_period);
//     if(!n) break;
//   }
//   return n;*/
//   return 1;
// }

/*
  sends a command, fetches response, compares with desired output
*/
int Gsm_Ard::send_cmd_comp_rsp(String cmd, const char* exp_rsp, int recv_wait_period){
  int i,n, rsp_len;
  char* rsp;
  for(i=0;i<CMD_MAX_TRIES;i++){
    _ss.println(cmd);
    n=_recv_string(&rsp, &rsp_len,recv_wait_period);
    Serial.println("RECV_STRING N:"+String(n));
    if(!n && strcmp(rsp,exp_rsp)) return 0;
  }
  return 1;
}

/*
  ret values:
  0 -> success
  1 -> buffer full before transmission ends
  2 -> transmission not fully received
*/
int Gsm_Ard::_recv_string(char** rsp, int* rsp_len, int wait_period){
  char temp;

  if(!_ss.available()){
    delay(wait_period);
  }
  if(!_ss.available()){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, NO RESPONSE RECEIVED!");
    #endif
    return 2;
  }

  _buff_idx=0;
  memset(_recv_buff, 0, RECV_BUFF_LEN);
  while(_ss.available() && _buff_idx<RECV_BUFF_LEN){
    temp=_ss.read();
    Serial.print("DB: ---");
    Serial.println((int) temp);
    _recv_buff[_buff_idx++]=temp;
  }

  if(_buff_idx==RECV_BUFF_LEN){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RECV BUFFER FULL");
    #endif
    return 1;
  }

  //debug
  *rsp_len=_buff_idx;
  strncpy(*rsp, _recv_buff,*rsp_len);
  // recv_str=String(_recv_buff);
  // Serial.println("RECVED STR:"+recv_str);

  return 0;
}

int Gsm_Ard::_fetch_result_from_rsp(char* cmd, int cmd_size, char* rsp, int rsp_size, char** result, int*res_len){
  int i, start_idx;
  //compare cmd
  for(i=0;i<cmd_size;i++){
    if(cmd[i]!=rsp[i]){
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR CMD AND RSP DO NOT MATCH!");
      #endif
      return 1;
    }
  }
  if(cmd[i]!=CR_CHAR || cmd[i+1]!=NL_CHAR || cmd[i+2]!=CR_CHAR || cmd[i+3]!=NL_CHAR){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR NL AND CR DO NOT MATCH!");
    #endif
    return 2;
  }

  start_idx=i;
  i+=4;
  while(rsp[i]!=CR_CHAR && rsp[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=rsp_size-2){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR RSP AND CMD SIZES DO NOT MATCH!");
    #endif
    return 3;
  }

  *res_len=i-start_idx;
  strncpy(*result, rsp+start_idx, *res_len);

  return 0;
}
