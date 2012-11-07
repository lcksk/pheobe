/*
 * xwifi_wpa_parser.c
 *
 *  Created on: Oct 17, 2012
 *      Author: buttonfly
 */

#include <string.h>
#include "os.h"
#include "nds/xwifi.h"
#include "xwifi_dbg.h"
#include "xwifi_wpa_parser.h"
#include "stringtokenizer.h"
#include "wpa_ctrl.h"
#include "str.h"
#include "xwifi_priv.h"

int xwifi_wpa_parser_interfaces_result(linkedlist list, const char* result)
{
	S_;
	const char *ptr = result;

	KASSERT(list);
	KASSERT(result);

	linkedlist_clear(list, free);

	stringtokenizer tokenizer = stringtokenizer_new((const char**) &ptr);
	int ret;
	int i;
	char dst[512];
	char delim;
	for(i=0; (ret = stringtokenizer_next_token2(tokenizer, "\t\n ", 2, dst, &delim)) != -1 ; i++) {
		if(delim == '\t') {
			Q_;
		}
		else if(delim == '\n') {
			M_("<%d> : %s", i, dst);
			char* alias = strdup(dst);
			linkedlist_add(list, alias);
		}
	}
	return 0;
}

int xwifi_wpa_parser_scan_result(linkedlist list, const char* result)
{
	S_;
//	M_("%s", result);
	const char *ptr = result;

	KASSERT(list);
	KASSERT(result);

	linkedlist_clear(list, free);

	// 1. skip head line : bssid / frequency / signal level / flags / ssid
	stringtokenizer tokenizer = stringtokenizer_new((const char**) &ptr);

	int ret;
	int i;
	char dst[512];
	char delim;
	XWifiScanInfo* scan_info = NULL;

	for(i=0; (ret = stringtokenizer_next_token2(tokenizer, "\t\n ", 2, dst, &delim)) != -1 ; i++) {

		if(delim == '\t') {
			KASSERT(scan_info);
//			M_("<%d> : %s", i, dst);

			KASSERT(i <= 5);
			switch(i) {
			case 1:  {// bssid
				sscanf(dst, "%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned int *) &scan_info->AP.BSSID[0],
						(unsigned int *) &scan_info->AP.BSSID[1],
						(unsigned int *) &scan_info->AP.BSSID[2],
						(unsigned int *) &scan_info->AP.BSSID[3],
						(unsigned int *) &scan_info->AP.BSSID[4],
						(unsigned int *) &scan_info->AP.BSSID[5]);
				break;
			}
			case 2: { // frequency (e.g.  2412)
				break;
			}
			case 3:  { // signal level (e.g. -87)
				break;
			}
			case 4: { // flags (e.g. [WEP][ESS])
				break;
			}
			default: {
				break;
			}
			}
		}
		else if(delim == '\n') {
//			M_("<%d> : %s", i, dst);
			if(i == 5) {
				strcpy((char*)scan_info->AP.SSID, (const char*)dst);
			}

			// We create node here.
			if(scan_info) {
				// here to the list.
				linkedlist_add(list, scan_info);
				scan_info = NULL;
			}
			scan_info = (XWifiScanInfo*) malloc(sizeof(*scan_info));
			memset(scan_info, 0, sizeof(*scan_info));
			i = 0;
		}
		else {
			W_("Unknown delim: %c", delim);
		}
		memset(dst, 0, sizeof(dst));
	}

	stringtokenizer_delete(tokenizer);
	return 0;
}




