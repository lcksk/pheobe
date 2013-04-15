/*
 * multicastcapture.h
 *
 *  Created on: Apr 15, 2013
 *      Author: buttonfly
 */

#ifndef MULTICASTCAPTURE_H_
#define MULTICASTCAPTURE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>

typedef void* multicastcapture;

typedef size_t (*multicastcapture_data_received) (void* context, u_int8_t* buf, size_t len);

typedef struct {
	multicastcapture_data_received cbr;
	void* context;
} cbr_t;

typedef struct {
	cbr_t cbr;
	int8_t* ip;
	u_int16_t port;
} multicastcapture_open_param_t;

multicastcapture multicastcapture_open(multicastcapture_open_param_t* param);

int multicastcapture_start(multicastcapture h);

int multicastcapture_start_async(multicastcapture h);

int multicastcapture_stop(multicastcapture h);

int multicastcapture_close(multicastcapture h);

#ifdef __cplusplus
}
#endif


#endif /* MULTICASTCAPTURE_H_ */
