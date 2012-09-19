/*
 * main.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: buttonfly
 */


#include <clutter/clutter.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include "GstPlaybin.h"
#include "ClutterUI.h"

#include "Platform.h"
#include "ServiceManager.h"
#include "PlaybackService.h"
#include "UserInterfaceService.h"

using namespace halkamalka;

typedef struct {
	ClutterActor *stage;
	ClutterActor *videoTexture;
} UserData;

//static gboolean keyPressed (ClutterStage *stage, ClutterEvent *event, gpointer data) ;
gboolean busCall (GstBus * bus, GstMessage * msg, gpointer data);

int main(int argc, char** argv) {

#if 0
	UserData context;

	if (!g_thread_supported ())
		g_thread_init (NULL);

	clutter_gst_init (&argc, &argv);

	context.stage = clutter_stage_get_default();
	clutter_actor_set_size(context.stage, (gfloat)960, (gfloat)540);
#endif

	Platform& platform  = Platform::getInstance();
	platform.init(&argc, &argv);

	IService* playbackService = new PlaybackService;
	ServiceManager::GetInstance().Invoke(playbackService);

	IService* uiService = new UserInterfaceService;
	ServiceManager::GetInstance().Invoke(uiService);


#if 0
	ClutterActor* videoTexture = clutter_texture_new ();
	GstPlaybin::getInstance().init(videoTexture);
	clutter_container_add_actor (CLUTTER_CONTAINER (context.stage), videoTexture);


#endif
//	ClutterUI::getInstance().init(CLUTTER_STAGE(context.stage));

//	clutter_actor_show(context.stage);
//	g_signal_connect (context.stage, "key-press-event", G_CALLBACK (keyPressed), &context);

//	clutter_main();
	platform.start();

	return EXIT_SUCCESS;
}

