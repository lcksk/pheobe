/*
 * main.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: buttonfly
 */


#include <stdio.h>
#include "webservice.hpp"
#include "osldbg.h"

using namespace k;

int main(int argc, char** argv) {

	webservice* ws = new webservice;
    ws->setOption("document_root", "/usr/local/html_root");
    ws->setOption("listening_ports", "8080");
    ws->setOption("num_threads", "20");
    ws->setOption("max_request_size", "131072");

    ws->start();
    getchar();
	return 0;
}

