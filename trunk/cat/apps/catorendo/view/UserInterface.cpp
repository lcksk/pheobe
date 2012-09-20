/*
 * UserInterface.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#include "UserInterface.h"
#include "Platform.h"
#include <clutter/clutter.h>
#include <stdlib.h>

#include "GstPlaybin.h"
#include "UpnpControlPoint.h"

namespace halkamalka {

UserInterface::UserInterface() {
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);

    m_fadeEffectTimeline = clutter_timeline_new(300);
    g_signal_connect (m_fadeEffectTimeline, "completed", G_CALLBACK (UserInterface::fadeEffectCompleted), this);
}

UserInterface::~UserInterface() {
	// TODO Auto-generated destructor stub
}

void UserInterface::init() {
	m_stage = static_cast<ClutterActor*>(Platform::getInstance().getWindow());
	g_assert(m_stage != NULL);

	g_signal_connect (CLUTTER_STAGE(m_stage), "key-press-event", G_CALLBACK (UserInterface::keyPressed), this);
//	g_signal_connect (CLUTTER_STAGE (m_stage), "fullscreen", G_CALLBACK (UserInterface::windowSizeChanged), this);
//	g_signal_connect (CLUTTER_STAGE (m_stage), "unfullscreen", G_CALLBACK (UserInterface::windowSizeChanged), this);
//	  g_signal_connect (ui->stage, "event", G_CALLBACK (event_cb), ui);

	m_text = clutter_text_new ();
	ClutterColor color = { 0x60, 0x60, 0x90, 0xFF }; /* blueish */
	clutter_text_set_color (CLUTTER_TEXT (m_text), &color);
//	clutter_text_set_font_name (CLUTTER_TEXT (context->text), "Sans 24");
	clutter_actor_set_position (m_text, 10, 10);
//	clutter_actor_set_opacity(text, (guint8) 0);
	clutter_text_set_text (CLUTTER_TEXT (m_text), "Hello world");
	clutter_container_add_actor (CLUTTER_CONTAINER (m_stage), m_text);
	clutter_actor_show (m_text);

#if 0
	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);

	UpnpControlPointService* upnpControlPoint = dynamic_cast<UpnpControlPointService*>(ServiceManager::GetInstance().GetServiceProxy(UpnpControlPointService::ID));
	g_assert(upnpControlPoint != NULL);
	upnpControlPoint->addDeviceListener(this);

	//TODO
	playbackService->setUri("file:///home/buttonfly/Videos/MV/1.avi");
#endif
	UpnpControlPoint::getInstance().addDeviceListener(this);

	GstPlaybin::getInstance().setUri("file:///home/buttonfly/Videos/MV/1.avi");
}


gboolean UserInterface::keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
#define SKIP_UNIT	10000000000 // TODO: read from conf.
	UserInterface* pThis = static_cast<UserInterface*>(data);
#if 0
	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);
#endif
	GstPlaybin& playbin = GstPlaybin::getInstance();

	guint16 keycode = clutter_event_get_key_symbol (event);
	g_print ("key code (%d)\n", keycode);
	switch(keycode) {
	case CLUTTER_Right: {
		gint64 pos =playbin.getPosition();
		gint64 duration = playbin.getDuration();
		pos +=SKIP_UNIT;
		if(duration < pos) {
			pos = duration;
		}
		playbin.setPosition(pos);
		break;
	}
	case CLUTTER_Left: {
		// skip back
		gint64 pos = playbin.getPosition();
		pos -=SKIP_UNIT;
		if(pos < 0) {
			pos = 0;
		}
		playbin.setPosition(pos);
		break;
	}
	case CLUTTER_Down: {
		gdouble level = playbin.getVolume();
		if(level > 0.1) {
			level -= 0.1;
		}
		playbin.setVolume(level);
		break;
	}
	case CLUTTER_Up: {
		gdouble level = playbin.getVolume();
		level += 0.1;
		playbin.setVolume(level);
		break;
	}

	case CLUTTER_Return: {
		gboolean isFullScreen = clutter_stage_get_fullscreen  (CLUTTER_STAGE(pThis->m_stage));
		clutter_stage_set_fullscreen(CLUTTER_STAGE(pThis->m_stage), !isFullScreen);
		break;
	}

	case CLUTTER_space:
		if(!playbin.isPlaying()) {
			playbin.resume();
		}
		else {
			playbin.pause();
		}
		clutter_timeline_stop(pThis->m_fadeEffectTimeline);
		ClutterAlpha *alpha = clutter_alpha_new_full (pThis->m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
		pThis->m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
		clutter_behaviour_apply (pThis->m_behaviourOpacity, pThis->m_text);

		clutter_timeline_start(pThis->m_fadeEffectTimeline);

		break;
	}
}

void UserInterface::fadeEffectCompleted(ClutterTimeline* timeline, gpointer data)
{
	UserInterface* pThis = static_cast<UserInterface*>(data);
	g_print ("fadeEffectCompleted\n");
	g_object_unref(pThis->m_behaviourOpacity);
	pThis->m_behaviourOpacity = NULL;
}

void UserInterface::deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	g_print ("deviceAdded\n");
	clutter_timeline_stop(m_fadeEffectTimeline);
	ClutterAlpha *alpha = clutter_alpha_new_full (m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
	m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
	clutter_behaviour_apply (m_behaviourOpacity, m_text);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	clutter_text_set_text (CLUTTER_TEXT (m_text), type);
	clutter_timeline_start(m_fadeEffectTimeline);
}

void UserInterface::deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
{
	g_print ("deviceRemoved\n");
	clutter_timeline_stop(m_fadeEffectTimeline);
	ClutterAlpha *alpha = clutter_alpha_new_full (m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
	m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
	clutter_behaviour_apply (m_behaviourOpacity, m_text);
	const char* type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
	clutter_text_set_text (CLUTTER_TEXT (m_text), type);
	clutter_timeline_start(m_fadeEffectTimeline);
}


} /* namespace halkamalka */
