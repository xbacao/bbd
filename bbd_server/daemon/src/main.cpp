#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "daemon.h"
#include "socket_data.h"
#include "db.h"
#include "log.h"

#define PID_FILE        "/var/run/bbda_server.pid"

#define SERVER_PORT     7777
#define N_VALVES        5     //ESTATICO, POSSIVEL MUDAR PRA DB

using namespace std;

static bool _stop_flag=false;

static void _signal_callback_handler(int signum){
   log_file << time(nullptr) <<": "<<"Caught signal " << signum<<endl;

   _stop_flag=true;
}

static int _start_socket_server(int* sockfd){
  struct sockaddr_in serv_addr;
  *sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    log_file << time(nullptr) <<": "<<"Error creating socket descriptor!"<<endl;
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(SERVER_PORT);
  if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    log_file << time(nullptr) <<": "<<"Error binding socket!"<<endl;
    return 2;
  }
  listen(*sockfd,5);
  return 0;
}

static int _run_server(){
  int newsockfd,n;
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  uint msg_size, msg_type, arduino_id;

  //init_db_handler();
  init_log();

  int sockfd;
  do{
    n=_start_socket_server(&sockfd);
    if(n){
      log_file << time(nullptr)<<": Error starting server"<<endl;
    }
  } while(n && !_stop_flag);

  log_file << time(nullptr) <<": "<<"Server initialized" << endl;

  while(!_stop_flag){
    log_file << time(nullptr) <<": "<<"Waiting for new connection"<<endl;
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
      log_file << time(nullptr) <<": "<<"Error accepting connection!"<<endl;
      return 1;
    }
    else{
      log_file << time(nullptr) <<": "<<"New connection accepted from "<< inet_ntoa(cli_addr.sin_addr)<<endl;
    }

    n=recv_socket_header(newsockfd, &arduino_id, &msg_type, &msg_size);
    if(n==0){
      switch(msg_type){
        default:
          break;
        case SYNC_TIME_MSG:
          n=send_time(newsockfd);
          if(n){
            log_file<<time(nullptr)<<": ERROR SENDING TIME MESSAGE"<<endl;
          }
          break;
        case CHECKIN_MSG:
        {
          schedule_data t_data;
          for(int i=0;i<N_VALVES;i++){
            n=get_new_schedule(i,&t_data);
            if(n==0){
              n=send_schedule(newsockfd, t_data);
              if(n!=0){
                log_file << time(nullptr)<<": MAIN : Error sending schedule"<<endl;
                break;
              }
              n=wait_schedule_reply(sockfd, t_data.schedule_id);
              if(n!=0){
                log_file<<time(nullptr)<<": MAIN : Error waiting schedule reply"<<endl;
              }
              n=set_schedule_sent(t_data.schedule_id);
              if(n!=0){
                log_file<<time(nullptr)<<": MAIN : Error setting schedule sent flag in db"<<endl;
              }
            }
          }
          break;
        }
      }
    }
    close(newsockfd);
  }
  close(sockfd);
  stop_log();
  return 0;
}


int main(int argc, char *argv[]){
  int act=0;
  if(argc == 2){
    if(strcmp(argv[1], "start")==0){
      act=1;
    }
    else if(strcmp(argv[1], "stop")==0){
      act=2;
    }
  }


  switch(act){
    default:
      cout << "Usage: bbda_server [exec_options] [sett_options]"<<endl;
      cout << "exec_options: start stop"<<endl;
      cout << "sett_options: none"<<endl;
      return 0;
      break;
    case 1:
      //daemonize(PID_FILE, _signal_callback_handler);
      return _run_server();
      break;
    case 2:
      send_stop_signal(PID_FILE);
      return 0;
      break;
  }

  return 0;
}
