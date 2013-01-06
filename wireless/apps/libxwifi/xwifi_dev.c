/*
 * xwifi_dev.c
 *
 *  Created on: Nov 30, 2012
 *      Author: buttonfly
 */


#define __INCif_etherh

#include <pthread.h>
#include "nds/xwifi.h"
#include <dirent.h>
#include <unistd.h>
#include "xwifi_dev.h"
#include "linkedlist.h"


#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "typedefs.h"
#include "bwl.h"

#include "osl.h"
#include "osldbg.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include "wlioctl.h"
#include <bcmutils.h>
#include <bcmendian.h>
#include <bcmwifi.h>
#include <bcmsrom_fmt.h>
#include <bcmsrom_tbl.h>
#include <bcmcdc.h>

/* need this for using exec */
#include <unistd.h>
#include <sys/wait.h>
#include "wlu.h"
#define stricmp strcasecmp
#define strnicmp strncasecmp

/* IOCTL swapping mode for Big Endian host with Little Endian dongle.  Default to off */
#ifdef IL_BIGENDIAN
bool swap = FALSE;
#define htod32(i) (swap?bcmswap32(i):i)
#define htod16(i) (swap?bcmswap16(i):i)
#define dtoh32(i) (swap?bcmswap32(i):i)
#define dtoh16(i) (swap?bcmswap16(i):i)
#define htodchanspec(i) htod16(i)
#define dtohchanspec(i) dtoh16(i)
#define htodenum(i) ((sizeof(i) == 4) ? htod32(i) : ((sizeof(i) == 2) ? htod16(i) : i))
#define dtohenum(i) ((sizeof(i) == 4) ? dtoh32(i) : ((sizeof(i) == 2) ? htod16(i) : i))
#else /* IL_BIGENDIAN */
#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i
#define htodchanspec(i) i
#define dtohchanspec(i) i
#define htodenum(i) i
#define dtohenum(i) i
#endif /* IL_BIGENDIAN */



struct xwifi_dev_t {
	BWL_Handle bwlHandle;
	int eventque;
	const char ifname[64];
	linkedlist lst_scanresult; // <wl_bss_info_t*>
	int band;
};


typedef void (*xwifi_dev_event_cb)(struct xwifi_dev_t* dev);
typedef struct {
	EventMessage_t 			e;
	xwifi_dev_event_cb 	fp;
} dev_event_cb_map;

#define EVENT_FUNC(name) static int dev_event_##name(struct xwifi_dev_t* self)
#define EVENT_FUNC_NAME(name) dev_event_##name

EVENT_FUNC(BWL_E_SCAN_COMPLETE);
EVENT_FUNC(BWL_E_LINK);


static dev_event_cb_map* dev_event_map() {
	static dev_event_cb_map cb_map[BWL_E_LAST] = {
		    {BWL_E_SET_SSID,   NULL},      /* indicates status of set SSID */
		    {BWL_E_JOIN, NULL},             /* differentiates join IBSS from found (WLC_E_START) IBSS */
		    {BWL_E_START, NULL},            /* STA founded an IBSS or AP started a BSS */
		    {BWL_E_AUTH, NULL},             /* 802.11 AUTH request */
		    {BWL_E_AUTH_IND, NULL},         /* 802.11 AUTH indication */
		    {BWL_E_DEAUTH,NULL},           /* 802.11 DEAUTH request */
		    {BWL_E_DEAUTH_IND,NULL},       /* 802.11 DEAUTH indication */
		    {BWL_E_ASSOC,NULL},            /* 802.11 ASSOC request */
		    {BWL_E_ASSOC_IND,NULL},        /* 802.11 ASSOC indication */
		    {BWL_E_REASSOC,NULL},          /* 802.11 REASSOC request */
		    {BWL_E_REASSOC_IND,NULL},      /* 802.11 REASSOC indication */
		    {BWL_E_DISASSOC,NULL},         /* 802.11 DISASSOC request */
		    {BWL_E_DISASSOC_IND,NULL},     /* 802.11 DISASSOC indication */
		    {BWL_E_QUIET_START,NULL},      /* 802.11h Quiet period started */
		    {BWL_E_QUIET_END,NULL},        /* 802.11h Quiet period ended */
		    {BWL_E_BEACON_RX,NULL},        /* BEACONS received/lost indication */
		    {BWL_E_LINK,EVENT_FUNC_NAME(BWL_E_LINK)},             /* generic link indication */
		    {BWL_E_MIC_ERROR,NULL},        /* TKIP MIC error occurred */
		    {BWL_E_NDIS_LINK,NULL},        /* NDIS style link indication */
		    {BWL_E_ROAM,NULL},             /* roam attempt occurred: indicate status & reason */
		    {BWL_E_TXFAIL,NULL},           /* change in dot11FailedCount (txfail) */
		    {BWL_E_PMKID_CACHE,NULL},      /* WPA2 pmkid cache indication */
		    {BWL_E_RETROGRADE_TSF,NULL},   /* current AP's TSF value went backward */
		    {BWL_E_PRUNE,NULL},            /* AP was pruned from join list for reason */
		    {BWL_E_AUTOAUTH,NULL},         /* report AutoAuth table entry match for join attempt */
		    {BWL_E_EAPOL_MSG,NULL},        /* Event encapsulating an EAPOL message */
		    {BWL_E_SCAN_COMPLETE, EVENT_FUNC_NAME(BWL_E_SCAN_COMPLETE)},    /* Scan results are ready or scan was aborted */
		    {BWL_E_ADDTS_IND,NULL},        /* indicate to host addts fail/success */
		    {BWL_E_DELTS_IND,NULL},        /* indicate to host delts fail/success */
		    {BWL_E_BCNSENT_IND,NULL},      /* indicate to host of beacon transmit */
		    {BWL_E_BCNRX_MSG,NULL},        /* Send the received beacon up to the host */
		    {BWL_E_BCNLOST_MSG,NULL},      /* indicate to host loss of beacon */
		    {BWL_E_ROAM_PREP,NULL},        /* before attempting to roam */
		    {BWL_E_PFN_NET_FOUND,NULL},    /* PFN network found event */
		    {BWL_E_PFN_NET_LOST,NULL},     /* PFN network lost event */
		    {BWL_E_RESET_COMPLETE,NULL},
		    {BWL_E_JOIN_START,NULL},
		    {BWL_E_ROAM_START,NULL},
		    {BWL_E_ASSOC_START,NULL},
		    {BWL_E_IBSS_ASSOC,NULL},
		    {BWL_E_RADIO,NULL},
		    {BWL_E_PSM_WATCHDOG,NULL},    /* PSM microcode watchdog fired */
		    {BWL_E_PROBREQ_MSG,NULL},     /* probe request received */
		    {BWL_E_SCAN_CONFIRM_IND,NULL},
		    {BWL_E_PSK_SUP,NULL},         /* WPA Handshake fail */
		    {BWL_E_COUNTRY_CODE_CHANGED,NULL},
		    {BWL_E_EXCEEDED_MEDIUM_TIME,NULL}, /* WMMAC excedded medium time */
		    {BWL_E_ICV_ERROR,NULL},       /* WEP ICV error occurred */
		    {BWL_E_UNICAST_DECODE_ERROR,NULL}, /* Unsupported unicast encrypted frame */
		    {BWL_E_MULTICAST_DECODE_ERROR,NULL},  /* Unsupported multicast encrypted frame */
		    {BWL_E_TRACE,NULL},
		    {BWL_E_IF,NULL},               /* I/F change (for dongle host notification) */
		    {BWL_E_RSSI,NULL},             /* indicate RSSI change based on configured levels */
		    {BWL_E_PFN_SCAN_COMPLETE,NULL},/* PFN completed scan of network list */
		    {BWL_E_EXTLOG_MSG,NULL},
		    {BWL_E_ACTION_FRAME,NULL},
		    {BWL_E_PRE_ASSOC_IND,NULL},    /* assoc request received */
		    {BWL_E_PRE_REASSOC_IND,NULL},  /* re-assoc request received */
		    {BWL_E_CHANNEL_ADOPTED,NULL},  /* channel adopted */
		    {BWL_E_AP_STARTED,NULL},       /* AP started */
		    {BWL_E_DFS_AP_STOP,NULL},      /* AP stopped due to DFS */
		    {BWL_E_DFS_AP_RESUME,NULL},    /* AP resumed due to DFS */
		    {BWL_E_LAST,NULL},             /* highest val + 1 for range checking */
	};
	return cb_map;
}



static wl_bss_info_t* xwifi_dev_find_by_bssid(struct xwifi_dev_t* dev, const XWifiMacAddress BSSID);
static int xwifi_dev_convert_bssInfo_to_apInfo(wl_bss_info_t* scanInfo, XWifiAPInformation* apInfo);
static int32_t dev_get_scan_results(struct xwifi_dev_t* dev, wl_scan_results_t *res, int buflen);
static int dev_rsn_ie_dump(bcm_tlv_t* ie, XWifiEncryption* encr);
static uint wl_rsn_ie_decode_cntrs(uint cntr_field);
static void* xwifi_dev_wait_event(void* arg);

typedef struct {
	uint16 version;
	wpa_suite_mcast_t *mcast;
	wpa_suite_ucast_t *ucast;
	wpa_suite_auth_key_mgmt_t *akm;
	uint8 *capabilities;
} rsn_parse_info_t;
static int wl_rsn_ie_parse_info(uint8* rsn_buf, uint len, rsn_parse_info_t *rsn);

xwifi_dev xwifi_dev_open(const char* ifname, int priority)
{
	struct xwifi_dev_t* obj = (struct xwifi_dev_t*) malloc(sizeof(struct xwifi_dev_t));
	memset(obj, 0, sizeof(struct xwifi_dev_t));
	obj->lst_scanresult = linkedlist_new();

	obj->bwlHandle = BWL_Open(ifname);
	BWL_Up(obj->bwlHandle);
	strcpy(obj->ifname, ifname);

	obj->eventque = xwifi_open_event_queue(NULL);
	osl_thread_create(xwifi_dev_wait_event, obj, 0, priority, NULL);

    RevInfo_t RevInfo={0};
    char b[8] = {0};
    int err = BWL_GetRevInfo (obj->bwlHandle, &RevInfo);

    M_("vendorid 0x%x", RevInfo.ulVendorId);
    M_("deviceid 0x%x", RevInfo.ulDeviceId);
    M_("radiorev 0x%x", RevInfo.ulRadioRev);
    M_("chipnum 0x%x", RevInfo.ulChipNum);
    M_("chiprev 0x%x", RevInfo.ulChipRev);
    M_("chippackage 0x%x", RevInfo.ulChipPkg);
    M_("corerev 0x%x", RevInfo.ulCoreRev);
    M_("boardid 0x%x", RevInfo.ulBoardId);
    M_("boardvendor 0x%x", RevInfo.ulBoardVendor);
    if (RevInfo.ulBoardRev < 0x100)
            sprintf(b, "%d.%d", (RevInfo.ulBoardRev & 0xf0) >> 4, RevInfo.ulBoardRev & 0xf);
    else
            sprintf(b, "%c%03x", ((RevInfo.ulBoardRev & 0xf000) == 0x1000) ? 'P' : 'A', RevInfo.ulBoardRev & 0xfff);
    b[8]='\0';
    M_("boardrev %s", b);
    M_("driverrev 0x%x", RevInfo.ulDriverRev);
    M_("ucoderev 0x%x", RevInfo.ulUcodeRev);
    M_("bus 0x%x", RevInfo.ulBus);
    M_("phytype 0x%x", RevInfo.ulPhyType);
    M_("phyrev 0x%x", RevInfo.ulPhyRev);
    M_("anarev 0x%x", RevInfo.ulAnaRev);

	return (xwifi_dev) obj;
}

int xwifi_dev_close(xwifi_dev dev)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	BWL_Close(self->bwlHandle);
	free(dev);
	return 0;
}

int xwifi_dev_get_credentials(xwifi_dev dev, XWifiCredentials* const credentials)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;

	Credential_t cred;
	memset(&cred, 0, sizeof(cred));
	int err = BWL_GetCredential(self->bwlHandle, &cred);
	KASSERT(err==BWL_ERR_SUCCESS);

	//TODO
	strcpy(credentials->AP.SSID, cred.acSSID);
	if(cred.peBSSID) {
		memcpy(credentials->AP.BSSID, cred.peBSSID->ether_addr_octet, ETH_ALEN);
	}

//    eWSecInvalid    = 0xff,
//    eWSecNone       = 0x01,
//    eWSecWep        = 0x02,
//    eWSecTkip       = 0x04,
//    eWSecAes        = 0x08,
//    eWSecAll        = 0x10,

#if 0
	if(cred.eWSec & eWSecNone) {
		credentials->connectionKey.encryptionType = XWIFI_CIPHER_SUITE_NONE;
	}

	if(cred.eWSec & eWSecWep) {
		credentials->connectionKey.encryptionType |= XWIFI_CIPHER_SUITE_NONE;
	}

	if(cred.eWSec & eWSecTkip) {
		credentials->connectionKey.encryptionType |= XWIFI_CIPHER_SUITE_TKIP;
	}

	if(cred.eWSec & eWSecAes) {
		credentials->connectionKey.encryptionType |= XWIFI_CIPHER_SUITE_CCMP;
	}

	if(cred.eWSec & eWSecAll) {
		credentials->connectionKey.encryptionType |= XWIFI_CIPHER_SUITE_TKIP | XWIFI_CIPHER_SUITE_CCMP;
	}
#endif

	if(cred.eWpaAuth &eWpaAuthNone)
		credentials->connectionKey.encryptionType = XWIFI_ENCRYPTION_NONE;

	if(cred.eWpaAuth & eWpaAuthWpaUnsp)
		credentials->connectionKey.encryptionType  |= XWIFI_ENCRYPTION_UNSUPPORTED;

	if(cred.eWpaAuth & eWpaAuthWpaPsk)
		credentials->connectionKey.encryptionType  |= XWIFI_ENCRYPTION_WPA;

	if(cred.eWpaAuth & eWpaAuthWpa2Unsp)
		credentials->connectionKey.encryptionType  |= XWIFI_ENCRYPTION_UNSUPPORTED;

	if(cred.eWpaAuth & eWpaAuthWpa2Unsp)
		credentials->connectionKey.encryptionType  |= XWIFI_ENCRYPTION_WPA2;


	switch(cred.eAuthType) {
	case eAuthTypeOpen:
		credentials->connectionKey.u.WEP64.authAlg = XWIFI_WEP_AUTH_ALG_OPEN;
		break;

	case eAuthTypeShare:
		credentials->connectionKey.u.WEP64.authAlg = XWIFI_WEP_AUTH_ALG_SHARED_KEY;
		break;

	case eAuthTypeOpenShare:
		credentials->connectionKey.u.WEP64.authAlg = XWIFI_WEP_AUTH_ALG_OPEN  | XWIFI_WEP_AUTH_ALG_SHARED_KEY;
		break;

	case eAuthTypeInvalid:
	default:
		break;
	}




//    eAuthTypeShare,
//    eAuthTypeOpenShare,
//    eAuthTypeInvalid
//	cred.eAuthType;
//	cred.ulWepIndex;
//	cred.eWpaSup;
//	credentials->connectionKey.u.WEB128;

	return err;
}

int xwifi_dev_generate_pwd(xwifi_dev dev, XWifiDevicePwd* const devicePwd)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	WSecPmk_t key;
	memset(&key, 0, sizeof(WSecPmk_t));
	int err = BWL_GenerateDptKey(self->bwlHandle, &key);
	KASSERT(err==BWL_ERR_SUCCESS);
	M_("flags: %d", key.Flags);
	H_(key.Key, key.KeyLen);
	return 0;
}

int xwifi_dev_wps_connect(xwifi_dev dev, const XWifiRegister* registerInfo)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	uint32_t connected;
	int err;
	err = BWL_IsConnectedAp(self->bwlHandle, &connected);
	if(connected){
		BWL_DisconnectAp(self->bwlHandle);
	}

#if 1
	switch(registerInfo->deviceConfigMethod) {
	case XWIFI_WPS_CONFIG_LABEL:
		break;

	case XWIFI_WPS_CONFIG_DISPLAY:
		break;

	case XWIFI_WPS_CONFIG_PUSH_BUTTON:
		err = BWL_WpsConnectByPb(self->bwlHandle, self->ifname, registerInfo->devicePwd, registerInfo->devicePwdLength);
		break;

	case XWIFI_WPS_CONFIG_KEYPAD:
//		err = BWL_WpsConnectByPin(self->bwlHandle, self->ifname, registerInfo->devicePwd, registerInfo->devicePwdLength);
		break;

	case XWIFI_WPS_CONFIG_NOT_SUPPORTED:
	default:
		W_("Unknown");
		break;
	}
	err = BWL_WpsConnectByPb(self->bwlHandle, self->ifname, registerInfo->devicePwd, registerInfo->devicePwdLength);
	// TODO:
//	KASSERT(err==BWL_ERR_SUCCESS);
	if(err == BWL_ERR_SUCCESS) {
		xwifi_sys_start_udhcpc(self->ifname);
	}
#else
	char cmd[512] = {0};
	sprintf(cmd, "wpsenr -if %s -pb -ssid %s", self->ifname, "halkamalka");
	M_("%s", cmd);
	system(cmd);
#endif
	return err;
}

int xwifi_dev_wps_abort(xwifi_dev dev)
{
	return 0;
}

int xwifi_dev_scan_request(xwifi_dev dev, int band)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	self->band = band;

	ScanParams_t param;
    memset(&param, 0, sizeof(ScanParams_t));
    param.lActiveTime  = BWL_DEFAULT_SCAN_DWELL_TIME;
    param.lPassiveTime = BWL_DEFAULT_SCAN_DWELL_TIME;
    param.lHomeTime    = BWL_DEFAULT_SCAN_DWELL_TIME;
	int err = BWL_Scan(self->bwlHandle,  &param);
	KASSERT(err==BWL_ERR_SUCCESS);
	return err;
}

const char* xwifi_dev_get_ifname(xwifi_dev dev)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	return self->ifname;
}

int xwifi_dev_abort_scan_request(xwifi_dev dev)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	int err = BWL_ScanAbort(self->bwlHandle);
	KASSERT(err==BWL_ERR_SUCCESS);
	return err;
}

int xwifi_dev_scan_result(xwifi_dev dev, int index, XWifiScanInfo* xscanInfo)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	wl_bss_info_t* bss_info = (wl_bss_info_t*) linkedlist_get(self->lst_scanresult, index);
	if(!bss_info) {
		ERR("No such AP exception");
		return -1;
	}

	memcpy(xscanInfo->AP.BSSID, bss_info->BSSID.ether_addr_octet, XWIFI_MAC_ADDR_LENGTH);
	memcpy(xscanInfo->AP.SSID, bss_info->SSID, bss_info->SSID_len);
	xwifi_dev_convert_bssInfo_to_apInfo(bss_info, &xscanInfo->APInfo);
	return 0;
}

int xwifi_dev_numof_scan_result(xwifi_dev dev)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	return linkedlist_size(self->lst_scanresult);
}

int xwifi_dev_connect(xwifi_dev dev, const char ssid[XWIFI_MAX_SSID_LENGTH], const char bssid[XWIFI_MAC_ADDR_LENGTH], const XWifiConnect* const connectionKey)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	int err;

	wl_bss_info_t* tmp = xwifi_dev_find_by_bssid(self, bssid);
	if(!tmp) {
		return -1;
	}
	ScanInfo_t scanInfo;
	memset(&scanInfo, 0, sizeof(ScanInfo_t));
	err = BWL_ParseBssInfo(tmp, &scanInfo);
	KASSERT(err==BWL_ERR_SUCCESS);
	if(err != BWL_ERR_SUCCESS) {
		ERR("Parse error: Invalid BSS Info");
		return -1;
	}

	switch(connectionKey->encryptionType) {
	case XWIFI_ENCRYPTION_NONE: {
		M_("XWIFI_ENCRYPTION_NONE");
		err = BWL_ConnectNoSec(self->bwlHandle, eNetOpModeInfra, ssid , NULL);
		break;
	}
	case XWIFI_ENCRYPTION_WEP64: {
		M_("XWIFI_ENCRYPTION_WEP64");
		err = BWL_ConnectWep(self->bwlHandle,
				eNetOpModeInfra,
				ssid,
				connectionKey->u.WEP64.key,
				connectionKey->u.WEP64.defaultKeyIndex,
				connectionKey->u.WEP64.authAlg,NULL);
		break;
	}
	case XWIFI_ENCRYPTION_WEP128: {
		M_("XWIFI_ENCRYPTION_WEP128");
		err = BWL_ConnectWep(self->bwlHandle,
				eNetOpModeInfra,
				ssid,
				connectionKey->u.WEP128.key,
				connectionKey->u.WEP128.defaultKeyIndex,
				connectionKey->u.WEP128.authAlg,NULL);
		break;
	}
	case XWIFI_ENCRYPTION_WPA: {
		M_("XWIFI_ENCRYPTION_WPA");
		if(scanInfo.tCredentials.eWSec & eWSecAes) {
			if(connectionKey->u.WPA.isAscii) {
				err = BWL_ConnectWpaAes(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.passphrase.key, NULL);
			}
			else {
				err = BWL_ConnectWpaAes(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.PSK, NULL);
			}
		}
		else if(scanInfo.tCredentials.eWSec & eWSecTkip) {
			if(connectionKey->u.WPA.isAscii) {
				err = BWL_ConnectWpaTkip(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.passphrase.key, NULL);
			}
			else {
				err = BWL_ConnectWpaTkip(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.PSK, NULL);
			}
		}
		break;
	}
	case XWIFI_ENCRYPTION_WPA2: {
		M_("XWIFI_ENCRYPTION_WPA2");
		if(scanInfo.tCredentials.eWSec & eWSecAes) {
			int err;
			if(connectionKey->u.WPA.isAscii) {
				err = BWL_ConnectWpa2Aes(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.passphrase.key, NULL);
			}
			else {
				err = BWL_ConnectWpa2Aes(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.PSK, NULL);
			}
		}
		else if(scanInfo.tCredentials.eWSec & eWSecTkip) {
			int err;
			if(connectionKey->u.WPA.isAscii) {
				err = BWL_ConnectWpa2Tkip(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.passphrase.key, NULL);
			}
			else {
				err = BWL_ConnectWpa2Tkip(self->bwlHandle, eNetOpModeInfra, ssid , connectionKey->u.WPA.u.PSK, NULL);
			}
		}
		break;
	}
	default : {
		M_("Unknown encryption: %d", connectionKey->encryptionType);
		break;
	}
	}
	return err;
}

int xwifi_dev_disconnect(xwifi_dev dev)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	int err = BWL_DisconnectAp(self->bwlHandle);
	KASSERT(err==BWL_ERR_SUCCESS);
	if(err != BWL_ERR_SUCCESS) {
		return -1;
	}
	return 0;
}

int xwifi_dev_get_ap_info(xwifi_dev dev, const char bssid[XWIFI_MAC_ADDR_LENGTH],  XWifiAPInformation* const apInfo)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;

	wl_bss_info_t* bss_info = xwifi_dev_find_by_bssid(dev, bssid);
	if(!bss_info)
		return -1;

	xwifi_dev_convert_bssInfo_to_apInfo(bss_info, apInfo);
	return 0;
}

int xwifi_dev_set_ifconfig(xwifi_dev dev, XWifiInterfaceConfig conf)
{
	KASSERT(dev);
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;
	if(conf.operatingMode == XWIFI_OPERATING_MODE_MANAGED) {
		M_("managed STA");
	}
	else if(conf.operatingMode == XWIFI_OPERATING_MODE_MASTER){
		M_("master AP");
	}
	else {
		W_("Unknown mode: %d", conf.operatingMode);
	}

	char country[4];
	memset(country, 0, sizeof(country));
	BWL_GetCountry(self->bwlHandle, country);
	M_("country: %s", country);
	M_("%s", conf.regulatoryDomain);
	int err = BWL_SetCountry(self->bwlHandle, conf.regulatoryDomain);
	KASSERT(err==BWL_ERR_SUCCESS);
	return err;
}

int xwifi_dev_get_capabilities(xwifi_dev dev, XWifiCapabilities* const cap)
{
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) dev;

	int err;
	Band_t bands;
	err = BWL_GetBand(self->bwlHandle, &bands);
	KASSERT(err==BWL_ERR_SUCCESS);
	if(err!=BWL_ERR_SUCCESS) {
		return -1;
	}

	switch(bands) {
	case eBandAuto:
		KASSERT(1); // ?
		break;

	case eBand5G:
		cap->frequencyBands = XWIFI_FREQUENCY_BAND_5GHZ;
		break;

	case eBand2G:
		cap->frequencyBands = XWIFI_FREQUENCY_BAND_2_4GHZ;
		break;

	case eBandAll:
		cap->frequencyBands = XWIFI_FREQUENCY_BAND_5GHZ  | XWIFI_FREQUENCY_BAND_2_4GHZ;
		break;

	case eBandInvalid:
		KASSERT(1); // ?
		break;
	}

	Credential_t credentials;
	err = BWL_GetCredential(self->bwlHandle, &credentials);
	KASSERT(err==BWL_ERR_SUCCESS);

	if(credentials.eWpaAuth &eWpaAuthNone)
		cap->encryptionTypes = XWIFI_CIPHER_SUITE_NONE;

	if(credentials.eWpaAuth & eWpaAuthWpaUnsp)
		cap->encryptionTypes |= XWIFI_ENCRYPTION_UNSUPPORTED;

	if(credentials.eWpaAuth & eWpaAuthWpaPsk)
		cap->encryptionTypes |= XWIFI_ENCRYPTION_WPA;

	if(credentials.eWpaAuth & eWpaAuthWpa2Unsp)
		cap->encryptionTypes |= XWIFI_ENCRYPTION_UNSUPPORTED;

	if(credentials.eWpaAuth & eWpaAuthWpa2Psk)
		cap->encryptionTypes |= XWIFI_ENCRYPTION_WPA2;

	uint32_t mode;
	err = BWL_Get802_11Modes(self->bwlHandle, &mode);
	KASSERT(err==BWL_ERR_SUCCESS);
	cap->wifiStandards = mode;

//#define XWIFI_FEATURE_ACL																				(0x00000001U)
//
//#define XWIFI_FEATURE_DENIED_ACL															(0x00000002U)
//
//#define XWIFI_FEATURE_UUID_AVAILABLE													(0x00000004U)
//
//#define XWIFI_FEATURE_ESTIMATE_SCAN_TIME											(0x00000008U)
//
//#define XWIFI_FEATURE_ESTIMATE_CONNECT_TIME								(0x00000010U)
	//	capabilities->UUID;
	//	capabilities->estimatedConnectTime;
	//	capabilities->estimatedScanTime;
	//	capabilities->features;
	//	capabilities->numWEP128KeysSupported;
	//	capabilities->numWEP64KeysSupported;
	//	capabilities->operatingModes;
	//	capabilities->wpsConfigMethods;
}

static wl_bss_info_t* xwifi_dev_find_by_bssid(struct xwifi_dev_t* dev, const XWifiMacAddress BSSID)
{
	int count = linkedlist_size(dev->lst_scanresult);
	int i;
	for(i = 0; i < count; i++) {
		wl_bss_info_t* bss_info = linkedlist_get(dev->lst_scanresult, i);
		if(memcmp(bss_info->BSSID.ether_addr_octet, BSSID, XWIFI_MAC_ADDR_LENGTH)==0) {
			return bss_info;
		}
	}
	return NULL;
}

static int xwifi_dev_convert_bssInfo_to_apInfo(wl_bss_info_t* bss_info, XWifiAPInformation* apInfo)
{
	KASSERT(bss_info);
	bss_info->chanspec = dtohchanspec(bss_info->chanspec);
	apInfo->primaryChannel = CHSPEC_CHANNEL(bss_info->chanspec);

	int rate = 0;
	apInfo->rxSignalLevel = bss_info->RSSI;
	apInfo->standards = 0;
	apInfo->standards |= CHSPEC_IS2G(bss_info->chanspec) ? XWIFI_STANDARD_802_11_B : 0;
	apInfo->standards |= (bss_info->n_cap) ? XWIFI_STANDARD_802_11_N : 0;


	//	if (bi->capability & DOT11_CAP_ESS) printf("ESS ");
	//	if (bi->capability & DOT11_CAP_IBSS) printf("IBSS ");
	//	if (bi->capability & DOT11_CAP_POLLABLE) printf("Pollable ");
	//	if (bi->capability & DOT11_CAP_POLL_RQ) printf("PollReq ");
	//	if (bi->capability & DOT11_CAP_PRIVACY) printf("WEP ");
	//	if (bi->capability & DOT11_CAP_SHORT) printf("ShortPre ");
	//	if (bi->capability & DOT11_CAP_PBCC) printf("PBCC ");
	//	if (bi->capability & DOT11_CAP_AGILITY) printf("Agility ");
	//	if (bi->capability & DOT11_CAP_SHORTSLOT) printf("ShortSlot ");
	//	if (bi->capability & DOT11_CAP_CCK_OFDM) printf("CCK-OFDM ");


    int i = 0;
	int j;

	if(bss_info->capability & DOT11_CAP_PRIVACY) {
		Q_;
//		apInfo->encryptions[i].encryptionType = XWIFI_ENCRYPTION_WEP64; // TODO
//		i++;
	}

    if (CHSPEC_IS5G(bss_info->chanspec)) {
    	apInfo->standards |= XWIFI_STANDARD_802_11_A;
    }
    else {
        for (j = 0; j < dtoh32(bss_info->rateset.count); j++) {
            unsigned int r = bss_info->rateset.rates[j] & 0x7f;
            if (r == 0)
                break;

            if (r > rate) {
            	rate = r;
            }
            if (r/2 == 54) {
            	apInfo->standards |= XWIFI_STANDARD_802_11_G;
                break;
            }
        }
    }

    /* Parse credentials */
    if (dtoh32(bss_info->ie_length)) {
		uint8_t     *cp = (uint8 *)(((uint8 *)bss_info) + dtoh16(bss_info->ie_offset));
		uint8_t     *parse = cp;
		uint32_t    parse_len = dtoh32(bss_info->ie_length);
		uint8_t     *wpaie;
		uint8_t     *rsnie;
		WpaInfo_t  wpa_info, rsn_info;

		memset(&wpa_info, 0, sizeof(WpaInfo_t));
		memset(&rsn_info, 0, sizeof(WpaInfo_t));

		while ((wpaie = wlu_parse_tlvs(parse, parse_len, DOT11_MNG_WPA_ID))) {
		 if (wlu_is_wpa_ie(&wpaie, &parse, &parse_len))
			 break;
		}

        /* Read the WPA information */
        if (wpaie) {
            wl_rsn_ie_dump((bcm_tlv_t*)wpaie, &wpa_info);
        }

        rsnie = wlu_parse_tlvs(cp, dtoh32(bss_info->ie_length), DOT11_MNG_RSN_ID);
        if (rsnie) {
            wl_rsn_ie_dump((bcm_tlv_t*)rsnie, &rsn_info);
        }

        if(wpaie) {
        	dev_rsn_ie_dump((bcm_tlv_t*) wpaie, &apInfo->encryptions[i]);
            i++;
        }

        if(rsnie) {
        	dev_rsn_ie_dump((bcm_tlv_t*) rsnie, &apInfo->encryptions[i]);
        	i++;
        }

        M_("WPA cipher: 0x%x", wpa_info.Cipher);
        M_("RSN cipher: 0x%x", rsn_info.Cipher);
#if 0
        pScanInfo->tCredentials.eWpaAuth = eWpaAuthDisabled; /* clear to start with */

        if (rsn_info.Akm == RSN_AKM_PSK)
        {
            pScanInfo->tCredentials.eWpaAuth |= eWpaAuthWpa2Psk;
        }
        if (rsn_info.Akm == RSN_AKM_UNSPECIFIED)
        {
            pScanInfo->tCredentials.eWpaAuth |= eWpaAuthWpa2Unsp;
        }
        if (wpa_info.Akm == RSN_AKM_PSK)
        {
            pScanInfo->tCredentials.eWpaAuth |= eWpaAuthWpaPsk;
        }
        if (wpa_info.Akm == RSN_AKM_UNSPECIFIED)
        {
            pScanInfo->tCredentials.eWpaAuth |= eWpaAuthWpaUnsp;
        }
        if( (rsn_info.Akm == RSN_AKM_NONE) ||
            (wpa_info.Akm == RSN_AKM_NONE) )
        {
            pScanInfo->tCredentials.eWpaAuth |= eWpaAuthNone;
        }
        /* Supported Encryption Method */
        pScanInfo->tCredentials.eWSec |= rsn_info.Cipher;
        pScanInfo->tCredentials.eWSec |= wpa_info.Cipher;
#endif
        /* Search for WPS */
        parse     = cp;
        parse_len = dtoh32(bss_info->ie_length);
        while ((wpaie = wlu_parse_tlvs(parse, parse_len, DOT11_MNG_WPA_ID))) {
        	apInfo->hasWpsInfo = bcm_is_wps_ie(wpaie, &parse, &parse_len);
            if (apInfo->hasWpsInfo) {
            	M_("WPS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
                break;
            }
        }
    }


//    /* WPS is disabled on the AP or registrar. */
//    #define XWIFI_WPS_STATE_DISALBED								(0x0001U)
//
//    /* WPS is not configured on the AP or registrar */
//    #define XWIFI_WPS_STATE_NOT_CONFIGURED				(0x0002U)
//
//    /* WPS is configured on the AP or registrar. */
//    #define XWIFI_WPS_STATE_CONFIGURED							(0x0004U)

//    /* WPS configuration is not supported */
//    #define XWIFI_WPS_CONFIG_NOT_SUPPORTED				(0x0000U)
//
//    /* Label WPS configuration is supported */
//    #define XWIFI_WPS_CONFIG_LABEL										(0x0001U)
//
//    /* Display WPS configuration is supported */
//    #define XWIFI_WPS_CONFIG_DISPLAY									(0x0002U)
//
//    /* Push button WPS configuration is supported */
//    #define XWIFI_WPS_CONFIG_PUSH_BUTTON						(0x0004U)
//
//    /* Keypad WPS configuration is supported */
//    #define XWIFI_WPS_CONFIG_KEYPAD									(0x0008U)
//
    if(apInfo->hasWpsInfo) {
        //TODO
        apInfo->wpsInfo.isRegistrarActive;
        apInfo->wpsInfo.wpsRegistrar.UUID;
        apInfo->wpsInfo.wpsRegistrar.isUUID;
        apInfo->wpsInfo.wpsRegistrar.rfBands;
        apInfo->wpsInfo.wpsRegistrar.wpsConfigMethod;
        apInfo->wpsInfo.wpsRegistrar.wpsDevicePasswordType;
        apInfo->wpsInfo.wpsState;
    }
    apInfo->wpsInfo.isAPLocked = (bss_info->capability & DOT11_CAP_PRIVACY);
    if(apInfo->wpsInfo.isAPLocked) {
//    	encr->encryptionType  = XWIFI_ENCRYPTION_WEP128;
    	 apInfo->encryptions[i].encryptionType = XWIFI_ENCRYPTION_WEP64; // TODO
    }
    else {
    	apInfo->encryptions[i].encryptionType = XWIFI_ENCRYPTION_NONE;
    }
	return 0;
}

#if 0
static int xwifi_dev_convert_scanInfo_to_apInfo(ScanInfo_t* scanInfo, XWifiAPInformation* apInfo)
{
	apInfo->primaryChannel = scanInfo->ulChan;
	apInfo->hasWpsInfo = scanInfo->bWPS;

//	XWIFI_STANDARD_802_11_A, e802_11_a
//	We assume that the values are exact same.
	apInfo->standards = scanInfo->ul802_11Modes;

	apInfo->rxSignalLevel = scanInfo->lRSSI;
	M_("rxSignal: %d", apInfo->rxSignalLevel);

	//TODO:
//	ScanInfo->APInfo.regulatoryDomain
//	scanInfo->ulAuthType;
	apInfo->wpsInfo.isAPLocked = scanInfo->bLocked;

	//    eWpaAuthDisabled    = 0x00,
	//    eWpaAuthNone        = 0x01,
	//    eWpaAuthWpaUnsp     = 0x02,
	//    eWpaAuthWpaPsk      = 0x04,
	//    eWpaAuthWpa2Unsp    = 0x08,
	//    eWpaAuthWpa2Psk     = 0x10,
	//    eWpaAuthInvalid     = 0xff
	int num = 0;
	if(scanInfo->tCredentials.eWpaAuth & eWpaAuthNone) {
		apInfo->encryptions[num].authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
		apInfo->encryptions[num].encryptionType = XWIFI_ENCRYPTION_NONE;
		apInfo->encryptions[num].groupCiphers =  XWIFI_CIPHER_SUITE_NONE;
		apInfo->encryptions[num].pairwiseCiphers =  XWIFI_CIPHER_SUITE_NONE;
	}

	if(scanInfo->tCredentials.eWpaAuth & eWpaAuthWpaUnsp) {
		apInfo->encryptions[num].authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
		apInfo->encryptions[num].encryptionType = XWIFI_ENCRYPTION_WPA;

		if(scanInfo->tCredentials.eWSec & eWSecTkip)  {
			apInfo->encryptions[num].groupCiphers =  XWIFI_CIPHER_SUITE_TKIP;
			apInfo->encryptions[num].pairwiseCiphers = XWIFI_CIPHER_SUITE_TKIP;
		}

		if(scanInfo->tCredentials.eWSec & eWSecAes) {
			apInfo->encryptions[num].groupCiphers |=  XWIFI_CIPHER_SUITE_CCMP;
			apInfo->encryptions[num].pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
		}
		num++;
	}

	if(scanInfo->tCredentials.eWpaAuth & eWpaAuthWpaPsk) {
		apInfo->encryptions[num].authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
		apInfo->encryptions[num].encryptionType = XWIFI_ENCRYPTION_WPA;

		if(scanInfo->tCredentials.eWSec & eWSecTkip)  {
			apInfo->encryptions[num].groupCiphers =  XWIFI_CIPHER_SUITE_TKIP;
			apInfo->encryptions[num].pairwiseCiphers = XWIFI_CIPHER_SUITE_TKIP;
		}

		if(scanInfo->tCredentials.eWSec & eWSecAes) {
			apInfo->encryptions[num].groupCiphers |=  XWIFI_CIPHER_SUITE_CCMP;
			apInfo->encryptions[num].pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
		}
		num++;
	}

	if(scanInfo->tCredentials.eWpaAuth & eWpaAuthWpa2Unsp) {
		apInfo->encryptions[num].authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
		apInfo->encryptions[num].encryptionType = XWIFI_ENCRYPTION_WPA;

		if(scanInfo->tCredentials.eWSec & eWSecTkip)  {
			apInfo->encryptions[num].groupCiphers =  XWIFI_CIPHER_SUITE_TKIP;
			apInfo->encryptions[num].pairwiseCiphers = XWIFI_CIPHER_SUITE_TKIP;
		}

		if(scanInfo->tCredentials.eWSec & eWSecAes) {
			apInfo->encryptions[num].groupCiphers |=  XWIFI_CIPHER_SUITE_CCMP;
			apInfo->encryptions[num].pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
		}
		num++;
	}

	if(scanInfo->tCredentials.eWpaAuth & eWpaAuthWpa2Psk) {
		apInfo->encryptions[num].authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
		apInfo->encryptions[num].encryptionType = XWIFI_ENCRYPTION_WPA2;

		if(scanInfo->tCredentials.eWSec & eWSecTkip)  {
			apInfo->encryptions[num].groupCiphers =  XWIFI_CIPHER_SUITE_TKIP;
			apInfo->encryptions[num].pairwiseCiphers = XWIFI_CIPHER_SUITE_TKIP;
		}

		if(scanInfo->tCredentials.eWSec & eWSecAes) {
			apInfo->encryptions[num].groupCiphers |=  XWIFI_CIPHER_SUITE_CCMP;
			apInfo->encryptions[num].pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
		}
		num++;
	}

	if((scanInfo->tCredentials.eWpaAuth & eWpaAuthInvalid) == eWpaAuthInvalid) {
		Q_;
	}

//	ScanInfo->APInfo.wpsInfo.isRegistrarActive;
//	ScanInfo->APInfo.wpsInfo.wpsRegistrar.UUID;
//	ScanInfo->APInfo.wpsInfo.wpsRegistrar.isUUID;
//	ScanInfo->APInfo.wpsInfo.wpsRegistrar.rfBands;
//	ScanInfo->APInfo.wpsInfo.wpsRegistrar.wpsConfigMethod;
//	ScanInfo->APInfo.wpsInfo.wpsRegistrar.wpsDevicePasswordType;
//	ScanInfo->APInfo.wpsInfo.wpsState;

	M_("auth type: %d", scanInfo->tCredentials.eAuthType);
	M_("wsec %d", scanInfo->tCredentials.eWSec);
	M_("wpaAuth: %d", scanInfo->tCredentials.eWpaAuth);
//    eWSecInvalid    = 0xff,
//    eWSecNone       = 0x01,
//    eWSecWep        = 0x02,
//    eWSecTkip       = 0x04,
//    eWSecAes        = 0x08,
//    eWSecAll        = 0x10,
//	M_("WpaSUP: %d", scanInfo->tCredentials.eWpaSup);
//	M_("key len: %d", scanInfo->tCredentials.ulKeyLen);
}
#endif

#if 0
static int dev_get_scan(void *wl, int opc, char *scan_buf, unsigned int buf_len)
{
	wl_scan_results_t *list = (wl_scan_results_t*)scan_buf;
	int ret;

	list->buflen = htod32(buf_len);
	ret = wlu_get(wl, opc, scan_buf, buf_len);
	if (ret < 0)
		return ret;
	ret = 0;

	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);
	if (list->buflen == 0) {
		list->version = 0;
		list->count = 0;
	} else if (list->version != WL_BSS_INFO_VERSION &&
	           list->version != LEGACY2_WL_BSS_INFO_VERSION &&
	           list->version != LEGACY_WL_BSS_INFO_VERSION) {
		fprintf(stderr, "Sorry, your driver has bss_info_version %d "
			"but this program supports only version %d.\n",
			list->version, WL_BSS_INFO_VERSION);
		list->buflen = 0;
		list->count = 0;
	}

	return ret;
}
#endif

static int32_t dev_get_scan_results(struct xwifi_dev_t* dev, wl_scan_results_t *res, int buflen)
{
	S_;
    int32_t   err = 0;
    const void* wl = BWL_GetWl(dev->bwlHandle);
    KASSERT(wl);

	wl_scan_results_t *lst = (wl_scan_results_t*)res;
	int ret;

	lst->buflen = htod32(buflen);
	ret = wlu_get(wl, WLC_SCAN_RESULTS, res, buflen);
	if (ret < 0)
		return ret;
	ret = 0;

	lst->buflen = dtoh32(lst->buflen);
	lst->version = dtoh32(lst->version);
	lst->count = dtoh32(lst->count);
	if (lst->buflen == 0) {
		lst->version = 0;
		lst->count = 0;
	} else if (lst->version != WL_BSS_INFO_VERSION &&
			lst->version != LEGACY2_WL_BSS_INFO_VERSION &&
			lst->version != LEGACY_WL_BSS_INFO_VERSION) {
		fprintf(stderr, "Sorry, your driver has bss_info_version %d "
			"but this program supports only version %d.\n",
			lst->version, WL_BSS_INFO_VERSION);
		lst->buflen = 0;
		lst->count = 0;
	}

	return ret;

#if 0



    if( !err )
    {
        wl_scan_results_t   *list = (wl_scan_results_t*)dump_buf;
        wl_bss_info_t       *bi;
        uint32_t              i;

        if( list->count == 0 )
        {
            goto BWL_EXIT;
        }
        else if( list->version != WL_BSS_INFO_VERSION &&
                 list->version != LEGACY_WL_BSS_INFO_VERSION )
        {
            fprintf( stderr, "Sorry, your driver has bss_info_version %d "
                             "but this program supports only version %d.\n",
                             list->version, WL_BSS_INFO_VERSION );
            goto BWL_EXIT;
        }

        bi = list->bss_info;
        for( i = 0; i < list->count; i++,
             bi = (wl_bss_info_t*)((int8*)bi + dtoh32( bi->length )) )
        {    int i;
        int rsn;
        wpa_ie_fixed_t *wpa = NULL;
        rsn_parse_info_t rsn_info;
        wpa_suite_t *suite;
        uint8 std_oui[3];
        int unicast_count = 0;
        int akm_count = 0;
        uint16 capabilities;
        uint cntrs;
        int err;

        if (ie->id == DOT11_MNG_RSN_ID) {
            rsn = TRUE;
            memcpy(std_oui, WPA2_OUI, WPA_OUI_LEN);
            err = wl_rsn_ie_parse_info(ie->data, ie->len, &rsn_info);
        } else {
            rsn = FALSE;
            memcpy(std_oui, WPA_OUI, WPA_OUI_LEN);
            wpa = (wpa_ie_fixed_t*)ie;
            err = wl_rsn_ie_parse_info((uint8*)&wpa->version, wpa->length - WPA_IE_OUITYPE_LEN,
                                       &rsn_info);
        }
        if (err || rsn_info.version != WPA_VERSION)
            return;

        if (rsn)
        {
            PRINTF(("RSN:\n"));
        }
        else
        {
            PRINTF(("WPA:\n"));
        }

        /* Check for multicast suite */
        if (rsn_info.mcast) {
            PRINTF(("\tmulticast cipher: "));
            if (!memcmp(rsn_info.mcast->oui, std_oui, 3)) {
                switch (rsn_info.mcast->type) {
                case WPA_CIPHER_NONE:
                    PRINTF(("NONE\n"));
                    info->Cipher |= eWSecNone;
                    break;
                case WPA_CIPHER_WEP_40:
                    PRINTF(("WEP64\n"));
                    info->Cipher |= eWSecWep;
                    break;
                case WPA_CIPHER_WEP_104:
                    PRINTF(("WEP128\n"));
                    info->Cipher |= eWSecWep;
                    break;
                case WPA_CIPHER_TKIP:
                    PRINTF(("TKIP\n"));
                    info->Cipher |= eWSecTkip;
                    break;
                case WPA_CIPHER_AES_OCB:
                    PRINTF(("AES-OCB\n"));
                    info->Cipher |= eWSecAes;
                    break;
                case WPA_CIPHER_AES_CCM:
                    PRINTF(("AES-CCMP\n"));
                    info->Cipher |= eWSecAes;
                    break;
                default:
                    PRINTF(("Unknown-%s(#%d)\n", rsn ? "RSN" : "WPA",
                           rsn_info.mcast->type));
                    break;
                }
            }
            else {
                PRINTF(("Unknown-%02X:%02X:%02X(#%d) ",
                       rsn_info.mcast->oui[0], rsn_info.mcast->oui[1],
                       rsn_info.mcast->oui[2], rsn_info.mcast->type));
            }
        }

        /* Check for unicast suite(s) */
        if (rsn_info.ucast) {
            unicast_count = ltoh16_ua(&rsn_info.ucast->count);
            PRINTF(("\tunicast ciphers(%d): ", unicast_count));
            for (i = 0; i < unicast_count; i++) {
                suite = &rsn_info.ucast->list[i];
                if (!memcmp(suite->oui, std_oui, 3)) {
                    switch (suite->type) {
                    case WPA_CIPHER_NONE:
                        PRINTF(("NONE "));
                        info->Cipher |= eWSecNone;
                        break;
                    case WPA_CIPHER_WEP_40:
                        PRINTF(("WEP64 "));
                        info->Cipher |= eWSecWep;
                        break;
                    case WPA_CIPHER_WEP_104:
                        PRINTF(("WEP128 "));
                        info->Cipher |= eWSecWep;
                        break;
                    case WPA_CIPHER_TKIP:
                        PRINTF(("TKIP "));
                        info->Cipher |= eWSecTkip;
                        break;
                    case WPA_CIPHER_AES_OCB:
                        PRINTF(("AES-OCB "));
                        info->Cipher |= eWSecAes;
                        break;
                    case WPA_CIPHER_AES_CCM:
                        PRINTF(("AES-CCMP "));
                        info->Cipher |= eWSecAes;
                        break;
                    default:
                        PRINTF(("WPA-Unknown-%s(#%d) ", rsn ? "RSN" : "WPA",
                               suite->type));
                        break;
                    }
                }
                else {
                    PRINTF(("Unknown-%02X:%02X:%02X(#%d) ",
                        suite->oui[0], suite->oui[1], suite->oui[2],
                        suite->type));
                }
            }
            PRINTF(("\n"));
        }
        /* Authentication Key Management */
        if (rsn_info.akm) {
            akm_count = ltoh16_ua(&rsn_info.akm->count);
            PRINTF(("\tAKM Suites(%d): ", akm_count));
            for (i = 0; i < akm_count; i++) {
                suite = &rsn_info.akm->list[i];
                if (!wlu_bcmp(suite->oui, std_oui, 3)) {
                    switch (suite->type) {
                    case RSN_AKM_NONE:
                        PRINTF(("None "));
                        info->Akm |= RSN_AKM_NONE;
                        break;
                    case RSN_AKM_UNSPECIFIED:
                        PRINTF(("WPA "));
                        info->Akm |= RSN_AKM_UNSPECIFIED;
                        break;
                    case RSN_AKM_PSK:
                        PRINTF(("WPA-PSK "));
                        info->Akm |= RSN_AKM_PSK;
                        break;
                    default:
                        PRINTF(("Unknown-%s(#%d)  ",
                               rsn ? "RSN" : "WPA", suite->type));
                        break;
                    }
                }
                else {
                    PRINTF(("Unknown-%02X:%02X:%02X(#%d)  ",
                        suite->oui[0], suite->oui[1], suite->oui[2],
                        suite->type));
                }
            }
            PRINTF(("\n"));
        }

        /* Capabilities */
        if (rsn_info.capabilities) {
            capabilities = ltoh16_ua(rsn_info.capabilities);
            PRINTF(("\tCapabilities(0x%04x): ", capabilities));
            if (rsn)
                PRINTF(("%sPre-Auth, ", (capabilities & RSN_CAP_PREAUTH) ? "" : "No "));

            PRINTF(("%sPairwise, ", (capabilities & RSN_CAP_NOPAIRWISE) ? "No " : ""));

            cntrs = wl_rsn_ie_decode_cntrs((capabilities & RSN_CAP_PTK_REPLAY_CNTR_MASK) >>
                                           RSN_CAP_PTK_REPLAY_CNTR_SHIFT);

            PRINTF(("%d PTK Replay Ctr%s", cntrs, (cntrs > 1)?"s":""));

            if (rsn) {
                cntrs = wl_rsn_ie_decode_cntrs(
                    (capabilities & RSN_CAP_GTK_REPLAY_CNTR_MASK) >>
                    RSN_CAP_GTK_REPLAY_CNTR_SHIFT);

                PRINTF(("%d GTK Replay Ctr%s\n", cntrs, (cntrs > 1)?"s":""));
            } else {
                PRINTF(("\n"));
            }
        } else {
            PRINTF(("\tNo %s Capabilities advertised\n", rsn ? "RSN" : "WPA"));
        }
            err = BWL_ParseBssInfo(bi, &pData[i]);
            BWL_CHECK_ERR( err );
        }
    }

BWL_EXIT:
    if( dump_buf )
        free( dump_buf );

    return( err );
#endif
    return 0;
}



static int dev_rsn_ie_dump(bcm_tlv_t* ie, XWifiEncryption* encr)
{
    int i;
    int rsn;
    wpa_ie_fixed_t *wpa = NULL;
    rsn_parse_info_t rsn_info;
    wpa_suite_t *suite;
    uint8 std_oui[3];
    int unicast_count = 0;
    int akm_count = 0;
    uint16 capabilities;
    uint cntrs;
    int err;

    if (ie->id == DOT11_MNG_RSN_ID) {
        rsn = TRUE;
        memcpy(std_oui, WPA2_OUI, WPA_OUI_LEN);
        err = wl_rsn_ie_parse_info(ie->data, ie->len, &rsn_info);
    } else {
        rsn = FALSE;
        memcpy(std_oui, WPA_OUI, WPA_OUI_LEN);
        wpa = (wpa_ie_fixed_t*)ie;
        err = wl_rsn_ie_parse_info((uint8*)&wpa->version, wpa->length - WPA_IE_OUITYPE_LEN, &rsn_info);
    }
    if (err || rsn_info.version != WPA_VERSION)
        return -1;

    if (rsn)
    {
        PRINTF(("RSN:\n"));
        encr->encryptionType = XWIFI_ENCRYPTION_WPA2;
    }
    else
    {
        PRINTF(("WPA:\n"));
        encr->encryptionType = XWIFI_ENCRYPTION_WPA;
    }

    /* Check for multicast suite */
    if (rsn_info.mcast) {
        PRINTF(("\tmulticast cipher: "));
        if (!memcmp(rsn_info.mcast->oui, std_oui, 3)) {
            switch (rsn_info.mcast->type) {
            case WPA_CIPHER_NONE:
                PRINTF(("NONE\n"));
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_NONE;
                break;
            case WPA_CIPHER_WEP_40:
                PRINTF(("WEP64\n"));
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_NONE;
                encr->encryptionType = XWIFI_ENCRYPTION_WEP64;
                break;
            case WPA_CIPHER_WEP_104:
                PRINTF(("WEP128\n"));
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_NONE;
                encr->encryptionType = XWIFI_ENCRYPTION_WEP128;
                break;
            case WPA_CIPHER_TKIP:
                PRINTF(("TKIP\n"));
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_TKIP;
                break;
            case WPA_CIPHER_AES_OCB:
                PRINTF(("AES-OCB\n"));
//                info->Cipher |= eWSecAes;
//                encr->encryptionType = XWIFI_ENCRYPTION_WPA2;
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_CCMP;
                break;
            case WPA_CIPHER_AES_CCM:
                PRINTF(("AES-CCMP\n"));
//                info->Cipher |= eWSecAes;
//                encr->encryptionType = XWIFI_ENCRYPTION_WPA2;
                encr->groupCiphers |= XWIFI_CIPHER_SUITE_CCMP;
                break;
            default:
                PRINTF(("Unknown-%s(#%d)\n", rsn ? "RSN" : "WPA",
                       rsn_info.mcast->type));
                break;
            }
        }
        else {
            PRINTF(("Unknown-%02X:%02X:%02X(#%d) ",
                   rsn_info.mcast->oui[0], rsn_info.mcast->oui[1],
                   rsn_info.mcast->oui[2], rsn_info.mcast->type));
        }
    }

    /* Check for unicast suite(s) */
    if (rsn_info.ucast) {
        unicast_count = ltoh16_ua(&rsn_info.ucast->count);
        PRINTF(("\tunicast ciphers(%d): ", unicast_count));
        for (i = 0; i < unicast_count; i++) {
            suite = &rsn_info.ucast->list[i];
            if (!memcmp(suite->oui, std_oui, 3)) {
                switch (suite->type) {
                case WPA_CIPHER_NONE:
                    PRINTF(("NONE "));
//                    info->Cipher |= eWSecNone;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_NONE;
                    break;
                case WPA_CIPHER_WEP_40:
                    PRINTF(("WEP64 "));
//                    info->Cipher |= eWSecWep;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_NONE;
                    encr->encryptionType = XWIFI_ENCRYPTION_WEP64;
                    break;
                case WPA_CIPHER_WEP_104:
                    PRINTF(("WEP128 "));
//                    info->Cipher |= eWSecWep;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_NONE;
                    encr->encryptionType = XWIFI_ENCRYPTION_WEP128;
                    break;
                case WPA_CIPHER_TKIP:
                    PRINTF(("TKIP "));
//                    info->Cipher |= eWSecTkip;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_TKIP;
                    break;
                case WPA_CIPHER_AES_OCB:
                    PRINTF(("AES-OCB "));
//                    info->Cipher |= eWSecAes;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
                    break;
                case WPA_CIPHER_AES_CCM:
                    PRINTF(("AES-CCMP "));
//                    info->Cipher |= eWSecAes;
                    encr->pairwiseCiphers |= XWIFI_CIPHER_SUITE_CCMP;
                    break;
                default:
                    PRINTF(("WPA-Unknown-%s(#%d) ", rsn ? "RSN" : "WPA",
                           suite->type));
                    break;
                }
            }
            else {
                PRINTF(("Unknown-%02X:%02X:%02X(#%d) ",
                    suite->oui[0], suite->oui[1], suite->oui[2],
                    suite->type));
            }
        }
        PRINTF(("\n"));
    }
    /* Authentication Key Management */
    if (rsn_info.akm) {
        akm_count = ltoh16_ua(&rsn_info.akm->count);
        PRINTF(("\tAKM Suites(%d): ", akm_count));
        for (i = 0; i < akm_count; i++) {
            suite = &rsn_info.akm->list[i];
            if (!memcmp(suite->oui, std_oui, 3)) {
                switch (suite->type) {
                case RSN_AKM_NONE:
                    PRINTF(("None "));
//                    info->Akm |= RSN_AKM_NONE;
                    encr->authenticationSuite = XWIFI_AUTHENTICATION_SUITE_NONE;
                    break;
                case RSN_AKM_UNSPECIFIED:
                    PRINTF(("WPA "));
//                    info->Akm |= RSN_AKM_UNSPECIFIED;
                    // TODO: ?????
                    encr->authenticationSuite |= XWIFI_AUTHENTICATION_SUITE_UNSUPPORTED;
                    break;
                case RSN_AKM_PSK:
                    PRINTF(("WPA-PSK "));
//                    info->Akm |= RSN_AKM_PSK;
                    encr->authenticationSuite |= XWIFI_AUTHENTICATION_SUITE_PSK;
                    break;
                default:
                    PRINTF(("Unknown-%s(#%d)  ",
                           rsn ? "RSN" : "WPA", suite->type));
                    break;
                }
            }
            else {
                PRINTF(("Unknown-%02X:%02X:%02X(#%d)  ",
                    suite->oui[0], suite->oui[1], suite->oui[2],
                    suite->type));
            }
        }
        PRINTF(("\n"));
    }

    /* Capabilities */
    if (rsn_info.capabilities) {
        capabilities = ltoh16_ua(rsn_info.capabilities);
        PRINTF(("\tCapabilities(0x%04x): ", capabilities));
        if (rsn)
            PRINTF(("%sPre-Auth, ", (capabilities & RSN_CAP_PREAUTH) ? "" : "No "));

        PRINTF(("%sPairwise, ", (capabilities & RSN_CAP_NOPAIRWISE) ? "No " : ""));

        cntrs = wl_rsn_ie_decode_cntrs((capabilities & RSN_CAP_PTK_REPLAY_CNTR_MASK) >>
                                       RSN_CAP_PTK_REPLAY_CNTR_SHIFT);

        PRINTF(("%d PTK Replay Ctr%s", cntrs, (cntrs > 1)?"s":""));

        if (rsn) {
            cntrs = wl_rsn_ie_decode_cntrs(
                (capabilities & RSN_CAP_GTK_REPLAY_CNTR_MASK) >>
                RSN_CAP_GTK_REPLAY_CNTR_SHIFT);

            PRINTF(("%d GTK Replay Ctr%s\n", cntrs, (cntrs > 1)?"s":""));
        } else {
            PRINTF(("\n"));
        }
    } else {
        PRINTF(("\tNo %s Capabilities advertised\n", rsn ? "RSN" : "WPA"));
    }
}


static void* xwifi_dev_wait_event(void* arg)
{
	S_;
	struct xwifi_dev_t* self = (struct xwifi_dev_t*) arg;

	int sock = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE_BRCM));
	KASSERT(sock >=0);
	if(sock < 0) {
		ERR("socket error");
		return NULL;
	}

	struct sockaddr_ll  sll;
	memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETHER_TYPE_BRCM);
    sll.sll_ifindex = if_nametoindex(self->ifname);

    if (bind(sock, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
    	ERR("bind error");
    	return NULL;
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    unsigned char buf[1024] = {0};

    for(;;) {
    	int result =  select(sock + 1, &readfds, NULL, NULL, NULL);
        M_("result: %d", result);
        if(result <= 0)
        	continue;

    	memset(buf, 0, sizeof(buf));
    	int nread;
    	if((FD_ISSET(sock, &readfds)) && ((nread = recv(sock, buf, sizeof(buf), 0)) > 0)) {
    		EventMessage_t  eEvent;
    		M_("event dump >>>>>>>>");
    		H_(buf,  nread);
            if (BWL_ParseEvent(self->bwlHandle, buf, nread, &eEvent) != BWL_ERR_SUCCESS) {
                continue;
            }

            if(dev_event_map()[eEvent].fp) {
            	dev_event_map()[eEvent].fp(self);
            }

#if 1 // just for log
            switch(eEvent) {
            case BWL_E_SCAN_COMPLETE: {
                M_("BWL_E_SCAN_COMPLETE");
                break;
            }
            case BWL_E_LINK: {
            	M_("BWL_E_LINK");
            	break;
            }
            case BWL_E_SET_SSID: {
            	 /* indicates status of set SSID */
            	M_("BWL_E_SET_SSID");
            	break;
            }
            case BWL_E_JOIN: {
            	/* differentiates join IBSS from found (WLC_E_START) IBSS */
            	M_("BWL_E_JOIN");
            	break;
            }
            case BWL_E_START: {
            	 /* STA founded an IBSS or AP started a BSS */
            	M_("BWL_E_START");
            	break;
            }
            case BWL_E_AUTH: {
            	/* 802.11 AUTH request */
            	M_("BWL_E_AUTH");
            	break;
            }
            case BWL_E_AUTH_IND: {
            	/* 802.11 AUTH indication */
            	M_("BWL_E_AUTH_IND");
            	break;
            }
            case  BWL_E_DEAUTH: {
            	/* 802.11 DEAUTH request */
            	M_("BWL_E_DEAUTH");
            	break;
            }
            case BWL_E_TXFAIL: {
            	M_("BWL_E_TXFAIL");
            	break;
            }
            case BWL_E_RETROGRADE_TSF: {
            	/* current AP's TSF value went backward */
            	M_("BWL_E_RETROGRADE_TSF");
            	break;
            }
            case BWL_E_DISASSOC: {
            	M_("BWL_E_DISASSOC");
            	break;
            }
            case BWL_E_AUTOAUTH: {
            	M_("BWL_E_AUTOAUTH");
            	break;
            }
            case BWL_E_REASSOC: {
            	M_("BWL_E_REASSOC");
            	break;
            }
            case BWL_E_PRUNE: {
            	M_("BWL_E_PRUNE");
            	break;
            }
            case BWL_E_PSK_SUP: {
            	M_("BWL_E_PSK_SUP");
            	break;
            }
//            BWL_E_DEAUTH_IND,       /* 802.11 DEAUTH indication */
//            BWL_E_ASSOC,            /* 802.11 ASSOC request */
//            BWL_E_ASSOC_IND,        /* 802.11 ASSOC indication */
//            BWL_E_REASSOC,          /* 802.11 REASSOC request */
//            BWL_E_REASSOC_IND,      /* 802.11 REASSOC indication */
//            BWL_E_DISASSOC,         /* 802.11 DISASSOC request */
//            BWL_E_DISASSOC_IND,     /* 802.11 DISASSOC indication */
//            BWL_E_QUIET_START,      /* 802.11h Quiet period started */
//            BWL_E_QUIET_END,        /* 802.11h Quiet period ended */
//            BWL_E_BEACON_RX,        /* BEACONS received/lost indication */
//            BWL_E_LINK,             /* generic link indication */
//            BWL_E_MIC_ERROR,        /* TKIP MIC error occurred */
//            BWL_E_NDIS_LINK,        /* NDIS style link indication */
//            BWL_E_ROAM,             /* roam attempt occurred: indicate status & reason */
//            BWL_E_TXFAIL,           /* change in dot11FailedCount (txfail) */
//            BWL_E_PMKID_CACHE,      /* WPA2 pmkid cache indication */
//            BWL_E_RETROGRADE_TSF,   /* current AP's TSF value went backward */
//            BWL_E_PRUNE,            /* AP was pruned from join list for reason */
//            BWL_E_AUTOAUTH,         /* report AutoAuth table entry match for join attempt */
//            BWL_E_EAPOL_MSG,        /* Event encapsulating an EAPOL message */
//            BWL_E_SCAN_COMPLETE,    /* Scan results are ready or scan was aborted */
//            BWL_E_ADDTS_IND,        /* indicate to host addts fail/success */
//            BWL_E_DELTS_IND,        /* indicate to host delts fail/success */
//            BWL_E_BCNSENT_IND,      /* indicate to host of beacon transmit */
//            BWL_E_BCNRX_MSG,        /* Send the received beacon up to the host */
//            BWL_E_BCNLOST_MSG,      /* indicate to host loss of beacon */
//            BWL_E_ROAM_PREP,        /* before attempting to roam */
//            BWL_E_PFN_NET_FOUND,    /* PFN network found event */
//            BWL_E_PFN_NET_LOST,     /* PFN network lost event */
//            BWL_E_RESET_COMPLETE,
//            BWL_E_JOIN_START,
//            BWL_E_ROAM_START,
//            BWL_E_ASSOC_START,
//            BWL_E_IBSS_ASSOC,
//            BWL_E_RADIO,
//            BWL_E_PSM_WATCHDOG,    /* PSM microcode watchdog fired */
//            BWL_E_PROBREQ_MSG,     /* probe request received */
//            BWL_E_SCAN_CONFIRM_IND,
//            BWL_E_PSK_SUP,         /* WPA Handshake fail */
//            BWL_E_COUNTRY_CODE_CHANGED,
//            BWL_E_EXCEEDED_MEDIUM_TIME, /* WMMAC excedded medium time */
//            BWL_E_ICV_ERROR,       /* WEP ICV error occurred */
//            BWL_E_UNICAST_DECODE_ERROR, /* Unsupported unicast encrypted frame */
//            BWL_E_MULTICAST_DECODE_ERROR,  /* Unsupported multicast encrypted frame */
//            BWL_E_TRACE,
//            BWL_E_IF,               /* I/F change (for dongle host notification) */
//            BWL_E_RSSI,             /* indicate RSSI change based on configured levels */
//            BWL_E_PFN_SCAN_COMPLETE,/* PFN completed scan of network list */
//            BWL_E_EXTLOG_MSG,
//            BWL_E_ACTION_FRAME,
//            BWL_E_PRE_ASSOC_IND,    /* assoc request received */
//            BWL_E_PRE_REASSOC_IND,  /* re-assoc request received */
//            BWL_E_CHANNEL_ADOPTED,  /* channel adopted */
//            BWL_E_AP_STARTED,       /* AP started */
//            BWL_E_DFS_AP_STOP,      /* AP stopped due to DFS */
//            BWL_E_DFS_AP_RESUME,    /* AP resumed due to DFS */
//            BWL_E_LAST,             /* highest val + 1 for range checking */
            default : {
            	M_("Unknown: %d", eEvent);
            	break;
            }

            }
#endif
    	}
    }

    if(sock > 0) {
        close(sock);
    }
    KASSERT(1);
    return NULL;
}



static int wl_rsn_ie_parse_info(uint8* rsn_buf, uint len, rsn_parse_info_t *rsn)
{
	uint16 count;

	memset(rsn, 0, sizeof(rsn_parse_info_t));

	/* version */
	if (len < sizeof(uint16))
		return 1;

	rsn->version = ltoh16_ua(rsn_buf);
	len -= sizeof(uint16);
	rsn_buf += sizeof(uint16);

	/* Multicast Suite */
	if (len < sizeof(wpa_suite_mcast_t))
		return 0;

	rsn->mcast = (wpa_suite_mcast_t*)rsn_buf;
	len -= sizeof(wpa_suite_mcast_t);
	rsn_buf += sizeof(wpa_suite_mcast_t);

	/* Unicast Suite */
	if (len < sizeof(uint16))
		return 0;

	count = ltoh16_ua(rsn_buf);

	if (len < (sizeof(uint16) + count * sizeof(wpa_suite_t)))
		return 1;

	rsn->ucast = (wpa_suite_ucast_t*)rsn_buf;
	len -= (sizeof(uint16) + count * sizeof(wpa_suite_t));
	rsn_buf += (sizeof(uint16) + count * sizeof(wpa_suite_t));

	/* AKM Suite */
	if (len < sizeof(uint16))
		return 0;

	count = ltoh16_ua(rsn_buf);

	if (len < (sizeof(uint16) + count * sizeof(wpa_suite_t)))
		return 1;

	rsn->akm = (wpa_suite_auth_key_mgmt_t*)rsn_buf;
	len -= (sizeof(uint16) + count * sizeof(wpa_suite_t));
	rsn_buf += (sizeof(uint16) + count * sizeof(wpa_suite_t));

	/* Capabilites */
	if (len < sizeof(uint16))
		return 0;

	rsn->capabilities = rsn_buf;

	return 0;
}


static uint wl_rsn_ie_decode_cntrs(uint cntr_field)
{
	uint cntrs;

	switch (cntr_field) {
	case RSN_CAP_1_REPLAY_CNTR:
		cntrs = 1;
		break;
	case RSN_CAP_2_REPLAY_CNTRS:
		cntrs = 2;
		break;
	case RSN_CAP_4_REPLAY_CNTRS:
		cntrs = 4;
		break;
	case RSN_CAP_16_REPLAY_CNTRS:
		cntrs = 16;
		break;
	default:
		cntrs = 0;
		break;
	}

	return cntrs;
}


EVENT_FUNC(BWL_E_SCAN_COMPLETE)
{
	S_;
	KASSERT(self->lst_scanresult);
    linkedlist_clear(self->lst_scanresult, free);

    int count;
	int err = BWL_GetScannedApNum(self->bwlHandle, &count);
	KASSERT(err==BWL_ERR_SUCCESS);

	M_("count: %d", count);
	#define DUMP_BUF_LEN (127 * 1024)
	wl_scan_results_t* scanInfo = NULL;
	scanInfo = (wl_scan_results_t*) malloc(DUMP_BUF_LEN);
	KASSERT(scanInfo);
	memset(scanInfo, 0, DUMP_BUF_LEN);

	err = dev_get_scan_results(self, scanInfo, DUMP_BUF_LEN);
	KASSERT(err==BWL_ERR_SUCCESS);

	wl_bss_info_t* bss_info = scanInfo->bss_info;
	int i;
	for(i = 0; i < count; i++, bss_info= (int8*)bss_info + dtoh32(bss_info->length)) {
		M_("index: %d, count: %d", i, count);
		if(self->band & XWIFI_FREQUENCY_BAND_2_4GHZ) {
			if(CHSPEC_IS2G(bss_info->chanspec)) {
				wl_bss_info_t* tmp = (wl_bss_info_t*) malloc(dtoh32(bss_info->length));
				memcpy(tmp, bss_info, dtoh32(bss_info->length));
				M_("%s", bss_info->SSID);
				linkedlist_add(self->lst_scanresult, tmp);
			}
		}

		if(self->band & XWIFI_FREQUENCY_BAND_5GHZ) {
			if(!CHSPEC_IS2G(bss_info->chanspec)) {
				wl_bss_info_t* tmp = (wl_bss_info_t*) malloc(dtoh32(bss_info->length));
				memcpy(tmp, bss_info, dtoh32(bss_info->length));
				linkedlist_add(self->lst_scanresult, tmp);
			}
		}
	}

	free(scanInfo);
	scanInfo = NULL;
	KASSERT(1);
	XWifiEvent	wifi_event = {0};
	strcpy(wifi_event.ifName, self->ifname);
	wifi_event.event = XWIFI_EVENT_SCAN_COMPLETED;
	xwifi_put_event(self->eventque, &wifi_event);
}

EVENT_FUNC(BWL_E_LINK)
{
	uint32_t linkup;
	int err = BWL_GetLinkStatus(self->bwlHandle, &linkup);
	if(err != BWL_ERR_SUCCESS) {
		return -1;
	}
	unsigned int addr = xwifi_sys_get_ip_address(self->ifname);
	M_("addr: %d", addr);

#if 0
	xwifi_sys_stop_udhcpc(self->ifname);
	xwifi_sys_ifconfig_clear(self->ifname);

	xwifi_sys_ifconfig(self->ifname, false);
#endif
#if 0
	BWL_Down(self->bwlHandle);

	for(uint32_t up=0; up; BWL_IsUp(self->bwlHandle, &up)) {
		BWL_Up(self->bwlHandle);
		usleep(100);
	}
	xwifi_sys_ifconfig(self->ifname, true);
#endif

	if(linkup) {
		err = xwifi_sys_start_udhcpc(self->ifname);
		if(err != 0) {
			fprintf(stderr, "failed to start dhcpc");
			return -1;
		}
		ScanInfo_t scanInfo;
		memset(&scanInfo, 0, sizeof(ScanInfo_t));
		err = BWL_GetConnectedInfo(self->bwlHandle, &scanInfo);
		KASSERT(err==BWL_ERR_SUCCESS);
		XWifiEvent	wifi_event = {0};
		strcpy(wifi_event.ifName, self->ifname);
		wifi_event.event = XWIFI_EVENT_CONNECTION_ESTABLISHED;
		xwifi_put_event(self->eventque, &wifi_event);
	}
	else if(linkup==0) {
		xwifi_sys_stop_udhcpc(self->ifname);

		XWifiEvent	wifi_event = {0};
		strcpy(wifi_event.ifName, self->ifname);
		wifi_event.event = XWIFI_EVENT_CONNECTION_DROPPED;
		xwifi_put_event(self->eventque, &wifi_event);
	}
	else {
		W_("Unknown status");
	}
}


