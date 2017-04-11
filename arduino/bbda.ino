#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <stdio.h>
#include "gsm_ard.h"
#include "socket_bbd.h"
#include "network.h"
#include "bbda.h"

#define ARDUINO_ID    1

#define R_OP_VALVE    "1+"
#define MSG_SIZE      50
#define N_SEPARATORS  4

#define REQUEST_CHECK_MULT  1            //arduino checks requests in multiples of 15 mins

#define ATTACH_TIMEOUT  5

bool timeSynced = false;

Gsm_Ard gsm;

int result;
int i=0;
boolean started=false;

bool request_checked = false;

void setup()
{
  int n;
  /* setup timer */
  //setSyncProvider(requestSync);
  /****/

  Serial.begin(9600);

  gsm=Gsm_Ard();

  /* setup gsm */
  Serial.println("GSM Shield init");

  do{
    n=gsm.init_gsm_module();
    if(n){
      Serial.print("ERROR: COULD NOT INITIALIZE GSM MODULE ");
      Serial.println(n);
      delay(10000);
    }
  }while(n);

  do{
    n=gsm.attachGPRS();
    if(!n){
      started=true;
      n=syncTimeWithServer();
      if(n){
        Serial.print("ERROR: SYNC TIME ");
        Serial.println(n);
      }
      n=gsm.dettachGPRS();
      if(n){
        Serial.print("ERROR: DETTACH GPRS ");
        Serial.println(n);
      }
      else{
        Serial.println("CLOCK");
        digitalClockDisplay();
        started=true;
      }
    }
    else{
      Serial.print("ERROR: COULD NOT ATTACH GPRS ");
      Serial.println(n);
    }

  } while(!started);

}

void loop()
{
  // if(debug_gsm){
  //   if(Serial.available()>0){
  //     String str = Serial.readString();
  //     Serial.println(str);
  //     // gsm.SimpleWriteln(str);
  //     // gsm.WaitResp(5000, 50, "DEBUG");
  //   }
  // }
  // switch(ARDUINO_STATE){
  //   default:
  //     break;
  //   case ARDUINO_GSM_ON_STATE:
  //     if(!request_checked && minute()%REQUEST_CHECK_MULT == 0){
  //       Serial.println("CHECKING REQUESTS");
  //       gsm_ard.attachGPRS();
  //       checkRequests();
  //       request_checked = true;
  //       syncTimeWithServer();
  //       gsm_ard.dettachGPRS();
  //     }
  //     else{
  //       if(minute()%REQUEST_CHECK_MULT != 0 && request_checked){
  //         request_checked = false;
  //       }
  //     }
  //     break;
  // }
}

void requestSync(){
  timeSynced = false;
}

// /*
//   return codes
//   -1    error decoding time message
// */
// int setNewTime(){
//   uint64_t time_v;
//   int n;
//
//   n = decodeTime(&time_v);
//
//   if(n != 1){
//       #ifdef DEBUG_ON
//         Serial.println("DB:ERROR DECODING TIME!");
//       #endif
//     return -1;
//   }
//
//   /* convert from ms to s and gmt to gmt+1*/
//   setTime((time_v/1000.0)+3600);
//   timeSynced = true;
//   return 1;
// }

/*
  return codes
  -1    error attaching gprs
  -2    bad http response
  -3    error setting new time
  -4    error detaching gprs
  -10   gprs module not active
*/
int syncTimeWithServer(){
  int n;
  unsigned int rsp_len;

  if(gsm.get_gsm_state()!=GSM_IP_STATE || !started){
    return 1;
  }

  char * msg = new char[REQUEST_TIME_MSG_SIZE];
  n=get_time_request_msg(&msg);
  if(n){
    return 2;
  }


  n=gsm.send_socket_msg(msg, REQUEST_TIME_MSG_SIZE, &rsp_len);
  if(n){
    return 3000+n;
  }

  char* rsp = new char[rsp_len];
  n=gsm.get_socket_rsp(&rsp);
  if(n){
    return 4000+n;
  }

  time_t cur_time;
  n=decode_time_rsp_msg(rsp, rsp_len, &cur_time);
  if(n){
    return 5000;
  }

  Serial.print("TIME:");
  Serial.println((long)cur_time);

  setTime(cur_time+3600);
  timeSynced = true;

  return 0;
}

/*
  return codes
  -1    error attaching gprs
  -2    bad http responde code
  -3    error handling request
  -4    error detaching gprs
  -10   gprs module not active
  0     no requests available
*/
int checkRequests(){
  // int n;
  // if(started){
  //   /*n = connectGPRS();
  //   if(n != 1){
  //     return -1;
  //   }*/
  //
  //   //Read IP address.
  //   gsm.SimpleWriteln("AT+CIFSR");
  //   delay(5000);
  //   //Read until serial buffer is empty.
  //   gsm.WhileSimpleRead();
  //
  //   //TCP Client GET, send a GET request to the server and
  //   n = getRequestMsg();
  //   if(n != 1){
  //     Serial.println("DB:BAD HTTP RESPONSE!");
  //     return -2;
  //   }
  //   else if(n == 0){
  //     Serial.println("DB:NO REQUESTS AT THIS MOMENT");
  //     return 0;
  //   }
  //   else{
  //     if(strcmp(msg, "||||")==0){
  //       Serial.println("DB:NO REQUESTS AVAILABLE");
  //       return 0;
  //     }
  //     n = handleRequest();
  //     if(n != 1){
  //       Serial.println("DB:ERROR HANDLING REQUEST");
  //       return -3;
  //     }
  //   }
  //   return 1;
  // }
  // Serial.println("DB:GPRS MODULE NOT ACTIVE!");
  // return -10;
  return 1;
}

/*format: |requestID|valveID|action|
  return codes
  -1    error decoding
  -2    request not suported
*/
int handleRequest(){
  int n;
  int requestID, valveID, action;

  n = decodeRequest(&requestID, &valveID, &action);

  if(n != 1){
    Serial.println("DB:ERROR DECODING!");
    return -1;
  }


  switch(action){
    default:
      Serial.println("DB:REQUEST NOT SUPPORTED");
      return -2;
      break;
    case 1:       //open/close valve
      Serial.print("--RECEIVED!");
      Serial.print(requestID);
      Serial.print("-");
      Serial.print(valveID);
      Serial.print("-");
      Serial.print(action);
      Serial.print("-");
      break;
    case 2:       //schedule changed
      Serial.print("--RECEIVED!");
      Serial.print(requestID);
      Serial.print("-");
      Serial.print(valveID);
      Serial.print("-");
      Serial.print(action);
      Serial.print("-");
      break;

  }
  return 1;
}

/*
  return codes
  0   no requests available
*/
int getRequestMsg(){
  // return inet.httpGET(SERVER_IP, PORT, PATH_GET, msg);
  return 1;
}

int getTimeMsg(){
  // return inet.httpGET(SERVER_IP, PORT, PATH_TIME, msg);
  return 1;
}

int markRequestServed(int requestID){
  // return inet.httpGET(SERVER_IP, PORT, PATH_SERVED+requestID, msg);
  return 1;
}

/*
  return codes
  -1  msg not formated
  -2  not a time message
*/
// int decodeTime(uint64_t *time_64){
//   int i = 0;
//   int k = 0;
//   int indx[N_SEPARATORS];
//   for(; i < MSG_SIZE || k < N_SEPARATORS; i++ ){
//     if(msg[i] == '|'){
//       indx[k] = i;
//       k++;
//     }
//   }
//
//   if(k != N_SEPARATORS){
//     Serial.println("DB:MSG NOT FORMATTED (TIME)");
//     return -1;
//   }
//
//   /*check if t is there */
//   if(indx[0] != 0 or indx[1] != 2 or msg[1] != 't'){
//     Serial.println("DB:MSG NOT A TIME MSG");
//     return -2;
//   }
//
//   uint64_t l_time = 0ULL;
//   for(int i = indx[1]+1; i < indx[2]; i++){
//     l_time |= (0x000000000000000fLL & ((uint64_t)msg[i])) << 4*(i-indx[1]-1);
//   }
//
//   *time_64 = l_time;
//
//   return 1;
// }

/*
  return codes
  -1    message not formated
*/
// int decodeRequest(int *requestID, int *valveID, int *action){
//   int i = 0;
//   int k = 0;
//   int indx[N_SEPARATORS];
//   for(; i < MSG_SIZE || k < N_SEPARATORS; i++ ){
//     if(msg[i] == '|'){
//       indx[k] = i;
//       k++;
//     }
//   }
//
//   if(k != N_SEPARATORS){
//     Serial.println("DB:MSG NOT FORMATTED");
//     return -1;
//   }
//
//   char* p_end;
//
//   *requestID = strtol(msg+indx[0], &p_end, 10);
//   *valveID = strtol(msg+indx[1], &p_end, 10);
//   *action = strtol(msg+indx[2], &p_end, 10);
//
//   return 1;
//
// }

/*
  return codes
  -1  could not attach
*/

/*******timer debug **********/
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
