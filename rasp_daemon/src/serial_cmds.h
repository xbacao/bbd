#ifndef _SERIAL_CMDS_H_
#define _SERIAL_CMDS_H_

#ifndef _GSM_CMDS_H_
#define _GSM_CMDS_H_

/*******AT COMMANDS******/
#define AT                "AT"
#define AT_OK             "OK"
#define AT_CPIN_R         "AT+CPIN?"
#define AT_CPIN_READY     "+CPIN: READY"

#define AT_CPIN_R         "AT+CPIN?"
#define AT_CPIN_READY     "+CPIN: READY"
#define AT_CPIN_SIM_PIN   "+CPIN: SIM PIN"
#define AT_CPIN_SET       "AT+CPIN=0048"

#define AT_CIURC_R        "AT+CIURC?"
#define AT_CIURC_0        "+CIURC: 1"
#define AT_CIURC_1        "+CIURC: 0"
#define AT_CIURC_SET      "AT+CIURC=0"
#define AT_CGATT_R        "AT+CGATT?"
#define AT_CGATT_0        "+CGATT: 0"
#define AT_CGATT_1        "+CGATT: 1"
#define AT_CGATT_SET_ON   "AT+CGATT=1"
#define AT_CGATT_SET_OFF  "AT+CGATT=0"

#define AT_CIFSR          "AT+CIFSR"

#define AT_CIPMODE_R      "AT+CIPMODE?"
#define AT_CIPMODE_0      "+CIPMODE: 0"
#define AT_CIPMODE_1      "+CIPMODE: 1"
#define AT_CIPMODE        "AT+CIPMODE=0"

#define AT_CGDCONT        "AT+CGDCONT=1,\"IP\",\"internet.vodafone.pt\""

#define AT_CIPSERVER_R    "AT+CIPSERVER?"
#define AT_CIPSERVER_RESP "+CIPSERVER: 0"
#define AT_CIPSERVER_SET  "AT+CIPSERVER=0"

#define AT_CIPSHUT        "AT+CIPSHUT"
#define AT_SHUT_OK        "SHUT OK"

#define AT_CSTT           "AT+CSTT=\"internet.vodafone.pt\",\"\",\"\""

#define AT_CIICR          "AT+CIICR"

#define AT_CIPMUX_R       "AT+CIPMUX?"
#define AT_CIPMUX_0       "+CIPMUX: 0"
#define AT_CIPMUX_1       "+CIPMUX: 1"
#define AT_CIPMUX_SET     "AT+CIPMUX=0"

/***************************/

#endif


#endif
