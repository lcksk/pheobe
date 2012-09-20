/*
 * UserInterfaceService.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: buttonfly
 */

#include "UserInterfaceService.h"
#include "Platform.h"
#include <clutter/clutter.h>
#include <stdlib.h>

#include "ServiceManager.h"
#include "PlaybackService.h"
#include "UpnpControlPointService.h"

const char* halkamalka::UserInterfaceService::ID = "UserInterfaceService";

namespace halkamalka {

UserInterfaceService::UserInterfaceService() {
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);

    m_fadeEffectTimeline = clutter_timeline_new(300);
    g_signal_connect (m_fadeEffectTimeline, "completed", G_CALLBACK (UserInterfaceService::fadeEffectCompleted), this);
}

UserInterfaceService::~UserInterfaceService() {
	// TODO Auto-generated destructor stub
}

const char* UserInterfaceService::GetID()
{
	return ID;
}

void UserInterfaceService::Run()
{
	m_stage = static_cast<ClutterActor*>(Platform::getInstance().getWindow());
	g_assert(m_stage != NULL);

	g_signal_connect (CLUTTER_STAGE(m_stage), "key-press-event", G_CALLBACK (UserInterfaceService::keyPressed), this);
//	g_signal_connect (CLUTTER_STAGE (m_stage), "fullscreen", G_CALLBACK (UserInterfaceService::windowSizeChanged), this);
//	g_signal_connect (CLUTTER_STAGE (m_stage), "unfullscreen", G_CALLBACK (UserInterfaceService::windowSizeChanged), this);
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

	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);

	UpnpControlPointService* upnpControlPoint = dynamic_cast<UpnpControlPointService*>(ServiceManager::GetInstance().GetServiceProxy(UpnpControlPointService::ID));
	g_assert(upnpControlPoint != NULL);
	upnpControlPoint->addDeviceListener(this);

	//TODO
	playbackService->setUri("file:///home/buttonfly/Videos/MV/1.avi");

	for(;;) {
		 pthread_cond_wait(&m_cond, &m_lck);
	}
}

gboolean UserInterfaceService::keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
#define SKIP_UNIT	10000000000 // TODO: read from conf.
	UserInterfaceService* pThis = static_cast<UserInterfaceService*>(data);
	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);

	guint16 keycode = clutter_event_get_key_symbol (event);
	g_print ("key code (%d)\n", keycode);
	switch(keycode) {
	case CLUTTER_Right: {
		gint64 pos = playbackService->getPosition();
		gint64 duration = playbackService->getDuration();
		pos +=SKIP_UNIT;
		if(duration < pos) {
			pos = duration;
		}
		playbackService->setPosition(pos);
		break;
	}
	case CLUTTER_Left: {
		// skip back
		gint64 pos = playbackService->getPosition();
		pos -=SKIP_UNIT;
		if(pos < 0) {
			pos = 0;
		}
		playbackService->setPosition(pos);
		break;
	}
	case CLUTTER_Down: {
		gdouble level = playbackService->getVolume();
		if(level > 0.1) {
			level -= 0.1;
		}
		playbackService->setVolume(level);
		break;
	}
	case CLUTTER_Up: {
		gdouble level = playbackService->getVolume();
		level += 0.1;
		playbackService->setVolume(level);
		break;
	}

	case CLUTTER_Return: {
		gboolean isFullScreen = clutter_stage_get_fullscreen  (CLUTTER_STAGE(pThis->m_stage));
		clutter_stage_set_fullscreen(CLUTTER_STAGE(pThis->m_stage), !isFullScreen);
		break;
	}

	case CLUTTER_space:
		if(!playbackService->isPlaying()) {
			playbackService->resume();
		}
		else {
			playbackService->pause();
		}
		clutter_timeline_stop(pThis->m_fadeEffectTimeline);
		ClutterAlpha *alpha = clutter_alpha_new_full (pThis->m_fadeEffectTimeline, CLUTTER_EASE_IN_SINE);
		pThis->m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
		clutter_behaviour_apply (pThis->m_behaviourOpacity, pThis->m_text);

		clutter_timeline_start(pThis->m_fadeEffectTimeline);

		break;
	}
}

void UserInterfaceService::fadeEffectCompleted(ClutterTimeline* timeline, gpointer data)
{
	UserInterfaceService* pThis = static_cast<UserInterfaceService*>(data);
	g_print ("fadeEffectCompleted\n");
	g_object_unref(pThis->m_behaviourOpacity);
	pThis->m_behaviourOpacity = NULL;
}

void UserInterfaceService::deviceAdded(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
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

void UserInterfaceService::deviceRemoved(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy)
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
