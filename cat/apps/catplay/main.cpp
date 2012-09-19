/*
 * main.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */


#include <clutter/clutter.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include "GstPlaybin.h"

#include "Platform.h"
#include "ServiceManager.h"
#include "PlaybackService.h"
#include "UserInterfaceService.h"

using namespace halkamalka;

gboolean busCall (GstBus * bus, GstMessage * msg, gpointer data);

int main(int argc, char** argv) {

	Platform& platform  = Platform::getInstance();
	platform.init(&argc, &argv);

	IService* playbackService = new PlaybackService;
	ServiceManager::GetInstance().Invoke(playbackService);

	IService* uiService = new UserInterfaceService;
	ServiceManager::GetInstance().Invoke(uiService);
	platform.start();
	return EXIT_SUCCESS;
}

