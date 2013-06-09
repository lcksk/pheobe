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
#include <json/json.h>
#include <jsonrpc/jsonrpc.h>
#include "plugin.h"

namespace k {

class WebService {
public:
	WebService();
	virtual ~WebService();

	int start(void);
	int start(const char *options[]);
	int stop(void);

    int add(IPlugin* p);
    int remove(IPlugin* p);

	std::string process(std::string& msg);
	int response(void *conn, std::string& msg, uint8_t key[4]);

private:
	void* m_context;
	Json::Rpc::Handler m_jsonHandler;
};

} /* namespace k */
#endif /* WEBSERVICE_H_ */
