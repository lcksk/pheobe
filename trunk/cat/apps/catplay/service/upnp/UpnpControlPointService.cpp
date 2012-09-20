/*
 * UpnpControlPointService.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#include "UpnpControlPointService.h"

const char* halkamalka::UpnpControlPointService::ID = "UpnpControlPointService";

namespace halkamalka {

UpnpControlPointService::UpnpControlPointService() {
	// TODO Auto-generated constructor stub
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);

	 m_context = gupnp_context_new (NULL, NULL, 0, NULL);
	 m_controlPoint = gupnp_control_point_new(m_context, "ssdp:all");

//	 m_discoverer = gupnp_dlna_discoverer_new(180, TRUE, FALSE);
}

UpnpControlPointService::~UpnpControlPointService() {
	// TODO Auto-generated destructor stub
}


const char* UpnpControlPointService::GetID()
{
	return ID;
}

void UpnpControlPointService::Run()
{
	g_signal_connect (m_controlPoint, "service-proxy-available", G_CALLBACK (UpnpControlPointService::serviceProxyAdded), this);
	g_signal_connect (m_controlPoint, "service-proxy-unavailable", G_CALLBACK (UpnpControlPointService::serviceProxyRemoved), this);
	g_signal_connect (m_controlPoint, "device-proxy-available", G_CALLBACK (UpnpControlPointService::deviceProxyAdded), this);
	g_signal_connect (m_controlPoint, "device-proxy-unavailable", G_CALLBACK (UpnpControlPointService::deviceProxyRemoved), this);
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (m_controlPoint), TRUE);

//	g_signal_connect (m_discoverer, "done", G_CALLBACK (UpnpControlPointService::discoverCompleted), this);


	for(;;) {
		pthread_cond_wait(&m_cond, &m_lck);
	}
}

void UpnpControlPointService::serviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPointService* pThis = static_cast<UpnpControlPointService*>(data);
	const char* type = gupnp_service_info_get_service_type(GUPNP_SERVICE_INFO(proxy));
	const char* location = gupnp_service_info_get_location(GUPNP_SERVICE_INFO(proxy));

	g_print("Service added: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
}

void UpnpControlPointService::serviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPointService* pThis = static_cast<UpnpControlPointService*>(data);
	const char* type = gupnp_service_info_get_service_type(GUPNP_SERVICE_INFO(proxy));
	const char* location = gupnp_service_info_get_location(GUPNP_SERVICE_INFO(proxy));

	g_print("Service removed: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
}

void UpnpControlPointService::deviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPointService* pThis = static_cast<UpnpControlPointService*>(data);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	const char* location = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));

	g_print("Device added: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
	pThis->fireDeviceAdded(cp, proxy);
}

void UpnpControlPointService::deviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	UpnpControlPointService* pThis = static_cast<UpnpControlPointService*>(data);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	const char* location = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));

	g_print("Device removed: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
	pThis->fireDeviceRemoved(cp, proxy);
}

void UpnpControlPointService::addDeviceListener(IUpnpDeviceListener* l)
{
	pthread_mutex_lock(&m_lck);
	m_deviceListener.insert(m_deviceListener.end(), l);
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPointService::removeDeviceListener(IUpnpDeviceListener* l)
{
	pthread_mutex_lock(&m_lck);
	m_deviceListener.remove(l);
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPointService::fireDeviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
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

void UpnpControlPointService::fireDeviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
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
