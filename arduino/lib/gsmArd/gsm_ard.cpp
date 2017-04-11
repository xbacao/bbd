#include "gsm_ard.h"

using namespace std;

/*******AT COMMANDS******/
const PROGMEM char AT[]="AT";
const PROGMEM char AT_CPIN_R[]="AT+CPIN?";
const PROGMEM char AT_CPIN_READY[]="+CPIN: READY";
const PROGMEM char AT_CPIN_SIM_PIN[]="+CPIN: SIM PIN";
const PROGMEM char AT_CPIN_SET[]="AT+CPIN=0048";

const PROGMEM char AT_CIURC_R[]="AT+CIURC?";
const PROGMEM char AT_CIURC_0[]="+CIURC: 1";
const PROGMEM char AT_CIURC_1[]="+CIURC: 0";
const PROGMEM char AT_CIURC_SET[]="AT+CIURC=0";

const PROGMEM char AT_CGATT_R[]="AT+CGATT?";
const PROGMEM char AT_CGATT_0[]="+CGATT: 0";
const PROGMEM char AT_CGATT_1[]="+CGATT: 1";
const PROGMEM char AT_CGATT_SET_ON[]="AT+CGATT=1";
const PROGMEM char AT_CGATT_SET_OFF[]="AT+CGATT=0";
const PROGMEM char AT_CIFSR[]="AT+CIFSR";

const PROGMEM char AT_CIPMODE_R[]="AT+CIPMODE?";
const PROGMEM char AT_CIPMODE_0[]="+CIPMODE: 0";
const PROGMEM char AT_CIPMODE_1[]="+CIPMODE: 1";
const PROGMEM char AT_CIPMODE[]="AT+CIPMODE=0";

const PROGMEM char AT_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"internet.vodafone.pt\"";

const PROGMEM char AT_CIPSERVER_R[]="AT+CIPSERVER?";
const PROGMEM char AT_CIPSERVER_RESP[]="+CIPSERVER: 0";
const PROGMEM char AT_CIPSERVER_SET[]="AT+CIPSERVER=0";

const PROGMEM char AT_CIPSHUT[]="AT+CIPSHUT";
const PROGMEM char AT_SHUT_OK[]="SHUT OK";

const PROGMEM char AT_CSTT[]="AT+CSTT=\"internet.vodafone.pt\",\"\",\"\"";

const PROGMEM char AT_CIICR[]="AT+CIICR";

const PROGMEM char AT_CIPMUX_R[]="AT+CIPMUX?";
const PROGMEM char AT_CIPMUX_0[]="+CIPMUX: 0";
const PROGMEM char AT_CIPMUX_1[]="+CIPMUX: 1";
const PROGMEM char AT_CIPMUX_SET[]="AT+CIPMUX=0";

const PROGMEM char AT_CIPSTART_SET[]="AT+CIPSTART=\"TCP\",\"178.62.6.44\",7777";
const PROGMEM char AT_CONNECT_OK[]="CONNECT OK";

const PROGMEM char AT_CIPSEND[]="AT+CIPSEND";
const PROGMEM char AT_CIPSEND_PROMPT[]=">";
const PROGMEM char AT_CIPSEND_OK[]="SEND OK";

const PROGMEM char AT_OK[]="OK";
const PROGMEM char AT_ERROR[]="ERROR";

const PROGMEM char AT_CIPCLOSE[]="AT+CIPCLOSE";
const PROGMEM char AT_CIPCLOSE_OK[]="CLOSE OK";

const PROGMEM char AT_CIPSTATUS[]="AT+CIPSTATUS";


/***************************/

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

int Gsm_Ard::init_gsm_module(){
  int i,n;
  _gsm_state=GSM_OFF_STATE;
  _ss = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
  _ss.begin(DEFAULT_SS_BAUDRATE);
  {
    char at_cmd_buffer[3];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(!n){
    _gsm_state=GSM_ON_STATE;
    #ifdef DEBUG_STATES
    Serial.println("DB: GSM_STATE=GSM_ON_STATE");
    #endif
  }
  for(i=0;i<8 && _gsm_state!=GSM_ON_STATE;i++){
    _ss.begin(_POSSIBLE_BRS[i]);
    {
      char at_cmd_buffer[3];
      char at_rsp_buffer[6];
      strcpy_P(at_cmd_buffer, AT);
      strcpy_P(at_rsp_buffer, AT_OK);
      n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
    }
    if(!n){
      _gsm_state=GSM_ON_STATE;
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_ON_STATE");
      #endif
    }
  }

  if(_gsm_state!=GSM_ON_STATE){
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
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 2;
      break;
    case 0:
      _gsm_state=GSM_PIN_STATE;
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
      #endif
      break;
    case 1:
      {
        char at_cmd_buffer[13];
        char at_rsp_buffer[6];
        strcpy_P(at_cmd_buffer, AT_CPIN_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        return 3;
      }
      _gsm_state=GSM_PIN_STATE;
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
      #endif
      break;
  }

  {
    char at_cmd_buffer[10];
    strcpy_P(at_cmd_buffer, AT_CIURC_R);

    char rsp1[10];
    char rsp2[10];
    strcpy_P(rsp1, AT_CIURC_0);
    strcpy_P(rsp2, AT_CIURC_1);
    char* rsps[]={rsp1, rsp2};
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 4;
      break;
    case 0:
      {
        char at_cmd_buffer[11];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CIURC_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        return 5;
      }
      break;
    case 1:
      break;
  }
  //TODO METER DELAY
  {
    char at_cmd_buffer[10];
    strcpy_P(at_cmd_buffer, AT_CGATT_R);

    char rsp1[10];
    char rsp2[10];
    strcpy_P(rsp1, AT_CGATT_0);
    strcpy_P(rsp2, AT_CGATT_1);
    char* rsps[]={rsp1, rsp2};
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 6;
      break;
    case 0:
      {
        char at_cmd_buffer[11];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CGATT_SET_ON);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,10000);
      }
      if(n){
        return 7;
      }
      break;
    case 1:
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_GPRS_STATE");
      #endif
      break;
  }

  {
    char at_cmd_buffer[12];
    strcpy_P(at_cmd_buffer, AT_CIPMODE_R);

    char rsp1[12];
    char rsp2[12];
    strcpy_P(rsp1, AT_CIPMODE_0);
    strcpy_P(rsp2, AT_CIPMODE_1);
    char* rsps[]={rsp1, rsp2};
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 8;
      break;
    case 0:
      break;
    case 1:
      {
        char at_cmd_buffer[13];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CIPMODE);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        return 9;
      }
      break;
  }

  {
    char at_cmd_buffer[41];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CGDCONT);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    return 10;
  }
  _gsm_state=GSM_GPRS_STATE;
  return 0;
}

int Gsm_Ard::attachGPRS(){
  int n;

  if(_gsm_state!=GSM_GPRS_STATE){
    return 1;
  }

  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT_CIFSR);
    strcpy_P(at_rsp_buffer, AT_ERROR);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  switch(n){
    default:
      return 2;
      break;
    case 1:
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_IP_STATE");
      #endif
      _gsm_state=GSM_IP_STATE;
      return 0;
      break;
    case 0:
      break;
  }

  {
    char at_cmd_buffer[14];
    char at_rsp_buffer[14];
    strcpy_P(at_cmd_buffer, AT_CIPSERVER_R);
    strcpy_P(at_rsp_buffer, AT_CIPSERVER_RESP);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  switch(n){
    default:
      return 3;
      break;
    case 1:
      {
        char at_cmd_buffer[15];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CIPSERVER_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
        if(!n){
          return 4;
        }
      }
      break;
    case 0:
      break;
  }

  {
    char at_cmd_buffer[37];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CSTT);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    return 6;
  }

  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CIICR);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    return 7;
  }


  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT_CIFSR);
    strcpy_P(at_rsp_buffer, AT_ERROR);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n!=1){
    return 8;
  }

  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_IP_STATE");
  #endif
  _gsm_state=GSM_IP_STATE;
  return 0;
}

int Gsm_Ard::dettachGPRS(){
  int n;
  if(_gsm_state!=GSM_IP_STATE){
    return 1;
  }

  {
    char at_cmd_buffer[11];
    char at_rsp_buffer[8];
    strcpy_P(at_cmd_buffer, AT_CIPSHUT);
    strcpy_P(at_rsp_buffer, AT_SHUT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,10000);
  }
  if(n){
    return 2;
  }

  {
    char at_cmd_buffer[10];
    strcpy_P(at_cmd_buffer, AT_CGATT_R);

    char rsp1[10];
    char rsp2[10];
    strcpy_P(rsp1, AT_CGATT_0);
    strcpy_P(rsp2, AT_CGATT_1);
    char* rsps[]={rsp1, rsp2};
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 3;
      break;
    case 0:
      break;
    case 1:
      {
        char at_cmd_buffer[11];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CGATT_SET_OFF);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,10000);
      }
      if(n){
        return 4;
      }
      break;
  }
  _gsm_state=GSM_PIN_STATE;
  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_PIN_STATE");
  #endif
  return 0;
}

int Gsm_Ard::_connect_tcp_socket(){
  int n;
  if(_gsm_state!=GSM_IP_STATE){
    return 10;
  }

  {
    char at_cmd_buffer[11];
    strcpy_P(at_cmd_buffer, AT_CIPMUX_R);

    char rsp1[11];
    char rsp2[11];
    strcpy_P(rsp1, AT_CIPMUX_0);
    strcpy_P(rsp2, AT_CIPMUX_1);
    char* rsps[]={rsp1, rsp2};
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
  }
  switch(n){
    default:
      return 20;
      break;
    case 0:
      break;
    case 1:
      {
        char at_cmd_buffer[12];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CIPMUX_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        return 30+n;
      }
      break;
  }

  {
    char at_cmd_buffer[37];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CIPSTART_SET);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    return 40+n;
  }

  n=_recv_string(10000,2);
  if(n){
    return 50+n;
  }

  {
    unsigned int rsp_len;
    char* rsp;
    n=_fetch_rsp_wo_cmd(&rsp_len);
    if(n){
      return 60+n;
    }
    if(!rsp_len){
      return 70;
    }

    rsp=new char[rsp_len];
    n=_get_rsp(&rsp);
    if(n){
      return 80+n;
    }

    char exp_rsp[11];
    strcpy_P(exp_rsp, AT_CONNECT_OK);
    if(strncmp(rsp, exp_rsp, rsp_len)!=0){
      return 90;
    }
    _gsm_state=GSM_TCP_STATE;
    #ifdef DEBUG_STATES
    Serial.println("DB: GSM_STATE=GSM_TCP_STATE");
    #endif
  }

  return 0;
}

int Gsm_Ard::_disconnect_tcp_socket(){
  int n;
  if(_gsm_state!=GSM_TCP_STATE){
    return 10;
  }

  {
    char at_cmd_buffer[13];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CIPSTATUS);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    return 20+n;
  }

  n=_recv_string(10000,2);
  if(n){
    return 30+n;
  }

  {
    unsigned int rsp_len;
    char* rsp;
    n=_fetch_rsp_wo_cmd(&rsp_len);
    if(n){
      return 40+n;
    }

    rsp=new char[rsp_len];
    n=_get_rsp(&rsp);
    if(n){
      return 50+n;
    }

    char exp_rsp[11];
    strcpy_P(exp_rsp, AT_CONNECT_OK);
    if(strncmp(rsp, exp_rsp, rsp_len)==0){
      {
        char at_cmd_buffer[12];
        char at_rsp_buffer[9];
        strcpy_P(at_cmd_buffer, AT_CIPCLOSE);
        strcpy_P(at_rsp_buffer, AT_CIPCLOSE_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        return 60+n;
      }
    }
  }

  _gsm_state=GSM_IP_STATE;
  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_IP_STATE");
  #endif
  return 0;
}

int Gsm_Ard::_send_tcp_data(char* data, unsigned int data_len){
  int n;
  if(_gsm_state!=GSM_TCP_STATE){
    return 10;
  }

  {
    bool prompt_received=false;
    char cmd[11];
    char rsp[2];
    char temp;

    strcpy_P(cmd, AT_CIPSEND);
    strcpy_P(rsp, AT_CIPSEND_PROMPT);
    _write_cmd(cmd);

    //wait for prompt
    for(int i=0;i<100 && !prompt_received;i++){
      if(!_ss.available()){
        delay(500);
      }
      else{
        temp=_ss.read();
        if(temp==rsp[0]){
          prompt_received=true;
        }
      }
    }
    if(!prompt_received){
      return 20;
    }
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

  n=_recv_string(10000,2);
  if(n){
    return 30+n;
  }

  {
    unsigned int rsp_len;
    char* rsp;
    n=_fetch_rsp_wo_cmd(&rsp_len);
    if(n){
      return 40+n;
    }

    rsp=new char[rsp_len];
    n=_get_rsp(&rsp);
    if(n){
      return 50+n;
    }

    char exp_rsp[8];
    strcpy_P(exp_rsp, AT_CIPSEND_OK);
    if(strncmp(rsp, exp_rsp, rsp_len)!=0){
      return 6;
    }
  }
  return 0;
}

int Gsm_Ard::_recv_tcp_data(unsigned int* data_len){
  int n;
  if(_sock_buff_state==BUFF_USED){
    return 10;
  }

  n=_recv_socket(10000);
  if(n){
    return 20+n;
  }

  _sock_buff_state=BUFF_USED;
  strncpy(_sock_buff, _recv_buff, _recv_buff_idx);
  _sock_buff_idx=_recv_buff_idx;

  _clear_recv_buff();
  *data_len=_sock_buff_idx;
  return 0;
}


/*
  sends a command, fetches response, compares with desired output
*/
int Gsm_Ard::_send_cmd_comp_rsp(const char* cmd, const char* exp_rsp, int recv_wait_period){
  int n;
  int cmd_size=strlen(cmd);
  char* rsp;
  unsigned int rsp_len;

  while(_ss.available()) _ss.read();

  #ifdef DEBUG_SS
  Serial.print("DB-SND: [");
  for(int i=0;i<cmd_size;i++){
    Serial.print((uint8_t) cmd[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif
  _write_cmd(cmd);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
        return 2;
      }
      if(!strncmp(rsp, exp_rsp, rsp_len)) return 0;
      else{
        return 1;
      }
    }
    else{
      return 3;
    }
  }
  else{
    return 4;
  }
}

int Gsm_Ard::_send_cmd_comp_several_rsp(const char* cmd, char** exp_rsps, unsigned int exp_rsps_len, int recv_wait_period){
  int n;
  int cmd_size=strlen(cmd);
  char* rsp;
  unsigned int rsp_len;
  while(_ss.available()) _ss.read();
  #ifdef DEBUG_SS
  Serial.print("DB-SND: [");
  for(int i=0;i<cmd_size;i++){
    Serial.print((uint8_t) cmd[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  #endif
  _write_cmd(cmd);
  n=_recv_string(recv_wait_period);
  if(!n){
    n=_fetch_rsp_from_recv(cmd, cmd_size, &rsp_len);
    if(!n){
      rsp=new char[rsp_len];
      n=_get_rsp(&rsp);
      if(n){
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
    return 1;
  }
  for(i=0;i<cmd_size;i++){
    if(cmd[i]!=_recv_buff[i]){
      return 2;
    }
  }

  if(_recv_buff[i]!=CR_CHAR || _recv_buff[i+1]!=NL_CHAR || _recv_buff[i+2]!=CR_CHAR || _recv_buff[i+3]!=NL_CHAR){
    return 3;
  }

  i+=4;
  start_idx=i;

  while(_recv_buff[i]!=CR_CHAR && _recv_buff[i+1]!=NL_CHAR){
    i++;
  }

  if(i!=_recv_buff_idx-2){
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
    return 100+n;
  }

  n=_send_tcp_data(data, data_len);
  if(n){
    return 200+n;
  }

  n=_recv_tcp_data(rsp_len);
  if(n){
    _disconnect_tcp_socket();
    return 300+n;
  }

  n=_disconnect_tcp_socket();
  if(n){
    return 400+n;
  }

  return 0;
}

int Gsm_Ard::get_socket_rsp(char** data){
  if(_sock_buff_state!=BUFF_USED){
    return 1;
  }

  strncpy(*data, _sock_buff, _sock_buff_idx);
  _clear_sock_buff();

  return 0;
}
