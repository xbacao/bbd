#ifndef _GSM_CMDS_H_
#define _GSM_CMDS_H_

/*******AT COMMANDS******/
const PROGMEM char AT[]="AT";
#define AT_LEN                2

const PROGMEM char AT_CPIN_R[]="AT+CPIN?";
const PROGMEM char AT_CPIN_READY[]="+CPIN: READY";
const PROGMEM char AT_CPIN_SIM_PIN[]="+CPIN: SIM PIN";
const PROGMEM char AT_CPIN_SET[]="AT+CPIN=0048";
#define AT_CPIN_R_LEN         8
#define AT_CPIN_READY_LEN     12
#define AT_CPIN_SIM_PIN_LEN   14
#define AT_CPIN_SET_LEN       12

const PROGMEM char AT_CIURC_R[]="AT+CIURC?";
const PROGMEM char AT_CIURC_0[]="+CIURC: 1";
const PROGMEM char AT_CIURC_1[]="+CIURC: 0";
const PROGMEM char AT_CIURC_SET[]="AT+CIURC=0";
#define AT_CIURC_R_LEN        9
#define AT_CIURC_0_LEN        9
#define AT_CIURC_1_LEN        9
#define AT_CIURC_SET_LEN      10

const PROGMEM char AT_CGATT_R[]="AT+CGATT?";
const PROGMEM char AT_CGATT_0[]="+CGATT: 0";
const PROGMEM char AT_CGATT_1[]="+CGATT: 1";
const PROGMEM char AT_CGATT_SET_ON[]="AT+CGATT=1";
const PROGMEM char AT_CGATT_SET_OFF[]="AT+CGATT=0";
#define AT_CGATT_R_LEN        9
#define AT_CGATT_0_LEN        9
#define AT_CGATT_1_LEN        9
#define AT_CGATT_SET_ON_LEN   10
#define AT_CGATT_SET_OFF_LEN  10

const PROGMEM char AT_CIFSR[]="AT+CIFSR";
#define AT_CIFSR_LEN          8

const PROGMEM char AT_CIPMODE_R[]="AT+CIPMODE?";
const PROGMEM char AT_CIPMODE_0[]="+CIPMODE: 0";
const PROGMEM char AT_CIPMODE_1[]="+CIPMODE: 1";
const PROGMEM char AT_CIPMODE[]="AT+CIPMODE=0";
#define AT_CIPMODE_R_LEN      11
#define AT_CIPMODE_0_LEN      11
#define AT_CIPMODE_1_LEN      11
#define AT_CIPMODE_LEN        12

const PROGMEM char AT_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"internet.vodafone.pt\"";
#define AT_CGDCONT_LEN        40

const PROGMEM char AT_CIPSERVER_R[]="AT+CIPSERVER?";
const PROGMEM char AT_CIPSERVER_RESP[]="+CIPSERVER: 0";
const PROGMEM char AT_CIPSERVER_SET[]="AT+CIPSERVER=0";
#define AT_CIPSERVER_R_LEN    13
#define AT_CIPSERVER_RESP_LEN 13
#define AT_CIPSERVER_SET_LEN  14

const PROGMEM char AT_CIPSHUT[]="AT+CIPSHUT";
const PROGMEM char AT_SHUT_OK[]="SHUT OK";
#define AT_CIPSHUT_LEN        10
#define AT_SHUT_OK_LEN        7

const PROGMEM char AT_CSTT[]="AT+CSTT=\"internet.vodafone.pt\",\"\",\"\"";
#define AT_CSTT_LEN           36

const PROGMEM char AT_CIICR[]="AT+CIICR";
#define AT_CIICR_LEN          8

const PROGMEM char AT_CIPMUX_R[]="AT+CIPMUX?";
const PROGMEM char AT_CIPMUX_0[]="+CIPMUX: 0";
const PROGMEM char AT_CIPMUX_1[]="+CIPMUX: 1";
const PROGMEM char AT_CIPMUX_SET[]="AT+CIPMUX=0";
#define AT_CIPMUX_R_LEN       10
#define AT_CIPMUX_0_LEN       10
#define AT_CIPMUX_1_LEN       10
#define AT_CIPMUX_SET_LEN     11

const PROGMEM char AT_CIPSTART_SET[]="AT+CIPSTART=\"TCP\",\"178.62.6.44\",7777";
const PROGMEM char AT_CONNECT_OK[]="CONNECT OK";
#define AT_CIPSTART_SET_LEN   36
#define AT_CONNECT_OK_LEN     10

const PROGMEM char AT_CIPSEND[]="AT+CIPSEND";
#define AT_CIPSEND_PROMPT     '>'
const PROGMEM char AT_CIPSEND_OK[]="SEND OK";
#define AT_CIPSEND_LEN        10
#define AT_CIPSEND_OK_LEN     7

const PROGMEM char AT_OK[]="OK";
const PROGMEM char AT_ERROR[]="ERROR";
#define AT_OK_LEN             2
#define AT_ERROR_LEN          5

const PROGMEM char AT_CIPCLOSE[]="AT+CIPCLOSE";
const PROGMEM char AT_CIPCLOSE_OK[]="CLOSE OK";
#define AT_CIPCLOSE_LEN       11
#define AT_CIPCLOSE_OK_LEN    8

const PROGMEM char AT_CIPSTATUS[]="AT+CIPSTATUS";
#define AT_CIPSTATUS_LEN      12


/***************************/

#endif
