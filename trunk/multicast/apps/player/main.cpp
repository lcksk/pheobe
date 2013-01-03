/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: buttonfly
 */




#include <gst/gst.h>

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
GMainLoop *loop = (GMainLoop *) data;

switch (GST_MESSAGE_TYPE (msg)) {

case GST_MESSAGE_EOS:
g_print("End of stream\n");
g_main_loop_quit(loop);
break;

case GST_MESSAGE_ERROR: {
gchar *debug;
GError *error;

gst_message_parse_error(msg, &error, &debug);
g_free(debug);

g_printerr("Error: %s\n", error->message);
g_error_free(error);

g_main_loop_quit(loop);
break;
}
default:
break;
}

return TRUE;
}

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
GstPad *sinkpad;
GstElement *decoder = (GstElement *) data;

/* We can now link this pad with the vorbis-decoder sink pad */
g_print("Dynamic pad created, linking demuxer/decoder\n");

sinkpad = gst_element_get_static_pad(decoder, "sink");

gst_pad_link(pad, sinkpad);

gst_object_unref(sinkpad);
}

static gboolean trackPositionListener(gpointer data) {
g_print("trackPositionListeneer");
GstFormat fmt = GST_FORMAT_TIME;
GstElement* pipeline = (GstElement*) data;
gint64 position, len;

if (gst_element_query_position(GST_ELEMENT(pipeline), &fmt, &position)
&& gst_element_query_duration(GST_ELEMENT(pipeline), &fmt, &len)) {
g_print("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
GST_TIME_ARGS (position), GST_TIME_ARGS (len));
}
return TRUE;
}

int main(int argc, char** argv) {
gst_init(&argc, &argv);

GMainLoop* loop = g_main_loop_new(NULL, FALSE);

GstElement *pipeline, *source, *demuxer, *decoder, *conv, *sink;
pipeline = gst_pipeline_new("audio-player");
source = gst_element_factory_make("filesrc", "file-source");
demuxer = gst_element_factory_make("oggdemux", "ogg-demuxer");
decoder = gst_element_factory_make("vorbisdec", "vorbis-decoder");
conv = gst_element_factory_make("audioconvert", "converter");
sink = gst_element_factory_make("autoaudiosink", "audio-output");

/* we set the input filename to the source element */
g_object_set(G_OBJECT (source), "location", argv[1], NULL);

/* we add a message handler */
GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
gst_bus_add_watch(bus, bus_call, loop);
gst_object_unref(bus);

/* we add all elements into the pipeline */
gst_bin_add_many(GST_BIN (pipeline), source, demuxer, decoder, conv, sink,
NULL);

/* we link the elements together */
gst_element_link(source, demuxer);
gst_element_link_many(decoder, conv, sink, NULL);
g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), decoder);

g_timeout_add(1000, (GSourceFunc) trackPositionListener, pipeline);

/* note that the demuxer will be linked to the decoder dynamically.
 The reason is that Ogg may contain various streams (for example
 audio and video). The source pad(s) will be created at run time,
 by the demuxer when it detects the amount and nature of streams.
 Therefore we connect a callback function which will be executed
 when the "pad-added" is emitted.*/

/* Set the pipeline to "playing" state*/
g_print("Now playing: %s\n", argv[1]);
gst_element_set_state(pipeline, GST_STATE_PLAYING);

/* Iterate */
g_print("Running...\n");
g_main_loop_run(loop);

/* Out of the main loop, clean up nicely */
g_print("Returned, stopping playback\n");
gst_element_set_state(pipeline, GST_STATE_NULL);

g_print("Deleting pipeline\n");
gst_object_unref(GST_OBJECT (pipeline));

return 0;
}
