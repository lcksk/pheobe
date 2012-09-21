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


#include "subNoti.h"

namespace halkamalka {

UserInterface::UserInterface() {
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);
}

UserInterface::~UserInterface() {

}

void UserInterface::init() {
	m_stage = static_cast<ClutterActor*>(Platform::getInstance().getWindow());
	g_assert(m_stage != NULL);

	g_signal_connect (CLUTTER_STAGE(m_stage), "key-press-event", G_CALLBACK (UserInterface::keyPressed), this);

	GstPlaybin::getInstance().setUri("file:///home/buttonfly/Videos/MV/1.avi");

	m_subNoti = new subNoti;
	m_subNoti->createPartControl(m_stage);
	m_subNoti->setVisible(true);
}


gboolean UserInterface::keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
#define SKIP_UNIT	10000000000 // TODO: read from conf.
	UserInterface* pThis = static_cast<UserInterface*>(data);
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
		break;
	}
}


} /* namespace halkamalka */
