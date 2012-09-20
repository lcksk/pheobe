/*
 * UpnpControlPoint.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#include "UpnpControlPoint.h"

namespace halkamalka {

UpnpControlPoint::UpnpControlPoint() : m_isInitialized(false) {

//	 m_discoverer = gupnp_dlna_discoverer_new(180, TRUE, FALSE);
}

UpnpControlPoint::~UpnpControlPoint() {
	// TODO Auto-generated destructor stub
}

void UpnpControlPoint::init() {
	if(m_isInitialized)
		return;

	m_isInitialized = true;

	pthread_attr_t attri;
	pthread_attr_init(&attri);
	pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

	pthread_mutex_init(&m_lck, NULL);
	pthread_cond_init(&m_cond, NULL);

	pthread_attr_destroy(&attri);

	m_context = gupnp_context_new (NULL, NULL, 0, NULL);
	m_controlPoint = gupnp_control_point_new(m_context, "ssdp:all");

	g_signal_connect (m_controlPoint, "service-proxy-available", G_CALLBACK (UpnpControlPoint::serviceProxyAdded), this);
	g_signal_connect (m_controlPoint, "service-proxy-unavailable", G_CALLBACK (UpnpControlPoint::serviceProxyRemoved), this);
	g_signal_connect (m_controlPoint, "device-proxy-available", G_CALLBACK (UpnpControlPoint::deviceProxyAdded), this);
	g_signal_connect (m_controlPoint, "device-proxy-unavailable", G_CALLBACK (UpnpControlPoint::deviceProxyRemoved), this);
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (m_controlPoint), TRUE);
}

void UpnpControlPoint::serviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_service_info_get_service_type(GUPNP_SERVICE_INFO(proxy));
	const char* location = gupnp_service_info_get_location(GUPNP_SERVICE_INFO(proxy));

	g_print("Service added: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
}

void UpnpControlPoint::serviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_service_info_get_service_type(GUPNP_SERVICE_INFO(proxy));
	const char* location = gupnp_service_info_get_location(GUPNP_SERVICE_INFO(proxy));

	g_print("Service removed: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
}

void UpnpControlPoint::deviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	const char* location = gupnp_device_info_get_location(GUPNP_DEVICE_INFO(proxy));

	g_print("Device added: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
	pThis->fireDeviceAdded(cp, proxy);
}

void UpnpControlPoint::deviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	const char* location = gupnp_device_info_get_location(GUPNP_DEVICE_INFO(proxy));

	g_print("Device removed: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
	pThis->fireDeviceRemoved(cp, proxy);
}

void UpnpControlPoint::addDeviceListener(IUpnpDeviceListener* l)
{
	pthread_mutex_lock(&m_lck);
	m_deviceListener.insert(m_deviceListener.end(), l);
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPoint::removeDeviceListener(IUpnpDeviceListener* l)
{
	pthread_mutex_lock(&m_lck);
	m_deviceListener.remove(l);
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPoint::fireDeviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpDeviceListener*>::iterator pos = m_deviceListener.begin(); pos != m_deviceListener.end(); pos++) {
		IUpnpDeviceListener* listener =dynamic_cast<IUpnpDeviceListener*> (*pos);
		if(listener) {
			listener->deviceAdded(cp, proxy);
		}
	}
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPoint::fireDeviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpDeviceListener*>::iterator pos = m_deviceListener.begin(); pos != m_deviceListener.end(); pos++) {
		IUpnpDeviceListener* listener = dynamic_cast<IUpnpDeviceListener*> (*pos);
		if(listener) {
			listener->deviceRemoved(cp, proxy);
		}
	}
	pthread_mutex_unlock(&m_lck);
}




} /* namespace halkamalka */
