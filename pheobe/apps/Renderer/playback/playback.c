/*
 * player.c
 *
 *  Created on: Sep 12, 2012
 *      Author: buttonfly
 */

#include <gst/gst.h>
#include "playback.h"

typedef enum {
  GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

typedef struct {
	GstElement *playbin2; /* Our one and only element */

	gint n_video; /* Number of embedded video streams */
	gint n_audio; /* Number of embedded audio streams */
	gint n_text; /* Number of embedded subtitle streams */

	gint current_video; /* Currently playing video stream */
	gint current_audio; /* Currently playing audio stream */
	gint current_text; /* Currently playing subtitle stream */

//	  GMainLoop *main_loop;  /* GLib's Main Loop */
} playback_t;

playback playback_getInstance() {
	static playback_t* instance = NULL;
	if (instance == NULL ) {
		gint flags;
		gst_init(NULL, NULL );
		instance = (playback_t*) g_malloc(sizeof(playback));
		instance->playbin2 = gst_element_factory_make("playbin2", "playbin2");

		if (!instance->playbin2) {
			g_printerr("Not all elements could be created.\n");
			return -1;
		}
		g_object_get(instance->playbin2, "flags", &flags, NULL );
		flags |= GST_PLAY_FLAG_VIDEO | GST_PLAY_FLAG_AUDIO;
		flags &= ~GST_PLAY_FLAG_TEXT;
		g_object_set(instance->playbin2, "flags", flags, NULL );

	}
	return instance;
}

int playback_set_uri(playback handle, const gchar* uri) {
	playback_t* instance = (playback_t*) handle;
	g_object_set(instance->playbin2, "uri", uri, NULL );
	g_print("playback_set_uri: %s\n", uri);
}

int playback_play(playback handle, gint speed) {
	playback_t* instance = (playback_t*) handle;
	g_print("playback_play\n");
	gst_element_set_state(instance->playbin2, GST_STATE_PLAYING);
}

int playback_stop(playback handle) {

}

int playback_pause(playback handle) {

}

int playback_resume(playback handle) {

}

