/*
 * main.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */


#include "nds/xwifi.h"
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
#include "linkedlist.h"
#include "osl.h"
#include "osldbg.h"

#define PRIORITY_LEVEL 10
#define DEFAULT_IFNAME	"wlan1"

static void* api_test_thread(void* arg);
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
	const char* ifname = DEFAULT_IFNAME;

	xwifi_initialise(PRIORITY_LEVEL);

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	XWifiInterfaceConfig interfaceConfig;
	memset(&interfaceConfig, 0, sizeof(XWifiInterfaceConfig));
	interfaceConfig.operatingMode = XWIFI_OPERATING_MODE_MANAGED;
	strcpy(interfaceConfig.regulatoryDomain, "KR");
//	memset(interfaceConfig.regulatoryDomain, 0x03,4);
	xwifi_interface_configure(ifname, interfaceConfig);
	xwifi_power_control(ifname, XWIFI_POWER_ON);
	/* Initialise the event queue */
	event_queue_fd = xwifi_open_event_queue(NULL);
	/* Subscribe to wi-fi events */
	events_mask = (XWIFI_EVENT_SCAN_COMPLETED | XWIFI_EVENT_CONNECTION_ESTABLISHED | XWIFI_EVENT_CONNECTION_DROPPED);
	xwifi_subscribe_events(event_queue_fd, events_mask);
	pollfds[0].fd = event_queue_fd;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;

	osl_thread_create(api_test_thread, NULL, 0, 0, NULL);

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

static void* api_test_thread(void* arg)
{
	S_;

	linkedlist ap_list = linkedlist_new();

	char c;
	for(;(c=getchar()) != 'q';) {
		// TODO:
		switch(c) {
		case 'a': {
			xwifi_abort_scan_request(DEFAULT_IFNAME);
			break;
		}
		case 's': {
			xwifi_scan_request("wlan1", XWIFI_FREQUENCY_BAND_2_4GHZ);
			break;
		}
		case 'r': {
			unsigned int num;
			linkedlist_clear(ap_list, free);
			num = xwifi_get_num_scan_result_entries(DEFAULT_IFNAME, &num);
			int i;
			for(i = 0; i < num ; i++) {
				XWifiScanInfo* info = NULL;
				info = (XWifiScanInfo*) malloc(sizeof(XWifiScanInfo));
				memset(info, 0, sizeof(XWifiScanInfo));
				xwifi_scan_retrieve_results(DEFAULT_IFNAME, i, info);
				char tmp[32] = {0};
				sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x",  info->AP.BSSID[0],  info->AP.BSSID[1],  info->AP.BSSID[2],  info->AP.BSSID[3],  info->AP.BSSID[4],  info->AP.BSSID[5]);
				M_("%s, [%d] [[[[[[[[[[[[[[[[[[ %s ]]]]]]]]]]]]]]]]]] <%d>", tmp, i, info->AP.SSID, info->APInfo.primaryChannel);
//				M_("%s", tmp);
				linkedlist_add(ap_list, info);
			}
			XWifiCredentials cred;
			memset(&cred, 0, sizeof(XWifiCredentials));
			xwifi_wps_get_credentials(DEFAULT_IFNAME, &cred);

			XWifiRegister registarar;
			memset(&registarar, 0, sizeof(registarar));
			xwifi_wps_generate_dev_pwd(DEFAULT_IFNAME, &registarar.devicePwd);
			registarar.devicePwdLength = XWIFI_WPS_MAX_DEVICE_PWD_LENGTH;
//			xwifi_wps_initiate_registration("wlan0", &registarar);
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

		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :
		case '0' : {

			int i;
			XWifiScanInfo* info = NULL;
#if 0
			for(i = 0; i < linkedlist_size(ap_list); i++) {
				info= (XWifiScanInfo*) linkedlist_get(ap_list, i);
				if(strcmp(info->AP.SSID, "halkamalka") ==0) {
					break;
				}
				info = NULL;
			}
#endif
			info= (XWifiScanInfo*)linkedlist_get(ap_list, c - '0');
			if(info) {
				XWifiConnect connect;
				memset(&connect, 0, sizeof(connect));
				connect.encryptionType = info->APInfo.encryptions[0].encryptionType;
				switch(connect.encryptionType) {
				case XWIFI_ENCRYPTION_NONE:
					break;

				case XWIFI_ENCRYPTION_WEP64:
					connect.u.WEP64.authAlg = XWIFI_ENCRYPTION_WEP64;
					connect.u.WEP64.defaultKeyIndex=1;
					strcpy(connect.u.WEP64.key,  "12345");
					break;

				case XWIFI_ENCRYPTION_WEP128:
					connect.u.WEP128.authAlg = XWIFI_ENCRYPTION_WEP128;
					connect.u.WEP128.defaultKeyIndex=1;
					strcpy(connect.u.WEP128.key,  "0123456789abc");
					break;

				case XWIFI_ENCRYPTION_WPA:
					connect.u.WPA.isAscii = 1;
					connect.u.WPA.u.passphrase.length = strlen("kaon1234");
					strcpy(connect.u.WPA.u.passphrase.key, "kaon1234");
					break;

				case XWIFI_ENCRYPTION_WPA2:
					connect.u.WPA.isAscii = 1;
					connect.u.WPA.u.passphrase.length = strlen("kaon1234");
					strcpy(connect.u.WPA.u.passphrase.key, "kaon1234");
					break;
				}
				connect.u.WPA.u.passphrase.length = strlen("kaon1234");
				strcpy(connect.u.WPA.u.passphrase.key, "kaon1234");
				xwifi_connect(DEFAULT_IFNAME, &info->AP, &connect);
			}
			break;
		}

		case 'd' : {
			xwifi_disconnect(DEFAULT_IFNAME);
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
		M_("event: %d", wifi_event->event);
//		xwifi_get_num_scan_result_entries("", 0);
		break;
	}
}

