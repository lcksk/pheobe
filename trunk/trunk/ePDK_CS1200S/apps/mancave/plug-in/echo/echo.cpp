/*
 * echo.cpp
 *
 *  Created on: Jun 9, 2013
 *      Author: buttonfly
 */

#include "echo.h"
#include "osldbg.h"

namespace k {

Echo::Echo() {
	// TODO Auto-generated constructor stub

}

Echo::~Echo() {
	// TODO Auto-generated destructor stub
}

int Echo::bind(Json::Rpc::Handler& handler) {
	handler.AddMethod(new Json::Rpc::RpcMethod<Echo>(*this, &Echo::echo, std::string("echo::echo")));
	return 0;
}

int Echo::unbind(Json::Rpc::Handler& handler) {
	handler.DeleteMethod(std::string("echo::echo"));
	return 0;
}

bool Echo::echo(const Json::Value& root, Json::Value& response) {
	S_;
	return true;
}

} /* namespace k */
