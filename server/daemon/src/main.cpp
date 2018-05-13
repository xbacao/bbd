#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "daemon/daemon.h"
#include "socket_data/socket_data.h"
#include "db/db.h"
#include "log/log.h"

#define PID_FILE        "/var/run/bbd_server.pid"
#define LOG_FILE_PATH   "/var/log/bbd_server_daemon.log"

#define SERVER_PORT     7777

#define MAGIC_NUMER     38017


using namespace std;

static int _start_socket_server(int* sockfd){
  struct sockaddr_in serv_addr;
  struct timeval tv;

  *sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    log_error("creating socket descriptor!");
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(SERVER_PORT);
  if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    log_error("binding socket!");
    return 2;
  }

  tv.tv_sec = 30;  /* 30 Secs Timeout */
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

  listen(*sockfd,5);
  return 0;
}

static int _run_server(){
  int newsockfd,n;
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  uint8_t msg_size, msg_type, device_id;
  int sockfd;

  n=init_logs_file(LOG_FILE_PATH);
  if(n){
    return 1;
  }

  do{
    n=_start_socket_server(&sockfd);
    if(n){
      log_error("starting server");
      usleep(1000000);
    }
  } while(n);

  log_info("server started");

  while(true){
    log_info("waiting for new connection");
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
      log_error("accepting connection!");
    }
    else{

      n=recv_socket_header(newsockfd, MAGIC_NUMER, &device_id, &msg_type,
        &msg_size);
      if(n==0){
        log_info("request received");
        log_request(inet_ntoa(cli_addr.sin_addr), device_id, msg_type, msg_size);
        switch(msg_type){
          default:
            break;
          case GET_ACTIVE_SCHES_MSG:
          {
            vector<schedule> sches;
            n=db_get_active_schedules(device_id, sches);
            if(n){
              log_error("receiving GET_ACTIVE_SCHES_MSG");
            }

            n=send_schedules_msg(newsockfd, sches);
            if(n){
              log_error("sending GET_ACTIVE_SCHES_MSG response");
            }
            break;
          }
          case CHECKIN_MSG:
          {
            //TODO
            break;
          }
          case SCHE_ACT_MSG:
          {
            //TODO
            break;
          }
        }
      }
      else{
        log_error("receiving message request header");
      }
      close(newsockfd);
    }
  }
  close(sockfd);
  close_logs();
  return 0;
}


int main(int argc, char *argv[]){
  // int act=0;
  // if(argc == 2){
  //   if(strcmp(argv[1], "start")==0){
  //     act=1;
  //   }
  //   else if(strcmp(argv[1], "stop")==0){
  //     act=2;
  //   }
  // }
  //
  //
  // switch(act){
  //   default:
  //     cout << "Usage: bbda_server [exec_options] [sett_options]"<<endl;
  //     cout << "exec_options: start stop"<<endl;
  //     cout << "sett_options: none"<<endl;
  //     return 0;
  //     break;
  //   case 1:
  //     // daemonize(PID_FILE, _signal_callback_handler);
  //     return _run_server();
  //     break;
  //   case 2:
  //     send_stop_signal(PID_FILE);
  //     return 0;
  //     break;
  // }
  //
  // return 0;
  return _run_server();
}
