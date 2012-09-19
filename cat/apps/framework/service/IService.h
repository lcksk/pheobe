/*
 * IService.h
 *
 *  Created on: Sep 30, 2011
 *      Author: buttonfly
 */

#ifndef ISERVICE_H_
#define ISERVICE_H_

#include "IRunnable.h"

namespace halkamalka {

class IService : public halkamalka::IRunnable{
public:
	virtual const char* GetID()=0;
};

} /* namespace kn */
#endif /* ISERVICE_H_ */
