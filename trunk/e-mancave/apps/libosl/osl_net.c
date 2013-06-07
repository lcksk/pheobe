/*
 * osl_net.h
 *
 *  Created on: Dec 18, 2012
 *      Author: buttonfly
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <syslog.h>
#include <net/if.h>
#include <sys/types.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <dirent.h>

#define BUFSIZE 8192

typedef struct route_info {
	u_int dstAddr;
	u_int srcAddr;
	u_int gateWay;
	char ifName[IF_NAMESIZE];
} ROUTE_INFO;

static unsigned int RequestAddr(int sock);
static unsigned int parseRoutes(struct nlmsghdr *nlHdr,
		struct route_info *rtInfo);
static int readNlSock(int sock, char *buf, int seqNo);

u_int32_t osl_get_gateway_addr(void) {

	unsigned int addr = 0;
	int sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
	if (sock < 0) {
		fprintf(stderr, "failed to socket\n");
		return -1;
	}

	addr = RequestAddr(sock);

	close(sock);
	return addr;
}

unsigned int RequestAddr(int sock) {

	char msgBuf[BUFSIZE];
	memset(msgBuf, 0, BUFSIZE);

	struct nlmsghdr *nlMsg = (struct nlmsghdr *) msgBuf;

	int msgSeq = 0;
	unsigned int len = 0;

	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
	nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

	if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
		printf("Write To Socket Failed...\n");
		return -1;
	}

	if ((len = readNlSock(sock, msgBuf, msgSeq)) < 0) {
		printf("Read From Socket Failed...\n");
		return -1;
	}

	struct route_info* rtInfo = (struct route_info *) malloc(
			sizeof(struct route_info));

	unsigned int gateway;
	for (; NLMSG_OK(nlMsg,len); nlMsg = NLMSG_NEXT(nlMsg,len)) {
		memset(rtInfo, 0, sizeof(struct route_info));
		gateway = parseRoutes(nlMsg, rtInfo);
		if (gateway)
			break;
	}
	free(rtInfo);
	return gateway;
}

int readNlSock(int sock, char *buf, int seqNo) {
	struct nlmsghdr *nlHdr;
	unsigned int readLen = 0, msgLen = 0;

	do {
		if ((readLen = recv(sock, buf, BUFSIZE - msgLen, 0)) < 0) {
			perror("SOCK READ: ");
			return -1;
		}

		nlHdr = (struct nlmsghdr *) buf;

		if ((NLMSG_OK(nlHdr, readLen) == 0)
				|| (nlHdr->nlmsg_type == NLMSG_ERROR)) {
			perror("Error in recieved packet");
			return -1;
		}

		if (nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		} else {
			buf += readLen;
			msgLen += readLen;
		}

		if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			break;
		}
	} while ((nlHdr->nlmsg_seq != (unsigned int) seqNo)
			|| (nlHdr->nlmsg_pid != (unsigned int) getpid()));
	return msgLen;
}

unsigned int parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo) {
	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;
	int rtLen;
	unsigned int gateway;

	rtMsg = (struct rtmsg *) NLMSG_DATA(nlHdr);

	if ((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
		return 0;

	rtAttr = (struct rtattr *) RTM_RTA(rtMsg);
	rtLen = RTM_PAYLOAD(nlHdr);
	for (; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen)) {
		switch (rtAttr->rta_type) {
		case RTA_OIF:
			if_indextoname(*(int *) RTA_DATA(rtAttr), rtInfo->ifName);
			break;
		case RTA_GATEWAY:
			rtInfo->gateWay = *(u_int *) RTA_DATA(rtAttr);
			break;
		case RTA_PREFSRC:
			rtInfo->srcAddr = *(u_int *) RTA_DATA(rtAttr);
			break;
		case RTA_DST:
			rtInfo->dstAddr = *(u_int *) RTA_DATA(rtAttr);
			break;
		}
	}

	gateway = rtInfo->gateWay;

	return gateway;
}



u_int32_t osl_get_hwaddr(u_int8_t *addr) {
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

const char* osl_get_default_wlan_ifname(void)
{
#ifndef CONFIG_CTRL_IFACE_DIR
#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#endif
	char *ifname = NULL;

	struct dirent *dent;
	DIR *dir = opendir(CONFIG_CTRL_IFACE_DIR);
	if (!dir) {
		return NULL;
	}
	while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
		/*
		 * Skip the file if it is not a socket. Also accept
		 * DT_UNKNOWN (0) in case the C library or underlying
		 * file system does not support d_type.
		 */
		if (dent->d_type != DT_SOCK && dent->d_type != DT_UNKNOWN)
			continue;
#endif /* _DIRENT_HAVE_D_TYPE */
		if (strcmp(dent->d_name, ".") == 0 ||
		    strcmp(dent->d_name, "..") == 0)
			continue;
		printf("Selected interface '%s'\n", dent->d_name);
		ifname = strdup(dent->d_name);
		break;
	}
	closedir(dir);

	return ifname;
}

u_int32_t osl_get_ipaddr(const char* ifname)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	unsigned int addr;
	struct ifreq        ifr;
	struct sockaddr_in  *p = (struct sockaddr_in *)&(ifr.ifr_addr);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
	p->sin_family = AF_INET;

	int err = ioctl(sock, SIOCGIFADDR, &ifr);
	close(sock);

	if(err >= 0) {
		return p->sin_addr.s_addr;
	}
	return -1;
}


