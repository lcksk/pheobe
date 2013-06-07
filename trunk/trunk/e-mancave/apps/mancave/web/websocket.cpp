/*
 * websocket.cpp
 *
 *  Created on: Jun 4, 2013
 *      Author: buttonfly
 */

#include "websocket.hpp"
#include "osldbg.h"
#include <assert.h>
#include <string>

namespace k {

websocket::websocket(const MongooseConnection& conn, const MongooseRequest &request)
: MongooseConnection(conn), m_module(request.getUri()) {

	S_;
	m_lck = osl_mutex_create();
	m_module.erase(m_module.begin()); // Get rid of the front slash
	mg_reg_callback(m_conn, websocket::data_received, this);
}

websocket::~websocket() {
	if(m_lck) {
		osl_mutex_close(m_lck);
		m_lck = NULL;
	}
}

int websocket::write(const std::string &text) {
	S_;
	return MongooseConnection::write(text);
}

bool websocket::operator==(const websocket& b) const {
	 return (m_conn == b.getInfo());
}

void websocket::data_received(void *user_data, char *buf, int length) {
	S_;
	websocket *pthis = (websocket*)user_data;
	osl_mutex_lock(pthis->m_lck);
    if (pthis != NULL) {
    	for(list<websocketlistener*>::iterator pos = pthis->m_listeners.begin(); pos != pthis->m_listeners.end(); pos++) {
    		websocketlistener* s = dynamic_cast<websocketlistener*>(*pos);
    		assert(s);
    		s->ws_data_received(pthis, std::string(buf, length), pthis->m_module);
    			break;
    	}
    }
    osl_mutex_unlock(pthis->m_lck);
}

void websocket::add(websocketlistener * listener) {
	osl_mutex_lock(m_lck);
	m_listeners.insert(m_listeners.end(), listener);
	osl_mutex_unlock(m_lck);
}

void websocket::remove(websocketlistener * listener) {
	osl_mutex_lock(m_lck);
	m_listeners.remove(listener);
	osl_mutex_unlock(m_lck);
}



} /* namespace k */
