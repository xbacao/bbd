#include "gsm_ard.h"

using namespace std;

static unsigned long int _possible_brs[8]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

Gsm_Ard::Gsm_Ard(){
  _buff_idx=0;
}

int Gsm_Ard::init_gsm_module(){
  int i,n,rsp_len;
  char* rsp;
  bool suc=false;
  #ifdef DEBUG_GSM
  Serial.println("DB: Setting baudrate");
  #endif
  #ifdef DEBUG_GSM
  Serial.println("DB: Trying  default baudrate "+String(DEFAULT_SS_BAUDRATE));
  #endif
  _ss.begin(DEFAULT_SS_BAUDRATE);
  n=send_cmd_comp_rsp("AT","OK",5000);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: COULD NOT SET DEFAULT BAUDRATE, TRYING OTHERS :");
    Serial.println(n);
    #endif
  }
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
  Serial.println("DB: CHECKING PIN STATE");
  #endif
  n=send_cmd_get_rsp("AT+CPIN=0048", 10000, &rsp, &rsp_len);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR CHECKING PIN STATE ");
    Serial.println(n);
    #endif
    return 2;
  }
  /*n=send_cmd_comp_rsp("AT+CPIN=0048", "OK",5000);
  if(n){
    return 2;
  }
  n=send_cmd_comp_rsp("AT+CGATT=1", "OK", 500);
  if(n){
    return 3;
  }*/
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
int Gsm_Ard::send_cmd_comp_rsp(const char* cmd, const char* exp_rsp, int recv_wait_period){
  int i,n, rsp_len,res_len;
  int cmd_size=strlen(cmd);
  char* rsp;
  char* result;
  for(i=0;i<CMD_MAX_TRIES;i++){
    #ifdef DEBUG_SS
    Serial.print("DB-SND: ");
    Serial.println(cmd);
    #endif
    _write_cmd(cmd);
    n=_recv_string(&rsp, &rsp_len,recv_wait_period);
    if(!n){
      n=_fetch_result_from_rsp(cmd, cmd_size, rsp, rsp_len, &result, &res_len);
      #ifdef DEBUG_SS
      Serial.print("EXP_RSP: [");
      for(unsigned int j=0;j<strlen(exp_rsp);j++){
        Serial.print((unsigned int)exp_rsp[j]);
        Serial.print(",");
      }
      Serial.println("]");

      Serial.print("RSP: [");
      for(int j=0;j<res_len;j++){
        Serial.print((unsigned int)result[j]);
        Serial.print(",");
      }
      Serial.println("]");
      #endif
      if(!n && !strncmp(result, exp_rsp, res_len)) return 0;
    }
  }
  return 1;
}

int Gsm_Ard::send_cmd_get_rsp(const char* cmd, int recv_wait_period, char** rsp_ret, int* rsp_ret_len){
  int i,n, rsp_len,res_len;
  int cmd_size=strlen(cmd);
  *rsp_ret_len=0;
  char* rsp;
  char* result;
  for(i=0;i<CMD_MAX_TRIES;i++){
    #ifdef DEBUG_SS
    Serial.print("DB-SND: ");
    Serial.println(cmd);
    #endif
    _write_cmd(cmd);
    n=_recv_string(&rsp, &rsp_len,recv_wait_period);
    if(!n){
      n=_fetch_result_from_rsp(cmd, cmd_size, rsp, rsp_len, &result, &res_len);
      #ifdef DEBUG_SS
      Serial.print("RSP: [");
      for(int j=0;j<res_len;j++){
        Serial.print((unsigned int)result[j]);
        Serial.print(",");
      }
      Serial.println("]");
      #endif
      if(!n){
        *rsp_ret=result;
        *rsp_ret_len=rsp_len;
        return 0;
      }
    }
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
  char* rsp_temp;
  *rsp_len=0;
  int nl_counter=0;

  _buff_idx=0;
  memset(_recv_buff, 0, RECV_BUFF_LEN);

  for(int i=0;i<10;i++){
    if(!_ss.available()){
      delay(wait_period/10);
    }
    else{
      while(_ss.available() && _buff_idx<RECV_BUFF_LEN && nl_counter<3){
        _recv_buff[_buff_idx++]=_ss.read();
        if(_buff_idx>1 && _recv_buff[_buff_idx-1]==NL_CHAR && _recv_buff[_buff_idx-2]==CR_CHAR){
          nl_counter++;
        }
      }
    }
  }

  if(_buff_idx==RECV_BUFF_LEN){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RECV BUFFER FULL");
    #endif
    return 1;
  }
  else if(nl_counter!=3){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, BAD RESPONSE FORMAT");
    #endif
    return 2;
  }

  *rsp_len=_buff_idx;
  rsp_temp=new char [*rsp_len];
  strncpy(rsp_temp, _recv_buff,*rsp_len);
  *rsp=rsp_temp;

  return 0;
}

int Gsm_Ard::_fetch_result_from_rsp(const char* cmd, int cmd_size, char* rsp, int rsp_size, char** result, int*res_len){
  char* res_temp;
  int i, start_idx;

  *res_len=0;
  i=0;

  for(;i<cmd_size;i++){
    if(cmd[i]!=rsp[i]){
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR CMD AND RSP DO NOT MATCH!");
      #endif
      return 1;
    }
  }

  if(rsp[i]!=CR_CHAR || rsp[i+1]!=NL_CHAR || rsp[i+2]!=CR_CHAR || rsp[i+3]!=NL_CHAR){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR NL AND CR DO NOT MATCH!");
    #endif
    return 2;
  }

  i+=4;
  start_idx=i;

  while(rsp[i]!=CR_CHAR && rsp[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=rsp_size-2){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR RSP AND CMD SIZES DO NOT MATCH!");
    #endif
    return 3;
  }

  #ifdef DEBUG_SS
  Serial.print("DB-RECV: [");
  for(int j=0;j<rsp_size;j++){
    Serial.print((unsigned int)rsp[j]);
    if(j!=rsp_size-1)  Serial.print(',');
  }
  Serial.println("]");
  #endif

  *res_len=i-start_idx;

  res_temp=new char[*res_len];

  strncpy(res_temp, rsp+start_idx, *res_len);
  *result=res_temp;

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
