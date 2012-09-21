/*
 * subNoti.cpp
 *
 *  Created on: Sep 21, 2012
 *      Author: buttonfly
 */

#include "subNoti.h"
#include "UpnpControlPoint.h"

namespace halkamalka {

subNoti::subNoti()
: m_visible(false)
{

}

subNoti::~subNoti() {
	// TODO Auto-generated destructor stub
}

void subNoti::createPartControl(ClutterActor* stage)
{

	m_text = clutter_text_new ();
	ClutterColor color = { 0x60, 0x60, 0x90, 0xFF }; /* blueish */
	clutter_text_set_color (CLUTTER_TEXT (m_text), &color);
//	clutter_text_set_font_name (CLUTTER_TEXT (context->text), "Sans 24");
	clutter_actor_set_position (m_text, 10, 10);
//	clutter_actor_set_opacity(text, (guint8) 0);
	clutter_text_set_text (CLUTTER_TEXT (m_text), "Hello world");
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), m_text);
	clutter_actor_show (m_text);
	UpnpControlPoint::getInstance().addDeviceListener(this);

	m_fadeEffectTimeline = clutter_timeline_new(300);
	g_signal_connect (m_fadeEffectTimeline, "completed", G_CALLBACK (subNoti::fadeEffectCompleted), this);
}

void subNoti::setVisible(bool visible)
{
	m_visible = visible;
}

void subNoti::fadeEffectCompleted(ClutterTimeline* timeline, gpointer data)
{
	subNoti* pThis = static_cast<subNoti*>(data);
	g_print ("fadeEffectCompleted\n");
	g_object_unref(pThis->m_behaviourOpacity);
	pThis->m_behaviourOpacity = NULL;
}

void subNoti::deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	g_print ("deviceAdded\n");
	clutter_timeline_stop(m_fadeEffectTimeline);
	ClutterAlpha *alpha = clutter_alpha_new_full (m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
	m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, (guint8)0,  (guint8)255);
	clutter_behaviour_apply (m_behaviourOpacity, m_text);
	const char* friendlyName = gupnp_device_info_get_friendly_name(GUPNP_DEVICE_INFO(proxy));
	clutter_text_set_text (CLUTTER_TEXT (m_text), friendlyName);
	clutter_timeline_start(m_fadeEffectTimeline);
}

void subNoti::deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	g_print ("deviceRemoved\n");
	clutter_timeline_stop(m_fadeEffectTimeline);
	ClutterAlpha *alpha = clutter_alpha_new_full (m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
	m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
	clutter_behaviour_apply (m_behaviourOpacity, m_text);
	const char* friendlyName = gupnp_device_info_get_friendly_name(GUPNP_DEVICE_INFO(proxy));
	clutter_text_set_text (CLUTTER_TEXT (m_text), friendlyName);
	clutter_timeline_start(m_fadeEffectTimeline);
}


} /* namespace halkamalka */
