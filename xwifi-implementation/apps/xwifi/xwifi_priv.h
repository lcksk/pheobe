/*
 * xwifi_priv.h
 *
 *  Created on: Oct 18, 2012
 *      Author: buttonfly
 */

#ifndef XWIFI_PRIV_H_
#define XWIFI_PRIV_H_


#ifdef __cplusplus
extern "C"
{
#endif

/* private use */
#define XWIFI_EVENT_BSS_ADDED				(0x00000300U)
#define XWIFI_EVENT_BSS_REMOVED		(0x00000300U)

const char* xwifi_get_default_ifname();

#ifdef __cplusplus
}
#endif


#endif /* XWIFI_PRIV_H_ */
