/*
 * websocket.h
 *
 *  Created on: Jun 10, 2013
 *      Author: buttonfly
 */

#ifndef WEBSOCKET_H_
#define WEBSOCKET_H_

#include "mongoose.h"
#include <stdio.h>
#include <string.h>
#include <string>

namespace k {

class WebSocket {
public:
	WebSocket(struct mg_connection* conn);
	virtual ~WebSocket();

	std::string read();
	int write(std::string& msg);

private:
	const struct mg_connection* m_conn;
};

} /* namespace k */
#endif /* WEBSOCKET_H_ */
