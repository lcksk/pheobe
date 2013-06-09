/*
 * webservice.h
 *
 *  Created on: Jun 8, 2013
 *      Author: buttonfly
 */

#ifndef WEBSERVICE_H_
#define WEBSERVICE_H_

#include <string>
#include <map>
#include <jsonrpc/jsonrpc.h>
#include "plugin.h"

namespace k {

class WebService {
public:
	WebService();
	virtual ~WebService();

	int start(void);
	int stop(void);

    int add(IPlugin* p);
    int remove(IPlugin* p);

	virtual std::string process(std::string& data);

private:
	void* m_context;
	Json::Rpc::Handler m_jsonHandler;
};

} /* namespace k */
#endif /* WEBSERVICE_H_ */
