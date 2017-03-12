#include "gsm_ard.h"

using namespace std;

Gsm_Ard::Gsm_Ard(){
  _buff_idx=0;
  gsm_state=GSM_OFF_STATE;
  #ifdef DEBUG_GSM
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
}

int Gsm_Ard::init_gsm_module(){
  int i,n,rsp_len;
  char* rsp;
  gsm_state=GSM_OFF_STATE;
  #ifdef DEBUG_GSM
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
  #ifdef DEBUG_GSM
  Serial.println("DB: Setting baudrate");
  #endif
  #ifdef DEBUG_GSM
  Serial.println("DB: Trying  default baudrate "+String(DEFAULT_SS_BAUDRATE));
  #endif
  _ss.begin(DEFAULT_SS_BAUDRATE);
  n=send_cmd_comp_rsp(AT,AT_OK,5000);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: COULD NOT SET DEFAULT BAUDRATE, TRYING OTHERS :");
    Serial.println(n);
    #endif
  }
  else{
    gsm_state=GSM_ON_STATE;
    #ifdef DEBUG_GSM
    Serial.println("DB: GSM_STATE=GSM_ON_STATE");
    #endif
  }
  for(i=0;i<8 && gsm_state!=GSM_ON_STATE;i++){
    #ifdef DEBUG_GSM
    Serial.println("DB: Trying baudrate "+String(_POSSIBLE_BRS[i]));
    #endif
    _ss.begin(_POSSIBLE_BRS[i]);
    n=send_cmd_comp_rsp(AT, AT_OK,5000);
    if(!n){
      gsm_state=GSM_ON_STATE;
      #ifdef DEBUG_GSM
      Serial.println("DB: GSM_STATE=GSM_ON_STATE");
      #endif
    }
  }

  if(gsm_state!=GSM_ON_STATE){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, COULD NOT CONNECT TO GSM");
    #endif
    return 1;
  }

  n=send_cmd_get_rsp(AT_CPIN_R, 5000, &rsp, &rsp_len);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR CHECKING PIN STATE ");
    Serial.println(n);
    #endif
    return 2;
  }

  if(!strncmp(rsp,AT_CPIN_READY,rsp_len)){
    gsm_state=GSM_PIN_STATE;
    #ifdef DEBUG_GSM
    Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
    #endif
  }
  else if(!strncmp(rsp,AT_CPIN_SIM_PIN,rsp_len)){
    char* temp_str=new char[0];
    strcat(temp_str, "AT+CPIN=");
    strcat(temp_str,SIM_PIN);
    n=send_cmd_comp_rsp(temp_str, AT_OK, 5000);
    if(!n){
      #ifdef DEBUG_GSM
      Serial.print("DB: ERROR SETTING SIM PIN ");
      Serial.println(n);
      #endif
      return 3;
    }
    gsm_state=GSM_PIN_STATE;
    #ifdef DEBUG_GSM
    Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
    #endif
  }
  else{
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR, PIN STATE RESPONSE=");
    Serial.println(rsp_len);
    #endif
    return 3;
  }

  n=send_cmd_get_rsp(AT_CGATT_R, 5000, &rsp, &rsp_len);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR REQUEST GPRS STATE ");
    Serial.println(n);
    #endif
    return 3;
  }

  if(!strncmp(rsp,AT_CGATT_1,rsp_len)){
    gsm_state=GSM_GPRS_STATE;
    #ifdef DEBUG_GSM
    Serial.println("DB: GSM_STATE=GSM_GPRS_STATE");
    #endif
  }
  else if(strncmp(rsp,AT_CGATT_0,rsp_len)!=0){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, REQUEST GPRS STATE BAD RESPONSE!");
    #endif
    return 4;
  }

  return 0;
}

int Gsm_Ard::attachGPRS(){
  int n, rsp_len;
  char* rsp;
  n=send_cmd_get_rsp("AT+CIFSR", 5000, &rsp, &rsp_len);
  if(strncmp(rsp, "ERROR", rsp_len)!=0){
    #ifdef DEBUG_GSM
    Serial.print("DB: IP ALREADY ASSIGNED: ");
    for(int i=0;i<rsp_len;i++){
      Serial.print(rsp[i]);
    }
    Serial.println("");
    #endif
    gsm_state=GSM_IP_STATE;
    return 0;
  }

  n=send_cmd_get_rsp("AT+CIPSERVER?", 5000, &rsp, &rsp_len);
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR SETTING CLIENT ONLY MODE ");
    Serial.println(n);
    #endif
    return 1;
  }

  Serial.print("DB: CIPSERVER: ");
  for(int i=0;i<rsp_len;i++){
    Serial.print(rsp[i]);
  }
  Serial.println("");

  //
  // n=send_cmd_comp_rsp("AT+CIPSHUT", "SHUT OK", 5000);
  // if(n){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: ERROR DEACTIVATING GPRS PDP CONTEXT ");
  //   Serial.println(n);
  //   #endif
  //   return 2;
  // }
  //
  // char* temp = new char[0];
  // strcat(temp, "AT+CSTT=\"");
  // strcat(temp, NETWORK_APN);
  // strcat(temp, "\",\"");
  // strcat(temp, NETWORK_USER);
  // strcat(temp, "\",\"");
  // strcat(temp, NETWORK_PASS);
  // strcat(temp, "\"");
  // n=send_cmd_comp_rsp(temp, "OK", 5000);
  // if(n){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: ERROR SETTING APN ");
  //   Serial.println(n);
  //   #endif
  //   return 3;
  // }
  //
  // n=send_cmd_comp_rsp("AT+CIICR","OK",10000);
  // if(n){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: ERROR SETTING APN ");
  //   Serial.println(n);
  //   #endif
  //   return 4;
  // }
  //
  // n=send_cmd_get_rsp("AT+CIFSR", 5000, &rsp, &rsp_len);
  // if(strncmp(rsp, "ERROR", rsp_len)!=0){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: IP ASSIGNED: ");
  //   for(int i=0;i<rsp_len;i++){
  //     Serial.print(rsp[i]);
  //   }
  //   Serial.println("");
  //   #endif
  //   GSM_STATE=GSM_IP_STATE;
  //   return 0;
  // }
  //
  // #ifdef DEBUG_GSM
  // Serial.print("DB: ERROR IP NOT ASSIGNED ");
  // Serial.println(n);
  // #endif
  // return 5;
  return 20;
}

int Gsm_Ard::dettachGPRS(){
  return 1;
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
  int n, rsp_len,res_len;
  int cmd_size=strlen(cmd);
  char* rsp;
  char* result;
  while(_ss.available()) _ss.read();

  #ifdef DEBUG_SS
  Serial.print("DB-SND: ");
  Serial.println(cmd);
  #endif
  _write_cmd(cmd);
  n=_recv_string(&rsp, &rsp_len,recv_wait_period);
  if(!n){
    n=_fetch_result_from_rsp(cmd, cmd_size, rsp, rsp_len, &result, &res_len);
    if(!n){
      if(!strncmp(result, exp_rsp, res_len)) return 0;
      else{
        #ifdef DEBUG_GSM
        Serial.println("DB: ERROR, RESULT AND EXPECTED DO NOT MATCH!");
        #endif
        return 1;
      }
    }
    else{
      #ifdef DEBUG_GSM
      Serial.print("DB: ERROR FETCHING RESULT ");
      Serial.println(n);
      #endif
      return 2;
    }
  }
  else{
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR RECEIVING STRING ");
    Serial.println(n);
    #endif
    return 3;
  }
}

int Gsm_Ard::send_cmd_get_rsp(const char* cmd, int recv_wait_period, char** rsp_ret, int* rsp_ret_len){
  int n, rsp_len,res_len;
  int cmd_size=strlen(cmd);
  *rsp_ret_len=0;
  char* rsp;
  char* result;
  while(_ss.available()) _ss.read();
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
      Serial.print(result[j]);
    }
    Serial.println("]");
    #endif
    if(!n){
      *rsp_ret=result;
      *rsp_ret_len=res_len;
      return 0;
    }
    else{
      return 2;
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

  #ifdef DEBUG_SS_RAW
  Serial.print("RECV: [");
  for(unsigned int j=0;j<_buff_idx;j++){
    Serial.print((unsigned int)_recv_buff[j]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif

  if(_buff_idx==RECV_BUFF_LEN){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RECV BUFFER FULL");
    #endif
    return 1;
  }
  else if(!_buff_idx){
    #ifdef DEBUG_GSM
    Serial.println("DB: RESPONSE NOT RECEIVED!");
    #endif
    return 2;
  }
  else if(nl_counter!=3){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR, BAD RESPONSE FORMAT ");
    Serial.println(nl_counter);
    #endif
    return 3;
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

  // #ifdef DEBUG_SS
  // Serial.print("DB-RECV: [");
  // for(int j=0;j<rsp_size;j++){
  //   Serial.print((unsigned int)rsp[j]);
  //   if(j!=rsp_size-1)  Serial.print(',');
  // }
  // Serial.println("]");
  // #endif

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
