/*
 * main.c
 *
 *  Created on: Dec 18, 2012
 *      Author: buttonfly
 */


#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <libnet.h>
#include <getopt.h>

#include "dbg.h"

extern unsigned int GetGatewayAddr(void);


int main(int argc, char** argv) {

	int uid = getuid();
	if(uid != 0) {
		M_("permission denied!");
		return 0;
	}

	unsigned int gateway = GetGatewayAddr();

	libnet_ptag_t t;
	char err[LIBNET_ERRBUF_SIZE];
	libnet_t* handle = libnet_init(LIBNET_LINK, NULL, err);

	struct libnet_ether_addr * hwaddr;
	hwaddr = libnet_get_hwaddr(handle);
	u_int32_t ipaddr;
	ipaddr = libnet_get_ipaddr4(handle);
	u_int8_t  tha[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	t = libnet_autobuild_arp(
			ARPOP_REQUEST,
			hwaddr->ether_addr_octet,
			&ipaddr,
			tha,
			&gateway,
			handle);

    t = libnet_autobuild_ethernet(
    		tha,
            ETHERTYPE_ARP,
            handle);

    int c = libnet_write(handle);
	if(c == -1) {
		printf("failed to send a packet: %d", c);
	}

	libnet_destroy(handle);
	return 0;
}

