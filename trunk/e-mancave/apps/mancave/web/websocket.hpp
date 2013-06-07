/*
 * websocket.hpp
 *
 *  Created on: Jun 4, 2013
 *      Author: buttonfly
 */

#ifndef WEBSOCKET_HPP_
#define WEBSOCKET_HPP_

#include "mongcpp.h"
#include <string>
#include <list>

#include "websocket.hpp"
#include "websocketlistener.hpp"
#include "osl.h"

using namespace mongoose;
using namespace std;

namespace k {

class websocket : public MongooseConnection{
public:
	websocket(const MongooseConnection& conn, const MongooseRequest &request);
	virtual ~websocket();

    virtual void add(websocketlistener * listener);
    virtual void remove(websocketlistener * listener);

    virtual int write(const std::string &text);
    bool operator==(const websocket& b) const;

private:
    static void data_received(void *user_data, char *buf, int length);
    string                     m_module;
    list<websocketlistener*> m_listeners;

    osl_mutex m_lck;
};

} /* namespace k */
#endif /* WEBSOCKET_HPP_ */
