#include <clutter-gst/clutter-gst.h>

static GstElement *pipeline;

void on_stage_button_press(ClutterStage* stage, ClutterEvent* event, gpointer data) {
	g_print("%s\n", __FUNCTION__);
//	gfloat x = 0;
//	gfloat y = 0;
//	clutter_event_get_coords(event, &x, &y);
//	ClutterActor* clicked = clutter_stage_get_actor_at_pos(CLUTTER_STAGE(stage), CLUTTER_PICK_ALL, x, y);
//
//	if(clicked == CLUTTER_ACTOR(stage))
//		return;
//
//	clutter_actor_hide(clicked);
}

void on_stage_key_press(ClutterStage* stage, ClutterEvent* event, gpointer data) {
	g_print("%s\n", __FUNCTION__);
	static gboolean toggle = FALSE;

	if(toggle) {
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}
	else {
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
	}
	toggle = !toggle;
}


/* Setup the video texture once its size is known */
void size_change(ClutterActor *texture, gint width, gint height,
		gpointer user_data) {
	g_print("%s\n", __FUNCTION__);
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
	animation = clutter_actor_animate(texture, CLUTTER_LINEAR, 10000,
			"rotation-angle-y", 360.0, NULL);
	clutter_animation_set_loop(animation, TRUE);
}

int main(int argc, char *argv[]) {
	GstElement *sink;
	ClutterTimeline *timeline;
	ClutterActor *stage, *texture;

	/* clutter-gst takes care of initializing Clutter and GStreamer */
	if (clutter_gst_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
		g_error("Failed to initialize clutter\n");
		return -1;
	}

	stage = clutter_stage_get_default();

	/* Make a timeline */
	timeline = clutter_timeline_new(1000);
	g_object_set(timeline, "loop", TRUE, NULL);

	/* Create new texture and disable slicing so the video is properly mapped onto it */
	texture = CLUTTER_ACTOR(g_object_new(CLUTTER_TYPE_TEXTURE, "disable-slicing", TRUE, NULL));
	g_signal_connect(texture, "size-change", G_CALLBACK(size_change), NULL);

	g_signal_connect(stage, "button-press-event", G_CALLBACK(on_stage_button_press), NULL);
	g_signal_connect(stage, "key-press-event", G_CALLBACK(on_stage_key_press), NULL);

	/* Build the GStreamer pipeline */
//  pipeline = gst_parse_launch ("playbin2 uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm", NULL);
//	pipeline = gst_parse_launch(	"playbin2 uri=file:///home/buttonfly/Videos/MV/1.avi", NULL);
	pipeline = gst_parse_launch(	"playbin2 uri=rtp://233.18.158.206:5000", NULL);

	/* Instantiate the Clutter sink */
#if 0
	sink = gst_element_factory_make("autocluttersink", NULL);
	if (sink == NULL) {
		/* Revert to the older cluttersink, in case autocluttersink was not found */
		sink = gst_element_factory_make("cluttersink", NULL);
	}
#else
	sink = gst_element_factory_make("cluttersink", NULL);
#endif
	if (sink == NULL) {
		g_printerr("Unable to find a Clutter sink.\n");
		return -1;
	}

	/* Link GStreamer with Clutter by passing the Clutter texture to the Clutter sink*/
	g_object_set(sink, "texture", texture, NULL);

	/* Add the Clutter sink to the pipeline */
	g_object_set(pipeline, "video-sink", sink, NULL);

	/* Start playing */
//	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* start the timeline */
	clutter_timeline_start(timeline);

	/* Add texture to the stage, and show it */
	ClutterColor stage_color = { 0, 0, 0, 50 };
	clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
	clutter_group_add(CLUTTER_GROUP(stage), texture);
	clutter_actor_show_all(stage);

	clutter_main();

	/* Free resources */
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}
