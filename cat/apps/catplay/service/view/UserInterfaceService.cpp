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
	ClutterActor* stage = static_cast<ClutterActor*>(Platform::getInstance().getWindow());
	g_assert(stage != NULL);

	g_signal_connect (stage, "key-press-event", G_CALLBACK (UserInterfaceService::keyPressed), this);

	ClutterActor* text = clutter_text_new ();
	ClutterColor color = { 0x60, 0x60, 0x90, 0xFF }; /* blueish */
	clutter_text_set_color (CLUTTER_TEXT (text), &color);
//	clutter_text_set_font_name (CLUTTER_TEXT (context->text), "Sans 24");
	clutter_actor_set_position (text, 10, 10);
//	clutter_actor_set_opacity(text, (guint8) 0);
	clutter_text_set_text (CLUTTER_TEXT (text), "Hello world");
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), text);
	clutter_actor_show (text);

	PlaybackService* playbackService = dynamic_cast<PlaybackService*>(ServiceManager::GetInstance().GetServiceProxy(PlaybackService::ID));
	g_assert(playbackService != NULL);

	playbackService->setUri("file:///home/buttonfly/Videos/MV/1.avi");
	playbackService->play();
	for(;;) {
		 pthread_cond_wait(&m_cond, &m_lck);
	}
}

gboolean UserInterfaceService::keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
	UserInterfaceService* pThis = static_cast<UserInterfaceService*>(data);

	guint16 keycode = clutter_event_get_key_code(event);
	g_print ("key code (%d)\n", keycode);
	switch(keycode) {
	case UserInterfaceService::VK_RIGHT:
		// skip
		break;

	case UserInterfaceService::VK_LEFT:
		// skip back
		break;

	case UserInterfaceService::VK_DOWN:
		// volume down
		break;

	case UserInterfaceService::VK_UP:
		// volume up
		break;
	}
}


} /* namespace halkamalka */
