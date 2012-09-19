/*
 * Platform.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#include "Platform.h"

#include <clutter/clutter.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include "GstPlaybin.h"

namespace halkamalka {

Platform::Platform() : m_initialized(false){
	// TODO Auto-generated constructor stub

}

Platform::~Platform() {
	// TODO Auto-generated destructor stub
}

void Platform::init(int* argc, char*** argv)
{
	if(m_initialized)
		return;

	if (!g_thread_supported ())
		g_thread_init (NULL);

	clutter_gst_init (argc, argv);

	m_stage = clutter_stage_get_default();
	clutter_actor_set_size(m_stage, (gfloat)960, (gfloat)540);

	m_videoTexture = clutter_texture_new ();
	GstPlaybin::getInstance().init(m_videoTexture);
	clutter_container_add_actor (CLUTTER_CONTAINER (m_stage), m_videoTexture);
	g_signal_connect (CLUTTER_STAGE (m_stage), "fullscreen", G_CALLBACK (Platform::windowSizeChanged), this);
	g_signal_connect (CLUTTER_STAGE (m_stage), "unfullscreen", G_CALLBACK (Platform::windowSizeChanged), this);
}

gint Platform::start(void)
{
	clutter_actor_show(m_stage);
	clutter_main();
	return 0;
}

void* Platform::getWindow() const
{
	return static_cast<void*>(m_stage);
}

void Platform::windowSizeChanged (ClutterStage * stage, gpointer data)
{
	Platform* pThis = static_cast<Platform*>(data);
	gfloat stageWidth = clutter_actor_get_width (pThis->m_stage);
	gfloat stageHeight = clutter_actor_get_height (pThis->m_stage);
	gfloat videoTextureWidth = clutter_actor_get_width (pThis->m_videoTexture);
	gfloat videoTextureHeight = clutter_actor_get_height (pThis->m_videoTexture);

	gfloat width = stageWidth;
	gfloat height = stageHeight;
	gfloat stageAspectRatio = stageWidth / stageHeight;
	gfloat videoTextureAspectRatio = videoTextureWidth / videoTextureHeight;

	if(videoTextureAspectRatio > stageAspectRatio) {
		height = stageWidth / videoTextureAspectRatio;
	}
	else {
		width = stageHeight * videoTextureAspectRatio;
	}

	g_print("stage: %f, %f\n", stageWidth, stageHeight);
	g_print("video texture: %f, %f\n", videoTextureWidth, videoTextureHeight);
	clutter_actor_set_size (pThis->m_videoTexture, width, height);
//		clutter_actor_set_position (pThis->m_videoTexture, w / 2,h / 2);

	videoTextureWidth = clutter_actor_get_width (pThis->m_videoTexture);
	videoTextureHeight = clutter_actor_get_height (pThis->m_videoTexture);
	g_print("stage: %f, %f\n", stageWidth, stageHeight);
	g_print("video texture: %f, %f\n", videoTextureWidth, videoTextureHeight);
}

} /* namespace halkamalka */
