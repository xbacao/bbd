#include "GPRS.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"
#include "serial_cmds.h"

static enum GPRS_STATE _gprs_state;

static void _change_gprs_state(enum GPRS_STATE new_state){
  _gprs_state=new_state;
}

static int _send_cmd_if_needed(const char* request_cmd, const char* done_ans,
const char* action_required_ans, const char* action_cmd){
  int n;

  n=send_at_cmd_wait_resp(request_cmd, START_COM_TMOUT, MAX_INTERCHAR_TMOUT,
     done_ans, N_ATTEMPTS);
  if(n==AT_RESP_ERR_DIF_RESP){
    n=send_at_cmd_wait_resp(request_cmd, START_COM_TMOUT, MAX_INTERCHAR_TMOUT,
       action_required_ans, N_ATTEMPTS);
    if(n!=AT_RESP_OK){
      #ifdef DEBUG
      log_error("none of the expected responses match");
      #endif
      return 2+10*n;
    }
    n=send_at_cmd_wait_resp(action_cmd, START_COM_TMOUT, MAX_INTERCHAR_TMOUT,
       AT_OK, N_ATTEMPTS);
    if(n){
      #ifdef DEBUG
      log_error("gprs not accepting action command");
      #endif
      return 3+10*n;
    }

    return 0;
  } else if(n!=AT_RESP_OK){
    #ifdef DEBUG
    log_error("gprs not ready for action");
    #endif
    return 1+10*n;
  }
  #ifdef DEBUG
  log_info("gprs does not require action");
  #endif
  return 0;

}

// static int _pin_config(){
//   int n;
//
//   #ifdef DEBUG
//   log_info("checking sim pin");
//   #endif
//   n=send_at_cmd_wait_resp(AT_CPIN_R, START_COM_TMOUT, 100, AT_CPIN_READY, 5);
//   if(n==AT_RESP_ERR_DIF_RESP){
//     n=send_at_cmd_wait_resp(AT_CPIN_R, START_COM_TMOUT, 100, AT_CPIN_SIM_PIN, 5);
//     if(n!=AT_RESP_OK){
//       #ifdef DEBUG
//       log_error("gprs not not accepting pin");
//       #endif
//       return 2+10*n;
//     }
//     n=send_at_cmd_wait_resp(AT_CPIN_SET, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
//     if(n){
//       #ifdef DEBUG
//       log_error("gprs bad pin");
//       #endif
//       return 3+10*n;
//     }
//
//     #ifdef DEBUG
//     log_info("gprs pin successfully entered");
//     #endif
//     return 0;
//   } else if(n!=AT_RESP_OK){
//     #ifdef DEBUG
//     log_error("gprs not ready for pin");
//     #endif
//     return 1+10*n;
//   }
//   #ifdef DEBUG
//   log_info("gprs pin already entered");
//   #endif
//   return 0;
// }

int init_gprs(){
  int i,n;
	char* baud_rate_str;

	baud_rate_str=(char*) malloc(sizeof(char)*15);

	// pullUpDnControl(GSM_ON,PUD_DOWN);
	// pullUpDnControl(GSM_RESET,PUD_DOWN);
	// SetCommLineStatus(CLS_ATCMD);
	serial_begin(DEFAULT_SS_BAUDRATE);

	if (send_at_cmd_wait_resp("AT", 500, 100, "OK", 5)==AT_RESP_ERR_NO_RESP){
  		// generate turn on pulse


	}

	if (send_at_cmd_wait_resp("AT", 500, 100, "OK", 5)==AT_RESP_ERR_DIF_RESP){
		for (i=1;i<7;i++){
			switch (i){
				case 1:
					serial_begin(4800);
				break;
				case 2:
					serial_begin(9600);
				break;
				case 3:
					serial_begin(19200);
				break;
				case 4:
					serial_begin(38400);
				break;
				case 5:
					serial_begin(57600);
				break;
				case 6:
					serial_begin(115200);
				break;
			}
			sprintf(baud_rate_str,"%d",DEFAULT_SS_BAUDRATE);

			delay(100);

			serialPuts("AT+IPR=");
			serialPuts(baud_rate_str);
			serialPuts("\r"); // send <CR>
			delay(500);
			serial_begin(DEFAULT_SS_BAUDRATE);
			delay(100);
			if (send_at_cmd_wait_resp("AT", 500, 100, "OK", 5)==AT_RESP_OK){
          _gprs_state=GPRS_ON_STATE;
					break;
			}
		}

		// SetCommLineStatus(CLS_FREE);
		// p_comm_buf = &comm_buf[0];
	}
  free(baud_rate_str);


  // SetCommLineStatus(CLS_FREE);


  // SetCommLineStatus(CLS_ATCMD);

  send_at_cmd_wait_resp("AT&F", 1000, 50, "OK", 5);
  // switch off echo
  //send_at_cmd_wait_resp("ATE0", 500, 50, "OK", 5);
  // setup fixed baud rate
  //send_at_cmd_wait_resp("AT+IPR=9600", 500, 50, "OK", 5);
  // setup mode
  //send_at_cmd_wait_resp("AT#SELINT=1", 500, 50, "OK", 5);
  // Switch ON User LED - just as signalization we are here
  //send_at_cmd_wait_resp("AT#GPIO=8,1,1", 500, 50, "OK", 5);
  // Sets GPIO9 as an input = user button
  //send_at_cmd_wait_resp("AT#GPIO=9,0,0", 500, 50, "OK", 5);
  // allow audio amplifier control
  //send_at_cmd_wait_resp("AT#GPIO=5,0,2", 500, 50, "OK", 5);
  // Switch OFF User LED- just as signalization we are finished
  //send_at_cmd_wait_resp("AT#GPIO=8,0,1", 500, 50, "OK", 5);
  // SetCommLineStatus(CLS_FREE);


  if(_gprs_state!=GPRS_ON_STATE){
    return 1;
  }

  // n=_pin_config();
  // if(n){
  //   return 2+10*n;
  // }
  #ifdef DEBUG
  log_info("checking gprs pin status");
  #endif
  n=_send_cmd_if_needed(AT_CPIN_R, AT_CPIN_READY, AT_CPIN_SIM_PIN, AT_CPIN_SET);
  if(n){
    #ifdef DEBUG
    log_error("error setting gprs pin");
    #endif
    return 2+10*n;
  }

  n=_send_cmd_comp_several_rsp(AT_CIURC_R, START_COM_TMOUT, AT_CIURC_0, AT_CIURC_0_LEN, AT_CIURC_1, AT_CIURC_1_LEN, 5000);
  switch(n){
    default:
      return 4+10*n;
      break;
    case 0:
      n=send_at_cmd_wait_resp(AT_CIURC_SET, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
      if(n){
        return 5+10*n;
      }
      break;
    case 1:
      break;
  }

  _change_gprs_state(GPRS_PIN_STATE);
  return 0;
}

int attachGPRS(){
  int n;

  if(_gprs_state!=GPRS_PIN_STATE){
    return 1;
  }

  n=_send_cmd_comp_several_rsp(AT_CGATT_R, AT_CGATT_R_LEN, AT_CGATT_0, AT_CGATT_0_LEN, AT_CGATT_1, AT_CGATT_1_LEN, 5000);
  switch(n){
    default:
      return 2+n*10;
      break;
    case 0:
    //10k
      n=send_at_cmd_wait_resp(AT_CGATT_SET_ON, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
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
      n=send_at_cmd_wait_resp(AT_CIPMODE, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
      if(n){
        return 5+n*10;
      }
      break;
  }

  n=send_at_cmd_wait_resp(AT_CGDCONT, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
  if(n){
    return 6+n*10;
  }

  n=send_at_cmd_wait_resp(AT_CIFSR, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_ERROR, N_ATTEMPTS);
  switch(n){
    default:
      return 7+n*10;
      break;
    case 1:
      _change_gprs_state(GPRS_IP_STATE);
      return 0;
      break;
    case 0:
      break;
  }

  n=send_at_cmd_wait_resp(AT_CIPSERVER_R, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_CIPSERVER_RESP, N_ATTEMPTS);
  switch(n){
    default:
      return 8+n*10;
      break;
    case 1:
      n=send_at_cmd_wait_resp(AT_CIPSERVER_SET, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
      if(!n){
        return 9+n*10;
      }
      break;
    case 0:
      break;
  }

  n=send_at_cmd_wait_resp(AT_CSTT, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
  if(n){
    return 10;
  }

  n=send_at_cmd_wait_resp(AT_CIICR, AT_CIICR_LEN, MAX_INTERCHAR_TMOUT, AT_OK, N_ATTEMPTS);
  if(n){
    return 11;
  }

  n=send_at_cmd_wait_resp(AT_CIFSR, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_ERROR, N_ATTEMPTS);
  if(n!=1){
    return 12;
  }

  _change_gprs_state(GPRS_IP_STATE);
  return 0;
}

int dettachGPRS(){
  if(_gprs_state!=GPRS_IP_STATE){
    return 1;
  }

  //10k
  n=send_at_cmd_wait_resp(AT_CIPSHUT, START_COM_TMOUT, MAX_INTERCHAR_TMOUT, AT_SHUT_OK, N_ATTEMPTS;
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
    //10k
      n=send_at_cmd_wait_resp(AT_CGATT_SET_OFF, START_COM_TMOUT,MAX_INTERCHAR_TMOUT, AT_OK, AN_ATTEMPTS);
      if(n){
        return 4+10*n;
      }
      break;
  }
  _change_gprs_state(GPRS_PIN_STATE);
  return 0;
}

enum GPRS_STATE get_gprs_state(){
  return _gprs_state;
}
