/*
 * main.c
 *
 *  Created on: Dec 18, 2012
 *      Author: buttonfly
 */


#include <pcap/pcap.h>
#include <libnet.h>
#include "dbg.h"

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



int main(int argc, char** argv) {

	int uid = getuid();
	if(uid != 0) {
		M_("permission denied!");
		return 0;
	}

	pcap_t* handle = NULL;
	char err[PCAP_ERRBUF_SIZE] = { 0 };
	int rc;

	char* dev  = pcap_lookupdev(err);
	KASSERT(dev);

	bpf_u_int32 mask;
	bpf_u_int32 net;
	rc = pcap_lookupnet(dev, &net, &mask, err);
	KASSERT(rc != -1);

	handle = pcap_open_live(dev, 65536, 0, 5000, err);
	KASSERT(handle);

	struct pcap_pkthdr* header;
	u_char* payload;

#define ETHERNET_LEN		14
#define ARPOP_REPLY 			2
	for(;;) {
		pcap_next_ex(handle, &header, (const u_char**)&payload);
		struct arp_hdr* ah = (struct arp_hdr*) (payload + ETHERNET_LEN);
		if(htons(ah->ar_op) == ARPOP_REPLY) {
			fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x\n",
					ah->src_hrd_addr[0],
					ah->src_hrd_addr[1],
					ah->src_hrd_addr[2],
					ah->src_hrd_addr[3],
					ah->src_hrd_addr[4],
					ah->src_hrd_addr[5]);
		}
	}

	pcap_close(handle);
	return 0;
}
