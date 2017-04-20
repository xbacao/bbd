#include "daemon.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

using namespace std;

void daemonize(const char* pid_file, sighandler_t handler){

  pid_t pid = fork();
  /* An error occurred */
  if (pid < 0){
    exit(EXIT_FAILURE);
  }

  /* Success: Let the parent terminate */
  if (pid > 0){
    exit(EXIT_SUCCESS);
  }

  pid_t pgid = setsid();

  signal(SIGUSR1, handler);
  chdir("/");
  umask(0);

  /*for(int x=sysconf(_SC_OPEN_MAX); x>0; x--){
    close (x);
  }*/

  ofstream pid_os;
  pid_os.open(pid_file);
  pid_os << pgid << endl;
  pid_os.close();


  //freopen("stdout.txt","w",stdout);
  //freopen("stderr.txt","w",stderr);
}

void send_stop_signal(const char* pid_file){
  ifstream pid_is;
  stringstream ss;
  pid_is.open(pid_file);
  ss << pid_is.rdbuf();
  string str = ss.str();
  pid_t pid = (pid_t) stol(str);
  kill(pid, SIGUSR1);
}
