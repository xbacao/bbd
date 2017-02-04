#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

void daemonize(const char* pid_file, sighandler_t handler);

void send_stop_signal(const char* pid_file);

#endif
