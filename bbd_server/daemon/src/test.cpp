#include "log.h"
#include "socket_data.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

static void _send_header(int sock){
	uint16_t s_header=htons(SOCKET_HEADER);
	write(sock, &s_header, sizeof(s_header));
	uint8_t ard_id=1;
	write(sock, &ard_id, 1);
	uint8_t msg_type=CHECKIN_MSG;
	write(sock, &msg_type, 1);
	uint8_t msg_size=0;
	write(sock, &msg_size, 1);
}

int main(void){
	int sock,n;
	struct sockaddr_in server;
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 7777 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	_send_header(sock);

	ArduinoSchedules a_s(1);
	uint16_t size;
	n=recv(sock, &size, sizeof(uint16_t), MSG_WAITALL);
	if(n<0) return 2;
	size=ntohs(size);
	char* buff= new char[size];
	n=recv(sock, buff, size, MSG_WAITALL);
	if(n<0){
		return 3;
	}
	n=a_s.decode_message(buff, size);
	if(n<0){
		std::cout << "ERROR "<<n<<std::endl;
	}
	else{
		std::cout << "SUCCESS "<<a_s<<std::endl;
	}

	return 0;
}
