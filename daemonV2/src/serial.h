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


enum at_resp_enum
{
	AT_RESP_ERR_NO_RESP = 2,   // nothing received
	AT_RESP_ERR_DIF_RESP = 1,   // response_string is different from the response
	AT_RESP_OK = 0,             // response_string was included in the response

	AT_RESP_LAST_ITEM
};

enum rx_state_enum
{
	RX_NOT_FINISHED = 0,      // not finished yet
	RX_FINISHED,              // finished, some character was received
	RX_FINISHED_STR_RECV,     // finished and expected string received
	RX_FINISHED_STR_NOT_RECV, // finished, but expected string not received
	RX_TMOUT_ERR,             // finished, no character received
	// initial communication tmout occurred
	RX_LAST_ITEM
};

void 	serialBegin				(int baud);
void 	TurnOn						(long baud_rate);
int   serialOpen      	(char *device, int baud);
void  serialClose     	(void);
void  serialFlush     	(void);
void  serialPutchar   	(unsigned char c);
void  serialPuts      	(char *s);
int   serialDataAvail 	(void);
int   serialGetchar   	(void);

void 	SetCommLineStatus	(byte new_status);
byte 	GetCommLineStatus	(void);
void 	RxInit						(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);
byte 	IsRxFinished			(void);

char 	SendATCmdWaitResp	(char *AT_cmd_string,	uint16_t start_comm_tmout,
  											uint16_t max_interchar_tmout,	char const *response_string,
  											byte no_of_attempts);

byte 	IsStringReceived	(char const *compare_string);
byte 	WaitResp					(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);



#endif
