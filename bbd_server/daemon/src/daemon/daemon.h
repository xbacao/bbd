#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <signal.h>

void daemonize(const char* pid_file, sighandler_t handler);

void send_stop_signal(const char* pid_file);

#endif
