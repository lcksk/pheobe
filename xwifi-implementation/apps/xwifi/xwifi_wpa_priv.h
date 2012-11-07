/*
 * xwifi_wpa.h
 *
 *  Created on: Oct 16, 2012
 *      Author: buttonfly
 */

#ifndef XWIFI_WPA_H_
#define XWIFI_WPA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#ifndef CONFIG_CTRL_IFACE_DIR
#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#endif

typedef void* XWifiWPA;

//XWifiWPA xwifi_wpa_get_instance();

XWifiWPA xwifi_wpa_open(const char* alias);

void xwifi_wpa_close(XWifiWPA handle);

int xwifi_wpa_open_connection(XWifiWPA handle, const char* ifname);

void xwifi_wpa_close_connection(XWifiWPA handle);

int xwifi_wpa_reconnect(XWifiWPA handle);

bool xwifi_wpa_is_connected(XWifiWPA handle);

int xwifi_wpa_get_num_scan_result_entries(XWifiWPA handle);

int xwifi_wpa_scan_retrieve_results(XWifiWPA handle, uint32_t index, _out_ XWifiScanInfo* ScanInfo);

int32_t xwifi_put_event(const int _fd, XWifiEvent* const event);

int32_t xwifi_wpa_get_capabilities(XWifiWPA handle, const XWifiIfName ifName, _out_ XWifiCapabilities* const capabilities);

void xwifi_wpa_cmd_ping(XWifiWPA handle, void* x	);

int xwifi_wpa_cmd_scan(XWifiWPA handle);

int xwifi_wpa_cmd_interface(XWifiWPA handle);

int xwifi_wpa_cmd_get_capabilities(XWifiWPA handle, _out_ XWifiCapabilities* const capabilities);

#ifdef __cplusplus
}
#endif


#endif /* XWIFI_WPA_H_ */
