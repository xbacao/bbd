#include "GPRS.h"
#include <wiringPi.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static int gFD;			// variables connected with communication buffer

static byte *p_comm_buf;               // pointer to the communication buffer
static byte comm_buf_len;              // num. of characters in the buffer
static byte rx_state;                  // internal state of rx state machine
static uint16_t start_reception_tmout; // max tmout for starting reception
static uint16_t interchar_tmout;       // previous time in msec.
static unsigned long prev_time;        // previous time in msec.
static byte comm_line_status;
static byte comm_buf[COMM_BUF_LEN+1];

void serialBegin(int baud){
	wiringPiSetup();
	gFD = serialOpen("/dev/ttyAMA0",baud);
}

void TurnOn(long baud_rate){
	int i;
	char* baud_rate_str;

	baud_rate_str=(char*) malloc(sizeof(char)*15);

	pullUpDnControl(GSM_ON,PUD_DOWN);
	pullUpDnControl(GSM_RESET,PUD_DOWN);
	SetCommLineStatus(CLS_ATCMD);
	serialBegin(baud_rate);

	#ifdef DEBUG_PRINT
	// parameter 0 - because module is off so it is not necessary
	// to send finish AT<CR> here
	DebugPrint("DEBUG: baud ", 0);
	DebugPrintD(baud_rate, 0);
	#endif

	if (AT_RESP_ERR_NO_RESP == SendATCmdWaitResp("AT", 500, 100, "OK", 5))
	{		//check power
    	// there is no response => turn on the module

		#ifdef DEBUG_PRINT
			// parameter 0 - because module is off so it is not necessary
			// to send finish AT<CR> here
			DebugPrint("DEBUG: GSM module is off\r\n", 0);
			DebugPrint("DEBUG: start the module\r\n", 0);
		#endif

		// generate turn on pulse

		digitalWrite(GSM_ON, HIGH);
		delay(1200);
		//sleep(2);
		digitalWrite(GSM_ON, LOW);
		delay(5000);
		//sleep(5);
	}
	else
	{
		#ifdef DEBUG_PRINT
			// parameter 0 - because module is off so it is not necessary
			// to send finish AT<CR> here
			DebugPrint("DEBUG: GSM module is on\r\n", 0);
		#endif
	}

	if (AT_RESP_ERR_DIF_RESP == SendATCmdWaitResp("AT", 500, 100, "OK", 5))
	{		//check OK

		#ifdef DEBUG_PRINT
			// parameter 0 - because module is off so it is not necessary
			// to send finish AT<CR> here
			DebugPrint("DEBUG: the baud is not ok\r\n", 0);
		#endif
			  //SendATCmdWaitResp("AT+IPR=9600", 500, 50, "OK", 5);

		for (i=1;i<7;i++)
		{
			switch (i)
			{
				case 1:
					serialBegin(4800);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 4800\r\n", 0);
					#endif
				break;
				case 2:
					serialBegin(9600);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 9600\r\n", 0);
					#endif
				break;
				case 3:
					serialBegin(19200);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 19200\r\n", 0);
					#endif
				break;
				case 4:
					serialBegin(38400);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 38400\r\n", 0);
					#endif
				break;
				case 5:
					serialBegin(57600);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 57600\r\n", 0);
					#endif
				break;
				case 6:
					serialBegin(115200);
					#ifdef DEBUG_PRINT
						DebugPrint("DEBUG: provo Baud 115200\r\n", 0);
					#endif
				break;
				// if nothing else matches, do the default
				// default is optional
			}
			/*
			p_char = strchr((char *)(comm_buf),',');
			p_char1 = p_char+2; // we are on the first phone number character
			p_char = strchr((char *)(p_char1),'"');
			if (p_char != NULL) {
			*p_char = 0; // end of string
			strcpy(phone_number, (char *)(p_char1));
			}
			*/
			sprintf(baud_rate_str,"%ld",baud_rate);

			delay(100);
			/*sprintf (buff,"AT+IPR=%f",baud_rate);
				#ifdef DEBUG_PRINT
					// parameter 0 - because module is off so it is not necessary
					// to send finish AT<CR> here
					DebugPrint("DEBUG: Stringa ", 0);
					DebugPrint(buff, 0);
				#endif
				*/
			serialPuts("AT+IPR=");
			//serialPuts(baud_rate);
			serialPuts(baud_rate_str);
			serialPuts("\r"); // send <CR>
			delay(500);
			serialBegin(baud_rate);
			delay(100);
			if (AT_RESP_OK == SendATCmdWaitResp("AT", 500, 100, "OK", 5))
			{
					#ifdef DEBUG_PRINT
						// parameter 0 - because module is off so it is not necessary
						// to send finish AT<CR> here
						DebugPrint("DEBUG: ricevuto ok da modulo, baud impostato: ", 0);
						DebugPrintD(baud_rate, 0);
					#endif
					break;
			}

		}

			// communication line is not used yet = free
			SetCommLineStatus(CLS_FREE);
			// pointer is initialized to the first item of comm. buffer
			p_comm_buf = &comm_buf[0];

		if (AT_RESP_ERR_NO_RESP == SendATCmdWaitResp("AT", 500, 50, "OK", 5))
		{
			#ifdef DEBUG_PRINT
				// parameter 0 - because module is off so it is not necessary
				// to send finish AT<CR> here
				DebugPrint("DEBUG: No answer from the module\r\n", 0);
			#endif
		}
		else
		{

			#ifdef DEBUG_PRINT
				// parameter 0 - because module is off so it is not necessary
				// to send finish AT<CR> here
				DebugPrint("DEBUG: 1 baud ok\r\n", 0);
			#endif
		}


	}
	else
	{
		#ifdef DEBUG_PRINT
			DebugPrint("DEBUG: 2 GSM module is on and baud is ok\r\n", 0);
		#endif

	}

  SetCommLineStatus(CLS_FREE);
  // send collection of first initialization parameters for the GSM module

	// check comm line
  if (CLS_FREE != GetCommLineStatus()) return;

	#ifdef DEBUG_PRINT
		DebugPrint("DEBUG: configure the module PARAM_SET_0\r\n", 0);
	#endif
  SetCommLineStatus(CLS_ATCMD);

  // Reset to the factory settings
  SendATCmdWaitResp("AT&F", 1000, 50, "OK", 5);
  // switch off echo
  //SendATCmdWaitResp("ATE0", 500, 50, "OK", 5);
  // setup fixed baud rate
  //SendATCmdWaitResp("AT+IPR=9600", 500, 50, "OK", 5);
  // setup mode
  //SendATCmdWaitResp("AT#SELINT=1", 500, 50, "OK", 5);
  // Switch ON User LED - just as signalization we are here
  //SendATCmdWaitResp("AT#GPIO=8,1,1", 500, 50, "OK", 5);
  // Sets GPIO9 as an input = user button
  //SendATCmdWaitResp("AT#GPIO=9,0,0", 500, 50, "OK", 5);
  // allow audio amplifier control
  //SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, "OK", 5);
  // Switch OFF User LED- just as signalization we are finished
  //SendATCmdWaitResp("AT#GPIO=8,0,1", 500, 50, "OK", 5);
  SetCommLineStatus(CLS_FREE);

	free(baud_rate_str);
}

/*
* serialOpen:
*	Open and initialise the serial port, setting all the right
*	port parameters - or as many as are required - hopefully!
*********************************************************************************
*/

int serialOpen (char *device, int baud)
{
	struct termios options ;
	speed_t myBaud ;
	int     status, fd ;

	switch (baud)
	{
	case     50:	myBaud =     B50 ; break ;
	case     75:	myBaud =     B75 ; break ;
	case    110:	myBaud =    B110 ; break ;
	case    134:	myBaud =    B134 ; break ;
	case    150:	myBaud =    B150 ; break ;
	case    200:	myBaud =    B200 ; break ;
	case    300:	myBaud =    B300 ; break ;
	case    600:	myBaud =    B600 ; break ;
	case   1200:	myBaud =   B1200 ; break ;
	case   1800:	myBaud =   B1800 ; break ;
	case   2400:	myBaud =   B2400 ; break ;
	case   9600:	myBaud =   B9600 ; break ;
	case  19200:	myBaud =  B19200 ; break ;
	case  38400:	myBaud =  B38400 ; break ;
	case  57600:	myBaud =  B57600 ; break ;
	case 115200:	myBaud = B115200 ; break ;
	case 230400:	myBaud = B230400 ; break ;

	default:
	return -2 ;
	}

	fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (fd == -1)
		return -1 ;

	fcntl (fd, F_SETFL, O_RDWR) ;
	tcgetattr (fd, &options) ;
	cfmakeraw   (&options) ;
	cfsetispeed (&options, myBaud) ;
	cfsetospeed (&options, myBaud) ;

	options.c_cflag |= (CLOCAL | CREAD) ;
	options.c_cflag &= ~PARENB ;
	options.c_cflag &= ~CSTOPB ;
	options.c_cflag &= ~CSIZE ;
	options.c_cflag |= CS8 ;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
	options.c_oflag &= ~OPOST ;

	options.c_cc [VMIN]  =   0 ;
	options.c_cc [VTIME] = 100 ;

	tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;
	ioctl (fd, TIOCMGET, &status);
	status |= TIOCM_DTR ;
	status |= TIOCM_RTS ;
	ioctl (fd, TIOCMSET, &status);
	usleep (10000);

return fd ;
}

void serialFlush (void)
{
	tcflush (gFD, TCIOFLUSH) ;
}

void serialClose (void)
{
	close (gFD) ;
}

void serialPutchar (unsigned char c)
{
	write (gFD, &c, 1) ;
}

void serialPuts (char *s)
{
	char *i= "\r";

	write(gFD,s,strlen(s));
	write(gFD,i,strlen(i));
}

void SetCommLineStatus(byte new_status)
{
comm_line_status = new_status;
}

byte GetCommLineStatus(void)
{
return comm_line_status;
}

/**********************************************************
Method sends AT command and waits for response

return:
AT_RESP_ERR_NO_RESP = -1,   // no response received
AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
AT_RESP_OK = 1,             // response_string was included in the response
**********************************************************/
char SendATCmdWaitResp(char *AT_cmd_string,uint16_t start_comm_tmout, uint16_t max_interchar_tmout,char const *response_string,byte no_of_attempts)
{
	byte status;

	char ret_val = AT_RESP_ERR_NO_RESP;
	byte i;

	for (i = 0; i < no_of_attempts; i++)
	{
	// delay 500 msec. before sending next repeated AT command
	// so if we have no_of_attempts=1 tmout will not occurred
		if (i > 0) delay(500);
		write(gFD,AT_cmd_string,strlen(AT_cmd_string));
		serialPuts("\r");

		status = WaitResp(start_comm_tmout, max_interchar_tmout);

		if (status == RX_FINISHED)
		{
		// something was received but what was received?
		// ---------------------------------------------
			if(IsStringReceived(response_string))
			{
				ret_val = AT_RESP_OK;
				break;  // response is OK => finish
			}
			else ret_val = AT_RESP_ERR_DIF_RESP;
		}
		else
		{
		// nothing was received
		// --------------------
		ret_val = AT_RESP_ERR_NO_RESP;
		}

	}


return (ret_val);
}

int serialDataAvail (void)
{
	int result ;

	if (ioctl (gFD, FIONREAD, &result) == -1)
	return -1 ;

	return result ;
}

int serialGetchar (void)
{
	uint8_t x;

	if (read (gFD, &x, 1) != 1)
	return -1 ;
	return ((int)x) & 0xFF;
}

/**********************************************************
Method waits for response

start_comm_tmout    - maximum waiting time for receiving the first response
character (in msec.)
max_interchar_tmout - maximum tmout between incoming characters
in msec.
return:
RX_FINISHED         finished, some character was received

RX_TMOUT_ERR        finished, no character received
initial communication tmout occurred
**********************************************************/
byte WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout)
{
	byte status;

	RxInit(start_comm_tmout, max_interchar_tmout);
	// wait until response is not finished
	do
	{
	status = IsRxFinished();
	}
	while (status == RX_NOT_FINISHED);
	return (status);
}

/**********************************************************
Initializes receiving process

start_comm_tmout    - maximum waiting time for receiving the first response
character (in msec.)
max_interchar_tmout - maximum tmout between incoming characters
in msec.
if there is no other incoming character longer then specified
tmout(in msec) receiving process is considered as finished
**********************************************************/
void RxInit(uint16_t start_comm_tmout, uint16_t max_interchar_tmout)
{
	rx_state = RX_NOT_STARTED;
	start_reception_tmout = start_comm_tmout;
	interchar_tmout = max_interchar_tmout;
	prev_time = millis();
	comm_buf[0] = 0x00; 		// end of string
	p_comm_buf = &comm_buf[0];
	comm_buf_len = 0;
//	serialFlush(); 		// erase rx circular buffer
}
/**********************************************************
Method checks if receiving process is finished or not.
Rx process is finished if defined inter-character tmout is reached

returns:
RX_NOT_FINISHED = 0,// not finished yet
RX_FINISHED,        // finished - inter-character tmout occurred
RX_TMOUT_ERR,       // initial communication tmout occurred
**********************************************************/
byte IsRxFinished(void)
{
	byte num_of_bytes;
	byte ret_val = RX_NOT_FINISHED;  // default not finished
	// Rx state machine
	// ----------------

if (rx_state == RX_NOT_STARTED)
{
	// Reception is not started yet - check tmout
	if (!serialDataAvail())
	{
	// still no character received => check timeout
	/*
	#ifdef DEBUG_GSMRX

	DebugPrint("\r\nDEBUG: reception timeout", 0);
	Serial.print((unsigned long)(millis() - prev_time));
	DebugPrint("\r\nDEBUG: start_reception_tmout\r\n", 0);
	Serial.print(start_reception_tmout);


	#endif
	*/
		if ((unsigned long)(millis() - prev_time) >= start_reception_tmout)
		{
			// timeout elapsed => GSM module didn't start with response
			// so communication is takes as finished
			/*
			#ifdef DEBUG_GSMRX
			DebugPrint("\r\nDEBUG: RECEPTION TIMEOUT", 0);
			#endif
			*/
			comm_buf[comm_buf_len] = 0x00;
			ret_val = RX_TMOUT_ERR;
		}
	}
	else
	{
		// at least one character received => so init inter-character
		// counting process again and go to the next state
		prev_time = millis(); // init tmout for inter-character space
		rx_state = RX_ALREADY_STARTED;
	}
}

	if (rx_state == RX_ALREADY_STARTED)
	{
		// Reception already started
		// check new received bytes
		// only in case we have place in the buffer
		num_of_bytes = serialDataAvail();
		// if there are some received bytes postpone the timeout
		if (num_of_bytes) prev_time = millis();

		// read all received bytes
		//printf("%d\n",num_of_bytes);

		while (num_of_bytes)
		{
			//printf("%d\n",num_of_bytes);
			num_of_bytes--;
			//if (comm_buf_len < COMM_BUF_LEN)
			//{
				// we have still place in the GSM internal comm. buffer =>
				// move available bytes from circular buffer
				// to the rx buffer
				// printf("understand\n");
				*p_comm_buf = serialGetchar();

				p_comm_buf++;
				comm_buf_len++;
				comm_buf[comm_buf_len] = 0x00;  // and finish currently received characters
				// so after each character we have
				// valid string finished by the 0x00
			//}
			//else
			//{
				// comm buffer is full, other incoming characters
				// will be discarded
				// but despite of we have no place for other characters
				// we still must to wait until
				// inter-character tmout is reached

				// so just readout character from circular RS232 buffer
				// to find out when communication id finished(no more characters
				// are received in inter-char timeout)
				//*p_comm_buf = serialGetchar();
			//}
		}

		// finally check the inter-character timeout
		/*
		#ifdef DEBUG_GSMRX

		DebugPrint("\r\nDEBUG: intercharacter", 0);
		Serial.print((unsigned long)(millis() - prev_time));
		DebugPrint("\r\nDEBUG: interchar_tmout\r\n", 0);
		Serial.print(interchar_tmout);


		#endif
		*/
		if ((unsigned long)(millis() - prev_time) >= interchar_tmout)
		{
			// timeout between received character was reached
			// reception is finished
			// ---------------------------------------------

			/*
			#ifdef DEBUG_GSMRX

			DebugPrint("\r\nDEBUG: OVER INTER TIMEOUT", 0);
			#endif
			*/
			comm_buf[comm_buf_len] = 0x00;  // for sure finish string again
			// but it is not necessary
			ret_val = RX_FINISHED;
		}
	}

	#ifdef DEBUG_GSMRX
	if (ret_val == RX_FINISHED)
	{
		DebugPrint("DEBUG: Received string\r\n", 0);
		for (i=0; i<comm_buf_len; i++)
		{
			serialPuts(byte(comm_buf[i]));
		}
	}
	#endif

return (ret_val);
}

/**********************************************************
Method checks received bytes

compare_string - pointer to the string which should be find

return: 0 - string was NOT received
1 - string was received
**********************************************************/
byte IsStringReceived(char const *compare_string)
{
	char *ch;
	byte ret_val = 0;

	if(comm_buf_len)
	{

/*	#ifdef DEBUG_GSMRX
	DebugPrint("DEBUG: Compare the string: \r\n", 0);
	for (i=0; i<comm_buf_len; i++)
	{
	printf("%c",comm_buf[i]);
	}

	DebugPrint("\r\nDEBUG: with the string: \r\n", 0);
	printf(compare_string);
	DebugPrint("\r\n", 0);
	#endif
*/
	ch = strstr((char *)comm_buf, compare_string);
/*
	for(i=0;i<200;i++)
	{
		printf("%c",comm_buf[i]);
	}
	printf("\n");
	printf("the compare string is :%s\n",compare_string);
*/
		if (ch != NULL)
		{
			ret_val = 1;
			#ifdef DEBUG_PRINT
			DebugPrint("\r\nDEBUG: expected string was received\r\n", 0);
			#endif

		}
		else
		{
			#ifdef DEBUG_PRINT
			//printf("%s\n",*ch);
			DebugPrint("\r\nDEBUG: expected string was NOT received\r\n", 0);
			#endif

		}
	}

return (ret_val);
}