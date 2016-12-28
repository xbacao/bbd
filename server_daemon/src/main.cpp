#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "daemon.h"
#include "socket_data.h"

#define PID_FILE        "/var/run/bbda_server.pid"
#define LOG_DIR_PATH    "/opt/bbda_server/logs/"

#define SERVER_PORT     6666

using namespace std;

static bool stop_flag=false;
static int sockfd;

static ofstream log_file;

static void _signal_callback_handler(int signum){
   log_file << time(nullptr) <<": "<<"Caught signal " << signum<<endl;

   stop_flag=true;
}

static int _start_socket_server(){
  struct sockaddr_in serv_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    log_file << time(nullptr) <<": "<<"Error creating socket descriptor!"<<endl;
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(SERVER_PORT);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    log_file << time(nullptr) <<": "<<"Error binding socket!"<<endl;
    return 2;
  }
  listen(sockfd,5);
  return 0;
}

static int _run_server(){
  int newsockfd,n;
  socklen_t clilen;
  struct sockaddr_in cli_addr;
  uint trans_size, trans_type, rasp_id;
  char rsp[1];


  stringstream ss_log;
  time_t curr_ts = time(nullptr);
  ss_log << LOG_DIR_PATH << "log" << curr_ts;
  log_file.open(ss_log.str());

  //init_db_handler();


  do{
    n=_start_socket_server();
    sleep(5);
  }while(n!=0);

  log_file << time(nullptr) <<": "<<"Server initialized" << endl;


  while(!stop_flag){
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

    n=recv_socket_header(newsockfd, &rasp_id, &trans_type, &trans_size);
    if(n==0){
      switch()
      
    }
    close(newsockfd);
  }
  close(sockfd);
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
      daemonize(PID_FILE, _signal_callback_handler);
      return _run_server();
      break;
    case 2:
      send_stop_signal(PID_FILE);
      return 0;
      break;
  }

  return 0;
}