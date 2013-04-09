/*
 * knetutil.c
 *
 *  Created on: Apr 7, 2011
 *      Author: buttonfly
 */
#include "kaonsys.h"
#include "kaonutil.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <syslog.h>
#include <errno.h>
#include <netinet/in.h>
#include <wchar.h>


typedef int 					_BOOL;
typedef int 					_bool;
#define _TRUE 					1
#define _FALSE 					0

int interface_auto_up;

void knGetStringID(int id, char* str_id)
{
	memset(str_id, 0, sizeof(str_id));
	sprintf(str_id, "%x", id);

}	
long knGetMacAddr(_out_ unsigned char *addr)
{
	S_;
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
        bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
    }
    else {
        return -1;
    }
    return 0;
}

#if 0
_BOOL     setNetworkInfo_ERR()
{
	return _FALSE;
	
}

_BOOL     setNetworkInfo_UP()
{	
	return _TRUE;
			
}
#endif

void interface_up(int fd, char *iface) 
{
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    	printf("Warning: Could not get interface flags.");

        return;
    }

    if ((ifr.ifr_flags & IFF_UP) == IFF_UP)
        return;

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
    	printf("Warning: Could not get interface address.");
    } else if (ifr.ifr_addr.sa_family != AF_INET) {
    	printf("Warning: The interface is not IP-based.");
    } else {
        ((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr.s_addr = INADDR_ANY;
        if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        	printf("Warning: Could not set interface address.");
        }
    }

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    	printf("Warning: Could not get interface flags.");

        return;
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
    	printf("Warning: Could not set interface flags.");
}


interface_status_t interface_detect_beat_priv(int fd, char *iface)
{
    struct ifreq ifr;
    char args[4];
  //int ibeat = 0;

    if (interface_auto_up)
        interface_up(fd, iface);

    memset(&ifr, 0, sizeof(ifr));
    memset(args, 0, sizeof(4));

    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);
    ifr.ifr_data = (__caddr_t) args;

    if (ioctl(fd, SIOCDEVPRIVATE, &ifr) == -1) {
    	printf("SIOCDEVPRIVATE failed: %s", strerror(errno));

        return IFSTATUS_ERR;
    }

    return (args[0]&0x0001) ? IFSTATUS_UP : IFSTATUS_DOWN;
}

int linkstate()
{
	int fd = 0;
	interface_status_t s;
	char* iface = "eth0";
	int ret=1;

	if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		return 0;
	}

	if ((s = interface_detect_beat_priv(fd, iface)) == IFSTATUS_ERR) {
		ret = 0;
		printf("IFSTATUS_ERR");
		//setNetworkInfo_ERR();
		return _FALSE;
		
	}else if ((s = interface_detect_beat_priv(fd, iface)) == IFSTATUS_DOWN){
		ret = 0;
		printf("IFSTATUS_DOWN");
		//setNetworkInfo_ERR();
		return _FALSE;
		

	}else if ((s = interface_detect_beat_priv(fd, iface)) == IFSTATUS_UP){
		ret = 1;
		printf("IFSTATUS_UP");
		//setNetworkInfo_UP();
		return _TRUE;

	}
	close(fd);
	return ret;
}

/*************************************************************************
*
* FUNCTION
*
*       kn_Copy_Char_To_WChar
*
* DESCRIPTION
*
*       Utility method copying a ASCII string to a WCHAR_T string, padding
*       the MSB of the WCHAR_T string with 0.  Note that sourceLen and
*       destLen should include the termination element in the arrays -
*       i.e. szDest[destLen] should not index off the end of the array
*       pointed to by szDest.
*
* INPUTS
*
*       szDest                  Pointer to destination string (wchar_t).
*       szSource                Pointer to source string  (char).
*       destLen                 Length of buffer pointed to by szDest.
*       sourceLen               Length of buffer pointed to by szSource.
*
* OUTPUTS
*
*       None
*
*************************************************************************/
void kn_Copy_Char_To_WChar(unsigned short* szDest,
                            const char* szSource,
                            unsigned int destLen,
                            unsigned int sourceLen)
{
	int ret = mbstowcs((wchar_t*)szDest, szSource, sourceLen);
	KASSERT(ret != -1);
} /* Multimedia_Copy_Char_To_WChar */

/*************************************************************************
*
* FUNCTION
*
*       kn_Copy_WChar_To_Char
*
* DESCRIPTION
*
*       Utility method copying a WCHAR_T string to an ASCII string,
*       discarding the MSB of the  WCHAR_T string.  Note that sourceLen
*       and destLen should include the termination element in the arrays
*       - i.e. szDest[destLen] should not index off the end of the array
*       pointed to by szDest.
*
* INPUTS
*
*       szDest                  Pointer to destination string (char).
*       szSource                Pointer to source string  (wchar_t).
*       sourceLen               Length of buffer pointed to by szSource.
*       destLen                 Length of buffer pointed to by szDest.
*
* OUTPUTS
*
*       None
*
*************************************************************************/
void kn_Copy_WChar_To_Char(char* szDest,
                            const unsigned short* szSource,
                            unsigned int sourceLen,
                            unsigned int destLen)
{
	int ret = wcstombs(szDest,szSource, sourceLen);
	KASSERT(ret != -1);
} /* Multimedia_Copy_WChar_To_Char */


int knToLowerCharacter(char*  src, char* dst, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(src[i] > 'A' && src[i] < 'Z')
			dst[i] = src[i] + ('a' - 'A');
		else
			dst[i] = src[i];
	}
	return 0;
}


/*
 * marco
 * for ANGA
 */
void knMount_tmpfs_dir(char *dir)
{
	S_;
	char data[128];
	sprintf(data, "mkdir -p /data/images/%s", dir);
	system(data);
}

void knMount_tmpfs(void)
{
	S_;
	char command[128];
	sprintf(command, "mkdir -p /data/images; mount -t tmpfs swap /data/images/ -o size=1500m -o remount");
	system(command);
}

char *knString_replaceAll(char *s, const char *olds, const char *news) {
  char *result, *sr;
  size_t i, count = 0;
  size_t oldlen = strlen(olds); if (oldlen < 1) return s;
  size_t newlen = strlen(news);


  if (newlen != oldlen) {
    for (i = 0; s[i] != '\0';) {
      if (memcmp(&s[i], olds, oldlen) == 0) count++, i += oldlen;
      else i++;
    }
  } else i = strlen(s);


  result = (char *) malloc(i + 1 + count * (newlen - oldlen));
  if (result == NULL) return NULL;


  sr = result;
  while (*s) {
    if (memcmp(s, olds, oldlen) == 0) {
      memcpy(sr, news, newlen);
      sr += newlen;
      s  += oldlen;
    } else *sr++ = *s++;
  }
  *sr = '\0';

  return result;
}
