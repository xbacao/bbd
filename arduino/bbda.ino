#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <stdio.h>
#include "gsm_ard.h"
#include "socket_bbd.h"
#include "arduino_cfg.h"
#include "bbda.h"
#include "scheduler.h"

Gsm_Ard gsm;

bool started=false;
bool request_checked = false;
bool timeSynced = false;
time_t last_sync;
time_t last_ci;

void setup()
{
  int n;
  bool error=false;
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
    error=false;
    n=gsm.attachGPRS();
    if(n){
      Serial.print("ERROR: COULD NOT ATTACH GPRS ");
      Serial.println(n);
    }
    else{
      if(!timeSynced){
        n=sync_time_with_server();
        if(n){
          Serial.print("ERROR: SYNC TIME ");
          Serial.println(n);
          error=true;
        }
      }
      if(!error){
        n=get_last_schedule();
        if(n){
          Serial.print("ERROR: GET LAST SCHE ");
          Serial.println(n);
        }
        else{
          started=true;
        }
      }
    }
    n=gsm.dettachGPRS();
    if(n){
      Serial.print("ERROR: DETTACH GPRS ");
      Serial.println(n);
    }
  } while(!started);

}

void loop()
{
  int n;

  bool need_time_sync=now()-last_sync>SYNC_TIME;
  bool need_ci=now()-last_ci>CI_TIME;
  
  if(need_time_sync || need_ci){
    n=gsm.attachGPRS();
    if(n){
      Serial.print("ERROR ATTACH GPRS ");
      Serial.println(n);
      goto scheduler_ops;
    }
    if(need_time_sync){
      n=sync_time_with_server();
      if(n){
        Serial.print("ERROR SYNC TIME ");
        Serial.println(n);
        gsm.dettachGPRS();
        goto scheduler_ops;
      }
    }
    if(need_ci){
      n=checkin_server();
      if(n){
        Serial.print("ERROR CHECK IN SERVER ");
        Serial.println(n);
        gsm.dettachGPRS();
        goto scheduler_ops;
      }
    }
    n=gsm.dettachGPRS();
    if(n){
      Serial.print("ERROR DETTACHING GPRS ");
      Serial.println(n);
      goto scheduler_ops;
    }
  }
scheduler_ops:
  scheduler_action();
}

int sync_time_with_server(){
  int n;
  unsigned int rsp_len;

  if(gsm.get_gsm_state()!=GSM_IP_STATE){
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

  setTime(cur_time+3600);
  timeSynced = true;
  last_sync=now();

  return 0;
}

int get_last_schedule(){
  int n;
  char* msg;
  unsigned int rsp_len;

  if(gsm.get_gsm_state()!=GSM_IP_STATE){
    return 1;
  }

  msg=new char[GET_LAST_SCHE_MSG_SIZE];
  n=get_last_sche_msg(&msg);
  if(n){
    return 2000+n;
  }

  n=gsm.send_socket_msg(msg, GET_LAST_SCHE_MSG_SIZE, &rsp_len);
  if(n){
    return 3000+n;
  }

  char* rsp = new char[rsp_len];
  n=gsm.get_socket_rsp(&rsp);
  if(n){
    return 4000+n;
  }

  ArduinoSchedules a_s(ARDUINO_ID);
  n=a_s.decode_message(rsp, rsp_len);
  if(n){
    return 5000+n;
  }

  n=init_scheduler(a_s);
  if(n){
    return 6000+n;
  }

  last_ci=now();
  return 0;
}

int checkin_server(){
  int n;
  char* msg;
  unsigned int rsp_len;

  if(gsm.get_gsm_state()!=GSM_IP_STATE){
    return 1;
  }

  msg=new char[CHECK_REQS_MSG_SIZE];
  n=get_check_requests_msg(&msg);
  if(n){
    return 2000+n;
  }

  n=gsm.send_socket_msg(msg, CHECK_REQS_MSG_SIZE, &rsp_len);
  if(n){
    return 3000+n;
  }

  char* rsp = new char[rsp_len];
  n=gsm.get_socket_rsp(&rsp);
  if(n){
    return 4000+n;
  }

  ArduinoSchedules a_s(ARDUINO_ID);
  n=a_s.decode_message(rsp, rsp_len);
  if(n){
    return 5000+n;
  }

  n=process_schedules(a_s);
  if(n){
    return 6000+n;
  }

  last_ci=now();
  return 0;
}


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
