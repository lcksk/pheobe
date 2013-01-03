/*
 * main.c
 *
 *  Created on: Dec 18, 2012
 *      Author: buttonfly
 */

#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <linux/limits.h>

#include "kaonutil.h"
#include "bcmsw_ioctl.h"

#define DEBUG_ARPLISTENER
#define EMUL
//#define DAEMONIZE

//#define TYPE_ARP	0x0806
//#define TYPE_IP		0x0800
#define PROT_IGMP	0x02

#define IGMP_MEMSHIP_QUERY 		0x11
#define IGMP_MEMSHIP_REPORT 	0x16
#define IGMP_LEAVE_GROUP				0x17

#define PROMISCUOUS_MODE	1


#define DEFAULT_PIDFILE_PATH	"/tmp/arplsn.d.pid"

typedef struct {
	char pidfile_path[PATH_MAX];
	unsigned hwaddr[ETH_ALEN];
} application_info_t;



#define FILTER_RULE 	"igmp || arp"
#define IGMP_SNOOP_CTRL_DEVICE 	"/dev/bcmsw"

const char OUI_SAMSUNG[] = {0x00, 0x16, 0x32};
#define OUI_LEN	3

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
static void signalhandler(int signo);
static void usage(const char* program);
static void get_default_settings(application_info_t* application_info);
static void decode_options(int argc, char **argv, application_info_t* application_info);
static int get_hwaddr(_out_ unsigned char *addr);
#ifdef DEBUG_ARPLISTENER
static inline void DBG_ETHER(struct ether_header *eh)
{
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
#else
#define DBG_ETHER(x)
#endif

static application_info_t* getcontext() {
	static application_info_t context;
	return &context;
};

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
	memset(getcontext(), 0, sizeof(application_info_t));
	get_default_settings(getcontext());

	decode_options(argc, argv, getcontext());
	write_pid(getcontext()->pidfile_path);

	signal(SIGINT, signalhandler);
	signal(SIGTERM, signalhandler);

#ifdef EMUL
	int fd = open(IGMP_SNOOP_CTRL_DEVICE, O_RDWR | O_NDELAY);
#else
	int fd = open(IGMP_SNOOP_CTRL_DEVICE, O_RDWR | O_NDELAY);
	KASSERT(fd >= 0);
#endif

	pcap_t* handle = NULL;
	char err[PCAP_ERRBUF_SIZE] = { 0 };

	char* dev = pcap_lookupdev(err);
	KASSERT(dev);

	bpf_u_int32 mask;
	bpf_u_int32 net;
	rc = pcap_lookupnet(dev, &net, &mask, err);
	KASSERT(rc >= -1);

	handle = pcap_open_live(dev, 60, PROMISCUOUS_MODE, 5000, err);
//	handle = pcap_open_live(dev, 60, 0, 5000, err);
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
	struct arphdr* ah = (struct arphdr*) (payload + sizeof(struct ether_header));
	if(ntohs(ah->ar_op) == ARPOP_REPLY) {
		struct ether_header *eh = (struct ether_header *) payload;
		if(memcmp(eh->ether_dhost, getcontext()->hwaddr, ETH_ALEN) == 0) {
			DBG_ETHER(eh);
		}
	}
}

static void igmp_rcv(const u_int8_t* payload, u_int8_t* user)
{
	struct ether_header *eh = (struct ether_header *) payload;
	struct igmp_hdr* igmp = (struct igmp_hdr*) (payload + sizeof(struct ether_header) + sizeof(struct ip) + 4/*options*/);
	if (igmp->type == IGMP_MEMSHIP_REPORT) {
		if(memcmp(OUI_SAMSUNG, eh->ether_shost, OUI_LEN) == 0) {
			ioctl(user, BCMSW_RESET_TIMER);
		}
#ifdef DEBUG_ARPLISTENER
		fprintf(stderr, "[IGMP_MEMBERSHIP_REPORT]");
		DBG_ETHER(eh);
#endif

	}
	else if(igmp->type == IGMP_LEAVE_GROUP) {
#ifdef DEBUG_ARPLISTENER
		fprintf(stderr, "[IGMP_LEAVE_GROUP]");
		DBG_ETHER(eh);
#endif
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
	unlink(file);
}

static void decode_options(int argc, char **argv, application_info_t* application_info)
{
    static const char *opt_string="p:h";
    static struct option const longopts[]   =  {
        {"pidfile",required_argument,NULL,'p'},
        {"help",no_argument,NULL,'h'},
        {NULL,0,NULL,0}
    };
    int optc, longind=0;
    const char *name = argv[0];

    while((optc=getopt_long(argc,argv,opt_string,longopts,&longind))!=-1) {

        switch (optc) {
        case 'h':
        	usage(name);
        	exit(0);

        case 'p':
        	strcpy(application_info->pidfile_path, optarg);
        	break;

        default:
        	usage(name);
        	exit(0);
        }
    }
}

static void usage(const char* program) {
	printf("Usage: %s <options>\n", program);
	printf("Options:\n"
			"\t-h, -help\n"
			"\t\tPrint this help\n"
			"\t-c, --pidfile <path>\n"
			"\t\tpid file path (default: /tmp/%s)\n", program
	);
}

static void get_default_settings(application_info_t* application_info)
{
	memset(application_info, 0, sizeof(application_info_t));
	strcpy(application_info->pidfile_path, DEFAULT_PIDFILE_PATH);
	get_hwaddr(application_info->hwaddr);
}

static void signalhandler(int signo) {
	S_;
	sigset_t    sigset, oldset;
	sigfillset(&sigset);
	sigprocmask(SIG_BLOCK, &sigset, &oldset);

    switch(signo) {
    case SIGTERM:
    case SIGINT:
    case SIGSEGV:
    	remove_pid(getcontext()->pidfile_path);
    	exit(1);
    	break;
    }
    sigprocmask(SIG_SETMASK, &oldset, NULL);
}

static int get_hwaddr(_out_ unsigned char *addr) {

	struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    int ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s==-1) {
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
                    ok = 1;
                    break;
                }
            }
        }
    }

    close(s);
    if (ok) {
        bcopy( ifr.ifr_hwaddr.sa_data, addr, ETH_ALEN);
    }
    else {
        return -1;
    }
    return 0;
}

