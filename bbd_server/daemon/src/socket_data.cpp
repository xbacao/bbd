#include "socket_data.h"

using namespace std;

static int _send_16(void* data, int sock_fd){
	uint16_t temp_16;
	char buffer_16[BUFFER_SIZE_16];
	int n;

	memcpy(&temp_16, data, BUFFER_SIZE_16);
	temp_16 = htons(temp_16);
	memcpy(buffer_16, &temp_16, BUFFER_SIZE_16);
	n=write(sock_fd, buffer_16, BUFFER_SIZE_16);
	if(n<0){
		return n;
	}
	return 0;
}

// static int _prep_to_send_64(void* data, char** msg){
//   uint32_t temp_32;
//   char buffer_64[BUFFER_SIZE_64];
//   char buffer_32[BUFFER_SIZE_32];
//   memcpy(buffer_64, data, BUFFER_SIZE_64);
//
//   for(uint i=0;i<(BUFFER_SIZE_64/BUFFER_SIZE_32);i++){
//     memcpy(&temp_32, buffer_64+i*BUFFER_SIZE_32, BUFFER_SIZE_32);
//     temp_32 = htonl(temp_32);
//     memcpy(buffer_32, &temp_32, BUFFER_SIZE_32);
// 		memcpy((*msg)+BUFFER_SIZE_32*i, &temp_32, BUFFER_SIZE_32);
//
//   }
//   return 0;
// }

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
  if(*trans_type!=SCHEDULE_MSG && *trans_type!=SYNC_TIME_MSG){
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

int send_schedule(int sock_fd, schedule_data data){
  int n,i;
  n=_send_16(&(data.valve_id), sock_fd);
  if(n<0){
    return -1;
  }

  n=_send_16(&(data.schedule_id), sock_fd);
  if(n<0){
    return -2;
  }

  n=_send_16(&(data.n_cicles), sock_fd);
  if(n<0){
    return -3;
  }

  for(i=0;i<data.n_cicles;i++){
    n=_send_16(data.cicle_starts+i,sock_fd);
    if(n<0){
    	return -4;
    }

    n=_send_16(data.cicle_stops+i,sock_fd);
    if(n<0){
    	return -5;
    }
  }
  return 0;
}

// int prep_to_send_trans_size(char** msg, uint8_t t_size){
// 	return write(sock_fd, &t_size, sizeof(uint8_t));
// }

int send_time(int sock_fd){
	char* msg=new char[TIME_RSP_SIZE+2*sizeof(uint8_t)];

	memcpy(msg, &TIME_RSP_SIZE, sizeof(uint8_t));

	uint32_t curr_time=htonl(time(nullptr));
	memcpy(msg+sizeof(uint8_t), &curr_time, sizeof(uint32_t));

	memcpy(msg+sizeof(uint8_t)+sizeof(uint32_t), &END_TRANS_CHAR, sizeof(uint8_t));

	return write(sock_fd, msg, TIME_RSP_SIZE+2*sizeof(uint8_t))<0;
}

std::ostream& operator<<(std::ostream& os, schedule_data &data){
  os << "{valve_id:"<<data.valve_id<<", schedule_id:"<<data.schedule_id<<", n_cicles:"<<data.n_cicles<<" cicle_starts:[";
  for(uint16_t i =0;i<data.n_cicles;i++){
    if(i!=0){
      os << ",";
    }
    os << data.cicle_starts[i];
  }
  os << "], cicle_stops:[";
  for(uint16_t i =0;i<data.n_cicles;i++){
    if(i!=0){
      os << ",";
    }
    os << data.cicle_stops[i];
  }
  os << "]}";
  return os;
}

/*
int recv_schedule_data(int sock_fd, schedule_data* data){
	int n,i, size_16;
	uint16_t n_cicles;
	uint16_t* cicle_starts;
	uint16_t* cicle_stops;

	size_16=sizeof(uint16_t);

	//VALVEID

	bzero(&n_cicles, size_16);
	n=_recv_16(&n_cicles, sock_fd);
	if(n<0){
    	return -1;
  	}

  	cicle_starts = new uint16_t[n_cicles];
  	cicle_stops = new uint16_t[n_cicles];
  	for(i=0;i<n_cicles;i++){
  		bzero(cicle_starts+i, size_16);
  		n=_recv_16(cicle_starts+i, sock_fd);
  		if(n<0){
  			return -2;
  		}

  		bzero(cicle_stops+i, size_16);
  		n=_recv_16(cicle_stops+i, sock_fd);
  		if(n<0){
  			return -3;
  		}
  	}

  	*data={.n_cicles=n_cicles, .cicle_starts=cicle_starts, .cicle_stops=cicle_stops};
  	return 0;
}*/

int wait_schedule_reply(int sock_fd, int schedule_id){
  int n;
  char rsp;
  n=read(sock_fd, &rsp, sizeof(char));
  if(n!=0){
    return -1;
  }
  return !(rsp==(char)schedule_id);
}

time_t get_time(int sock_fd){
	time_t currt;
	_recv_64(&currt, sock_fd);
	return currt;
}
