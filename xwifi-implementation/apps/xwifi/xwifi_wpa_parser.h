/*
 * xwifi_wpa_parser.h
 *
 *  Created on: Oct 17, 2012
 *      Author: buttonfly
 */

#ifndef XWIFI_WPA_PARSER_H_
#define XWIFI_WPA_PARSER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "linkedlist.h"

int xwifi_wpa_parser_scan_result(linkedlist list, const char* result);

int xwifi_wpa_parser_interfaces_result(linkedlist list, const char* result);

#ifdef __cplusplus
}
#endif


#endif /* XWIFI_WPA_PARSER_H_ */
