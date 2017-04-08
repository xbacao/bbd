#include "socket_bbd.h"

int get_time_request_msg(char** msg){
    memcpy(*msg, &SOCKET_HEADER, sizeof(SOCKET_HEADER));
    memcpy(*msg+sizeof(SOCKET_HEADER), &SYNC_TIME_MSG, 1);
    return 0;
}
