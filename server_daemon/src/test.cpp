#include "db.h"
#include "log.h"
#include "socket_data.h"

int main(void){
	int n;
	init_log();
	schedule_data data;
	n=get_new_schedule(1,&data);
	if(n!=0){
		std::cout << "ERROR GETTING SCHEDULE "<<n<<std::endl;
	}
	std::cout << data << std::endl;
	exit(1);
}