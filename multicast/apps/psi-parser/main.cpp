// EMM address
#define TEST_EMM_ADDR	"234.10.20.32"
#define TEST_EMM_PORT	5000

// Live stream address
#define TEST_LIVE_STREAM_ADDR	"238.20.20.5"
#define TEST_LIVE_STREAM_PORT	5000
//#define TEST_LIVE_STREAM_ADDR	"224.1.1.1"
//#define TEST_LIVE_STREAM_PORT	1234



#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "kaonutil.h"
#include "workerthread.h"

#include <fcntl.h>

#include <dvbpsi/dvbpsi.h>
#include <dvbpsi/psi.h>
#include <dvbpsi/pat.h>



#define UDP_BUFFER_SIZE	1316
#define RTP_BUFFER_SIZE 1328

#define RECV_TIMEOUT	5	/* 5 seconds */


typedef struct {
	int sockfd;
	int client_addr_len;
	struct ip_mreq mreq;
	struct sockaddr_in serveraddr;
	dvbpsi_handle hdvbpsi;
} conn;



static conn* join(const char* ip, short port);
static void leave(conn* conn);
static void receive_emm(conn* conn);
static void receive_avstream(conn* conn);
static void receive_pat(void* p_zero, dvbpsi_pat_t* p_pat);

int main (int argc, char **argv)
{
	workerthread hworker = workerthread_new(10);

	conn* tsconn = join(TEST_LIVE_STREAM_ADDR, TEST_LIVE_STREAM_PORT);
	tsconn->hdvbpsi = dvbpsi_AttachPAT(receive_pat, tsconn);
	workerthread_put_task(hworker, (void(*)(void*))receive_avstream, tsconn, NULL);

	for(;;) {
		sleep(1);
	}
	dvbpsi_DetachPAT(tsconn->hdvbpsi);

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

static void receive_avstream(conn* conn)
{
#define RTP_HEADER_SIZE 12
#define PACKET_SIZE	188
    unsigned char payload[PACKET_SIZE + RTP_HEADER_SIZE] = {0};
    for(;;) {
    	int nread = recvfrom(conn->sockfd, (void *)payload, sizeof(payload), 0, ( struct sockaddr *)&conn->serveraddr,  (socklen_t *)&conn->client_addr_len);
    	if(nread < 0) { // timeout
    		ERR("no stream");
    		continue;
    	}

    	if(nread != PACKET_SIZE + RTP_HEADER_SIZE) {
    		ERR("rtp: %d / %d", nread, PACKET_SIZE + RTP_HEADER_SIZE);
    		H_(payload, 16);
    	}
    	else {
    		fprintf(stderr, "@\r");
#if (RTP_HEADER_SIZE == 12)
    		memmove(payload, &payload[12], 188);
#endif
    		KASSERT(payload[0] == 0x47);
    		unsigned short pid = ((uint16_t)(payload[1] & 0x1f) << 8) + payload[2];
    		if(pid == 0) { // PAT
    			dvbpsi_PushPacket(conn->hdvbpsi, payload);
    		}
    	}
//    	H_(payload, 16);
    }
	leave(conn);
}

static void receive_pat(void* p_zero, dvbpsi_pat_t* p_pat)
{
	dvbpsi_pat_program_t* p_program = p_pat->p_first_program;
	printf(  "\n");
	printf(  "New PAT\n");
	printf(  "  transport_stream_id : %d\n", p_pat->i_ts_id);
	printf(  "  version_number      : %d\n", p_pat->i_version);
	printf(  "    | program_number @ [NIT|PMT]_PID\n");
	while(p_program)
	{
		printf("    | %14d @ 0x%x (%d)\n",
			   p_program->i_number, p_program->i_pid, p_program->i_pid);
		p_program = p_program->p_next;
	}
	printf(  "  active              : %d\n", p_pat->b_current_next);
	dvbpsi_DeletePAT(p_pat);

}

