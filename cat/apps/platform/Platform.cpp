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

	ClutterActor* videoTexture = clutter_texture_new ();
	GstPlaybin::getInstance().init(videoTexture);
	clutter_container_add_actor (CLUTTER_CONTAINER (m_stage), videoTexture);
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

} /* namespace halkamalka */
