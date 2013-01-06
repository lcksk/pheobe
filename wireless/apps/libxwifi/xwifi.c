/*
 * xwifi.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */



#include <pthread.h>
#include "nds/xwifi.h"
#include <dirent.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include "bwl.h"
#include "xwifi_dev.h"

#include "osl.h"
#include "osldbg.h"

#define XWIFI_MAX_WIFI_DONGLES	2

#define XWIFI_ALIAS_WIRELESS_LAN_0	"wlan1"
#define XWIFI_ALIAS_WIRELESS_LAN_1	"wlan2"

const XWifiMacAddress xwifi_bss_any = {0x00,0x00,0x00,0x00,0x00,0x00};

struct xwifi {
	int priority;
	xwifi_dev dev[XWIFI_MAX_WIFI_DONGLES];
};

struct xwifi_event_param {
	struct xwifi* self;
	const char ifname[64];
};

static struct xwifi* xwifi_get_instance();
static xwifi_dev xwifi_find_dev(struct xwifi* self, const char* ifname);

const char *buildDate() {
    return __DATE__ " " __TIME__;
}

int32_t xwifi_initialise(const int priority)
{
    fprintf(stderr,
        "xwifi (C)2001-2011, Kaonmedia \n"
    "  Built on %s\n", buildDate());
	S_;
    int uid = getuid();
	if(uid != 0) {
		M_("permission denied!");
		exit(0);
	}
	struct xwifi* self = xwifi_get_instance();

	// built-in
//    WPS_Init();
	self->dev[0] = xwifi_dev_open(XWIFI_ALIAS_WIRELESS_LAN_0, priority);
	KASSERT(self->dev[0]);
	return 0;
}

void xwifi_terminated(void)
{
	S_;
	struct xwifi* self = xwifi_get_instance();
	int i;

	for(i = 0; i < XWIFI_MAX_WIFI_DONGLES; i++) {
		if(self->dev[i] != NULL) {
			xwifi_sys_stop_udhcpc(xwifi_dev_get_ifname(self->dev[i]));
			xwifi_dev_close(self->dev[i]);
			self->dev[i] = NULL;
		}
	}
//	WPS_Uninit();
	//TODO:
	exit(0);
}

int32_t xwifi_get_capabilities(const XWifiIfName ifName, _out_ XWifiCapabilities* const capabilities)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_get_capabilities(dev, capabilities);
}


int32_t xwifi_interface_configure(const XWifiIfName ifName, XWifiInterfaceConfig interfaceConfig)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_set_ifconfig(dev, interfaceConfig);
}

static struct xwifi* xwifi_get_instance() {
	static struct xwifi* instance = NULL;
	if(instance == NULL) {
		instance = (struct xwifi*) malloc(sizeof(*instance));
		memset(instance, 0, sizeof(*instance));
	}
	return instance;
}


int32_t xwifi_scan_request(const XWifiIfName ifName, uint8_t band)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_scan_request(dev, band);
}

int32_t xwifi_abort_scan_request(const XWifiIfName ifName)
{
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_abort_scan_request(dev);
}

int32_t xwifi_get_num_scan_result_entries(const XWifiIfName ifName, uint32_t* const numEntries)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_numof_scan_result(dev);
}

int32_t xwifi_scan_retrieve_results(const XWifiIfName ifName, uint32_t index, XWifiScanInfo* xscanInfo)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	memset(xscanInfo, 0, sizeof(XWifiScanInfo));
	return xwifi_dev_scan_result(dev, index, xscanInfo);
}

int32_t xwifi_get_access_point_info(const XWifiIfName ifName, const XWifiMacAddress BSSID, _out_ XWifiAPInformation* const APInfo)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return 	xwifi_dev_get_ap_info(dev, BSSID, APInfo);
}

int32_t xwifi_power_control(const XWifiIfName ifName, const uint32_t powerState)
{
	NOT_IMPL;
	return 0;
}


int32_t xwifi_connect(const XWifiIfName ifName, const XWifiAP* const AP, const XWifiConnect* const connectionKey)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	M_("connect to: %s", AP->SSID);
	return xwifi_dev_connect(dev, AP->SSID, AP->BSSID, connectionKey);
}

int32_t xwifi_disconnect(const XWifiIfName ifName)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_disconnect(dev);
}


static xwifi_dev xwifi_find_dev(struct xwifi* self, const char* ifname)
{
	if(strcmp(ifname, XWIFI_ALIAS_WIRELESS_LAN_0)==0) {
		return self->dev[0];
	}
	else if(strcmp(ifname, XWIFI_ALIAS_WIRELESS_LAN_1)==0) {
		return self->dev[1];
	}
	return NULL;
}

int32_t xwifi_wps_initiate_registration (const XWifiIfName ifName, const XWifiRegister* registerInfo)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_wps_connect(dev, registerInfo);
}

int32_t xwifi_wps_cancel_registration(const XWifiIfName ifName)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return 0;
}

int32_t xwifi_wps_get_credentials (const XWifiIfName ifName, XWifiCredentials* const credentials)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_get_credentials(dev, credentials);


#if 0
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);

	wps_credentials cred;
	memset(&cred, 0, sizeof(wps_credentials));
	wps_api_get_credentials(&cred);
//	cred.encrType;
//	cred.keyMgmt;
//	cred.nwKey;
//	cred.nwKeyShareable;
//	cred.ssid;
//	cred.wepIndex;
//	if(credentials.encrType == WPS_ENCRYPT_NONE)
//		printf("NONE\n");
//	if(credentials.encrType & WPS_ENCRYPT_WEP)
//		printf(" WEP");
//	if(credentials.encrType & WPS_ENCRYPT_TKIP)
//		printf(" TKIP");
//	if(credentials.encrType & WPS_ENCRYPT_AES)
//		printf(" AES");
	credentials->AP.BSSID;
	credentials->AP.SSID;
//	if(cred.encrType == WPS_ENCRYPT_NONE)

	credentials->connectionKey.encryptionType;
	credentials->connectionKey;
#endif
}

int32_t xwifi_wps_generate_dev_pwd(const XWifiIfName ifName, XWifiDevicePwd* const devicePwd)
{
	S_;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
	return xwifi_dev_generate_pwd(dev, devicePwd);
}

int32_t xwifi_ap_configure(const XWifiIfName ifName, const XWifiAPConfig config)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t xwifi_ap_get_num_associated_devices(const XWifiIfName ifName, uint32_t* const numDevices)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t xwifi_ap_get_associated_device_addr(const XWifiIfName ifName, uint32_t index, XWifiMacAddress* addr)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t	xwifi_ap_configure_access_control_list(const XWifiIfName ifName, const uint8_t enable, XWifiAPACL* const accessControlList)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t xwifi_ap_get_num_denied_sta(const XWifiIfName ifName, uint32_t* numDeniedSTA)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t xwifi_ap_retrive_denied_sta_list(const XWifiIfName ifName, uint32_t numDeniedSTA, XWifiMacAddress* deniedList)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}

int32_t xwifi_ap_reset_denied_sta_list (const XWifiIfName ifName)
{
	NOT_IMPL;
	xwifi_dev dev = xwifi_find_dev(xwifi_get_instance(), ifName);
	KASSERT(dev);
}
