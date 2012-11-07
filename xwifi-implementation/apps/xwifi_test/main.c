/*
 * main.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */


#include "nds/xwifi.h"
#include "xwifi_dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>

#define PRIORITY_LEVEL 10

static void* api_test_thread(void* arg);
static int xwifi_create_thread(void *(*__start_routine) (void *), void* param, int priority);
static void parse_event(XWifiEvent*	wifi_event);
static void signal_handler(int i) {
	xwifi_terminated();
}

int main(int argc, char** argv)
{
	struct pollfd pollfds[1];
	XWifiEvent	wifi_event;
	uint32_t	events_mask;
	int 	event_queue_fd;
	const char* ifname = "wlan0";

	xwifi_initialise(PRIORITY_LEVEL);

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	XWifiInterfaceConfig interfaceConfig;
	interfaceConfig.operatingMode = XWIFI_OPERATING_MODE_MANAGED;
	memset(interfaceConfig.regulatoryDomain, 0x03,4);
	xwifi_interface_configure(ifname, interfaceConfig);
	xwifi_power_control(ifname, XWIFI_POWER_ON);
	/* Initialise the event queue */
	event_queue_fd = xwifi_open_event_queue(&event_queue_fd);

	/* Subscribe to wi-fi events */
	events_mask = (XWIFI_EVENT_SCAN_COMPLETED | XWIFI_EVENT_CONNECTION_ESTABLISHED | XWIFI_EVENT_CONNECTION_DROPPED);
	xwifi_subscribe_events(event_queue_fd, events_mask);
	pollfds[0].fd = event_queue_fd;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;

	xwifi_create_thread(api_test_thread, NULL, 0);
	while(poll(pollfds, 1, -1) > 0) {
		if(pollfds[0].revents & POLLIN) {
			xwifi_get_event(event_queue_fd, &wifi_event);
			parse_event(&wifi_event);
		}
	}

	xwifi_close_event_queue(event_queue_fd);
	xwifi_terminated();
	return 0;
}

static int xwifi_create_thread(void *(*__start_routine) (void *), void* param, int priority)
{
    pthread_attr_t        attribute;
    pthread_attr_init(&attribute);
    pthread_attr_setscope(&attribute, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);
    struct sched_param sched;
    pthread_attr_getschedparam(&attribute,&sched);
    if(priority > 0)
    {
    	sched.sched_priority = priority;
    }
    pthread_attr_setschedparam(&attribute,&sched);

    pthread_t thread;
    pthread_create(&thread, &attribute, (void*(*)(void *))__start_routine, (void*)NULL);

    pthread_attr_destroy(&attribute);
    return 0;
}

static void* api_test_thread(void* arg)
{
	S_;

	char c;
	for(;(c=getchar()) != 'q';) {
		// TODO:
		switch(c) {
		case 's': {
			xwifi_scan_request("", 0);
			break;
		}
		case 'r': {
			unsigned int num;
			num = xwifi_get_num_scan_result_entries("", &num);
			int i;
			for(i = 0; i < num ; i++) {
				XWifiScanInfo info;
				xwifi_scan_retrieve_results("", i, &info);
				char tmp[32] = {0};
				sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x",  info.AP.BSSID[0],  info.AP.BSSID[1],  info.AP.BSSID[2],  info.AP.BSSID[3],  info.AP.BSSID[4],  info.AP.BSSID[5]);
				M_("%s, %s", tmp, info.AP.SSID);
			}
#if 0
			XWifiCredentials credentials;
			xwifi_wps_get_credentials(wifi_event->ifName, &credentials);
			XWifiDevicePwd pwd;
			xwifi_wps_generate_dev_pwd(wifi_event->ifName, &pwd);
	//		xwifi_connect(wifi_event->ifName);
	//		int32_t xwifi_connect(const XWifiIfName ifName, const XWifiAP* const AP, const XWifiConnect* const connectionKey)
			 XWifiCapabilities capabilities;
			xwifi_get_capabilities(wifi_event->ifName, &capabilities);
#endif
			break;
		}
		}
	}
	return NULL;
}

static void parse_event(XWifiEvent*	wifi_event)
{
	S_;
	M_("<%s>: event: %d", wifi_event->ifName, wifi_event->event);
	switch(wifi_event->event)
	{
	case XWIFI_EVENT_SCAN_COMPLETED: {
		M_("%s", "XWIFI_EVENT_SCAN_COMPLETED");
		break;
	}

	case XWIFI_EVENT_CONNECTION_ESTABLISHED: {
		M_("%s", "XWIFI_EVENT_CONNECTION_ESTABLISHED");
		break;
	}

	case XWIFI_EVENT_CONNECTION_DROPPED: {
		M_("%s", "XWIFI_EVENT_CONNECTION_DROPPED");
		break;
	}
	default:
		Q_;
//		xwifi_get_num_scan_result_entries("", 0);
		break;
	}
}

