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
	pthread_mutex_destroy(&m_lck);
	pthread_cond_destroy(&m_cond);
}

void UserInterface::init() {
	m_stage = static_cast<ClutterActor*>(Platform::getInstance().getWindow());
	g_assert(m_stage != NULL);

	g_signal_connect (CLUTTER_STAGE(m_stage), "key-press-event", G_CALLBACK (UserInterface::keyPressed), this);
	g_signal_connect(CLUTTER_TEXTURE((ClutterActor*) Platform::getInstance().getTexture()), "size-change", G_CALLBACK(UserInterface::size_change), this);
	// TODO: remove
#if 0
	GstPlaybin::getInstance().setUri("http://192.168.1.5:49152/web/7.avi");
#endif
#if 0
	m_subNoti = new subNoti;
	m_subNoti->createPartControl(m_stage);
	m_subNoti->setVisible(true);
#endif
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

void UserInterface::size_change(ClutterActor *texture, gint width, gint height, gpointer data) {

	UserInterface* pThis = static_cast<UserInterface*>(data);
	ClutterActor *stage;
	gfloat new_x, new_y, new_width, new_height;
	gfloat stage_width, stage_height;
	ClutterAnimation *animation = NULL;

	stage = clutter_actor_get_stage(texture);
	if (stage == NULL)
		return;

	clutter_actor_get_size(stage, &stage_width, &stage_height);

	/* Center video on window and calculate new size preserving aspect ratio */
	new_height = (height * stage_width) / width;
	if (new_height <= stage_height) {
		new_width = stage_width;

		new_x = 0;
		new_y = (stage_height - new_height) / 2;
	} else {
		new_width = (width * stage_height) / height;
		new_height = stage_height;

		new_x = (stage_width - new_width) / 2;
		new_y = 0;
	}
	clutter_actor_set_position(texture, new_x, new_y);
	clutter_actor_set_size(texture, new_width, new_height);
	clutter_actor_set_rotation(texture, CLUTTER_Y_AXIS, 0.0, stage_width / 2, 0,
			0);
	/* Animate it */
//	animation = clutter_actor_animate(texture, CLUTTER_LINEAR, 10000, "rotation-angle-y", 360.0, NULL);
	animation = clutter_actor_animate(texture, CLUTTER_LINEAR, 100, "rotation-angle-y", 360.0, NULL);
//	clutter_animation_set_loop(animation, TRUE);
}


} /* namespace halkamalka */
