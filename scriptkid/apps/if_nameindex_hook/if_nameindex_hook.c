/*
 * if_nameindex_hook.c
 *
 *  Created on: Jan 17, 2013
 *      Author: buttonfly
 */


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

/*
 *  @description:
 *  We assume that 'wl0.1' is reserved for WiFi Direct RCU in the CANAL project.
 *  LD_PRELOAD=bin/libif_nameindex_hook.so bin/streamer
 */
#define RCU_IFNAME 	"wl0.1"

static struct if_nameindex* (*if_nameindex0)(void) __THROW;

void __attribute__((constructor)) __init() {
	if_nameindex0 = dlsym(RTLD_NEXT, "if_nameindex");
}

static int if_count(void);

struct if_nameindex* if_nameindex(void) {

	const struct if_nameindex *ifs = if_nameindex0();    /* retrieve the current interfaces */

	unsigned int index = if_nametoindex(RCU_IFNAME);
	if(index==0) { // no WIFI Direct RCU
		return ifs;
	}

	const int num = if_count();

	int i;
	if(ifs) {
		struct if_nameindex *ptr = ifs;
		for(i=0; ptr->if_name; ptr++,i++) {
			if(memcmp(ptr->if_name, RCU_IFNAME, strlen(RCU_IFNAME))==0) {
				memmove(ptr, ptr+1, sizeof(struct if_nameindex) * (num - i));
				break;
			}
		}
	}
	return ifs;
}


static int if_count(void)
{
	int count;
	const struct if_nameindex *ifs = if_nameindex0();    /* retrieve the current interfaces */
	if(ifs) {
		struct if_nameindex *ptr = ifs;
		for(count=0; ptr->if_name; ptr++,count++) ;
		if_freenameindex(ifs);
	}
	return count;
}
