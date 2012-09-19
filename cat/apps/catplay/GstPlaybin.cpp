/*
 * GstPlaybin.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */

#include "GstPlaybin.h"

namespace halkamalka {

GstPlaybin::GstPlaybin() :
m_isInitialized(false) {
}

GstPlaybin::~GstPlaybin() {

}

void GstPlaybin::init(ClutterActor* videoTexture)
{
	if(m_isInitialized) {
		return;
	}
	m_sink = clutter_gst_video_sink_new (CLUTTER_TEXTURE (videoTexture));
	m_playbin = gst_element_factory_make ("playbin2", "playbin2");
	g_object_set (G_OBJECT (m_playbin), "video-sink", m_sink, NULL);
	m_bus = gst_pipeline_get_bus (GST_PIPELINE (m_playbin));
//	gst_bus_add_watch (m_bus, busCall, this);
	gst_object_unref (m_bus);
	m_isInitialized = true;
}

int GstPlaybin::play()
{
	gst_element_set_state(m_playbin, GST_STATE_PLAYING);
}

int GstPlaybin::stop()
{
	gst_element_set_state(m_playbin, GST_STATE_NULL);
}

int GstPlaybin::pause()
{

}

int GstPlaybin::resume()
{

}

int GstPlaybin::setUri(const gchar* uri)
{
	g_object_set(m_playbin, "uri", uri, NULL);
}


} /* namespace halkamalka */
