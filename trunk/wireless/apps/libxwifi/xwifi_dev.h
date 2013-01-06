/*
 * xwifi_dev.h
 *
 *  Created on: Nov 30, 2012
 *      Author: buttonfly
 */

#ifndef XWIFI_DEV_H_
#define XWIFI_DEV_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <net/ethernet.h>
#include "nds/xwifi.h"

typedef void* xwifi_dev;

xwifi_dev xwifi_dev_open(const char* ifname, int priority);

int xwifi_dev_scan_request(xwifi_dev dev, int band);

int xwifi_dev_abort_scan_request(xwifi_dev dev);

int xwifi_dev_numof_scan_result(xwifi_dev dev);

int xwifi_dev_scan_result(xwifi_dev dev, int index, XWifiScanInfo* ScanInfo);

int xwifi_dev_connect(xwifi_dev dev, const char ssid[XWIFI_MAX_SSID_LENGTH], const char bssid[XWIFI_MAC_ADDR_LENGTH], const XWifiConnect* const connectionKey);

int xwifi_dev_disconnect(xwifi_dev dev);

int xwifi_dev_get_ap_info(xwifi_dev dev, const char bssid[XWIFI_MAC_ADDR_LENGTH],  XWifiAPInformation* const APInfo);

int xwifi_dev_set_ifconfig(xwifi_dev dev, XWifiInterfaceConfig conf);

int xwifi_dev_get_capabilities(xwifi_dev dev, XWifiCapabilities* const cap);

int xwifi_dev_close(xwifi_dev dev);

int xwifi_dev_get_credentials(xwifi_dev dev, XWifiCredentials* const credentials);

int xwifi_dev_generate_pwd(xwifi_dev dev, XWifiDevicePwd* const devicePwd);

int xwifi_dev_wps_connect(xwifi_dev dev, const XWifiRegister* registerInfo);

int xwifi_dev_wps_abort(xwifi_dev dev);

const char* xwifi_dev_get_ifname(xwifi_dev dev);

#ifdef __cplusplus
}
#endif


#endif /* XWIFI_DEV_H_ */
