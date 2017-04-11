#include "db.h"
#include "log.h"
#include "socket_data.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

int main(void){
	// int n;
	// init_log();
	// schedule_data data;
	// n=get_new_schedule(1,&data);
	// if(n!=0){
	// 	std::cout << "ERROR GETTING SCHEDULE "<<n<<std::endl;
	// }
	// std::cout << data << std::endl;
	// exit(1);

	int sockfd, servlen,n;
	struct sockaddr_un  serv_addr;
	char buffer[82];

	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "127.0.0.1");
	servlen = strlen(serv_addr.sun_path) +
								sizeof(serv_addr.sun_family);
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM,0)) < 0)
		return 1;
	if (connect(sockfd, (struct sockaddr *)
												&serv_addr, servlen) < 0)
			return 2;
	write(sockfd, SOCKET_HEADER, sizeof(SOCKET_HEADER));
	write(sockfd, SYNC_TIME_MSG, 1);

	time_t t_time=get_time(sockfd);
	std::cout << "TIME:"<<t_time<<std::endl;
	return 0;
}
