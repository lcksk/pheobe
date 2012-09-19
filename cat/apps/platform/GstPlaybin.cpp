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
	gst_bus_add_watch (m_bus, GstPlaybin::busMessage, this);
	gst_object_unref (m_bus);
//	  g_signal_connect (ui->stage, "event", G_CALLBACK (event_cb), ui);
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
	gst_element_set_state(m_playbin, GST_STATE_PAUSED);
}

int GstPlaybin::resume()
{
	gst_element_set_state(m_playbin, GST_STATE_PLAYING);
}

int GstPlaybin::setUri(const gchar* uri)
{
	g_object_set(m_playbin, "uri", uri, NULL);
}

void GstPlaybin::setPosition(gint64 position)
{
	  GstFormat fmt = GST_FORMAT_TIME;
	  gst_element_seek_simple (m_playbin, fmt, GST_SEEK_FLAG_FLUSH, position);
	  g_print("fmt: %d, cur: %lld\n", fmt, position);
}

gint64 GstPlaybin::getDuration()
{
	gint64 duration;
	gboolean rc;
	GstFormat fmt =  GST_FORMAT_TIME;

	rc = gst_element_query_duration(m_playbin, &fmt, &duration);
	g_assert(rc != FALSE);
	return duration;
}

gint64 GstPlaybin::getPosition()
{
	gboolean rc;
	gint64 cur = 0;
	GstFormat fmt;
	rc = gst_element_query_position(m_playbin, &fmt, &cur);
	g_assert(rc != FALSE);
	g_print("fmt: %d, cur: %lld\n", fmt, cur);
	return cur;
}

gdouble GstPlaybin::getVolume()
{
	gdouble volume;
	g_object_get (m_playbin, "volume", &volume, NULL);
	return volume;
}

void GstPlaybin::setVolume(gdouble level)
{
	  g_object_set (m_playbin, "volume", level, NULL);
}


gboolean GstPlaybin::isPlaying() {
	GstStateChangeReturn ret;
	GstState state;
	GstState pending;
	ret = gst_element_get_state(m_playbin, &state, &pending,  GST_CLOCK_TIME_NONE);
	if(state == GST_STATE_PLAYING) {
		return true;
	}
	return false;
}


gboolean GstPlaybin::busMessage (GstBus * bus, GstMessage * msg, gpointer data)
{
	GstPlaybin* pThis = static_cast<GstPlaybin*>(data);
	switch (GST_MESSAGE_TYPE (msg)) {
	 case GST_MESSAGE_EOS: {
		 g_debug ("End-of-stream\n");
		 break;
	 }
	 case GST_MESSAGE_ERROR:  {
		 gchar *debug = NULL;
		 GError *err = NULL;
		 gst_message_parse_error (msg, &err, &debug);
		 g_debug ("Error: %s\n", err->message);
		 g_error_free (err);
		 if (debug) {
			 g_debug ("Debug details: %s\n", debug);
			 g_free (debug);
		 }
		 break;
	 }
	 case GST_MESSAGE_STATE_CHANGED: {
			GstState oldState, newState, pending;
			gst_message_parse_state_changed (msg, &oldState, &newState, &pending);
			if (newState == GST_STATE_PLAYING) {
			/* If loading file */
//			if (!engine->has_started) {
//				gint64 position;
//
//				/* Check if URI was left unfinished, if so seek to last position */
//				position = is_uri_unfinished_playback (engine, engine->uri);
//				if (position != -1) {
////				engine_seek (engine, position);
//			}

//			if (!engine->secret)
//			add_uri_to_history (engine->uri);
//			else
//			g_print ("Secret mode. Not saving uri in history.\n");
//
//			interface_update_controls (ui);
//			engine->has_started = TRUE;

			}
			break;
	 }

	 case GST_MESSAGE_STEP_DONE: {
		 break;
	 }

	} // switch
	return TRUE;
}

} /* namespace halkamalka */
