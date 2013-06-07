/*
 * dlnaplugin.h
 *
 *  Created on: Jun 4, 2013
 *      Author: buttonfly
 */

#ifndef DLNAPLUGIN_H_
#define DLNAPLUGIN_H_

#include "plugin.hpp"
#include <map>
#include <string>

using namespace std;

namespace k {

class dlnaplugin : public virtual plugin{
public:
	const static char* id;
	dlnaplugin();
	virtual ~dlnaplugin();

	const char* get_id();

private:
};

} /* namespace k */
#endif /* DLNAPLUGIN_H_ */
