#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <stdio.h>
#include <Arduino.h>
#include <gsm_ard.h>
#include <socket_bbd.h>
#include <dbg.h>
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

  Serial.begin(9600);

  gsm=Gsm_Ard();

  /* setup gsm */
  Serial.println("GSM Shield init");

  do{
    n=gsm.init_gsm_module();
    if(n){
      #ifdef DEBUG
      dbg_print_error(__FILE__, __LINE__, n);
      #endif
      delay(10000);
    }
  }while(n);

  do{
    n=gsm.attachGPRS();
    if(n){
      #ifdef DEBUG
      dbg_print_error(__FILE__, __LINE__, n);
      #endif
    }
    else{
      if(!timeSynced){
        n=sync_time_with_server();
        if(n){
          #ifdef DEBUG
          dbg_print_error(__FILE__, __LINE__, n);
          #endif
          error=true;
        }
      }
      if(!error){
        n=get_active_sches();
        if(n){
          #ifdef DEBUG
          dbg_print_error(__FILE__, __LINE__, n);
          #endif
        }
        started=true;
      }
      // if(!error){
      //   n=get_last_schedule();
      //   if(n){
      //     Serial.print("ERROR: GET LAST SCHE ");
      //     Serial.println(n);
      //   }
      //   else{
      //     started=true;
      //   }
      //   Serial.print("MEMORY=");
      //   Serial.println(freeMemory());
      // }
      // n=get_last_schedule();
      // if(n){
      //   Serial.print("ERROR: GET LAST SCHE ");
      //   Serial.println(n);
      // }
      // else{
      //   Serial.println("WHAT");
      // }
      n=gsm.dettachGPRS();
      if(n){
        #ifdef DEBUG
        dbg_print_error(__FILE__, __LINE__, n);
        #endif
      }
    }
    delay(1000);
  } while(!started);

}

void loop()
{
//   int n;
//
//   bool need_time_sync=now()-last_sync>SYNC_TIME;
//   bool need_ci=now()-last_ci>CI_TIME;
//
//   if(need_time_sync || need_ci){
//     n=gsm.attachGPRS();
//     if(n){
//       Serial.print("ERROR ATTACH GPRS ");
//       Serial.println(n);
//       goto scheduler_ops;
//     }
//     if(need_time_sync){
//       n=sync_time_with_server();
//       if(n){
//         Serial.print("ERROR SYNC TIME ");
//         Serial.println(n);
//         gsm.dettachGPRS();
//         goto scheduler_ops;
//       }
//     }
//     if(need_ci){
//       n=checkin_server();
//       if(n){
//         Serial.print("ERROR CHECK IN SERVER ");
//         Serial.println(n);
//         gsm.dettachGPRS();
//         goto scheduler_ops;
//       }
//     }
//     n=gsm.dettachGPRS();
//     if(n){
//       Serial.print("ERROR DETTACHING GPRS ");
//       Serial.println(n);
//       goto scheduler_ops;
//     }
//   }
// scheduler_ops:
//   scheduler_action();
}

int sync_time_with_server(){
  int n,err=0;
  unsigned int rsp_len;
  char* rsp;
  char* msg;

  n=gsm.connect_tcp_socket();
  if(n){
    #ifdef DEBUG
    if(err){
      dbg_print_error(__FILE__, __LINE__, 1+10*n);
    }
    #endif
    return 1+10*n;
  }

  msg = new char[REQUEST_TIME_MSG_SIZE];
  n=get_time_request_msg(&msg);
  if(n){
    err=2+10*n;
    goto exit_clean;
  }

  n=gsm.send_socket_msg(msg, REQUEST_TIME_MSG_SIZE, &rsp_len);
  if(n){
    err=3+10*n;
    goto exit_clean;
  }

  rsp = new char[rsp_len];
  n=gsm.get_socket_rsp(&rsp);
  if(n){
    err=4+10*n;
    goto exit_clean;
  }

  time_t cur_time;
  n=decode_time_rsp_msg(rsp, rsp_len, &cur_time);
  if(n){
    err=5+10*n;
    goto exit_clean;
  }

  setTime(cur_time+3600);
  timeSynced = true;
  last_sync=now();

  #ifdef DEBUG
  digitalClockDisplay();
  #endif

  err=0;
  goto exit_clean;

exit_clean:
  delete[] msg;
  delete[] rsp;
  gsm.disconnect_tcp_socket();
  #ifdef DEBUG
  if(err){
    dbg_print_error(__FILE__, __LINE__, err);
  }
  #endif
  return err;
}

int get_active_sches(){
  unsigned int rsp_len;
  char* rsp;
  char* msg;
  int err=0,n;
  schedule* sches;
  uint16_t n_sches;

  n=gsm.connect_tcp_socket();
  if(n){
    #ifdef DEBUG
    if(err){
      dbg_print_error(__FILE__, __LINE__, 1+10*n);
    }
    #endif
    return 1+10*n;
  }

  msg = new char[REQUEST_ACTIVE_SCHES_MSG_SIZE];
  n=get_active_request_msg(&msg);
  if(n){
    err=2+10*n;
    goto exit_clean;
  }

  n=gsm.send_socket_msg(msg, REQUEST_ACTIVE_SCHES_MSG_SIZE, &rsp_len);
  if(n){
    err=3+10*n;
    goto exit_clean;
  }

  rsp = new char[rsp_len];
  n=gsm.get_socket_rsp(&rsp);
  if(n){
    err=4+10*n;
    goto exit_clean;
  }

  n_sches=rsp_len/sizeof(schedule);
  sches=new schedule[n_sches];

  decode_sches_rsp(rsp, rsp_len, sches);

  set_active_schedules(sches, n_sches);

exit_clean:
  delete[] msg;
  delete[] rsp;
  gsm.disconnect_tcp_socket();
  #ifdef DEBUG
  dbg_print_error(__FILE__, __LINE__, err);
  #endif
  return err;
}

// int get_last_schedule(){
//   int n,err=0;
//   unsigned int rsp_len;
//   char* msg;
//   char* rsp;
//   Serial.print("MEMORY1=");
//   Serial.println(freeMemory());
//   ArduinoSchedules a_s(ARDUINO_ID);
//   Serial.print("MEMORY2=");
//   Serial.println(freeMemory());
//
//   if(gsm.get_gsm_state()!=GSM_IP_STATE){
//     return 1;
//   }
//
//   msg=new char[GET_LAST_SCHE_MSG_SIZE];
//   n=get_last_sche_msg(&msg);
//   if(n){
//     err=2+10*n;
//     // goto exit_clean_l;
//   }
//
//   n=gsm.send_socket_msg(msg, GET_LAST_SCHE_MSG_SIZE, &rsp_len);
//   if(n){
//     err=3+10*n;
//     // goto exit_clean_l;
//   }
//
//
//   Serial.print("MEMORY=");
//   Serial.println(freeMemory());
//
//   Serial.print("RSP LEN ");
//   Serial.println((unsigned int)rsp_len);
//   rsp = new char[rsp_len];
//   memset(rsp, 0, rsp_len);
//   n=gsm.get_socket_rsp(&rsp);
//   if(n){
//     err=4+10*n;
//     goto exit_clean_l;
//   }
//
//   //TODO EMPANCA
//
//   n=a_s.decode_message(rsp, rsp_len);
//   if(n){
//     err=5+10*n;
//     goto exit_clean_l;
//   }
//
//   n=init_scheduler(a_s);
//   if(n){
//     err=6+10*n;
//     goto exit_clean_l;
//   }
//
//   last_ci=now();
//
//   err=0;
//   // goto exit_clean_l;
//
// exit_clean_l:
//   delete[] msg;
//   delete[] rsp;
//   Serial.println("HERE");
//   Serial.println(err);
//   return err;
// }


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