/*
 * capture.c
 *
 *  Created on: Apr 15, 2013
 *      Author: buttonfly
 */





#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <assert.h>

#include "multicastcapture.h"
#include "osl.h"


#define UDP_BUFFER_SIZE	1316
#define RTP_BUFFER_SIZE 1328
#define RTP_HEADER_SIZE 12 /*minimum*/
#define RECV_TIMEOUT	5	/* 5 seconds */

typedef struct {
	int sockfd;
	int client_addr_len;
	struct ip_mreq mreq;
	struct sockaddr_in serveraddr;
	FILE* filefd;
} conn;

typedef struct {
	conn* conn;
	cbr_t cbr;
	protocol_t prot;
} multicastcapture_t;

static conn* join(const char* ip, short port);
static void leave(conn* conn);
static void receive_stream(conn* conn);
static bool is_multicast(const struct sockaddr_storage *saddr, socklen_t len);

multicastcapture multicastcapture_open(multicastcapture_open_param_t* param) {
	multicastcapture_t* context = (multicastcapture_t*) malloc(sizeof(multicastcapture_t));
	memset(context, 0, sizeof(multicastcapture_t));
	context->cbr.cbr = param->cbr.cbr;
	context->cbr.context = param->cbr.context;
	context->prot = param->prot;
	context->conn = join(param->ip, param->port);
	return context;
}

int multicastcapture_start(multicastcapture h) {
	static unsigned long long __n = 0;
	multicastcapture_t* context = (multicastcapture_t*) h;
	conn* conn = context->conn;

	size_t size = 0 ;
	u_int8_t* payload = NULL;
    u_int8_t* ptr = NULL;
    size_t rtp_hdr_len = 0;
	if(context->prot == prot_eUDP) {
		size = UDP_BUFFER_SIZE;
		payload = (u_int8_t*) malloc(size);
		ptr = payload;
	}
	else if(context->prot == prot_eRTP){
		size = RTP_BUFFER_SIZE;
		payload = (u_int8_t*) malloc(size);
		ptr = payload + RTP_HEADER_SIZE;
		rtp_hdr_len = RTP_HEADER_SIZE;
	}
	else {
		fprintf(stderr, "E: Unknown protocol: %d\n", context->prot);
		return -1;
	}


    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, size, 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		fprintf(stderr, "no stream");
    		continue;
    	}

    	if(context->cbr.cbr) {
    		context->cbr.cbr(context->cbr.context, ptr, nread - rtp_hdr_len);
    	}

    	__n += nread;
#if 0
    	fprintf(stderr, "%d\r", __n );
#endif
    }

    if(payload)
    	free(payload);

	leave(conn);
}

int multicastcapture_start_async(multicastcapture h) {
	osl_thread_create(multicastcapture_start, h, 0, 0, NULL);
}

int multicastcapture_stop(multicastcapture h) {
	return 0;
}

int multicastcapture_close(multicastcapture h) {
	multicastcapture_t* context = (multicastcapture_t*) h;
	leave(context->conn);
	free(context);
	return 0;
}

int multicastcapture_get_fd(multicastcapture h)
{
	multicastcapture_t* context = (multicastcapture_t*) h;
	return context->conn->sockfd;
}


static conn* join(const char* ip, short port)
{
	assert(ip);
	assert(port > 0);

	conn* con = (conn*) malloc(sizeof(conn));
	assert(con);
	memset(con, 0, sizeof(conn));

	con->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(con->sockfd > 0);
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

#if 0
	setsockopt(con->sockfd, SOL_SOCKET, 	SO_RCVBUF, (char *)&sockbufsize, &size);
#endif
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
	assert(conn);
	setsockopt (conn->sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &conn->mreq, sizeof(struct ip_mreq));
	close(conn->sockfd);
	free(conn);
}

static void receive_stream(conn* conn)
{
	static unsigned long long __n = 0;
    unsigned char payload[RTP_BUFFER_SIZE] = {0};
    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, sizeof(payload), 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		fprintf(stderr, "no stream");
    		continue;
    	}

    	fwrite(&payload[RTP_HEADER_SIZE], nread - RTP_HEADER_SIZE,1, conn->filefd);
    	__n += nread;
    	fprintf(stderr, "%d\r", __n );
    }
	leave(conn);
}

static bool is_multicast(const struct sockaddr_storage *saddr, socklen_t len) {
    const struct sockaddr *addr = (const struct sockaddr *) saddr;

    switch(addr->sa_family) {
#if defined(IN_MULTICAST)
        case AF_INET: {
            const struct sockaddr_in *ip = (const struct sockaddr_in *)saddr;
            if ((size_t)len < sizeof (*ip))
                return false;
            return IN_MULTICAST(ntohl(ip->sin_addr.s_addr)) != 0;
        }
#endif
#if defined(IN6_IS_ADDR_MULTICAST)
        case AF_INET6: {
            const struct sockaddr_in6 *ip6 = (const struct sockaddr_in6 *)saddr;
            if ((size_t)len < sizeof (*ip6))
                return false;
            return IN6_IS_ADDR_MULTICAST(&ip6->sin6_addr) != 0;
        }
#endif
    }
    return false;
}

