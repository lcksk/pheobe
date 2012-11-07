/*
 * xwifi_wpa.c
 *
 *  Created on: Oct 16, 2012
 *      Author: buttonfly
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>
#include <stdbool.h>
#include "nds/xwifi.h"
#include "xwifi_wpa_priv.h"
#include "xwifi_wpa_parser.h"
#include "wpa_ctrl.h"
#include "os.h"
#include "xwifi_dbg.h"
#include "str.h"
#include "linkedlist.h"
#include "eloop.h"
#include "xwifi_priv.h"

#define COMMAND_SCAN "SCAN"
#define COMMAND_SCAN_RESULTS "SCAN_RESULTS"
#define COMMAND_INTERFACES	"INTERFACES"
#define COMMAND_GET_CAPABILITIES	"GET_CAPABILITY"


struct xwifi_wpa {
	int unused;
	struct wpa_ctrl* ctrl_conn;
	struct wpa_ctrl* mon_conn;
	bool attached;
	char* ifname;
	linkedlist entries; // <XWifiScanInfo>;
	linkedlist interfaces; // char*
};

static int xwifi_wpa_ctrl_cmd(XWifiWPA handle, struct wpa_ctrl *ctrl, char *cmd, void* user);
static int xwifi_wpa_parse_cmd_result(XWifiWPA handle, char* cmd, char* result, void* user);
static void xwifi_wpa_msg_cb(char *msg, size_t len);
static void xwifi_wpa_event_received(int sock, void *eloop_ctx, void *sock_ctx);
static int xwifi_wpa_do_extra(XWifiWPA handle, int event_id);
static XWifiScanInfo* xwifi_wpa_find_by_bssid(linkedlist list, XWifiMacAddress bssid);
static int xwifi_wpa_parse_event(XWifiWPA handle, const char* event);


int xwifi_wpa_open_connection(XWifiWPA handle, const char* ifname)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;

	const char* ctrl_iface_dir = CONFIG_CTRL_IFACE_DIR;

	char *cfile = NULL;
	int flen, res;

	KASSERT(ifname);
	if (ifname == NULL)
		return -1;

	if(instance->ifname) {
		free(instance->ifname);
	}
	instance->ifname = strdup(ifname);

	if (cfile == NULL) {
		flen = os_strlen(ctrl_iface_dir) + os_strlen(ifname) + 2;
		cfile = os_malloc(flen);
		if (cfile == NULL)
			return -1;
		res = os_snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ifname);
		if (res < 0 || res >= flen) {
			os_free(cfile);
			return -1;
		}
	}

	instance->interfaces = linkedlist_new();

	instance->ctrl_conn = wpa_ctrl_open(cfile);
	if (instance->ctrl_conn == NULL) {
		os_free(cfile);
		return -1;
	}

	instance->mon_conn = wpa_ctrl_open(cfile);
	os_free(cfile);

	if (instance->mon_conn) {
		if (wpa_ctrl_attach(instance->mon_conn) == 0) {
			M_("success to attach to wpa_supplicant.");
			instance->attached = true;
			instance->entries = linkedlist_new();
			eloop_register_read_sock(wpa_ctrl_get_fd(instance->mon_conn), xwifi_wpa_event_received, instance, NULL);
		} else {
			ERR("Warning: Failed to attach to wpa_supplicant.\n");
			return -1;
		}
	}
	return 0;
}

void xwifi_wpa_close_connection(XWifiWPA handle)
{
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	if (instance->ctrl_conn == NULL)
		return;

	if (instance->attached) {
		wpa_ctrl_detach(instance->mon_conn);
		instance->attached = true;
	}
	wpa_ctrl_close(instance->ctrl_conn);
	instance->ctrl_conn = NULL;
	if (instance->mon_conn) {
		eloop_unregister_read_sock(wpa_ctrl_get_fd(instance->mon_conn));
		wpa_ctrl_close(instance->mon_conn);
		instance->mon_conn = NULL;
	}
	if(instance->entries) {
		linkedlist_delete(instance->entries, free) ;
		instance->entries = NULL;
	}

	if(instance->interfaces) {
		linkedlist_delete(instance->interfaces, free) ;
		instance->interfaces = NULL;
	}
}

int xwifi_wpa_reconnect(XWifiWPA handle)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	xwifi_wpa_close_connection(handle);
	return xwifi_wpa_open_connection(handle, instance->ifname);
}

static void xwifi_wpa_event_received(int sock, void *eloop_ctx, void *sock_ctx)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)eloop_ctx;
	KUNUSED(sock);
	KUNUSED(sock_ctx);

	int event_id;

	if(!xwifi_wpa_is_connected(instance)) {
		xwifi_wpa_reconnect(instance);
		return;
	}

	struct wpa_ctrl* ctrl = instance->mon_conn;
	for(;wpa_ctrl_pending(instance->mon_conn) > 0;) {
		char buf[256];
		size_t len = sizeof(buf) - 1;
		if(wpa_ctrl_recv(ctrl, buf, &len) == 0) {
			buf[len] = '\0';

			event_id = xwifi_wpa_parse_event(instance, buf);
			xwifi_wpa_do_extra(instance, event_id);

			if(event_id > 0)	{
				int event_queue_fd;
				event_queue_fd = xwifi_open_event_queue(&event_queue_fd);
				XWifiEvent	wifi_event;
				strcpy(wifi_event.ifName, instance->ifname);
				wifi_event.event = event_id;
				xwifi_put_event(event_queue_fd, &wifi_event);
			}
		}
		else {
			ERR("Could not read pending message.\n");
			break;
		}
	}
	if (wpa_ctrl_pending(ctrl) < 0) {
		xwifi_wpa_reconnect(instance);
	}
}

bool xwifi_wpa_is_connected(XWifiWPA handle)
{
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	return (instance->ctrl_conn) ? true : false;
}

XWifiWPA xwifi_wpa_open(const char* alias)
{
	S_;
	if (eloop_init()) {
		ERR("Failed to eloop_init");
		return NULL;
	}

	struct xwifi_wpa* object = (struct xwifi_wpa*) malloc(sizeof(*object));
	memset(object, 0, sizeof(*object ));
	if(alias) {
		object->ifname = strdup(alias);
	}
	else {
		const char* tmp = xwifi_get_default_ifname();
		if(tmp) {
			object->ifname = strdup(tmp);
		}
		else {
			ERR("cannot find a network device.");
			goto error;
		}
	}
	return (XWifiWPA) object;

error:
	if(object) {
		if(object->ifname) {
			free(object->ifname);
		}
		free(object);
	}
	return NULL;
}

void xwifi_wpa_close(XWifiWPA handle)
{
	S_;
	xwifi_wpa_close_connection(handle);
	eloop_terminate();
	free(handle);
}


void xwifi_wpa_cmd_ping(XWifiWPA handle, void* x)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	if (instance->ctrl_conn && xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, "PING", NULL)) {
		printf("Connection to wpa_supplicant lost - trying to reconnect\n");
		xwifi_wpa_close_connection(instance);
	}
	if (!instance->ctrl_conn)
		xwifi_wpa_reconnect(instance);

	eloop_register_timeout(5, 0, xwifi_wpa_cmd_ping, handle, NULL);
}

/*@see also:  xwifi_wpa_parse_cmd_result*/
static int xwifi_wpa_ctrl_cmd(XWifiWPA handle, struct wpa_ctrl *ctrl, char *cmd, void* user)
{
	char buf[2048];
	size_t len;
	int ret;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(ctrl, cmd, os_strlen(cmd), buf, &len, xwifi_wpa_msg_cb);
	if (ret == -2) {
		ERR("'%s' command timed out.", cmd);
		return -2;
	} else if (ret < 0) {
		ERR("'%s' command failed.", cmd);
		return -1;
	}

	buf[len] = '\0';
	xwifi_wpa_parse_cmd_result(handle, cmd, buf, user);
	return 0;
}

static void xwifi_wpa_msg_cb(char *msg, size_t len)
{
	M_("request msg: %s", msg);
}

int xwifi_wpa_cmd_scan(XWifiWPA handle)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	return xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_SCAN, NULL);
}

int xwifi_wpa_get_num_scan_result_entries(XWifiWPA handle)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	KASSERT(instance->entries);
	return linkedlist_size(instance->entries);
}

int xwifi_wpa_scan_retrieve_results(XWifiWPA handle, uint32_t index, _out_ XWifiScanInfo* ScanInfo)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	KASSERT(instance->entries);
	const XWifiScanInfo* tmp = (const XWifiScanInfo*) linkedlist_get(instance->entries, index);
	memcpy(ScanInfo, tmp, sizeof(*tmp));
	return 0;
}

int xwifi_wpa_cmd_scan_result(XWifiWPA handle)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	return xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_SCAN_RESULTS, NULL);
}

int xwifi_wpa_cmd_interface(XWifiWPA handle)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	return xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_INTERFACES, NULL);
}

int xwifi_wpa_cmd_get_capabilities(XWifiWPA handle, _out_ XWifiCapabilities* const capabilities)
{
	S_;
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KASSERT(instance);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " eap", capabilities);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " pairwise", capabilities);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " group", capabilities);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " key_mgmt", capabilities);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " proto", capabilities);
	xwifi_wpa_ctrl_cmd(instance, instance->ctrl_conn, COMMAND_GET_CAPABILITIES " auth_alg", capabilities);
	return 0;
}

static int xwifi_wpa_do_extra(XWifiWPA handle, int event_id)
{
	switch(event_id)
	{
	case XWIFI_EVENT_SCAN_COMPLETED : {
		xwifi_wpa_cmd_scan_result(handle);
		xwifi_wpa_cmd_interface(handle);
		break;
	}
	case XWIFI_EVENT_CONNECTION_ESTABLISHED : {
		// TODO:
		break;
	}
	case XWIFI_EVENT_CONNECTION_DROPPED : {
		// TODO:
		break;
	}

	}
	return 0;
}

static int xwifi_wpa_parse_cmd_result(XWifiWPA handle, char* cmd, char* result, void* user)
{
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;

	if (os_strcmp(cmd, COMMAND_SCAN_RESULTS) == 0) {
		xwifi_wpa_parser_scan_result(instance->entries, result);
	}
	else if(os_strcmp(cmd, COMMAND_INTERFACES) == 0) {
		xwifi_wpa_parser_interfaces_result(instance->interfaces, result);
	}
	else {
		M_("%s: %s", cmd, result);
	}

	return 0;
}

int32_t xwifi_wpa_get_capabilities(XWifiWPA handle, const XWifiIfName ifName, _out_ XWifiCapabilities* const capabilities)
{
	xwifi_wpa_cmd_get_capabilities(handle, capabilities);
	return 0;
}

static XWifiScanInfo* xwifi_wpa_find_by_bssid(linkedlist list, XWifiMacAddress bssid)
{
	XWifiScanInfo* scanInfo = NULL;
	int size = linkedlist_size(list);
	int i;
	for(i = 0; i < size; i++) {
		scanInfo = (XWifiScanInfo*)linkedlist_get(list, i);
		if(memcmp(scanInfo->AP.BSSID, bssid, XWIFI_MAC_ADDR_LENGTH)==0) {
			return scanInfo;
		}
	}
	return NULL;
}

static int xwifi_wpa_parse_event(XWifiWPA handle, const char* event)
{
	struct xwifi_wpa* instance = (struct xwifi_wpa*)handle;
	KUNUSED(instance);

	M_("%s", event);
	const char *start, *s;

	start = os_strchr(event, '>');
	if (start == NULL)
		return -1;

	start++;

	if (str_starts(start, WPA_EVENT_SCAN_RESULTS)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
		return XWIFI_EVENT_SCAN_COMPLETED;
	}

	if (str_starts(start, WPA_EVENT_CONNECTED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
		return XWIFI_EVENT_CONNECTION_ESTABLISHED;
	}

	if (str_starts(start, WPA_EVENT_DISCONNECTED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
		return XWIFI_EVENT_CONNECTION_DROPPED;
	}

	if (str_starts(start, WPA_EVENT_BSS_ADDED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
		s = os_strchr(s + 1, ' ');
		if (s == NULL)
			return -1;
//		cli_txt_list_add(&bsses, s + 1);
#if 0
		char tmp[64] = {0};
		sscanf(s+1, "%02x:%02x:%02x:%02x:%02x:%02x",
				(unsigned int *) &tmp[0],
				(unsigned int *) &tmp[1],
				(unsigned int *) &tmp[2],
				(unsigned int *) &tmp[3],
				(unsigned int *) &tmp[4],
				(unsigned int *) &tmp[5]);
		XWifiScanInfo* scanInfo = xwifi_wpa_find_by_bssid(instance->entries, tmp);
		if(!scanInfo) {
			linkedlist_add(instance->entries, scanInfo);
		}
		else {
			W_("duplicated");
		}
#endif
		return -1;
	}

	if (str_starts(start, WPA_EVENT_BSS_REMOVED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
		s = os_strchr(s + 1, ' ');
		if (s == NULL)
			return -1;
//		cli_txt_list_del_addr(&bsses, s + 1);
		return -1;
	}

#ifdef CONFIG_P2P
	if (str_starts(start, P2P_EVENT_DEVICE_FOUND)) {
		s = os_strstr(start, " p2p_dev_addr=");
		if (s == NULL)
			return -1;
//		cli_txt_list_add_addr(&p2p_peers, s + 14);
		return -1;
	}

	if (str_starts(start, P2P_EVENT_DEVICE_LOST)) {
		s = os_strstr(start, " p2p_dev_addr=");
		if (s == NULL)
			return -1;
//		cli_txt_list_del_addr(&p2p_peers, s + 14);
		return 0;
	}

	if (str_starts(start, P2P_EVENT_GROUP_STARTED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
//		cli_txt_list_add_word(&p2p_groups, s + 1);
		return  0;
	}

	if (str_starts(start, P2P_EVENT_GROUP_REMOVED)) {
		s = os_strchr(start, ' ');
		if (s == NULL)
			return -1;
//		cli_txt_list_del_word(&p2p_groups, s + 1);
		return 0;
	}
#endif /* CONFIG_P2P */
	return 0;
}


