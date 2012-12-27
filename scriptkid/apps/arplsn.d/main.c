/*
 * main.c
 *
 *  Created on: Dec 18, 2012
 *      Author: buttonfly
 */

#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "dbg.h"
#include "bcmsw_ioctl.h"

//#define M_
//#define KASSERT

//#define DAEMONIZE

#define TYPE_ARP	0x0806
#define TYPE_IP		0x0800
#define PROT_IGMP	0x02

#define IGMP_MEMSHIP_QUERY 		0x11
#define IGMP_MEMSHIP_REPORT 	0x16

#define FILTER_RULE 	"arp || igmp"
#define IGMP_SNOOP_CTRL_DEVICE 	"/dev/bcmsw"

struct igmp_hdr {
	u_int8_t type;
	u_int8_t max_reponse;
	u_int16_t chksum;
	u_int32_t addr;
};

static int daemon_init(void);

static void skb_rcv(u_int8_t* user, const struct pcap_pkthdr* h, const u_int8_t* payload);
static void igmp_rcv(const u_int8_t* payload, u_int8_t* user);
static void arp_rcv(const u_int8_t* payload, u_int8_t* user);
static void write_pid(const char* file);
static void remove_pid(const char* file);
static void signal_handler(int i) {
	remove_pid(NULL);
}

int main(int argc, char** argv) {

	int uid = getuid();
	if (uid != 0) {
		M_("permission denied!");
		return 0;
	}

	int rc;
#ifdef DAEMONIZE
	rc= daemon_init();
	if(rc < 0) {
		return -1;
	}
#endif

	int fd = 1 ; //open(IGMP_SNOOP_CTRL_DEVICE, O_RDWR | O_NDELAY);
	KASSERT(fd >= 0);

	write_pid("/tmp/arplsn.d");

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	pcap_t* handle = NULL;
	char err[PCAP_ERRBUF_SIZE] = { 0 };

	char* dev = pcap_lookupdev(err);
	KASSERT(dev);

	bpf_u_int32 mask;
	bpf_u_int32 net;
	rc = pcap_lookupnet(dev, &net, &mask, err);
	KASSERT(rc >= -1);

	handle = pcap_open_live(dev, 65536, 1, 5000, err);
	KASSERT(handle);

	struct pcap_pkthdr* header;
	u_char* payload;

	struct bpf_program program;
	rc = pcap_compile(handle, &program, FILTER_RULE, 0, mask);
	KASSERT(rc >= -1);

	rc = pcap_setfilter(handle, &program);
	KASSERT(rc >= -1);
	pcap_loop(handle, 0, skb_rcv, fd);

	pcap_close(handle);
	close(fd);
	return 0;
}

static int daemon_init(void) {
	pid_t pid;

	if ((pid = fork()) < 0) {
		return -1;
	} else if (pid != 0) {
		exit(0);
	}
	setsid();
	umask(0);
	return 0;
}

static void skb_rcv(u_int8_t* user, const struct pcap_pkthdr* h, const u_int8_t* payload)
{
	struct ether_header *eh = (struct ether_header *) payload;
	u_int16_t type = ntohs(eh->ether_type);
	if (type == ETHERTYPE_IP) {
		struct ip *iph = (struct ip *) (payload + sizeof(struct ether_header));
		if (iph->ip_p == PROT_IGMP) {
			igmp_rcv(payload, user);
		}
	}
	else if(type == ETHERTYPE_ARP) {
		arp_rcv(payload, user);
	}
	return;
}

static void arp_rcv(const u_int8_t* payload, u_int8_t* user)
{
	fprintf(stderr, ">> arp:\n");
	struct ether_header *eh = (struct ether_header *) payload;
	fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x >>>> ",
	eh->ether_shost[0],
	eh->ether_shost[1],
	eh->ether_shost[2],
	eh->ether_shost[3],
	eh->ether_shost[4],
	eh->ether_shost[5]
	);

	fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x\n",
	eh->ether_dhost[0],
	eh->ether_dhost[1],
	eh->ether_dhost[2],
	eh->ether_dhost[3],
	eh->ether_dhost[4],
	eh->ether_dhost[5]
	);
}


static void igmp_rcv(const u_int8_t* payload, u_int8_t* user)
{
	struct ether_header *eh = (struct ether_header *) payload;
	struct igmp_hdr* igmp = (struct igmp_hdr*) (payload + sizeof(struct ether_header) + sizeof(struct ip) + 4/*options*/);
	if (igmp->type == IGMP_MEMSHIP_REPORT) {
		fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x >>>> ",
		eh->ether_shost[0],
		eh->ether_shost[1],
		eh->ether_shost[2],
		eh->ether_shost[3],
		eh->ether_shost[4],
		eh->ether_shost[5]
		);

		fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		eh->ether_dhost[0],
		eh->ether_dhost[1],
		eh->ether_dhost[2],
		eh->ether_dhost[3],
		eh->ether_dhost[4],
		eh->ether_dhost[5]
		);

		int value = 1;
		ioctl((int)user, BCMSW_RESET_TIMER, &value);
	}
}


static void write_pid(const char* file)
{
	S_;
	FILE* fp = NULL;
	fp = fopen(file, "w");
	KASSERT(fp);
	fprintf(fp, "%d\n", getpid());
	fclose(fp);
}

static void remove_pid(const char* file)
{
	S_;
	exit(0);
}


