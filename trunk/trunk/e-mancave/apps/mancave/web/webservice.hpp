/*
 * webservice.hpp
 *
 *  Created on: Jun 3, 2013
 *      Author: buttonfly
 */

#ifndef WEBSERVICE_HPP_
#define WEBSERVICE_HPP_

#include "mongcpp.h"
using namespace mongoose;

#include "websocket.hpp"
#include "websocketlistener.hpp"
#include <list>

#include "osl.h"

#include "plugin.hpp"

using namespace std;

namespace k {

class webservice  :  public virtual websocketlistener, public MongooseServer {
public:
	const static char* id;

	webservice();
	virtual ~webservice();

	void run();
	const char* get_id();

	// @implements websocketlistener
	virtual void ws_data_received(websocket* connection, const std::string& data, const std::string& module);

    int add(plugin* p);
    int remove(plugin* p);

protected:
	// @Override
    virtual bool handleEvent(ServerHandlingEvent eventCode, MongooseConnection& connection, const MongooseRequest& request, MongooseResponse& response);

private:
    void add(websocket* s);
    void remove(websocket* s);
    void process(websocket *s, const std::string& module, const std::string& data);

    list<websocket*> m_websockets;

    osl_mutex m_lck;
};

} /* namespace k */
#endif /* WEBSERVICE_HPP_ */
