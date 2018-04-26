#include "serial.h"
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


static int   	serial_open      			(char *device, int baud);
static int   	serial_data_avail 		(void);
static int   	serial_get_char   		(void);

static void 	set_comm_line_status	(byte new_status);
static byte 	get_comm_line_status	(void);

static void 	rx_init								(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);
static byte 	is_rx_finished				(void);

void serial_begin(int baud){
	wiringPiSetup();
	gFD = serial_open("/dev/ttyAMA0",baud);
}

void turn_on_signal(){
	digitalWrite(GPRS_ON, HIGH);
	delay(1200);
	digitalWrite(GPRS_ON, LOW);
	delay(5000);
}

/*
* serial_open:
*	Open and initialise the serial port, setting all the right
*	port parameters - or as many as are required - hopefully!
*********************************************************************************
*/
int serial_open (char *device, int baud) {
	struct termios options ;
	speed_t myBaud ;
	int     status, fd ;

	switch (baud)	{
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
		default:	return -2 ;
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

void serialClose (void)
{
	close (gFD) ;
}

void serialPuts (char *s)
{
	char *i= "\r";

	write(gFD,s,strlen(s));
	write(gFD,i,strlen(i));
}

void set_comm_line_status(byte new_status)
{
comm_line_status = new_status;
}

byte get_comm_line_status(void)
{
return comm_line_status;
}

/**********************************************************
Method sends AT command and waits for response

return:
AT_BUFFER_BUSY = 3  	 					// response buffer busy
AT_RESP_ERR_NO_RESP = 2,   // no response received
AT_RESP_ERR_DIF_RESP = 1,   // response_string is different from the response
AT_RESP_OK = 0             					// response_string was included in the response
**********************************************************/
char send_at_cmd_wait_resp(const char *AT_cmd_string,uint16_t start_comm_tmout,
uint16_t max_interchar_tmout,char const *response_string,byte no_of_attempts){
	byte status;

	char ret_val = AT_RESP_ERR_NO_RESP;
	byte i;

	if(get_comm_line_status()!=CLS_FREE){
		return AT_BUFFER_BUSY;
	}

	set_comm_line_status(CLS_ATCMD);

	for (i = 0; i < no_of_attempts; i++){
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
	set_comm_line_status(CLS_FREE);

	return (ret_val);
}

int serial_data_avail (void)
{
	int result ;

	if (ioctl (gFD, FIONREAD, &result) == -1)
	return -1 ;

	return result ;
}

int serial_get_char (void)
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

	rx_init(start_comm_tmout, max_interchar_tmout);
	// wait until response is not finished
	do
	{
	status = is_rx_finished();
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
void rx_init(uint16_t start_comm_tmout, uint16_t max_interchar_tmout)
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
byte is_rx_finished(void)
{
	byte num_of_bytes;
	byte ret_val = RX_NOT_FINISHED;  // default not finished
	// Rx state machine
	// ----------------

if (rx_state == RX_NOT_STARTED)
{
	// Reception is not started yet - check tmout
	if (!serial_data_avail())
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
		num_of_bytes = serial_data_avail();
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
				*p_comm_buf = serial_get_char();

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
				//*p_comm_buf = serial_get_char();
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
