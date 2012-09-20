/*
 * IUpnpDeviceListener.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef IUPNPDEVICELISTENER_H_
#define IUPNPDEVICELISTENER_H_

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-dlna/gupnp-dlna-discoverer.h>

namespace halkamalka {

class IUpnpDeviceListener {
public:
	virtual void deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)=0;
	virtual void deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)=0;
};

} /* namespace halkamalka */
#endif /* IUPNPDEVICELISTENER_H_ */
