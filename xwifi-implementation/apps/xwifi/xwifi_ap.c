/*
 * xwifi_ap.c
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */


#include "nds/xwifi.h"
#include "xwifi_dbg.h"

int32_t xwifi_ap_configure(const XWifiIfName ifName, const XWifiAPConfig config)
{
	NOT_IMPL;
}

int32_t xwifi_ap_get_num_associated_devices(const XWifiIfName ifName, uint32_t* const numDevices)
{
	NOT_IMPL;
}

int32_t xwifi_ap_get_associated_device_addr(const XWifiIfName ifName, uint32_t index, XWifiMacAddress* addr)
{
	NOT_IMPL;
}

int32_t	xwifi_ap_configure_access_control_list(const XWifiIfName ifName, const uint8_t enable, XWifiAPACL* const accessControlList)
{
	NOT_IMPL;
}

int32_t xwifi_ap_get_num_denied_sta(const XWifiIfName ifName, uint32_t* numDeniedSTA)
{
	NOT_IMPL;
}

int32_t xwifi_ap_retrive_denied_sta_list(const XWifiIfName ifName, uint32_t numDeniedSTA, XWifiMacAddress* deniedList)
{
	NOT_IMPL;
}

int32_t xwifi_ap_reset_denied_sta_list (const XWifiIfName ifName)
{
	NOT_IMPL;
}
