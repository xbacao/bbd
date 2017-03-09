#include "SIM900.h"

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

//#define RESETPIN 7

SIMCOM900 gsm;
SIMCOM900::SIMCOM900(){};
SIMCOM900::~SIMCOM900(){};

char SIMCOM900::forceON(){
	char ret_val=0;
	char *p_char;
	char *p_char1;

	SimpleWriteln(F("AT+CREG?"));
	WaitResp(5000, 100, "OK");
	if(IsStringReceived("OK")){
		ret_val=1;
	}
	//BCL
	p_char = strchr((char *)(gsm.comm_buf),',');
	p_char1 = p_char+1;  //we are on the first char of BCS
	*(p_char1+2)=0;
	p_char = strchr((char *)(p_char1), ',');
	if (p_char != NULL) {
          *p_char = 0;
    }

	if((*p_char1)=='4'){
		digitalWrite(GSM_ON, HIGH);
		delay(1200);
		digitalWrite(GSM_ON, LOW);
		delay(10000);
		ret_val=2;
	}

	return ret_val;
}

int SIMCOM900::configandwait(char* pin)
{
  int connCode;
  //_tf.setTimeout(_GSM_CONNECTION_TOUT_);

  if(pin) setPIN(pin); //syv

  // Try 10 times to register in the network. Note this can take some time!
  for(int i=0; i<10; i++)
  {
    //Ask for register status to GPRS network.
    SimpleWriteln(F("AT+CGREG?"));

    //Se espera la unsolicited response de registered to network.
    while(gsm.WaitResp(5000, 50, "+CGREG: 0,")!=RX_FINISHED_STR_RECV)
	//while (_tf.find("+CGREG: 0,"))  // CHANGE!!!!
	{
		//connCode=_tf.getValue();
		connCode=_cell.read();
		if((connCode==1)||(connCode==5))
		{
		  setStatus(READY);

		SimpleWriteln(F("AT+CMGF=1")); //SMS text mode.
		delay(200);
		  // Buah, we should take this to readCall()
		SimpleWriteln(F("AT+CLIP=1")); //SMS text mode.
		delay(200);
		//_cell << "AT+QIDEACT" <<  _DEC(cr) << endl; //To make sure not pending connection.
		//delay(1000);

		  return 1;
		}
	}
  }
  return 0;
}

int SIMCOM900::read(char* result, int resultlength)
{
	char temp;
	int i=0;
	for(i=0; i<resultlength;i++){
    //while(gsm.available() <= 0) ;
		temp=gsm.read();
		//if(temp>0){
			result[i]=temp;
		//}
	}
  return i;
}


#define OK_STR "OK"
#define ACTION_STR "+HTTPACTION:"
#define ACTION_LEN  12
int SIMCOM900::read_httpaction(int method, int status_code, int data_len){
  //char ok_str[2] = {'O', 'K'};
  char result[100];
  char temp = gsm.read();
  int i = 0;
  do                                    //searches for OK string
  {
    i = 0;
    while(temp != '\n' && i < 100){     //fetches a string
      //while(gsm.available() <= 0) ;
      temp = gsm.read();
      result[i] = temp;
    }
  } while(strncmp(result, OK_STR, i) != 0);

  do                                    //searches for +HTTPACTION
  {
    i = 0;
    while(temp != '\n' && i < 100){     //fetches a string
      //while(gsm.available() <= 0) ;
      temp = gsm.read();
      result[i] = temp;
      i++;
    }
  } while(i < ACTION_LEN || strncmp(result, ACTION_STR, ACTION_LEN) != 0);

  //find dommas_idx
  int commas_idx[2];
  int commas_idx_idx = 0;
  for(int j = 0; j < i && commas_idx_idx < 2; i++){
    if(result[j] == ','){
      commas_idx[commas_idx_idx] = j;
      commas_idx_idx++;
    }
  }

  method = atoi(result+ACTION_LEN);
  status_code = atoi(result+commas_idx[0]+1);
  data_len = atoi(result+commas_idx[1]+1);

  /*while(gsm.available() > 0){
    gsm.read();
  }*/
  return 0;
}

int SIMCOM900::httpRspCode(){
  char temp;
  int i = 0;
  char str_read[100];
  temp = gsm.read();
  while(temp != '\n' && i < 100){
    if(temp > 0){
      str_read[i] = temp;
      i++;
    }
    temp = gsm.read();
  }

  char char_code[4];
  memcpy(char_code, str_read+9, 3);
  char_code[3] = '\0';
  int code;
  sscanf(char_code, "%d", &code);

  //read remaining headers
  int k = 0;
  while(k < 5){
    if(gsm.read() == '\n'){
      k++;
    }
  }


  return code;
}

int SIMCOM900::getRestContent(char cont[50]){
	char temp;
  unsigned int cont_idx = 0;
  do{
    temp = gsm.read();
  } while(temp != '+');
	do{
		temp = gsm.read();
	} while(temp != ':');
	while(true){
		temp = gsm.read();
		if(temp == '\n'){
			break;
		}
		else{
			cont[cont_idx++] = temp;
		}
	}

	unsigned int cont_len=atoi(cont);
	for(cont_idx=0;cont_idx<cont_len;cont_idx++){
		cont[cont_idx]=gsm.read();
	}
	return 0;
  /*char temp;
  int cont_idx;
  do{
    temp = gsm.read();
  } while(temp != '|');
  cont[cont_idx] = temp;
  cont_idx++;

  int slash_cnt = 1;

  while(slash_cnt < 4 && cont_idx < 50){
    temp = gsm.read();
    //if(temp > 0){
    if(temp == '|'){
      slash_cnt++;
    }
    cont[cont_idx] = temp;
    cont_idx++;
    //}
  }

  if(cont_idx == 50){
      //#ifdef DEBUG_ON
      //  Serial.println("DB:CONT FETCHED");
      //#endif
    return -1;
  }

  return 1;*/

}

 int SIMCOM900::readCellData(int &mcc, int &mnc, long &lac, long &cellid)
{
  if (getStatus()==IDLE)
    return 0;

   //_tf.setTimeout(_GSM_DATA_TOUT_);
   //_cell.flush();
  SimpleWriteln(F("AT+QENG=1,0"));
  SimpleWriteln(F("AT+QENG?"));
  if(gsm.WaitResp(5000, 50, "+QENG")!=RX_FINISHED_STR_NOT_RECV)
    return 0;

  //mcc=_tf.getValue(); // The first one is 0
  mcc=_cell.read();
  //mcc=_tf.getValue();
  mcc=_cell.read();
  //mnc=_tf.getValue();
  mnc=_cell.read();
  //lac=_tf.getValue();
  lac=_cell.read();
  //cellid=_tf.getValue();
  cellid=_cell.read();

  gsm.WaitResp(5000, 50, "+OK");
  SimpleWriteln(F("AT+QENG=1,0"));
  gsm.WaitResp(5000, 50, "+OK");
  return 1;
}

boolean SIMCOM900::readSMS(char* msg, int msglength, char* number, int nlength)
{
  Serial.println(F("This method is deprecated! Please use GetSMS in the SMS class."));
  long index;
  char *p_char;
  char *p_char1;

  /*
  if (getStatus()==IDLE)
    return false;
  */
  #ifdef UNO
	_tf.setTimeout(_GSM_DATA_TOUT_);
  #endif
  //_cell.flush();
  WaitResp(500, 500);
  SimpleWriteln(F("AT+CMGL=\"REC UNREAD\",1"));

  WaitResp(5000, 500);
  if(gsm.IsStringReceived("+CMGL"))
  {

	//index
	p_char = strchr((char *)(gsm.comm_buf),'+CMGL');
	p_char1 = p_char+3;  //we are on the first char of string
	p_char = p_char1+1;
          *p_char = 0;
	index=atoi(p_char1);

	p_char1 = p_char+1;
	p_char = strstr((char *)(p_char1), "\",\"");
	p_char1 = p_char+3;
	p_char = strstr((char *)(p_char1), "\",\"");
	if (p_char != NULL) {
          *p_char = 0;
    }
	strcpy(number, (char *)(p_char1));
	//////

	p_char1 = p_char+3;
	p_char = strstr((char *)(p_char1), "\",\"");
	p_char1 = p_char+3;

	p_char = strstr((char *)(p_char1), "\n");
	p_char1 = p_char+1;
	p_char = strstr((char *)(p_char1), "\n");
	if (p_char != NULL) {
          *p_char = 0;
    }
	strcpy(msg, (char *)(p_char1));

	// #ifdef UNO
		// index=_tf.getValue();
	// #endif
	// #ifdef MEGA
	//index=_cell.read();
	// #endif
	// Serial.println("DEBUG");
	// #ifdef UNO
		// _tf.getString("\",\"", "\"", number, nlength);
	// #endif
	// Serial.println("PRIMA");
	// #ifdef MEGA
		// _cell.getString("\",\"", "\"", number, nlength);
	// #endif
	// Serial.println("DEBUG");
	// #ifdef UNO
		// _tf.getString("\n", "\nOK", msg, msglength);
	// #endif
	// #ifdef MEGA
		// _cell.getString("\n", "\nOK", msg, msglength);
	// #endif

    SimpleWrite(F("AT+CMGD="));
	SimpleWriteln(index);
	// Serial.print("VAL= ");
	// Serial.println(index);
    gsm.WaitResp(5000, 50, "OK");
    return true;
  };
  return false;
};

boolean SIMCOM900::readCall(char* number, int nlength)
{
  int index;

  if (getStatus()==IDLE)
    return false;

  //_tf.setTimeout(_GSM_DATA_TOUT_);
  if(gsm.WaitResp(5000, 50, "+CLIP: \"")!=RX_FINISHED_STR_RECV)
  //if(_tf.find("+CLIP: \""))
  {
	#ifdef UNO
		_tf.getString("", "\"", number, nlength);
	#endif
	#ifdef MEGA
		_cell.getString("", "\"", number, nlength);
	#endif
    SimpleWriteln("ATH");
    delay(1000);
    //_cell.flush();
    return true;
  };
  return false;
};

boolean SIMCOM900::call(char* number, unsigned int milliseconds)
{
  if (getStatus()==IDLE)
    return false;

  //_tf.setTimeout(_GSM_DATA_TOUT_);

  SimpleWrite("ATD");
  SimpleWrite(number);
  SimpleWriteln(";");
  delay(milliseconds);
  SimpleWriteln("ATH");

  return true;

}

int SIMCOM900::setPIN(char *pin)
{
  //Status = READY or ATTACHED.
  if((getStatus() != IDLE))
    return 2;

  //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to set PIN.
  SimpleWrite(F("AT+CPIN="));
  SimpleWriteln(pin);

  //Expect "OK".

  if(gsm.WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
    return 0;
  else
    return 1;
}

int SIMCOM900::changeNSIPmode(char mode)
{
    //_tf.setTimeout(_TCP_CONNECTION_TOUT_);

    //if (getStatus()!=ATTACHED)
    //    return 0;

    //_cell.flush();

    SimpleWrite(F("AT+QIDNSIP="));
	SimpleWriteln(mode);
	if(gsm.WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV) return 0;
    //if(!_tf.find("OK")) return 0;

    return 1;
}

int SIMCOM900::getCCI(char *cci)
{
  //Status must be READY
  if((getStatus() != READY))
    return 2;

  //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to get CCID.
  SimpleWriteln(F("AT+QCCID"));

  //Read response from modem
  #ifdef UNO
	_tf.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);
  #endif
  #ifdef MEGA
	_cell.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);
  #endif

  //Expect "OK".
  if(gsm.WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
    return 0;
  else
    return 1;
}

int SIMCOM900::getIMEI(char *imei)
{

  //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to get IMEI.
  SimpleWriteln(F("AT+GSN"));

  //Read response from modem
  #ifdef UNO
	_tf.getString("\r\n","\r\n",imei, 16);
  #endif
  #ifdef MEGA
	_cell.getString("\r\n","\r\n",imei, 16);
  #endif

  //Expect "OK".
  if(gsm.WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
    return 0;
  else
    return 1;
}

uint8_t SIMCOM900::read()
{
  while(_cell.available() <= 0);
  return _cell.read();
}

void SIMCOM900::SimpleRead()
{
	char datain;
	if(_cell.available()>0){
		datain=_cell.read();
		if(datain>0){
			Serial.print(datain);
		}
    //Serial.print(_cell.read());
	}
}

void SIMCOM900::SimpleWrite(char *comm)
{
	_cell.print(comm);
}

void SIMCOM900::SimpleWrite(const char *comm)
{
	_cell.print(comm);
}

void SIMCOM900::SimpleWrite(int comm)
{
	_cell.print(comm);
}

void SIMCOM900::SimpleWrite(const __FlashStringHelper *pgmstr)
{
	_cell.print(pgmstr);
}

void SIMCOM900::SimpleWriteln(char *comm)
{
	_cell.println(comm);
}

void SIMCOM900::SimpleWriteln(String comm)
{
	_cell.println(comm);
}

void SIMCOM900::SimpleWriteln(const __FlashStringHelper *pgmstr)
{
	_cell.println(pgmstr);
}

void SIMCOM900::SimpleWriteln(char const *comm)
{
	_cell.println(comm);
}

void SIMCOM900::SimpleWriteln(int comm)
{
	_cell.println(comm);
}

void SIMCOM900::WhileSimpleRead()
{
	char datain;
	while(_cell.available()>0){
		datain=_cell.read();
		if(datain>0){
			Serial.print(datain);
		}
	}
}


byte GSM::IsRegistered(void)
{
  return (module_status & STATUS_REGISTERED);
}

byte GSM::IsInitialized(void)
{
  return (module_status & STATUS_INITIALIZED);
}


/**********************************************************
Method checks if the GSM module is registered in the GSM net
- this method communicates directly with the GSM module
  in contrast to the method IsRegistered() which reads the
  flag from the module_status (this flag is set inside this method)

- must be called regularly - from 1sec. to cca. 10 sec.

return values:
      REG_NOT_REGISTERED  - not registered
      REG_REGISTERED      - GSM module is registered
      REG_NO_RESPONSE     - GSM doesn't response
      REG_COMM_LINE_BUSY  - comm line between GSM module and Arduino is not free
                            for communication
**********************************************************/
byte GSM::CheckRegistration(void)
{
  byte status;
  byte ret_val = REG_NOT_REGISTERED;

  if (CLS_FREE != GetCommLineStatus()) return (REG_COMM_LINE_BUSY);
  SetCommLineStatus(CLS_ATCMD);
  _cell.println(F("AT+CREG?"));
  // 5 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  status = WaitResp(5000, 50);

  if (status == RX_FINISHED) {
    // something was received but what was received?
    // ---------------------------------------------
    if(IsStringReceived("+CREG: 0,1")
      || IsStringReceived("+CREG: 0,5")) {
      // it means module is registered
      // ----------------------------
      module_status |= STATUS_REGISTERED;


      // in case GSM module is registered first time after reset
      // sets flag STATUS_INITIALIZED
      // it is used for sending some init commands which
      // must be sent only after registration
      // --------------------------------------------
      if (!IsInitialized()) {
        module_status |= STATUS_INITIALIZED;
        SetCommLineStatus(CLS_FREE);
        InitParam(PARAM_SET_1);
      }
      ret_val = REG_REGISTERED;
    }
    else {
      // NOT registered
      // --------------
      module_status &= ~STATUS_REGISTERED;
      ret_val = REG_NOT_REGISTERED;
    }
  }
  else {
    // nothing was received
    // --------------------
    ret_val = REG_NO_RESPONSE;
  }
  SetCommLineStatus(CLS_FREE);


  return (ret_val);
}
