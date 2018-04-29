#include "GPRS.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "serial.h"
#include "serial_cmds.h"

static enum GPRS_STATE _gprs_state;

static void _change_gprs_state(enum GPRS_STATE new_state){
  _gprs_state=new_state;
}

/**********************************************************
sends a command with 2 possible responses, if the first response is returned,
do nothing, if the second response is obtained, send a new command with expected
response. Used to check if configurations are already in place or required
to be entered

return:
AT_BUFFER_BUSY = 3  	 					// response buffer busy
AT_RESP_ERR_NO_RESP = 2,   // no response received
AT_RESP_ERR_DIF_RESP = 1,   // response_string is different from the response
AT_RESP_OK = 0             					// response_string was included in the response
**********************************************************/
static int _send_cmd_if_needed(const char* request_cmd, const char* done_ans,
const char* action_required_ans, const char* action_cmd, const char* action_rsp){
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
      return 2;
    }
    n=send_at_cmd_wait_resp(action_cmd, START_COM_TMOUT, MAX_INTERCHAR_TMOUT,
       action_rsp, N_ATTEMPTS);
    if(n){
      #ifdef DEBUG
      log_error("gprs not accepting action command");
      #endif
      return 3;
    }

    return 0;
  } else if(n!=AT_RESP_OK){
    #ifdef DEBUG
    log_error("gprs not ready for action");
    #endif
    return 1;
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

			usleep(100000);

			serialPuts("AT+IPR=");
			serialPuts(baud_rate_str);
			serialPuts("\r"); // send <CR>
			usleep(500000);
			serial_begin(DEFAULT_SS_BAUDRATE);
			usleep(500000);
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

  #ifdef DEBUG
  log_info("checking gprs pin status");
  #endif
  n=_send_cmd_if_needed(AT_CPIN_R, AT_CPIN_READY, AT_CPIN_SIM_PIN, AT_CPIN_SET,
    AT_OK);
  if(n){
    #ifdef DEBUG
    log_error("error setting gprs pin");
    #endif
    return 2+10*n;
  }

  #ifdef DEBUG
  log_info("disabling urc presentation");
  #endif
  n=_send_cmd_if_needed(AT_CIURC_R, AT_CIURC_1, AT_CIURC_0, AT_CIURC_SET, AT_OK);
  if(n){
    #ifdef DEBUG
    log_error("error disabling urc presentation");
    #endif
    return 3+10*n;
  }

  _change_gprs_state(GPRS_PIN_STATE);
  return 0;
}

enum GPRS_STATE get_gprs_state(){
  return _gprs_state;
}
