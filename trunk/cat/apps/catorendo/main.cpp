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

#include "Platform.h"

#if 0
#include "ServiceManager.h"
#include "PlaybackService.h"
#include "UserInterfaceService.h"
#include "UpnpControlPointService.h"
#endif
#include "UpnpControlPoint.h"
#include "UserInterface.h"

using namespace halkamalka;

int main(int argc, char** argv) {

	Platform& platform  = Platform::getInstance();
	platform.init(&argc, &argv);

	UpnpControlPoint::getInstance().init();

	UserInterface::getInstance().init();
	platform.start();
	return EXIT_SUCCESS;
}

