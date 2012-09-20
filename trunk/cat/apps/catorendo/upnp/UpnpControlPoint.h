/*
 * UpnpControlPoint.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef UPNPCONTROLPOINT_H_
#define UPNPCONTROLPOINT_H_

#include <pthread.h>

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-dlna/gupnp-dlna-discoverer.h>

#include <list>
#include "IUpnpDeviceListener.h"

namespace halkamalka {

class UpnpControlPoint  {
public:
	static UpnpControlPoint& getInstance() {
		static UpnpControlPoint instance;
		return instance;
	}

	void init();

	void addDeviceListener(IUpnpDeviceListener* l);
	void removeDeviceListener(IUpnpDeviceListener* l);

private:
	UpnpControlPoint();
	virtual ~UpnpControlPoint();

	static void serviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);
	static void serviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);
	static void deviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);
	static void deviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);

	void fireDeviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
	void fireDeviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);

	GUPnPContext *m_context;
	GUPnPControlPoint *m_controlPoint;
	GUPnPDLNADiscoverer* m_discoverer;

    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    std::list<IUpnpDeviceListener*> m_deviceListener;

    bool m_isInitialized;
};

} /* namespace halkamalka */
#endif /* UPNPCONTROLPOINT_H_ */
