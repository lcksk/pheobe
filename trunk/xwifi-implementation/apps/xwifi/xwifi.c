/*
 * xwifi.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */



#include <pthread.h>
#include "nds/xwifi.h"
#include "xwifi_priv.h"
#include "xwifi_dbg.h"
#include "wpa_ctrl.h"
#include <dirent.h>
#include "eloop.h"
#include <unistd.h>
#include "xwifi_wpa_priv.h"

const XWifiMacAddress xwifi_bss_any = {0x00,0x00,0x00,0x00,0x00,0x00};

struct xwifi {
	int priority;
	XWifiWPA wpa;
	int event_queue_fd;
};

static struct xwifi* xwifi_get_instance();
static void* xwifi_run(void* v);
static int xwifi_create_thread(void *(*__start_routine) (void *), void* param, int priority);


int32_t xwifi_initialise(const int priority)
{
	S_;
	int uid = getuid();
	if(uid != 0) {
		M_("permission denied!");
		exit(0);
	}

	struct xwifi* instance = xwifi_get_instance();
	instance->priority = priority;

	const char* ctrl_ifname = xwifi_get_default_ifname();
	M_("default if: %s", ctrl_ifname);

	instance->wpa = xwifi_wpa_open(ctrl_ifname);
	KASSERT(instance->wpa);

	for (;;) {
		if (xwifi_wpa_open_connection(instance->wpa, ctrl_ifname) == 0) {
			printf("Connection established.\n");
			break;
		}
		sleep(1);
		continue;
	}

	xwifi_create_thread(xwifi_run, instance, priority);

	return 0;
}

void xwifi_terminated(void)
{
	S_;
	struct xwifi* instance = xwifi_get_instance();
	xwifi_wpa_close(instance->wpa);
	instance->wpa = NULL;
	exit(0);
}

int32_t xwifi_get_capabilities(const XWifiIfName ifName, _out_ XWifiCapabilities* const capabilities)
{
	S_;
	return xwifi_wpa_get_capabilities(xwifi_get_instance()->wpa, ifName, capabilities);
}


int32_t xwifi_interface_configure(const XWifiIfName ifName, XWifiInterfaceConfig interfaceConfig)
{
	S_;
	if(interfaceConfig.operatingMode == XWIFI_OPERATING_MODE_MANAGED) {
		M_("managed STA");
	}
	else if(interfaceConfig.operatingMode == XWIFI_OPERATING_MODE_MASTER){
		M_("master AP");
	}
	else {
		W_("Unknown mode: %d", interfaceConfig.operatingMode);
	}
	return 0;
}

static struct xwifi* xwifi_get_instance()
{
	static struct xwifi* instance = NULL;
	if(instance == NULL) {
		instance = malloc(sizeof(*instance));
	}
	return instance;
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

static void* xwifi_run(void* arg)
{
	S_;
	struct xwifi* instance  = (struct xwifi*) arg;
	KUNUSED(instance);
	eloop_run();
	return NULL;
}

int32_t xwifi_scan_request(const XWifiIfName ifName, uint8_t band)
{
	S_;
	xwifi_wpa_cmd_scan(xwifi_get_instance()->wpa);
	return 0;
}

int32_t xwifi_abort_scan_request(const XWifiIfName ifName)
{
	NOT_IMPL;
}

int32_t xwifi_get_num_scan_result_entries(const XWifiIfName ifName, uint32_t* const numEntries)
{
	S_;
	if(numEntries) {
		*numEntries = xwifi_wpa_get_num_scan_result_entries(xwifi_get_instance()->wpa);
	}
	return *numEntries;
}

int32_t xwifi_scan_retrieve_results(const XWifiIfName ifName, uint32_t index, XWifiScanInfo* ScanInfo)
{
	xwifi_wpa_scan_retrieve_results(xwifi_get_instance()->wpa, index, ScanInfo);
	return 0;
}

int32_t xwifi_get_access_point_info(const XWifiIfName ifName, const XWifiMacAddress BSSID, _out_ XWifiAPInformation* const APInfo)
{
	NOT_IMPL;
}

