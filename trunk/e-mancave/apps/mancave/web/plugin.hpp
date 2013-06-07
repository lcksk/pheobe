/*
 * plugin.hpp
 *
 *  Created on: Jun 4, 2013
 *      Author: buttonfly
 */

#ifndef PLUGIN_HPP_
#define PLUGIN_HPP_

namespace k {

class plugin {
public:
	virtual const char* get_id()=0;
};

} /* namespace k */
#endif /* PLUGIN_HPP_ */
