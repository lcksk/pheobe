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

#include "UpnpControlPoint.h"
#include "UserInterface.h"

using namespace halkamalka;

int main(int argc, char** argv) {

	Platform& platform  = Platform::getInstance();
	platform.init(&argc, &argv);

	// init. Upnp
	UpnpControlPoint::getInstance().init();

	// init. UI
	UserInterface::getInstance().init();

	platform.start();
	return EXIT_SUCCESS;
}

