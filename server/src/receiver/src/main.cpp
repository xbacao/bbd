#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "socket_data/socket_data.h"
#include "db/db.h"
#include "log/log.h"

#define PID_FILE        "/var/run/bbd_server.pid"
#define LOG_FILE_PATH   "/var/log/bbd_server_daemon.log"

#define SERVER_PORT       7777
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
  uint8_t msg_type_int, device_id;
  uint16_t msg_size, magic_number;
  int sockfd;
  char* client_ip_address;
  enum request_type msg_type;

  n=init_logs_file(LOG_FILE_PATH);
  if(n){
    return 1;
  }

  do{
    n=_start_socket_server(&sockfd);
    if(n){
      usleep(5000000);
    }
  } while(n);

  log_info("server started");

  while(true){
    log_info("waiting for new connection");
    clilen = sizeof(cli_addr);
    do{
      newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0 && errno != EAGAIN){
        log_error("accepting connection!");
      }
    } while(newsockfd<0);
    n=recv_socket_header(newsockfd, &magic_number, &device_id, &msg_type_int,
      &msg_size);
    msg_type=(enum request_type) msg_type_int;
    if(n==0){
      client_ip_address=inet_ntoa(cli_addr.sin_addr);
      if(magic_number!=MAGIC_NUMER){
        log_error(client_ip_address, "bad magic number!");
        log_request(client_ip_address, device_id, msg_type);
      } else {
        log_info("request received");
        log_request(client_ip_address, device_id, msg_type);
        switch(msg_type){
          default:
            break;
          case GET_DEVICE_VALVES_MSG:
          {
            uint16_t rsp_len;
            char* rsp;
            vector<uint16_t> valve_ids;

            n=db_get_device_valves(device_id, valve_ids);
            if(n){
              log_error("db fetch GET_DEVICE_VALVES_MSG");
              break;
            }

            rsp=craft_device_valves_rsp(valve_ids);
            rsp_len=sizeof(uint16_t)*valve_ids.size();

            n=send_rsp_msg(newsockfd, rsp, rsp_len);
            if(n){
              log_error("sending GET_DEVICE_VALVES_MSG response");
            } else {
              log_response(client_ip_address, device_id, msg_type, rsp_len);
            }

            free(rsp);

            break;
          }
          case GET_ACTIVE_SCHES_MSG:
          {
            vector<schedule> sches;
            vector<bool> sents;
            uint16_t rsp_len;
            char* rsp;

            n=db_get_active_schedules(device_id, sches, sents);
            if(n){
              log_error("db fetch GET_ACTIVE_SCHES_MSG");
              break;
            }

            rsp_len=sizeof(schedule)*sches.size();

            rsp=craft_active_schedules_rsp(sches);

            n=send_rsp_msg(newsockfd, rsp, rsp_len);
            if(n){
              log_error("sending GET_ACTIVE_SCHES_MSG response");
            } else {
              log_response(client_ip_address, device_id, msg_type, rsp_len);
            }

            free(rsp);

            n=set_schedules_sent(sches, device_id);
            if(n){
              log_error("db call set_schedules_sent");
            }

            n=deactivation_requests_done();
            if(n){
              log_error("db call deactivation_requests_done");
            }
            break;
          }
          case CHECK_NEW_SCHEDULES_MSG:
          {
            vector<schedule> sches;
            vector<bool> sents;
            uint16_t rsp_len;
            bool need_update=false;
            char* rsp;

            n=db_get_active_schedules(device_id, sches, sents);
            if(n){
              log_error("db fetch GET_ACTIVE_SCHES_MSG");
              break;
            }

            for(auto itr=sents.begin();itr!=sents.end();itr++){
              if(!(*itr)){
                need_update=true;
              }
            }

            if(need_update){
              rsp_len=sizeof(schedule)*sches.size();
              rsp=craft_active_schedules_rsp(sches);
            } else {
              rsp_len=0;
              rsp=NULL;
            }

            n=send_rsp_msg(newsockfd, rsp, rsp_len);
            if(n){
              log_error("sending CHECK_NEW_SCHEDULES_MSG response");
            } else {
              log_response(client_ip_address, device_id, msg_type, rsp_len);
            }

            free(rsp);

            n=set_schedules_sent(sches, device_id);
            if(n){
              log_error("db call set_schedules_sent");
            }
            break;
          }
        }
      }
    }
    else{
      log_error("receiving message request header");
    }
    close(newsockfd);

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
