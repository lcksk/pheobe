/*
 * UpnpControlPointService.h
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#ifndef UPNPCONTROLPOINTSERVICE_H_
#define UPNPCONTROLPOINTSERVICE_H_

#include "IService.h"
#include <pthread.h>

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-dlna/gupnp-dlna-discoverer.h>

#include <list>
#include "IUpnpDeviceListener.h"

namespace halkamalka {

class UpnpControlPointService  : public halkamalka::IService  {
public:
	const static char* ID;

	UpnpControlPointService();
	virtual ~UpnpControlPointService();

	void Run();
	const char* GetID();

	void addDeviceListener(IUpnpDeviceListener* l);
	void removeDeviceListener(IUpnpDeviceListener* l);

private:
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
};

} /* namespace halkamalka */
#endif /* UPNPCONTROLPOINTSERVICE_H_ */
