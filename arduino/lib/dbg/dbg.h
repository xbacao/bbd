#ifndef _DBG_H_
#define _DBG_H_

#define DEBUG
#define DEBUG_SS
#define DEBUG_SOCKET

#ifdef DEBUG
void dbg_print_serial_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size);

void dbg_print_sock_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size);

void dbg_print_error(const char* file, int line, int code);
#endif

#endif
