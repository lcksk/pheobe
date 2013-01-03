/*
 * main.c
 *
 *  Created on: Dec 14, 2012
 *      Author: buttonfly
 */


#include <stdio.h>
#include <string.h>
#include <libnet.h>
#include <getopt.h>
#include <pcap/pcap.h>
#include <libnet.h>

#include <pthread.h>
#include "osl.h"
#include "osldbg.h"

#define PATH_MAX 512
#define DEFAULT_CONF_FILE_NAME	"env.conf"

struct addr {
	unsigned long ip;
	unsigned short port;
};

typedef struct {
	char* dev;
	osl_semaphore sem;
} application_info_t;

static void usage(const char* program);

static void listen_arp(application_info_t* arg);
static void send_arp(application_info_t* arg);

int main(int argc, char** argv) {
	int uid = getuid();
	if(uid != 0) {
		printf("permission denied!\n");
		return 0;
	}

	static application_info_t application_info;
	memset(&application_info, 0, sizeof(application_info_t));

	char err[PCAP_ERRBUF_SIZE] = { 0 };
	application_info.dev  = pcap_lookupdev(err);
	KASSERT(application_info.dev);

	bpf_u_int32 mask;
	bpf_u_int32 net;
	int rc = pcap_lookupnet(application_info.dev, &net, &mask, err);
	KASSERT(rc != -1);

	application_info.sem = osl_semaphore_create();
	osl_thread_create((void(*)(void*))send_arp, &application_info, 0, NULL, NULL);

	listen_arp(&application_info);
	return 0;
}

static void usage(const char* program) {
	printf("Usage: %s <options>\n", program);
	printf("Options:\n"
			"\t-h, -help\n"
			"\t\tPrint this help\n"
			"\t-c, --config <path>\n"
			"\t\tconf file path (default: env.conf)\n"
	);
}


static void listen_arp(application_info_t* app)
{
	struct arp_hdr {
		u_int16_t ar_hrd;
		u_int16_t ar_pro;
		u_int8_t ar_hln;
		u_int8_t ar_pln;
		u_int16_t ar_op;
		u_int8_t src_hrd_addr[6];
		u_int8_t src_pro_addr[4];
		u_int8_t dst_hrd_addr[6];
		u_int8_t dst_pro_addr[4];
	};


	pcap_t* handle = NULL;
	char err[PCAP_ERRBUF_SIZE] = { 0 };
	int rc;

	handle = pcap_open_live(app->dev, 65536, 0, 5000, err);
	KASSERT(handle);

	struct pcap_pkthdr* header;
	u_char* payload;

	#define ETHERNET_LEN		14
	#define ARPOP_REPLY 			2

	osl_semaphore_signal(app->sem);
	for(;;) {
		pcap_next_ex(handle, &header, (const u_char**)&payload);
		struct arp_hdr* ah = (struct arp_hdr*) (payload + ETHERNET_LEN);
		if(htons(ah->ar_op) == ARPOP_REPLY) {
			fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x - %d.%d.%d.%d\n",
					ah->src_hrd_addr[0],
					ah->src_hrd_addr[1],
					ah->src_hrd_addr[2],
					ah->src_hrd_addr[3],
					ah->src_hrd_addr[4],
					ah->src_hrd_addr[5],

					ah->src_pro_addr[0],
					ah->src_pro_addr[1],
					ah->src_pro_addr[2],
					ah->src_pro_addr[3]
			);
		}
	}

	pcap_close(handle);
}


static void send_arp(application_info_t* app)
{
	osl_semaphore_wait(app->sem);

	unsigned int gateway = osl_get_gateway_addr();
	char err[LIBNET_ERRBUF_SIZE];
	libnet_t* handle = libnet_init(LIBNET_LINK, NULL, err);
	KASSERT(handle);
	libnet_ptag_t t;

	bpf_u_int32 mask;
	bpf_u_int32 net;
	int rc = pcap_lookupnet((char*)app->dev, &net, &mask, err);
	KASSERT(rc != -1);

	struct libnet_ether_addr * hwaddr;
	hwaddr = libnet_get_hwaddr(handle);
	u_int8_t  tha[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	u_int8_t  etha[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	u_int32_t ipaddr;
	ipaddr = libnet_get_ipaddr4(handle);

	M_("net: %08x", net);
	M_("mask: %08x", mask);
	u_int32_t netaddr = net & mask;
	M_("network: %08x", netaddr);

	u_int32_t num = 200;

	for(int i=1; i < num; i++) {
		u_int32_t tmp  = netaddr + htonl(i);

		char payload[18] = { 0 };
		t = libnet_autobuild_arp(
				ARPOP_REQUEST,
				(u_int8_t*) hwaddr->ether_addr_octet,
				(u_int8_t*) &ipaddr,
				(u_int8_t*) etha,
				(u_int8_t*) &tmp,
				handle);

	    t = libnet_autobuild_ethernet(
	    		tha,
	            ETHERTYPE_ARP,
	            handle);

    	int c = libnet_write(handle);
		if(c == -1) {
			fprintf(stderr, "failed to send a packet: %d\n", c);
			M_("ipaddr: %08x", tmp);
			break;
		}
		libnet_clear_packet(handle);
		usleep(100);
	}
	libnet_destroy(handle);
}

