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
} conn;

static conn* join(const char* ip, short port);
static void leave(conn* conn);
static void receive_emm(conn* conn);
static void receive_avstream(conn* conn);

int main (int argc, char **argv)
{
	workerthread hworker = workerthread_new(10);

	// start EMM
	conn* emmconn = join(TEST_EMM_ADDR, TEST_EMM_PORT);
	conn* tsconn = join(TEST_LIVE_STREAM_ADDR, TEST_LIVE_STREAM_PORT);

	workerthread_put_task(hworker, (void(*)(void*))receive_emm, emmconn, NULL);
	workerthread_put_task(hworker, (void(*)(void*))receive_avstream, tsconn, NULL);

	for(;;) {
		sleep(1);
	}
	return 0;
}


static conn* join(const char* ip, short port)
{
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

static void receive_emm(conn* conn)
{
    unsigned char payload[UDP_BUFFER_SIZE] = {0};
    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, sizeof(payload), 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		ERR("no stream");
    		continue;
    	}

    	if(nread != UDP_BUFFER_SIZE) {
    		ERR("udp: %d / %d", nread, UDP_BUFFER_SIZE);
    	}
    	else {
    		fprintf(stderr, "*\r");
    		H_(payload, 16);
    	}
//    	sleep(1);
    }
    leave(conn);
}

static void receive_avstream(conn* conn)
{
    unsigned char payload[RTP_BUFFER_SIZE] = {0};
    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, sizeof(payload), 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		ERR("no stream");
    		continue;
    	}

    	if(nread != RTP_BUFFER_SIZE) {
    		ERR("rtp: %d / %d", nread, RTP_BUFFER_SIZE);
//    		H_(payload, 16);
    	}
    	else {
    		fprintf(stderr, "@\r");
    	}
//    	H_(payload, 16);
    }
	leave(conn);
}
