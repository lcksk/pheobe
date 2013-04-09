// EMM address
#if 1
#define TEST_EMM_ADDR	"234.10.20.32"
#define TEST_EMM_PORT	5000
#else // alticast
#define TEST_EMM_ADDR	"233.18.158.252"
#define TEST_EMM_PORT	1901
#endif

// Live stream address
#define TEST_LIVE_STREAM_ADDR	"238.20.20.5"
#define TEST_LIVE_STREAM_PORT	5000


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include "kaonutil.h"
#include "workerthread.h"

#define UDP_BUFFER_SIZE	1316
#define RTP_BUFFER_SIZE 1328

#define RECV_TIMEOUT	5	/* 5 seconds */

typedef struct {
	int sockfd;
	int client_addr_len;
	struct ip_mreq mreq;
	struct sockaddr_in serveraddr;
	FILE* filefd;
} conn;

typedef struct _application_info_t
{
	conn con;
	char* multicast_ip;
	unsigned short multicast_port;
	char* filepath;
} application_info_t;

static application_info_t application_info;


static void usage(const char* program);

static void decode_options(application_info_t* application_info, int argc, char **argv);

static conn* join(const char* ip, short port);
static void leave(conn* conn);
static void receive_avstream(conn* conn);

int main (int argc, char **argv)
{
	memset(&application_info, 0, sizeof(application_info));
	decode_options(&application_info, argc, argv);

	workerthread hworker = workerthread_new(10);

	if(application_info.multicast_ip == NULL || application_info.multicast_port > 0) {
		usage("ipstrm-dump");
		exit(0);
	}

	conn* tsconn = join(application_info.multicast_ip, application_info.multicast_port);
	tsconn->filefd  = fopen(application_info.filepath, "wt+");
	KASSERT(tsconn->filefd > 0);

	workerthread_put_task(hworker, (void(*)(void*))receive_avstream, tsconn, NULL);

	for(;;) {
		sleep(1);
	}

	fclose(tsconn->filefd);
	return 0;
}


static conn* join(const char* ip, short port)
{
	KASSERT(ip);
	KASSERT(port > 0);

	conn* con = (conn*) malloc(sizeof(conn));
	KASSERT(con);
	memset(con, 0, sizeof(conn));

	con->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	KASSERT(con->sockfd > 0);
	if(con->sockfd < 0) {
		goto error;
	}

    memset(&con->serveraddr, 0, sizeof(con->serveraddr));
    con->serveraddr.sin_family = AF_INET;
    con->serveraddr.sin_addr.s_addr = inet_addr(ip);
    con->serveraddr.sin_port = htons(port);
    con->client_addr_len = sizeof(con->serveraddr);
	bind(con->sockfd, (struct sockaddr *)&(con->serveraddr), sizeof(struct sockaddr));

	// join group
	memset(&con->mreq, 0, sizeof(con->mreq));
	con->mreq.imr_multiaddr.s_addr = inet_addr(ip);
	con->mreq.imr_interface.s_addr = INADDR_ANY;
	setsockopt(con->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&con->mreq, sizeof(con->mreq));

	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = RECV_TIMEOUT;
	setsockopt(con->sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
	setsockopt(con->sockfd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

	return con;

error:
	if(con != NULL) {
		free(con);
		con = NULL;
	}
	return NULL;
}

static void leave(conn* conn)
{
	KASSERT(conn);
	close(conn->sockfd);
	free(conn);
}


static void receive_avstream(conn* conn)
{
	static unsigned long long __n = 0;
    unsigned char payload[UDP_BUFFER_SIZE] = {0};
    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, sizeof(payload), 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		ERR("no stream");
    		continue;
    	}

    	fwrite(payload, nread,1, conn->filefd);
    	__n += nread;
    	fprintf(stderr, "%d\r", __n );
    }
	leave(conn);
}





static void usage(const char* program){

	printf("Usage: %s <options>\n", program);
	printf("Options:\n"
			"\t-h, -help\n"
			"\t\tPrint this help\n"
			"\t-i, --ip <ip>\n"
			"\t\tmulticast ip address\n"
			"\t-p, --port\n"
			"\t\tmulticast port\n"
			"\t-d, --dst\n"
			"\t\tfile to store\n"
	);
}

static void decode_options(application_info_t* application_info, int argc, char **argv)
{
    static const char *opt_string="i:p:d:h";
    static struct option const longopts[]   =  {
        {"ip",required_argument,NULL,'i'},
        {"port",required_argument,NULL,'p'},
        {"dst",required_argument,NULL,'d'},
        {"help",no_argument,NULL,'h'},
        {NULL,0,NULL,0}
    };
    int optc, longind=0;
    const char *name = argv[0];

    while((optc=getopt_long(argc,argv,opt_string,longopts,&longind))!=-1)
    {
        switch (optc)
        {
        case 'h':
        	usage(name);
        	exit(0);

        case 'i':
        	application_info->multicast_ip = strdup(optarg);
        	break;

        case 'p':
        	application_info->multicast_port = atoi(optarg);
        	break;

        case 'd':
        	application_info->filepath = strdup(optarg);
        	break;

        default:
        	usage(name);
        	exit(0);
        }
    }
}


