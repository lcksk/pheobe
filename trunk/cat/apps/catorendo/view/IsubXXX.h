/*
 * IsubXXX.h
 *
 *  Created on: Sep 21, 2012
 *      Author: buttonfly
 */

#ifndef ISUBXXX_H_
#define ISUBXXX_H_

#include <clutter/clutter.h>
#include <stdlib.h>

namespace halkamalka {

class IsubXXX {
public:
	virtual void createPartControl(ClutterActor* parent)=0;
	virtual void setVisible(bool visible)=0;
};

} /* namespace halkamalka */
#endif /* ISUBXXX_H_ */
