/*
 * main.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: buttonfly
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "multicastcapture.h"
#include "platform/Platform.h"
#include "service/IService.h"
#include "service/ServiceManager.h"
#include "service/playback/PlaybackService.h"

using namespace halkamalka;

size_t data_received(void* context, u_int8_t* buf, size_t len);

int main(int argc, char** argv) {

#if 0
	multicastcapture_open_param_t param;
	memset(&param, 0, sizeof(multicastcapture_open_param_t));
	param.ip = (int8_t*)"233.15.200.1";
	param.port = 5000;
	param.cbr.cbr = data_received;
	param.cbr.context = NULL;
	multicastcapture cap = multicastcapture_open(&param);
	multicastcapture_start_async(cap);
	getchar();
	multicastcapture_stop(cap);
	multicastcapture_close(cap);
#else
	Platform& platform = Platform::getInstance();
	platform.init(&argc, &argv);

	IService* playbackService = new PlaybackService;
	ServiceManager::GetInstance().Invoke(playbackService);

	PlaybackService* service = (PlaybackService*) playbackService;
	if(service != NULL) {
		service->setUri("fd://0");
//		service->setUri("rtp://233.15.200.1:5000");
		service->play();
	}

	platform.start();
#endif
	return 0;
}

size_t data_received(void* context, u_int8_t* buf, size_t len) {
	return fwrite(buf, 1, len, stdout);
}


