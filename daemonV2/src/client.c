#include "GPRS.h"



int main(void)
{
	init_gprs();          		//module power on

	pon_rnet();

	check_ppp_interface();

	return 0;
}
