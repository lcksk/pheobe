/*
 * ClutterUI.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#include "ClutterUI.h"
#include "GstPlaybin.h"

namespace halkamalka {

ClutterUI::ClutterUI() {
	// TODO Auto-generated constructor stub

}

ClutterUI::~ClutterUI() {
	// TODO Auto-generated destructor stub
}

void ClutterUI::init(ClutterStage* stage) {
	if(m_initialized)
		return;

	m_stage = stage;
	m_text = clutter_text_new ();
	ClutterColor color = { 0x60, 0x60, 0x90, 0xFF }; /* blueish */
	clutter_text_set_color (CLUTTER_TEXT (m_text), &color);
//	clutter_text_set_font_name (CLUTTER_TEXT (context->text), "Sans 24");
	clutter_actor_set_position (m_text, 10, 10);
//	clutter_actor_set_opacity(text, (guint8) 0);
	clutter_text_set_text (CLUTTER_TEXT (m_text), "Hello world");
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), m_text);
	clutter_actor_show (m_text);

//	g_assert(m_timeline != NULL);
	m_timeline = clutter_timeline_new(300 /* frames per second. */);
	g_signal_connect (m_timeline, "completed", G_CALLBACK (ClutterUI::timelineRotationCompleted), this);
	g_signal_connect (m_stage, "key-press-event", G_CALLBACK (ClutterUI::keyPressed), this);
	m_initialized = true;
}

void ClutterUI::timelineRotationCompleted(ClutterTimeline* timeline, gpointer data)
{
	ClutterUI* pThis = static_cast<ClutterUI*>(data);
	g_print ("key timelineRotationCompleted\n");
//	UserData* context = (UserData*) data;
	g_object_unref (pThis->m_behaviourOpacity);
	pThis->m_behaviourOpacity = NULL;
}

gboolean ClutterUI::keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data)
{
	ClutterUI* pThis = static_cast<ClutterUI*>(data);
	guint16 keycode = clutter_event_get_key_code(event);

	g_print ("key code (%d)\n", keycode);

//	UserData* context = (UserData*) data;

	g_assert(pThis != NULL);
	if(pThis->m_timeline && clutter_timeline_is_playing(pThis->m_timeline)) {
		return FALSE;
	}

	// TODO
	clutter_timeline_stop(pThis->m_timeline);
	ClutterAlpha *alpha = clutter_alpha_new_full (pThis->m_timeline, CLUTTER_EASE_IN_SINE);
	pThis->m_behaviourOpacity = clutter_behaviour_opacity_new (alpha, 0, 255);
	clutter_behaviour_apply (pThis->m_behaviourOpacity, pThis->m_text);

	clutter_timeline_start(pThis->m_timeline);

	GstPlaybin::getInstance().setUri( "file:///home/buttonfly/Videos/MV/1.avi");
	GstPlaybin::getInstance().play();
	return TRUE;
}


} /* namespace halkamalka */
