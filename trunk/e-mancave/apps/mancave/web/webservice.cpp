/*
 * webservice.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: buttonfly
 */

#include "webservice.hpp"
#include "websocket.hpp"
#include <stdio.h>
#include "osldbg.h"
#include <assert.h>

#include "json.h"

const char* k::webservice::id = "webservice";

namespace k {

webservice::webservice() :
		MongooseServer() {
	m_lck = osl_mutex_create();
}

webservice::~webservice() {
	if(m_lck) {
		osl_mutex_close(m_lck);
		m_lck = NULL;
	}
}

void webservice::run() {
	start();
}

const char* webservice::get_id() {
	return id;
}

bool webservice::handleEvent(ServerHandlingEvent eventCode,
		MongooseConnection& connection, const MongooseRequest& request,
		MongooseResponse& response) {

	S_;
	bool result = false;

	switch (eventCode) {
	case MG_NEW_REQUEST: {
		M_("MG_NEW_REQUEST");
		if ((request.getRequestMethod() == "POST")
				&& (!request.getXHRData().empty())
				&& (!request.getUri().empty())) {
			std::string module = request.getUri();
			module.erase(module.begin()); // Get rid of the front slash
			websocket* s = dynamic_cast<websocket*>(&connection);
			assert(s);
			process(s, module, request.getXHRData());
			result = true;
		}
		break;
	}

	case MG_WS_CONNECTED: {
		M_("MG_WS_CONNECTED");
		websocket* s = new websocket(connection, request);
		add(s);
		result = true;
		break;
	}

	case MG_WS_DISCONNECTED: {
		M_("MG_WS_DISCONNECTED");
		websocket* s = new websocket(connection, request);
		remove(s);
		result = true; /* Tell mongoose that the request is valid and we are processing it */
		break;
	}

	}
	return result;
}


void webservice::add(websocket* s) {
	osl_mutex_lock(m_lck);
	s->add(this);
	m_websockets.insert(m_websockets.end(), s);
	osl_mutex_unlock(m_lck);
}

void webservice::remove(websocket* s) {
	osl_mutex_lock(m_lck);
	for(list<websocket*>::iterator pos = m_websockets.begin(); pos != m_websockets.end(); pos++) {
		websocket* tmp = dynamic_cast<websocket*>(*pos);
		if(s && (*tmp == *s)) {
			m_websockets.remove(s);
			delete s;
			break;
		}
	}
	osl_mutex_unlock(m_lck);
}

void webservice::ws_data_received(websocket* s, const std::string& data, const std::string& module) {
	S_;
	process(s, module, data);
}

void webservice::process(websocket *s, const std::string& module, const std::string& data) {
	S_;
	M_("module: %s, data: %s", module.c_str(), data.c_str());
	Json::Value root;
	Json::Reader reader;
	bool error = reader.parse(data, root );
	if(!error) {
		 const Json::Value plugins = root["plug-ins"];
	}
}

int webservice::add(plugin* p) {
	return 0;
}

int webservice::remove(plugin* p) {
	return 0;
}

} /* namespace k */
