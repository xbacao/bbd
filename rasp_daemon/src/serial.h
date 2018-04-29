#ifndef _SERIAL_H_
#define _SERIAL_H_
#include <stdint.h>


#define byte 	unsigned char

#define GPRS_ON              0 // connect GSM Module turn ON to pin 77
#define GPRS_RESET           1 // connect GSM Module RESET to pin 35
// length for the internal communication buffer
#define COMM_BUF_LEN        300
// some constants for the IsRxFinished() method
#define RX_NOT_STARTED      0
#define RX_ALREADY_STARTED  1


enum at_resp_enum {
	AT_BUFFER_BUSY = 3,
	AT_RESP_ERR_NO_RESP = 2,   // nothing received
	AT_RESP_ERR_DIF_RESP = 1,   // response_string is different from the response
	AT_RESP_OK = 0,             // response_string was included in the response

	AT_RESP_LAST_ITEM
};

enum rx_state_enum {
	RX_NOT_FINISHED = 0,      // not finished yet
	RX_FINISHED,              // finished, some character was received
	RX_FINISHED_STR_RECV,     // finished and expected string received
	RX_FINISHED_STR_NOT_RECV, // finished, but expected string not received
	RX_TMOUT_ERR,             // finished, no character received
	// initial communication tmout occurred
	RX_LAST_ITEM
};

enum comm_line_status_enum {
	// CLS like CommunicationLineStatus
	CLS_FREE,   // line is free - not used by the communication and can be used
	CLS_ATCMD,  // line is used by AT commands, includes also time for response
	CLS_DATA,   // for the future - line is used in the CSD or GPRS communication
	CLS_LAST_ITEM
};

void 	serial_begin				(int baud);
void 	turn_on_signal			();
void  serial_close     	(void);
// void  serialFlush     	(void);
// void  serialPutchar   	(unsigned char c);
void  serialPuts      	(char *s);
// int   serialDataAvail 	(void);
// int   serialGetchar   	(void);

// void 	SetCommLineStatus	(byte new_status);
// byte 	GetCommLineStatus	(void);
// void 	RxInit						(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);
// byte 	IsRxFinished			(void);

/**********************************************************
Method sends AT command and waits for response

return:
AT_BUFFER_BUSY = 3  	 					// response buffer busy
AT_RESP_ERR_NO_RESP = 2,   // no response received
AT_RESP_ERR_DIF_RESP = 1,   // response_string is different from the response
AT_RESP_OK = 0             					// response_string was included in the response
**********************************************************/
char 	send_at_cmd_wait_resp	(const char *AT_cmd_string,	uint16_t start_comm_tmout,
  											uint16_t max_interchar_tmout,	char const *response_string,
  											byte no_of_attempts);

byte 	IsStringReceived	(char const *compare_string);
byte 	WaitResp					(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);



#endif
