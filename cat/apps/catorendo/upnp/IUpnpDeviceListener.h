/*
 * IUpnpDeviceListener.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef IUPNPDEVICELISTENER_H_
#define IUPNPDEVICELISTENER_H_

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-av/gupnp-av.h>
//#include <libgupnp-dlna/gupnp-dlna-discoverer.h>

namespace halkamalka {

class IUpnpDeviceListener {
public:
	virtual void deviceAdded(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)=0;
	virtual void deviceRemoved(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)=0;

#if 0
	virtual void serviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)=0;
	virtual void serviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)=0;
#endif
	virtual void didlObjectFound(GUPnPDIDLLiteObject* object)=0;
};

} /* namespace halkamalka */
#endif /* IUPNPDEVICELISTENER_H_ */
