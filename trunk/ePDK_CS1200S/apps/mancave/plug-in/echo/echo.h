/*
 * echo.h
 *
 *  Created on: Jun 9, 2013
 *      Author: buttonfly
 */

#ifndef ECHO_H_
#define ECHO_H_

#include "plugin.h"
#include <jsonrpc/jsonrpc.h>

namespace k {

class Echo : public IPlugin{
public:
	Echo();
	virtual ~Echo();


	int bind(Json::Rpc::Handler& handler);
	int unbind(Json::Rpc::Handler& handler);

	bool echo(const Json::Value& root, Json::Value& response);

};

} /* namespace k */
#endif /* ECHO_H_ */
