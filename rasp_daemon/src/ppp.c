#include "ppp.h"
#include <stdlib.h>

void pon_rnet(){
  system("pon rnet");
}

int check_ppp_interface(){
  struct ifreq ifr;
    int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "ppp0");
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            perror("SIOCGIFFLAGS");
    }
    close(sock);
    !!(ifr.ifr_flags & IFF_UP);
}

void poff_rnet(){
  system("poff rnet");
}
