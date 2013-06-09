/*
 * main.cpp
 *
 *  Created on: Jun 8, 2013
 *      Author: buttonfly
 */

#include <stdio.h>
#include <string.h>
#include "osldbg.h"
#include "webservice.h"
#include "plugin.h"

#include "echo.h"

int main(void) {

	k::WebService web;

	k::Echo* echoPlugin = new k::Echo;
	web.add(echoPlugin);

	const char *options[] = {
			"listening_ports", "8080",
			"document_root","webapp",
			NULL };

	web.start(options);

	getchar();
	web.stop();
	return 0;
}

