#include "gsm_ard.h"

using namespace std;

/*******AT COMMANDS******/
const char AT[] PROGMEM="AT";
const char AT_CPIN_R[] PROGMEM="AT+CPIN?";
const char AT_CPIN_READY[] PROGMEM="+CPIN: READY";
const char AT_CPIN_SIM_PIN[] PROGMEM="+CPIN: SIM PIN";
const char AT_CPIN_SET[] PROGMEM="AT+CPIN=0048";
// const char* const AT_CPIN_RESPONSES_TABLE[] PROGMEM={AT_CPIN_READY, AT_CPIN_SIM_PIN};


const PROGMEM char AT_CGATT_R[]="AT+CGATT?";
const PROGMEM char AT_CGATT_0[]="+CGATT: 0";
const PROGMEM char AT_CGATT_1[]="+CGATT: 1";

const PROGMEM char AT_OK[] ="OK";

/***************************/

const unsigned long int _POSSIBLE_BRS[8] ={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

const PROGMEM char NETWORK_APN[] ="internet.vodafone.pt";
const PROGMEM char NETWORK_USER[]="";
const PROGMEM char NETWORK_PASS[]="";

Gsm_Ard::Gsm_Ard(){
  _gsm_state=GSM_OFF_STATE;
  _clear_recv_buff();
  _clear_rsp_buff();
  #ifdef DEBUG_GSM
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
}

int Gsm_Ard::init_gsm_module(){
  int i,n;
  _gsm_state=GSM_OFF_STATE;
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
  {
    char at_cmd_buffer[3];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: COULD NOT SET DEFAULT BAUDRATE, TRYING OTHERS :");
    Serial.println(n);
    #endif
  }
  else{
    _gsm_state=GSM_ON_STATE;
    #ifdef DEBUG_GSM
    Serial.println("DB: GSM_STATE=GSM_ON_STATE");
    #endif
  }
  for(i=0;i<8 && _gsm_state!=GSM_ON_STATE;i++){
    #ifdef DEBUG_GSM
    Serial.println("DB: Trying baudrate "+String(_POSSIBLE_BRS[i]));
    #endif
    _ss.begin(_POSSIBLE_BRS[i]);
    {
      char at_cmd_buffer[3];
      char at_rsp_buffer[6];
      strcpy_P(at_cmd_buffer, AT);
      strcpy_P(at_rsp_buffer, AT_OK);
      n=send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
    }
    if(!n){
      _gsm_state=GSM_ON_STATE;
      #ifdef DEBUG_GSM
      Serial.println("DB: GSM_STATE=GSM_ON_STATE");
      #endif
    }
  }

  if(_gsm_state!=GSM_ON_STATE){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, COULD NOT CONNECT TO GSM");
    #endif
    return 1;
  }

  {
    char at_cmd_buffer[9];
    strcpy_P(at_cmd_buffer, AT_CPIN_R);

    char rsp1[13];
    char rsp2[15];
    strcpy_P(rsp1, AT_CPIN_READY);
    strcpy_P(rsp2, AT_CPIN_SIM_PIN);
    char* rsps[]={rsp1, rsp2};
    n=send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      #ifdef DEBUG_GSM
      Serial.print("DB: ERROR CHECKING PIN STATE ");
      Serial.println(n);
      #endif
      return 2;
      break;
    case 0:
      _gsm_state=GSM_PIN_STATE;
      #ifdef DEBUG_GSM
      Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
      #endif
      break;
    case 1:
      {
        char at_cmd_buffer[13];
        char at_rsp_buffer[6];
        strcpy_P(at_cmd_buffer, AT_CPIN_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        #ifdef DEBUG_GSM
        Serial.print("DB: ERROR SETTING SIM PIN ");
        Serial.println(n);
        #endif
        return 3;
      }
      _gsm_state=GSM_PIN_STATE;
      #ifdef DEBUG_GSM
      Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
      #endif
      break;
  }

  {
    char at_cmd_buffer[10];
    strcpy_P(at_cmd_buffer, AT_CGATT_R);

    char rsp1[10];
    char rsp2[10];
    strcpy_P(rsp1, AT_CGATT_0);
    strcpy_P(rsp2, AT_CGATT_1);
    char* rsps[]={rsp1, rsp2};
    n=send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  // n=send_cmd_get_rsp(AT_CGATT_R, 5000, &rsp, &rsp_len);
  // if(n<0){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: ERROR REQUEST GPRS STATE ");
  //   Serial.println(n);
  //   #endif
  //   return 3;
  // }

  switch(n){
    default:
      #ifdef DEBUG_GSM
      Serial.print("DB: ERROR REQUEST GPRS STATE ");
      Serial.println(n);
      #endif
      return 4;
      break;
    case 0:
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR, REQUEST GPRS STATE BAD RESPONSE!");
      #endif
      return 5;
      break;
    case 1:
      _gsm_state=GSM_GPRS_STATE;
      #ifdef DEBUG_GSM
      Serial.println("DB: GSM_STATE=GSM_GPRS_STATE");
      #endif
      break;
  }
  return 0;
}

int Gsm_Ard::attachGPRS(){
  // int n, rsp_len;
  // char* rsp;
  // n=send_cmd_get_rsp("AT+CIFSR", 5000, &rsp, &rsp_len);
  // if(strncmp(rsp, "ERROR", rsp_len)!=0){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: IP ALREADY ASSIGNED: ");
  //   for(int i=0;i<rsp_len;i++){
  //     Serial.print(rsp[i]);
  //   }
  //   Serial.println("");
  //   #endif
  //   _gsm_state=GSM_IP_STATE;
  //   return 0;
  // }
  //
  // n=send_cmd_get_rsp("AT+CIPSERVER?", 5000, &rsp, &rsp_len);
  // if(n){
  //   #ifdef DEBUG_GSM
  //   Serial.print("DB: ERROR SETTING CLIENT ONLY MODE ");
  //   Serial.println(n);
  //   #endif
  //   return 1;
  // }
  //
  // Serial.print("DB: CIPSERVER: ");
  // for(int i=0;i<rsp_len;i++){
  //   Serial.print(rsp[i]);
  // }
  // Serial.println("");

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
  int n;
  int cmd_size=strlen(cmd);
  char* rsp;
  unsigned int rsp_len;

  while(_ss.available()) _ss.read();

  #ifdef DEBUG_SS
  Serial.print("DB-SND: ");
  Serial.println(cmd);
  #endif
  _write_cmd(cmd);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
        // #ifdef DEBUG_GSM
        // Serial.println("DB: ERROR GETTING RESPONSE");
        // #endif
        return 2;
      }
      if(!strncmp(rsp, exp_rsp, rsp_len)) return 0;
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
      return 3;
    }
  }
  else{
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR RECEIVING STRING ");
    Serial.println(n);
    #endif
    return 4;
  }
}

int Gsm_Ard::send_cmd_comp_several_rsp(const char* cmd, char** exp_rsps, unsigned int exp_rsps_len, int recv_wait_period){
  int n;
  int cmd_size=strlen(cmd);
  char* rsp;
  unsigned int rsp_len;
  while(_ss.available()) _ss.read();
  #ifdef DEBUG_SS
  Serial.print("DB-SND: ");
  Serial.println(cmd);
  #endif
  _write_cmd(cmd);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
        // #ifdef DEBUG_GSM
        // Serial.println("DB: ERROR GETTING RESPONSE");
        // #endif
        return -2;
      }
      for(unsigned int j=0;j<exp_rsps_len;j++){
        if(!strncmp(exp_rsps[j], rsp, strlen(exp_rsps[j]))) return j;
      }
    }
    else{
      return -3;
    }
  }
  return -1;
}

/*
  ret values:
  0 -> success
  1 -> buffer full before transmission ends
  2 -> transmission not fully received
*/
int Gsm_Ard::_recv_string(int wait_period){
  // char* rsp_temp;
  if(_recv_buff_state!=BUFF_READY){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, BUFFER NOT READY!");
    #endif
    return 1;
  }
  int nl_counter=0;

  for(int i=0;i<10;i++){
    if(!_ss.available()){
      delay(wait_period/10);
    }
    else{
      while(_ss.available() && _recv_buff_idx<RECV_BUFF_LEN && nl_counter<3){
        _recv_buff[_recv_buff_idx++]=_ss.read();
        if(_recv_buff_idx>1 && _recv_buff[_recv_buff_idx-1]==NL_CHAR && _recv_buff[_recv_buff_idx-2]==CR_CHAR){
          nl_counter++;
        }
      }
    }
  }

  #ifdef DEBUG_SS_RAW
  Serial.print("RECV: [");
  for(unsigned int j=0;j<_recv_buff_idx;j++){
    Serial.print((unsigned int)_recv_buff[j]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif

  if(_recv_buff_idx==RECV_BUFF_LEN){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RECV BUFFER FULL");
    #endif
    _clear_recv_buff();
    return 2;
  }
  else if(!_recv_buff_idx){
    #ifdef DEBUG_GSM
    Serial.println("DB: RESPONSE NOT RECEIVED!");
    #endif
    _clear_recv_buff();
    return 3;
  }
  else if(nl_counter!=3){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR, BAD RESPONSE FORMAT ");
    Serial.println(nl_counter);
    #endif
    _clear_recv_buff();
    return 4;
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

int Gsm_Ard::_fetch_rsp_from_recv(const char* cmd, unsigned int cmd_size, unsigned int* rsp_len){
  unsigned int i, start_idx;
  if(_rsp_buff_state!=BUFF_READY || _recv_buff_state!=BUFF_USED){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RSP BUFF NOT READY, OR RECV BUFF NOT USED!");
    #endif
    return 1;
  }
  for(i=0;i<cmd_size;i++){
    if(cmd[i]!=_recv_buff[i]){
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR CMD AND RSP DO NOT MATCH!");
      #endif
      return 2;
    }
  }

  if(_recv_buff[i]!=CR_CHAR || _recv_buff[i+1]!=NL_CHAR || _recv_buff[i+2]!=CR_CHAR || _recv_buff[i+3]!=NL_CHAR){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR NL AND CR DO NOT MATCH!");
    #endif
    return 3;
  }

  i+=4;
  start_idx=i;

  while(_recv_buff[i]!=CR_CHAR && _recv_buff[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=_recv_buff_idx-2){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR RSP AND CMD SIZES DO NOT MATCH!");
    #endif
    return 4;
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
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RSP BUFF NOT USED!");
    #endif
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
