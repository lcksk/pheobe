/*
 * websocketlistener.hpp
 *
 *  Created on: Jun 4, 2013
 *      Author: buttonfly
 */

#ifndef WEBSOCKETLISTENER_HPP_
#define WEBSOCKETLISTENER_HPP_

#include "websocket.hpp"
#include <string>

namespace k {

class websocket;

class websocketlistener {
public:
	virtual void ws_data_received(websocket* connection, const std::string& data, const std::string& module)=0;
};

} /* namespace k */
#endif /* WEBSOCKETLISTENER_HPP_ */
