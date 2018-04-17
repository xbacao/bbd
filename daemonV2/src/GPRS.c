#include "GPRS.h"

#include <string.h>
#include "serial.h"
#include "serial_cmds.h"

const unsigned long int _POSSIBLE_BRS[8] ={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

static void _change_gprs_state(GSM_STATE new_state){
  _gprs_state=new_state;
  #ifdef DEBUG_STATES
  switch(_gprs_state){
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

int init_gprs(){
  int i,n;

  _change_gprs_state(GSM_OFF_STATE);
  _serialFD = SoftwareSerial(_GSM_RXPIN_, _GSM_TXPIN_);
  _serialFD.begin(DEFAULT_SS_BAUDRATE);

  n=_send_cmd_comp_rsp(AT, AT_LEN, AT_OK, AT_OK_LEN, 5000);
  if(!n){
    _change_gprs_state(GSM_ON_STATE);
  }
  for(i=0;i<8 && _gprs_state!=GSM_ON_STATE;i++){
    _serialFD.begin(_POSSIBLE_BRS[i]);
    n=_send_cmd_comp_rsp(AT, AT_LEN, AT_OK, AT_OK_LEN, 5000);
    if(!n){
      _change_gprs_state(GSM_ON_STATE);
    }
  }

  if(_gprs_state!=GSM_ON_STATE){
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

  _change_gprs_state(GSM_PIN_STATE);
  return 0;
}

int attachGPRS(){
  int n;

  if(_gprs_state!=GSM_PIN_STATE){
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
      _change_gprs_state(GSM_IP_STATE);
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

  _change_gprs_state(GSM_IP_STATE);
  return 0;
}

int dettachGPRS(){
  int n;
  if(_gprs_state!=GSM_IP_STATE){
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
  _change_gprs_state(GSM_PIN_STATE);
  return 0;
}


enum GPRS_STATE get_gprs_state(){
  return _gprs_state;
}
