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
const PROGMEM char AT_CGATT_SET[]="AT+CGATT=1";
const PROGMEM char AT_CIFSR[]="AT+CIFSR";

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

const PROGMEM char AT_OK[] ="OK";
const PROGMEM char AT_ERROR[] ="ERROR";


/***************************/

const unsigned long int _POSSIBLE_BRS[8] ={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

Gsm_Ard::Gsm_Ard(){
  _gsm_state=GSM_OFF_STATE;
  _clear_recv_buff();
  _clear_rsp_buff();
  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_OFF_STATE");
  #endif
}

int Gsm_Ard::init_gsm_module(){
  int i,n;
  _gsm_state=GSM_OFF_STATE;
  #ifdef DEBUG_STATES
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
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: COULD NOT SET DEFAULT BAUDRATE, TRYING OTHERS :");
    Serial.println(n);
    #endif
  }
  else{
    _gsm_state=GSM_ON_STATE;
    #ifdef DEBUG_STATES
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
    n=_send_cmd_comp_several_rsp(at_cmd_buffer, rsps,2,5000);
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
        #ifdef DEBUG_GSM
        Serial.print("DB: ERROR SETTING SIM PIN ");
        Serial.println(n);
        #endif
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
      #ifdef DEBUG_GSM
      Serial.print("DB: ERROR REQUEST GPRS STATE ");
      Serial.println(n);
      #endif
      return 4;
      break;
    case 0:
      {
        char at_cmd_buffer[11];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CGATT_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
      }
      if(n){
        #ifdef DEBUG_GSM
        Serial.println("DB: COULD NOT ATTACH GPRS SERVICE");
        #endif
        return 5;
      }
      break;
    case 1:
      _gsm_state=GSM_GPRS_STATE;
      #ifdef DEBUG_STATES
      Serial.println("DB: GSM_STATE=GSM_GPRS_STATE");
      #endif
      break;
  }
  return 0;
}

int Gsm_Ard::attachGPRS(){
  int n;

  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT_CIFSR);
    strcpy_P(at_rsp_buffer, AT_ERROR);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  switch(n){
    default:
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR REQUESTING LOCAL IP");
      #endif
      return 1;
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
      #ifdef DEBUG_GSM
      Serial.println("DB: ERROR GETTING CIP MODE RESPONSE");
      #endif
      return 2;
      break;
    case 1:
      {
        char at_cmd_buffer[15];
        char at_rsp_buffer[3];
        strcpy_P(at_cmd_buffer, AT_CIPSERVER_SET);
        strcpy_P(at_rsp_buffer, AT_OK);
        n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
        if(!n){
          #ifdef DEBUG_GSM
          Serial.println("DB: ERROR SETTING CIP CLIENT MODE")
          #endif
          return 3;
        }
      }
      break;
    case 0:
      break;
  }

  {
    char at_cmd_buffer[14];
    char at_rsp_buffer[8];
    strcpy_P(at_cmd_buffer, AT_CIPSHUT);
    strcpy_P(at_rsp_buffer, AT_SHUT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR DEACTIVATING GPRS PDP CONTEXT ");
    Serial.println(n);
    #endif
    return 3;
  }

  {
    char at_cmd_buffer[37];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CSTT);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR SETTING APN ");
    Serial.println(n);
    #endif
    return 4;
  }

  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[3];
    strcpy_P(at_cmd_buffer, AT_CIICR);
    strcpy_P(at_rsp_buffer, AT_OK);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR SETTING APN ");
    Serial.println(n);
    #endif
    return 5;
  }


  {
    char at_cmd_buffer[9];
    char at_rsp_buffer[6];
    strcpy_P(at_cmd_buffer, AT_CIFSR);
    strcpy_P(at_rsp_buffer, AT_ERROR);
    n=_send_cmd_comp_rsp(at_cmd_buffer,at_rsp_buffer,5000);
  }
  if(n!=1){
    #ifdef DEBUG_GSM
    Serial.print("DB: ERROR IP NOT ASSIGNED ");
    Serial.println(n);
    #endif
    return 6;
  }

  #ifdef DEBUG_STATES
  Serial.println("DB: GSM_STATE=GSM_IP_STATE");
  #endif
  _gsm_state=GSM_IP_STATE;
  return 0;
}

int Gsm_Ard::dettachGPRS(){
  return 1;
}

int Gsm_Ard::get_time_sync_msg(){

  return 20;
}

int Gsm_Ard::_connect_tcp_socket(){
  int n;
  if(_gsm_state!=GSM_IP_STATE){
    return 1;
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
      return 2;
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
        return 3;
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
    return 4;
  }

  n=_recv_string(10000,1);
  if(n){
    return 5;
  }

  {
    unsigned int rsp_len;
    char* rsp;
    n=_fetch_rsp_wo_cmd(&rsp_len);
    if(n){
      return 6;
    }

    rsp=new char[rsp_len];
    n=_get_rsp(&rsp);
    if(n){
      return 7;
    }

    char exp_rsp[11];
    strcpy_P(exp_rsp, AT_CONNECT_OK);
    if(strncmp(rsp, exp_rsp, rsp_len)){
      return 8;
    }
    _gsm_state=GSM_TCP_STATE;
    #ifdef DEBUG_STATES
    Serial.println("DB: GSM_STATE=GSM_TCP_STATE");
    #endif
  }

  return 0;
}

int Gsm_Ard::_send_tcp_data(char* data, unsigned int data_len){
  int n;
  if(_gsm_state!=GSM_TCP_STATE){
    return 1;
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
    for(int i=0;i<10 && !prompt_received;i++){
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
      return 1;
    }
  }

  for(unsigned int i=0;i<data_len;i++){
    _ss.write(data[i]);
  }
  _ss.write(SUB_CHAR);

  n=_recv_string(10000,1);
  if(n){
    return 2;
  }

  {
    unsigned int rsp_len;
    char* rsp;
    n=_fetch_rsp_wo_cmd(&rsp_len);
    if(n){
      return 3;
    }

    rsp=new char[rsp_len];
    n=_get_rsp(&rsp);
    if(n){
      return 4;
    }

    char exp_rsp[8];
    strcpy_P(exp_rsp, AT_CIPSEND_OK);
    if(strncmp(rsp, exp_rsp, rsp_len)){
      return 5;
    }
  }
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
    Serial.print((unsigned int) cmd[i]);
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

int Gsm_Ard::_send_cmd_comp_several_rsp(const char* cmd, char** exp_rsps, unsigned int exp_rsps_len, int recv_wait_period){
  int n;
  int cmd_size=strlen(cmd);
  char* rsp;
  unsigned int rsp_len;
  while(_ss.available()) _ss.read();
  #ifdef DEBUG_SS
  Serial.print("DB-SND: [");
  for(int i=0;i<cmd_size;i++){
    Serial.print((unsigned int) cmd[i]);
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
  else if(nl_counter!=max_nl){
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

int Gsm_Ard::_fetch_rsp_wo_cmd(unsigned int* rsp_len){
  unsigned int i, start_idx;
  if(_rsp_buff_state!=BUFF_READY || _recv_buff_state!=BUFF_USED){
    #ifdef DEBUG_GSM
    Serial.println("DB: ERROR, RSP BUFF NOT READY, OR RECV BUFF NOT USED!");
    #endif
    return 1;
  }
  start_idx=i=0;

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

GSM_STATE Gsm_Ard::get_gsm_state(){
  return _gsm_state;
}
