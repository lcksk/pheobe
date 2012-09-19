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

const char* halkamalka::UserInterfaceService::ID = "UserInterfaceService";

namespace halkamalka {

UserInterfaceService::UserInterfaceService() {
    pthread_attr_t attri;
    pthread_attr_init(&attri);
    pthread_attr_setdetachstate(&attri, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_lck, NULL);
    pthread_cond_init(&m_cond, NULL);

    pthread_attr_destroy(&attri);
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
	g_signal_connect (CLUTTER_STAGE (m_stage), "fullscreen", G_CALLBACK (UserInterfaceService::windowSizeChanged), this);
	g_signal_connect (CLUTTER_STAGE (m_stage), "unfullscreen", G_CALLBACK (UserInterfaceService::windowSizeChanged), this);
//	  g_signal_connect (ui->stage, "event", G_CALLBACK (event_cb), ui);

	ClutterActor* text = clutter_text_new ();
	ClutterColor color = { 0x60, 0x60, 0x90, 0xFF }; /* blueish */
	clutter_text_set_color (CLUTTER_TEXT (text), &color);
//	clutter_text_set_font_name (CLUTTER_TEXT (context->text), "Sans 24");
	clutter_actor_set_position (text, 10, 10);
//	clutter_actor_set_opacity(text, (guint8) 0);
	clutter_text_set_text (CLUTTER_TEXT (text), "Hello world");
	clutter_container_add_actor (CLUTTER_CONTAINER (m_stage), text);
	clutter_actor_show (text);

	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);

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

	guint16 keycode = clutter_event_get_key_code(event);
	g_print ("key code (%d)\n", keycode);
	switch(keycode) {
	case UserInterfaceService::VK_RIGHT: {
		gint64 pos = playbackService->getPosition();
		gint64 duration = playbackService->getDuration();
		pos +=SKIP_UNIT;
		if(duration < pos) {
			pos = duration;
		}
		playbackService->setPosition(pos);
		break;
	}
	case UserInterfaceService::VK_LEFT: {
		// skip back
		gint64 pos = playbackService->getPosition();
		pos -=SKIP_UNIT;
		if(pos < 0) {
			pos = 0;
		}
		playbackService->setPosition(pos);
		break;
	}
	case UserInterfaceService::VK_DOWN: {
		gdouble level = playbackService->getVolume();
		if(level > 0.1) {
			level -= 0.1;
		}
		playbackService->setVolume(level);
		break;
	}
	case UserInterfaceService::VK_UP: {
		gdouble level = playbackService->getVolume();
		level += 0.1;
		playbackService->setVolume(level);
		break;
	}

	case UserInterfaceService::VK_ENTER: {
		gboolean isFullScreen = clutter_stage_get_fullscreen  (CLUTTER_STAGE(pThis->m_stage));
		clutter_stage_set_fullscreen(CLUTTER_STAGE(pThis->m_stage), !isFullScreen);
		break;
	}

	case UserInterfaceService::VK_SPACE_BAR:
		if(!playbackService->isPlaying()) {
			playbackService->resume();
		}
		else {
			playbackService->pause();
		}
		break;
	}
}

void UserInterfaceService::windowSizeChanged (ClutterStage * stage, gpointer data)
{
	UserInterfaceService* pThis = static_cast<UserInterfaceService*>(data);

	g_print("windowSizeChanged\n");

}

//static void
//size_change (ClutterStage * stage, UserInterface * ui)
//{
//  gfloat stage_width, stage_height;
//  gfloat new_width, new_height;
//  gfloat media_width, media_height;
//  gfloat stage_ar, media_ar;
//
//  media_width = clutter_actor_get_width (ui->texture);
//  media_height = clutter_actor_get_height (ui->texture);
//
//  stage_width = clutter_actor_get_width (ui->stage);
//  stage_height = clutter_actor_get_height (ui->stage);
//
//  ui->stage_width = stage_width;
//  ui->stage_height = stage_height;
//
//  stage_ar = stage_width / stage_height;
//
//  new_width = stage_width;
//  new_height = stage_height;
//
//  if (media_height > 0.0f && media_width > 0.0f) {
//    /* if we're rotated, the media_width and media_height are swapped */
//    if (ui->rotated) {
//      media_ar = media_height / media_width;
//    } else {
//      media_ar = media_width / media_height;
//    }
//
//    /* calculate new width and height
//     * note: when we're done, new_width/new_height should equal media_ar */
//    if (media_ar > stage_ar) {
//      /* media has wider aspect than stage so use new width as stage width and
//       * scale down height */
//      new_height = stage_width / media_ar;
//    } else {
//      new_width = stage_height * media_ar;
//    }
//  } else {
//    g_debug ("Warning: not considering texture dimensions %fx%f\n", media_width,
//        media_height);
//  }
//
//  clutter_actor_set_size (CLUTTER_ACTOR (ui->texture), new_width, new_height);
//  clutter_actor_set_position (CLUTTER_ACTOR (ui->texture), stage_width / 2,
//      stage_height / 2);
//
//  update_controls_size (ui);
//  center_controls (ui);
//  progress_timing (ui);
//}


} /* namespace halkamalka */
