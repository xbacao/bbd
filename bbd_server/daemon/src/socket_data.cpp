#include "socket_data.h"

using namespace std;

static int _recv_64(void* data, int sock_fd){
	int n;
	char buffer_32[BUFFER_SIZE_32];
	char buffer_64[BUFFER_SIZE_64];
	uint32_t temp_32;

	for(uint i=0;i<(BUFFER_SIZE_64/BUFFER_SIZE_32);i++){
		n=read(sock_fd, buffer_32, BUFFER_SIZE_32);
		if(n<0){
			return n;
		}
		memcpy(&temp_32, buffer_32, BUFFER_SIZE_32);
		temp_32 = ntohl(temp_32);
		memcpy(buffer_64+i*BUFFER_SIZE_32, &temp_32, BUFFER_SIZE_32);
	}
	memcpy(data, buffer_64, BUFFER_SIZE_64);
	return 0;
}

static int _recv_16(void* data, int sock_fd){
	int n;
	char buffer_16[BUFFER_SIZE_16];
	uint16_t temp_16;

	n=read(sock_fd, buffer_16, BUFFER_SIZE_16);
	if(n<0){
		return n;
	}

	memcpy(&temp_16, buffer_16, BUFFER_SIZE_16);
	temp_16 = ntohs(temp_16);
	memcpy(data, &temp_16, BUFFER_SIZE_16);
	return 0;
}

int recv_socket_header(int sock_fd, uint8_t* arduino_id, uint8_t* trans_type, uint8_t* trans_size){
  uint16_t temp_16;
  char buffer[1];
  int n;
  n= _recv_16(&temp_16, sock_fd);
  if(n!=0){
    return 1;
  }
  if(temp_16 != SOCKET_HEADER){
		log_file<<time(nullptr)<<"ERROR: bad header:"<<temp_16<<endl;
    return 2;
  }

  bzero(buffer, 1);
  n=read(sock_fd,buffer,1);
  if (n < 0){
    return 3;
  }
  *arduino_id= (uint8_t) (buffer[0]);

  bzero(buffer, 1);
  n=read(sock_fd,buffer,1);
  if (n < 0){
    return 4;
  }
  *trans_type= (uint8_t) (buffer[0]);
  if(*trans_type!=SYNC_TIME_MSG && *trans_type!=LAST_SCHE_MSG && *trans_type!=CHECKIN_MSG && *trans_type!=SCHE_ACT_MSG){
    return 5;
  }

  bzero(buffer, 1);
  n=read(sock_fd,buffer,1);
  if (n < 0){
    return 6;
  }
  *trans_size= (uint) (buffer[0]);
  if(*trans_size<0 || *trans_size>128){
    return 7;
  }

  return 0;
}


int send_time(int sock_fd){
	char* msg=new char[TIME_RSP_SIZE+2*sizeof(uint8_t)];

	memcpy(msg, &TIME_RSP_SIZE, sizeof(uint8_t));

	uint32_t curr_time=htonl(time(nullptr));
	memcpy(msg+sizeof(uint8_t), &curr_time, sizeof(uint32_t));

	memcpy(msg+sizeof(uint8_t)+sizeof(uint32_t), &END_TRANS_CHAR, sizeof(uint8_t));

	return write(sock_fd, msg, TIME_RSP_SIZE+2*sizeof(uint8_t))<0;
}

int send_msg(char* msg, uint32_t size, int sock_fd){
	return write(sock_fd, msg, size)<0;
}

int send_empty_msg(int sock_fd){
	return write(sock_fd, &END_TRANS_CHAR, sizeof(uint8_t))<0;
}

int recv_sche_act_msg_size(int sock_fd, uint16_t* size){
	uint16_t temp;
	int n;
	n=read(sock_fd, &temp, BUFFER_SIZE_16);
	if(n<0){
		return 1;
	}

	*size=ntohs(temp);
	return 0;
}

int recv_sche_act_msg(int sock_fd, uint16_t size, uint16_t **sche_ids){
	uint16_t temp,i;
	for(i=0;i<size;i++){
		if(read(sock_fd, &temp, BUFFER_SIZE_16)<0){
			return 1;
		}
		(*sche_ids)[i]=ntohs(temp);
	}
	return 0;
}

//FOR TESTING
time_t get_time(int sock_fd){
	time_t currt;
	_recv_64(&currt, sock_fd);
	return currt;
}
