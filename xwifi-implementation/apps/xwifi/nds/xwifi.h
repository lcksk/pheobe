/*
 * xwifi.h
 *
 *  Created on: Oct 9, 2012
 *      Author: buttonfly
 */

#ifndef XWIFI_H_
#define XWIFI_H_

#include <stdint.h>
#include "net/if.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define _out_

/****************************************************************************************
 * Encryption Standard
 ****************************************************************************************/

/* Undefined encryption mode */
#define XWIFI_ENCRYPTION_UNDEF							(0x00U)

/* Unencrypted */
#define XWIFI_ENCRYPTION_NONE								(0x01U)

/* Unsupported encryption method */
#define XWIFI_ENCRYPTION_UNSUPPORTED			(0x02U)

/* WEB64 encryption */
#define XWIFI_ENCRYPTION_WEP64							(0x04U)

/* WEB128 encryption */
#define XWIFI_ENCRYPTION_WEB128						(0x08U)

/* WPA encryption */
#define XWIFI_ENCRYPTION_WPA								(0x10U)

/* WPA2 encryption */
#define XWIFI_ENCRYPTION_WPA2								(0x20U)


/****************************************************************************************
 * Cipher Suites
 ****************************************************************************************/

/* No cipher suite */
#define XWIFI_CIPHER_SUITE_NONE							(0x01U)

/* Unsupported cipher suite */
#define XWIFI_CIPHER_SUITE_UNSUPPORTED		(0x02U)

/* TKIP cipher suite */
#define XWIFI_CIPHER_SUITE_TKIP								(0x04U)

/* CCMP cipher suite */
#define XWIFI_CIPHER_SUITE_CCMP							(0x08U)

/****************************************************************************************
 * Authentication Suites
 ****************************************************************************************/

/* No authentication suite */
#define XWIFI_AUTHENTICATION_SUITE_NONE		(0x01U)

/* Unsupported authentication suite */
#define XWIFI_AUTHENTICATION_SUITE_UNSUPPORTED (0x02U)

/* PSK authentication suite */
#define XWIFI_AUTHENTICATION_SUITE_PSK			(0x04U)


/****************************************************************************************
 * Supported Frequency Bands
 ****************************************************************************************/

/* 2.4GHz frequency band */
#define XWIFI_FREQUENCY_BAND_2_4GHZ			(0x01U)

/* 5GHz frequency band */
#define XWIFI_FREQUENCY_BAND_5GHZ					(0x02U)


/****************************************************************************************
 * WEP Authentication Algorithms
 ****************************************************************************************/

/* WEP open authentication  */
#define XWIFI_WEP_AUTH_ALG_OPEN						(0x01U)

/* WEB shared key authentication */
#define XWIFI_WEP_AUTH_ALG_SHARED_KEY		(0x02U)


/* XWifiIfName
 * The name of a wireless network interface. This shall be a null-terminated string.
 * */
typedef char XWifiIfName[IF_NAMESIZE];

/* Max SSID Length
 * The maximum length of the SSID in bytes including NULL character.
 *  */
#define XWIFI_MAX_SSID_LENGTH									(33U)

/* XWifiSSID
 * The name of the AP as a null-terminated string.
 * */
typedef uint8_t XWifiSSID[XWIFI_MAX_SSID_LENGTH];

/* MAC Address Length
 * The length of MAC address in bytes
 * */
#define XWIFI_MAC_ADDR_LENGTH								(6U)


typedef uint8_t XWifiMacAddress[XWIFI_MAC_ADDR_LENGTH];


/* XWifiAP
 * This structure holds information to identify a specific AP
 * */
typedef struct
{
	XWifiMacAddress			BSSID;	/* MAC Address of AP */
	XWifiSSID						SSID;	/* NULL terminated name of the AP */
} XWifiAP;


/****************************************************************************************
 * Wi-Fi Standard Definitions
 ****************************************************************************************/

/* Wi-Fi standard 802.11a network */
#define XWIFI_STANDARD_802_11_A								(0x0001U)

/* Wi-fi standard 802.11b network */
#define XWIFI_STANDARD_802_11_B								(0x0002U)

/* Wi-fi standard 802.11g network */
#define XWIFI_STANDARD_802_11_G								(0x0004U)

/* Wi-fi standard 802.11n network */
#define XWIFI_STANDARD_802_11_N								(0x0008U)


/****************************************************************************************
 * Wi-Fi Operating Mode
 ****************************************************************************************/

/* The wireless network is operating in managed mode (it is a managed STA) */
#define XWIFI_OPERATING_MODE_MANAGED				(0x01U)

/* The wireless network is operating in master mode (it is an AP) */
#define XWIFI_OPERATING_MODE_MASTER						(0x02U)

/****************************************************************************************
 * Maximum Encryptions
 * The number of encryptions supported by a wireless AP.
 ****************************************************************************************/
#define XWIFI_MAX_NUM_ENCRYPTIONS							(3U)


/****************************************************************************************
 * XWifiRegulatoryDomain
 ****************************************************************************************/
typedef uint8_t	XWifiRegulatoryDomain[4];

/* The encryption details*/
typedef struct
{
	uint8_t		encryptionType;
	uint8_t		groupCiphers;
	uint8_t		pairwiseCiphers;
	uint8_t		authenticationSuite;
} XWifiEncryption;


/****************************************************************************************
 * WPS State
 * The Wi-Fi protected (WPS) state of an AP or registrar.
 ****************************************************************************************/

/* WPS is disabled on the AP or registrar. */
#define XWIFI_WPS_STATE_DISALBED								(0x0001U)

/* WPS is not configured on the AP or registrar */
#define XWIFI_WPS_STATE_NOT_CONFIGURED				(0x0002U)

/* WPS is configured on the AP or registrar. */
#define XWIFI_WPS_STATE_CONFIGURED							(0x0004U)


/****************************************************************************************
 * WPS PIN type
 * The WPS PIN type to register with a registrar.
 ****************************************************************************************/

/* Default WPS PIN type */
#define XWIFI_WPS_PIN_TYPE_DEFAULT							(0x0000U)

/* User specified WPS PIN type */
#define XWIFI_WPS_PIN_TYPE_USER_SPECIFIED			(0x0001U)

/* Machine specified WPS PIN type */
#define XWIFI_WPS_PIN_TYPE_MACHINE_SPECIFIED	(0x0002U)

/* Re-key WPS_PIN type */
#define XWIFI_WPS_PIN_TYPE_REKEY								(0x0003U)

/* Push button WPS PIN type */
#define XWIFI_WPS_PIN_TYPE_PUSH_BUTTON				(0x0004U)

/* Registrar-specified WPS PIN type */
#define XWIFI_WPS_PIN_TYPE_REGISTRAR_SPECIFIED	(0x0005U)


/****************************************************************************************
 * WPS Configuration Methods
 * WPS configuration methods supported by a Wi-Fi STA.
 ****************************************************************************************/

/* WPS configuration is not supported */
#define XWIFI_WPS_CONFIG_NOT_SUPPORTED				(0x0000U)

/* Label WPS configuration is supported */
#define XWIFI_WPS_CONFIG_LABEL										(0x0001U)

/* Display WPS configuration is supported */
#define XWIFI_WPS_CONFIG_DISPLAY									(0x0002U)

/* Push button WPS configuration is supported */
#define XWIFI_WPS_CONFIG_PUSH_BUTTON						(0x0004U)

/* Keypad WPS configuration is supported */
#define XWIFI_WPS_CONFIG_KEYPAD									(0x0008U)


/****************************************************************************************
 * UUID Length
 ****************************************************************************************/

/* The UUID length in bytes */
#define XWIFI_UUID_LENGTH													(16U)

/****************************************************************************************
 * XWifiUUID
 ****************************************************************************************/

/* Type for the UUID of an AP or registrar */
typedef uint8_t XWifiUUID[XWIFI_UUID_LENGTH];


/****************************************************************************************
 * XWifiRegistrarInfo
 ****************************************************************************************/

/* This structure holds the registrar information. */
typedef struct
{
	uint16_t		wpsDevicePasswordType;
	uint16_t		wpsConfigMethod;
	uint8_t			isUUID;
	XWifiUUID	UUID;
	uint8_t			rfBands;
} XWifiRegistrarInfo;

/* This structure holds the WPS information of an AP or registrar */
typedef struct
{
	uint16_t						wpsState;
	uint8_t							isAPLocked;
	uint8_t							isRegistrarActive;
	XWifiRegistrarInfo	wpsRegistrar;
} XWifiWPSInfo;


/****************************************************************************************
 * XWifiAPInformation
 ****************************************************************************************/

/* This structure holds information about an AP */
typedef struct
{
	uint32_t						primaryChannel;
	uint16_t						standards;
	uint8_t							operatingMode;
	uint8_t							rxSignalLevel;
	XWifiEncryption		encryptions[XWIFI_MAX_NUM_ENCRYPTIONS];
	uint8_t							hasWpsInfo;
	XWifiWPSInfo			wpsInfo;
	XWifiRegulatoryDomain	regulatoryDomain;
} XWifiAPInformation;



/****************************************************************************************
 * Power Levels
 ****************************************************************************************/

/* The wireless network interface is disabled and powered off */
#define XWIFI_POWER_OFF					(0x00000001U)

/* The wireless network interface is enabled and power on */
#define XWIFI_POWER_ON						(0x00000002U)

/****************************************************************************************
 * xwifi-bss-any
 * : Structure containing MAC address 00:00:00:00:00:00
 ****************************************************************************************/
extern const XWifiMacAddress xwifi_bss_any;


/****************************************************************************************
 * WEB64 Key Length
 * : The WEP64 key length in bytes
 ****************************************************************************************/
#define XWIFI_WEP64_KEY_LENGTH									(5U)

/****************************************************************************************
 * XWifiWEP64Key
 *  : Type used to store 40 bit WEP64 key
 ****************************************************************************************/
typedef uint8_t XWifiWEP64Key[XWIFI_WEP64_KEY_LENGTH];

/****************************************************************************************
 * WEB128 Key Length
 * : The WEP128 key length in bytes
 ****************************************************************************************/
#define XWIFI_WEP104_KEY_LENGTH									(13U)

/****************************************************************************************
 * XWifiWEP128Key
 *  : Type used to store 104 bit WEP128 key
 ****************************************************************************************/
typedef uint8_t XWifiWEP128Key[XWIFI_WEP104_KEY_LENGTH];


/****************************************************************************************
 * WPA Key Length
 * : WPA key length in bytes
 ****************************************************************************************/
#define XWIFI_WPA_KEY_LENGTH												(32U)

/****************************************************************************************
 * XWifiWPAKey
 * : Type used to store a 256 bit WPA key
 ****************************************************************************************/
typedef uint8_t XWifiWPAKey[XWIFI_WPA_KEY_LENGTH];



/****************************************************************************************
 * WPA ASCII Key Length
 * : WPA ASCII key length in bytes, including the null terminator.
 ****************************************************************************************/
#define XWIFI_WPA_ASCII_KEY_LENGTH												(64U)

/****************************************************************************************
 * XWifiWPAAsciiKey
 * : Type used to store an ASCII WPA passphrase of length from 8 to 63 characters, as a null-terminated string.
 ****************************************************************************************/
typedef uint8_t XWifiWPAAsciiKey[XWIFI_WPA_ASCII_KEY_LENGTH];

/* Maximum WEP Keys */
#define XWIFI_MAX_NUM_WEP_KEYS				(4U)


/****************************************************************************************
 * XWifiWPAPassphrase
 * : The structure containing the WPA ASCII passphrase.
 ****************************************************************************************/
typedef struct
{
	uint8_t							length;
	XWifiWPAAsciiKey	key;
} XWifiWPAPassphrase;


/****************************************************************************************
 * XWifiConnect
 * : Structure containing the connection key to use for the connection to the AP.
 ****************************************************************************************/
typedef struct
{
	uint8_t				encryptionType;
	union
	{
		struct
		{
			uint8_t		authAlg;
			uint8_t		defaultKeyIndex;
			XWifiWEP64Key		key[XWIFI_MAX_NUM_WEP_KEYS];
		} WEB64;
		struct
		{
			uint8_t		authAlg;
			uint8_t		defaultKeyIndex;
			XWifiWEP128Key key[XWIFI_MAX_NUM_WEP_KEYS];
		} WEB128;
		struct
		{
			uint8_t		isAscii;
			union
			{
				XWifiWPAKey					PSK;
				XWifiWPAPassphrase passphrase;
			} u;
		} WPA;
	} u;
} XWifiConnect;




/****************************************************************************************
 * Wi-Fi Device Maximum Password Length
 * : The maximum device password length in bytes.
 ****************************************************************************************/
#define XWIFI_WPS_MAX_DEVICE_PWD_LENGTH						(8U)


/****************************************************************************************
 * XWifiDevicePwd
 * : This structure holds the PIN used when associating an AP with a managed STA
 *  using the WPS mechanism.
 ****************************************************************************************/
typedef uint8_t			XWifiDevicePwd[XWIFI_WPS_MAX_DEVICE_PWD_LENGTH];


#define XWIFI_FEATURE_ACL																				(0x00000001U)

#define XWIFI_FEATURE_DENIED_ACL															(0x00000002U)

#define XWIFI_FEATURE_UUID_AVAILABLE													(0x00000004U)

#define XWIFI_FEATURE_ESTIMATE_SCAN_TIME											(0x00000008U)

#define XWIFI_FEATURE_ESTIMATE_CONNECT_TIME								(0x00000010U)


typedef struct
{
	uint16_t				wifiStandards;
	uint8_t					operatingModes;
	uint8_t					encryptionTypes;
	uint8_t					frequencyBands;
	uint16_t				wpsConfigMethods;
	uint32_t				features;
	uint8_t					numWEP64KeysSupported;
	uint8_t					numWEP128KeysSupported;
	XWifiUUID			UUID;
	uint32_t				estimatedScanTime;
	uint32_t				estimatedConnectTime;
} XWifiCapabilities;


typedef struct
{
	uint8_t										operatingMode;
	XWifiRegulatoryDomain	regulatoryDomain;
} XWifiInterfaceConfig;








/**
 * Function prototypes
 */

/**
 * @param : priority
 *
 */
int32_t xwifi_initialise(const int priority);

void xwifi_terminated(void);

int32_t xwifi_get_capabilities(const XWifiIfName ifName, _out_ XWifiCapabilities* const capabilities);

int32_t xwifi_interface_configure(const XWifiIfName ifName, XWifiInterfaceConfig interfaceConfig);





/* API Events */
#define XWIFI_EVENT_SCAN_COMPLETED													(0x00000001U)

#define XWIFI_EVENT_CONNECTION_ESTABLISHED									(0x00000002U)

#define XWIFI_EVENT_CONNECTION_DROPPED											(0x00000004U)

#define XWIFI_EVENT_WPS_REGISTRATION_SUCCESSFUL						(0x00000010U)

#define XWIFI_EVENT_WPS_MULTIPLE_PBC_SESSIONS_DETECTED	(0x00000020U)

#define XWIFI_EVENT_WPS_REGISTRATION_FAILED									(0x00000040U)

#define XWIFI_EVENT_WPS_INVALID_PIN														(0x00000080U)

#define XWIFI_EVENT_AP_STA_ASSOCIATED												(0x00000100U)

#define XWIFI_EVENT_AP_STA_DISASSOCIATED											(0x00000200U)

int xwifi_open_event_queue(_out_ int* const fd);

int xwifi_close_event_queue(const int fd);

int32_t xwifi_subscribe_events(const int fd, const uint32_t events);

typedef struct
{
	uint32_t					event;
	XWifiIfName			ifName;
} XWifiEvent;

int32_t xwifi_get_event(const int fd, _out_ XWifiEvent* const event);



int32_t xwifi_scan_request(const XWifiIfName ifName, uint8_t band);

int32_t xwifi_abort_scan_request(const XWifiIfName ifName);

int32_t xwifi_get_num_scan_result_entries(const XWifiIfName ifName, _out_ uint32_t* const numEntries);


typedef struct
{
	XWifiAP							AP;
	XWifiAPInformation	APInfo;
} XWifiScanInfo;

int32_t xwifi_scan_retrieve_results(const XWifiIfName ifName, uint32_t index, _out_ XWifiScanInfo* ScanInfo);


int32_t xwifi_get_access_point_info(const XWifiIfName ifName, const XWifiMacAddress BSSID, _out_ XWifiAPInformation* const APInfo);

int32_t xwifi_connect(const XWifiIfName ifName, const XWifiAP* const AP, const XWifiConnect* const connectionKey);

int32_t xwifi_disconnect(const XWifiIfName ifName);

int32_t xwifi_power_control(const XWifiIfName ifName, const uint32_t powerState);




typedef struct
{
	XWifiUUID				UUID;
	uint16_t					deviceConfigMethod;
	uint16_t					devicePwdType;
	XWifiDevicePwd	devicePwd;
	uint8_t						devicePwdLength;
} XWifiRegister;

int32_t xwifi_wps_initiate_registration (const XWifiIfName ifName, const XWifiRegister* registerInfo);

int32_t xwifi_wps_cancel_registration(const XWifiIfName ifName);

typedef struct
{
	XWifiAP					AP;
	XWifiConnect			connectionKey;
} XWifiCredentials;

int32_t xwifi_wps_get_credentials (const XWifiIfName ifName, XWifiCredentials* const credentials);

int32_t xwifi_wps_generate_dev_pwd(const XWifiIfName ifName, XWifiDevicePwd* const devicePwd);




typedef struct
{
	uint32_t 					wpsConfigMethods;
	uint16_t					wpsState;
	uint8_t						isAPLocked;
	XWifiUUID				UUID;
	uint8_t						deviceName[32];
	XWifiDevicePwd	apPin;
	uint8_t						apPinLength;
} XWifiAPWPSConfig;

typedef struct
{
	uint16_t							apStandard;
	XWifiSSID						apSSID;
	uint32_t							apChannel;
	uint8_t								apBroadcastSSID;
	XWifiEncryption			apEncryption;
	XWifiConnect					apConnectionKey;
	XWifiAPWPSConfig		apWPSConfig;
} XWifiAPConfig;

int32_t xwifi_ap_configure(const XWifiIfName ifName, const XWifiAPConfig config);

int32_t xwifi_ap_get_num_associated_devices(const XWifiIfName ifName, uint32_t* const numDevices);

int32_t xwifi_ap_get_associated_device_addr(const XWifiIfName ifName, uint32_t index, XWifiMacAddress* addr);


#define XWIFI_AP_ACL_TYPE_BLACK_LIST					(0x01U)
#define XWIFI_AP_ACL_TYPE_WHITE_LIST					(0x02U)

typedef struct
{
	uint16_t							aclListType;
	uint32_t							numEntriesInList;
	XWifiMacAddress*		list;
} XWifiAPACL ;

int32_t	xwifi_ap_configure_access_control_list(const XWifiIfName ifName, const uint8_t enable, XWifiAPACL* const accessControlList);

int32_t xwifi_ap_get_num_denied_sta(const XWifiIfName ifName, uint32_t* numDeniedSTA);

int32_t xwifi_ap_retrive_denied_sta_list(const XWifiIfName ifName, uint32_t numDeniedSTA, XWifiMacAddress* deniedList);

int32_t xwifi_ap_reset_denied_sta_list (const XWifiIfName ifName);

#ifdef __cplusplus
}
#endif


#endif /* XWIFI_H_ */
