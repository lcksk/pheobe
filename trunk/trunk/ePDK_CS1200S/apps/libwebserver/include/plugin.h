/*
 * IPlugin.h
 *
 *  Created on: Jun 9, 2013
 *      Author: buttonfly
 */

#ifndef IPLUGIN_H_
#define IPLUGIN_H_

#include <jsonrpc/jsonrpc.h>

namespace k {

class IPlugin {
public:
	virtual int bind(Json::Rpc::Handler& handler)=0;
	virtual int unbind(Json::Rpc::Handler& handler)=0;
};

} /* namespace k */
#endif /* IPLUGIN_H_ */
