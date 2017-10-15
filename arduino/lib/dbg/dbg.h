#ifndef _DBG_H_
#define _DBG_H_

void dbg_print_serial_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size);

void dbg_print_sock_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size);

#endif
