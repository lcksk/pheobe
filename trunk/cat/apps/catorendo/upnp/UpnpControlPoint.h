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
#include <libgupnp-av/gupnp-av.h>
//#include <libgupnp-dlna/gupnp-dlna-discoverer.h>

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

#if 0
	static void serviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);
	static void serviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer user_data);
#endif
	static void deviceProxyAdded (GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy, gpointer user_data);
	static void deviceProxyRemoved (GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy, gpointer user_data);

	void fireDeviceAdded(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy);
	void fireDeviceRemoved(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy);
#if 0
	void fireServiceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
	void fireServiceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
#endif
	void fireObjectFound(GUPnPDIDLLiteObject* object);

	static void systemUpdateIDChanged(GUPnPServiceProxy *proxy, const char *variable, GValue *value, gpointer data);
	static void subscriptionLost(GUPnPServiceProxy *proxy, const GError *reason, gpointer data);

	void printUpnpDeviceInfo(GUPnPDeviceInfo* device);
	void printServiceInfo(GUPnPServiceInfo* service);
	void printServiceIntrospections(GUPnPServiceIntrospection* intro);
	void printServiceActionInfo(GUPnPServiceActionInfo* info);
	void printBrowse(GUPnPServiceProxy *proxy, const char* id);
	void printContainer(GUPnPDIDLLiteContainer* container);
	void printObject(GUPnPDIDLLiteObject* object);
	void printItem(GUPnPDIDLLiteItem* item);
	void printResource(GUPnPDIDLLiteResource* res);
	void printProtocolInfo(GUPnPProtocolInfo* protocol);
	void printElementName(xmlNode* node);

	static void didlContainerDetected(GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteContainer *container, gpointer data);
	static void didlItemDetected(GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteItem  *item, gpointer data);
	static void didlObjectDetected(GUPnPDIDLLiteParser *parser,  GUPnPDIDLLiteObject *object, gpointer data);

	GUPnPContext *m_context;
	GUPnPControlPoint *m_controlPoint;

    pthread_mutex_t		m_lck;
    pthread_cond_t		m_cond;

    std::list<IUpnpDeviceListener*> m_deviceListener;

    GUPnPDIDLLiteParser* m_didlParser;

    bool m_isInitialized;
};

} /* namespace halkamalka */
#endif /* UPNPCONTROLPOINT_H_ */
