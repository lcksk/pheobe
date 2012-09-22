/*
 * UpnpControlPoint.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#include "UpnpControlPoint.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "dbg.h"

namespace halkamalka {

UpnpControlPoint::UpnpControlPoint() : m_isInitialized(false), m_didlParser(NULL), m_context(NULL), m_controlPoint(NULL) {
}

UpnpControlPoint::~UpnpControlPoint() {
	if(m_context)
		g_object_unref (m_context);

	if(m_controlPoint)
		g_object_unref (m_controlPoint);

	if(m_didlParser)
		g_object_unref (m_didlParser);
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
//	m_controlPoint = gupnp_control_point_new(m_context, "urn:schemas-upnp-org:service:ContentDirectory:1");


//	g_signal_connect (m_controlPoint, "service-proxy-available", G_CALLBACK (UpnpControlPoint::serviceProxyAdded), this);
//	g_signal_connect (m_controlPoint, "service-proxy-unavailable", G_CALLBACK (UpnpControlPoint::serviceProxyRemoved), this);
	g_signal_connect (m_controlPoint, "device-proxy-available", G_CALLBACK (UpnpControlPoint::deviceProxyAdded), this);
	g_signal_connect (m_controlPoint, "device-proxy-unavailable", G_CALLBACK (UpnpControlPoint::deviceProxyRemoved), this);
	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (m_controlPoint), TRUE);

	m_didlParser = gupnp_didl_lite_parser_new();
	g_signal_connect (m_didlParser, "container-available", G_CALLBACK (UpnpControlPoint::didlContainerDetected), this);
	g_signal_connect (m_didlParser, "item-available", G_CALLBACK (UpnpControlPoint::didlItemDetected), this);
	g_signal_connect (m_didlParser, "object-available", G_CALLBACK (UpnpControlPoint::didlObjectDetected), this);
}

void UpnpControlPoint::systemUpdateIDChanged(GUPnPServiceProxy *proxy, const char *variable, GValue *value, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
    g_print ("Received a notification for variable '%s':\n", variable);
    g_print ("\tvalue:     %d\n", g_value_get_uint (value));
}

void UpnpControlPoint::subscriptionLost(GUPnPServiceProxy *proxy, const GError *reason, gpointer data)
{
	M_ ("Lost subscription: %s\n", reason->message);
}

#if 0
// important!
void UpnpControlPoint::serviceProxyAdded (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);

	pThis->printServiceInfo(GUPNP_SERVICE_INFO(proxy));

	gupnp_service_proxy_add_notify(proxy, "SystemUpdateID", G_TYPE_UINT, UpnpControlPoint::systemUpdateIDChanged, pThis);

    g_signal_connect (proxy, "subscription-lost", G_CALLBACK (UpnpControlPoint::subscriptionLost), pThis);
    gupnp_service_proxy_set_subscribed (proxy, TRUE);

    pThis->fireServiceAdded(cp, proxy);
    pThis->printBrowse(proxy, "0");
}

void UpnpControlPoint::serviceProxyRemoved (GUPnPControlPoint *cp, GUPnPServiceProxy *proxy, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_service_info_get_service_type(GUPNP_SERVICE_INFO(proxy));
	const char* location = gupnp_service_info_get_location(GUPNP_SERVICE_INFO(proxy));
	pThis->fireServiceRemoved(cp, proxy);
	g_print("Service removed: \n");
	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
}
#endif

void UpnpControlPoint::deviceProxyAdded (GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);

	pThis->printUpnpDeviceInfo(GUPNP_DEVICE_INFO(proxy));

	GUPnPServiceInfo*  service = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(proxy), "urn:schemas-upnp-org:service:ContentDirectory:1");

	if(service) {
		g_print("service is not null!!!!!!!!!!!!!!!!!!\n");
		pThis->fireDeviceAdded(cp, proxy);
		g_object_unref(service);
	}
}

void UpnpControlPoint::deviceProxyRemoved (GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	const char* location = gupnp_device_info_get_location(GUPNP_DEVICE_INFO(proxy));

	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);

	pThis->fireDeviceRemoved(cp, proxy);
}

void UpnpControlPoint::didlContainerDetected(GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteContainer *container, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	pThis->printContainer(container);
}

void UpnpControlPoint::didlItemDetected(GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteItem  *item, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	pThis->printItem(item);
}

void UpnpControlPoint::didlObjectDetected(GUPnPDIDLLiteParser *parser,  GUPnPDIDLLiteObject *object, gpointer data)
{
	S_;
	UpnpControlPoint* pThis = static_cast<UpnpControlPoint*>(data);
	pThis->fireObjectFound(object);
	pThis->printObject(object);
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


void UpnpControlPoint::fireDeviceAdded(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)
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

void UpnpControlPoint::fireDeviceRemoved(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpDeviceListener*>::iterator pos = m_deviceListener.begin(); pos != m_deviceListener.end(); pos++) {
		IUpnpDeviceListener* listener =dynamic_cast<IUpnpDeviceListener*> (*pos);
		if(listener) {
			listener->deviceRemoved(cp, proxy);
		}
	}
	pthread_mutex_unlock(&m_lck);
}

#if 0
void UpnpControlPoint::fireServiceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpServiceListener*>::iterator pos = m_serviceListener.begin(); pos != m_serviceListener.end(); pos++) {
		IUpnpServiceListener* listener =dynamic_cast<IUpnpServiceListener*> (*pos);
		if(listener) {
			listener->serviceAdded(cp, proxy);
		}
	}
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPoint::fireServiceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpServiceListener*>::iterator pos = m_serviceListener.begin(); pos != m_serviceListener.end(); pos++) {
		IUpnpServiceListener* listener = dynamic_cast<IUpnpServiceListener*> (*pos);
		if(listener) {
			listener->serviceRemoved(cp, proxy);
		}
	}
	pthread_mutex_unlock(&m_lck);
}
#endif

void UpnpControlPoint::fireObjectFound(GUPnPDIDLLiteObject* object)
{
	pthread_mutex_lock(&m_lck);
	for(std::list<IUpnpDeviceListener*>::iterator pos = m_deviceListener.begin(); pos != m_deviceListener.end(); pos++) {
		IUpnpDeviceListener* listener = dynamic_cast<IUpnpDeviceListener*> (*pos);
		if(listener) {
			listener->didlObjectFound(object);
		}
	}
	pthread_mutex_unlock(&m_lck);
}

void UpnpControlPoint::printUpnpDeviceInfo(GUPnPDeviceInfo* device)
{
	const char* type = gupnp_device_info_get_device_type(device);
	const char* location = gupnp_device_info_get_location(device);
	const char* friendlyName = gupnp_device_info_get_friendly_name(device);
	const char* udn = gupnp_device_info_get_udn(device);
	const char* manufacturer = gupnp_device_info_get_manufacturer(device);
	char* iconUrl = gupnp_device_info_get_icon_url (device, NULL, -1, -1, -1, TRUE,  NULL, NULL, NULL, NULL);

	g_print("\ttype: %s\n", type);
	g_print("\tlocation: %s\n", location);
	g_print("\tfriendly-name: %s\n", friendlyName);
	g_print("\tudn: %s\n", udn);
	g_print("\tmanufacturer: %s\n", manufacturer);
	g_print("\ticon url: %s\n", iconUrl);


	GList* iter = gupnp_device_info_list_services(device);
	for (; iter; iter = iter->next) {
		GUPnPServiceInfo*  service = (GUPnPServiceInfo*)iter->data;
		printServiceInfo(service);

		GError* error = NULL;
		GUPnPServiceIntrospection * serviceIntrospection = gupnp_service_info_get_introspection(service, &error);
		if(error != NULL) {
			g_warning("E: %s\n", error->message);
			g_error_free(error);
			error = NULL;
		}

		if(serviceIntrospection) {
			printServiceIntrospections(serviceIntrospection);
		}
	}
}

void UpnpControlPoint::printServiceInfo(GUPnPServiceInfo* service)
{
	const char* location = gupnp_service_info_get_location(service);
	const char* type = gupnp_service_info_get_service_type(service);
	const char* scpdUrl = gupnp_service_info_get_scpd_url(service);
	const char* ctrlUrl = gupnp_service_info_get_control_url(service);
	char* eventSubscriptionUrl = gupnp_service_info_get_event_subscription_url(service);

	g_print("\n\tservice: \n");
	g_print("\t\tlocation: %s\n", location);
	g_print("\t\ttype: %s\n", type);
	g_print("\t\tscpd url: %s\n", scpdUrl);
	g_print("\t\tcontrol url: %s\n", ctrlUrl);
	g_print("\t\tevent subscription url: %s\n", eventSubscriptionUrl);
}

void UpnpControlPoint::printServiceIntrospections(GUPnPServiceIntrospection* intro)
{
	g_assert(intro);
	const GList* iter = gupnp_service_introspection_list_actions(intro);
	for(; iter; iter = iter->next) {
		GUPnPServiceActionInfo* action = (GUPnPServiceActionInfo*) iter->data;
		printServiceActionInfo(action);
	}
}

void UpnpControlPoint::printServiceActionInfo(GUPnPServiceActionInfo* info)
{
	g_print("\t\t\taction: %s(", info->name);
	GList* iter = info->arguments;
	for(;iter; iter=iter->next) {
		GUPnPServiceActionArgInfo* arg = (GUPnPServiceActionArgInfo*) iter->data;
		g_print("%s,", arg->name);
	}
	g_print(")\n");
}

void UpnpControlPoint::printBrowse(GUPnPServiceProxy *proxy, const char* id)
{
	GError *error = NULL;
	char *result = NULL;
	guint count, total;
	gupnp_service_proxy_send_action (proxy,
										 "Browse",
										 &error,
										 /* IN args */
										 "ObjectID",
												G_TYPE_STRING,
												id,
										 "BrowseFlag",
												G_TYPE_STRING,
												"BrowseDirectChildren",
										 "Filter",
												G_TYPE_STRING,
												"*",
										 "StartingIndex",
												G_TYPE_UINT,
												0,
										 "RequestedCount",
												G_TYPE_UINT,
												0,
										 "SortCriteria",
												G_TYPE_STRING,
												"",
										 NULL,
										 /* OUT args */
										 "Result",
												G_TYPE_STRING,
												&result,
										 "NumberReturned",
												G_TYPE_UINT,
												&count,
										 "TotalMatches",
												G_TYPE_UINT,
												&total,
										 NULL);

    g_print ("Browse returned:\n");
    g_print ("\tResult:         %s\n", result);
    g_print ("\tNumberReturned: %u\n", count);
    g_print ("\tTotalMatches:   %u\n", total);

    if(error) {
    	g_warning(error->message);
    	g_error_free(error);
    	error = NULL;
    }

    error = NULL;
    if(result != NULL) {
    	gupnp_didl_lite_parser_parse_didl(m_didlParser, result, &error);
		if(error) {
			g_warning(error->message);
			g_error_free(error);
		}
    	g_free (result);
    }
}

void UpnpControlPoint::printContainer(GUPnPDIDLLiteContainer* container)
{
	gboolean searchable = gupnp_didl_lite_container_get_searchable(container);
	g_return_if_fail(!searchable);

	gint count = gupnp_didl_lite_container_get_child_count(container);
	g_print("count: %d\n", count);

	GList* iter = gupnp_didl_lite_container_get_create_classes(container);
	for(; iter; iter=iter->next) {
		gchar* qlass = (gchar*)iter->data;
		g_print("class : %s\n", qlass);
		if(qlass)
			g_free(qlass);
	}
	g_list_free(iter);
}

void UpnpControlPoint::printObject(GUPnPDIDLLiteObject* object)
{
	const char* id = gupnp_didl_lite_object_get_id(object);
	const char* parentId = gupnp_didl_lite_object_get_parent_id(object);
	const char* title = gupnp_didl_lite_object_get_title(object);
	const char* desc = gupnp_didl_lite_object_get_description(object);
	const char* qlass = gupnp_didl_lite_object_get_upnp_class(object);
	const char* album = gupnp_didl_lite_object_get_album(object);
	const char* albumArt = gupnp_didl_lite_object_get_album_art(object);
	const char* genre = gupnp_didl_lite_object_get_genre(object);
	const char* artist = gupnp_didl_lite_object_get_artist(object);
	const char* creator = gupnp_didl_lite_object_get_creator(object);
	const char* author = gupnp_didl_lite_object_get_author(object);

	xmlNode *  xml = gupnp_didl_lite_object_get_xml_node(object);
	printElementName(xml);

	g_print("\t\tid %s, parent id: %s\n", id, parentId);
	g_print("\t\ttitle: %s\n", title);
	g_print("\t\tdesc: %s\n", desc);
	g_print("\t\tclass: %s\n", qlass);
	g_print("\t\talbum: %s\n", album);
	g_print("\t\talbumArt: %s\n", albumArt);
	g_print("\t\tgenre: %s\n", genre);
	g_print("\t\tartist: %s\n", artist);
	g_print("\t\tcreator: %s\n", creator);
	g_print("\t\tauthor: %s\n", author);

	if(gupnp_didl_lite_object_get_restricted(object)) {
		g_print("\t\trestricted\n");
	}

	GList* iter = gupnp_didl_lite_object_get_resources(object);
	for(;iter; iter=iter->next) {
		GUPnPDIDLLiteResource* res = (GUPnPDIDLLiteResource*) iter->data;
		if(res) {
			printResource(res);
			g_object_unref(res);
		}
	}
	 g_list_free(iter);
	 g_print("\n");
//
//	 if(g_strncasecmp(qlass, "object.container", strlen("object.container")) == 0) {
//	 }
}

void UpnpControlPoint::printResource(GUPnPDIDLLiteResource* res)
{
	const char* uri = gupnp_didl_lite_resource_get_uri(res);
	if(uri) {
		g_print("\t\t\turi: %s\n", uri);
	}
	GUPnPProtocolInfo* protocol = gupnp_didl_lite_resource_get_protocol_info(res);
	if(protocol)
		printProtocolInfo(protocol);
}

void UpnpControlPoint::printItem(GUPnPDIDLLiteItem* item)
{
	const char* refId = gupnp_didl_lite_item_get_ref_id(item);
	if(refId)
		g_print("\t\t\trefId: %s\n", refId);
}

void UpnpControlPoint::printProtocolInfo(GUPnPProtocolInfo* protocol)
{
	const char* prot = gupnp_protocol_info_get_protocol(protocol);
	const char* network = gupnp_protocol_info_get_network(protocol);
	const char* mimeType = gupnp_protocol_info_get_mime_type(protocol);
	const char* dlnaProfile = gupnp_protocol_info_get_dlna_profile(protocol);
	const char** speeds = gupnp_protocol_info_get_play_speeds(protocol);
	GUPnPDLNAConversion conversion = gupnp_protocol_info_get_dlna_conversion(protocol);
	GUPnPDLNAOperation  operation = gupnp_protocol_info_get_dlna_operation(protocol);
	GUPnPDLNAFlags flags = gupnp_protocol_info_get_dlna_flags(protocol);

	g_print("protocol: %s\n", prot);
	g_print("network: %s\n", network);
	g_print("mime-type: %s\n", mimeType);
	g_print("dlna profile: %s\n", dlnaProfile);

	g_print("transcoded: %d\n", conversion);
	g_print("operation: %d\n", operation);
	g_print("flags: %d\n", flags);
}

void UpnpControlPoint::printElementName(xmlNode* node)
{
	 xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s - %s\n", cur_node->name, cur_node->content);
        }

        printElementName(cur_node->children);
    }
}

} /* namespace halkamalka */
