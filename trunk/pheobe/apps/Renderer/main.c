/*
 * main.c
 *
 *  Created on: Sep 12, 2012
 *      Author: buttonfly
 */

#include "gdbus-Renderer-generated.h"
#include "def-renderer.h"

/* ---------------------------------------------------------------------------------------------------- */

static GDBusObjectManagerServer *manager = NULL;

static gboolean on_renderer_poke(PheobeRenderer *renderer,
		GDBusMethodInvocation *invocation, gboolean make_sad,
		gboolean make_happy, gpointer user_data) {
	if ((make_sad && make_happy) || (!make_sad && !make_happy)) {
		g_dbus_method_invocation_return_dbus_error(invocation,
				GDBUS_RENDERER_INTERFACE_NAME".Error.Failed",
				"Exactly one of make_sad or make_happy must be TRUE");
		goto out;
	}

	if (make_sad) {
		if (g_strcmp0(pheobe_renderer_get_mood(renderer), "Sad") == 0) {
			g_dbus_method_invocation_return_dbus_error(invocation,
					GDBUS_RENDERER_INTERFACE_NAME".Error.SadAnimalIsSad",
					"Sad animal is already sad");
			goto out;
		}

		pheobe_renderer_set_mood(renderer, "Sad");
		pheobe_renderer_complete_poke(renderer, invocation);
		goto out;
	}

	if (make_happy) {
		if (g_strcmp0(pheobe_renderer_get_mood(renderer), "Happy") == 0) {
			g_dbus_method_invocation_return_dbus_error(invocation,
					GDBUS_RENDERER_INTERFACE_NAME".Error.HappyAnimalIsHappy",
					"Happy animal is already happy");
			goto out;
		}

		pheobe_renderer_set_mood(renderer, "Happy");
		pheobe_renderer_complete_poke(renderer, invocation);
		goto out;
	}

	g_assert_not_reached();

	out: return TRUE; /* to indicate that the method was handled */
}

static void on_bus_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {
	PheobeObjectSkeleton *object;
	guint n;

	g_print("Acquired a message bus connection\n");

	/* Create a new org.freedesktop.DBus.ObjectManager rooted at /example/renderers */
	manager = g_dbus_object_manager_server_new(GDBUS_RENDERER_OBJECT_PATH);

	for (n = 0; n < 10; n++) {
		gchar *s;
		PheobeRenderer *renderer;

		/* Create a new D-Bus object at the path /example/renderers/N where N is 000..009 */
		s = g_strdup_printf(GDBUS_RENDERER_OBJECT_PATH"/%03d", n);
		object = pheobe_object_skeleton_new(s);
		g_free(s);

		/* Make the newly created object export the interface
		 * org.gtk.GDBus.Example.ObjectManager.renderer (note
		 * that @object takes its own reference to @animal).
		 */
		renderer = pheobe_renderer_skeleton_new();
		pheobe_renderer_set_mood(renderer, "Happy");
		pheobe_object_skeleton_set_renderer(object, renderer);
		g_object_unref(renderer);

		/* Cats are odd animals - so some of our objects implement the
		 * org.gtk.GDBus.Example.ObjectManager.Cat interface in addition
		 * to the .Animal interface
		 */
		if (n % 2 == 1) {
			PheobeCat *cat;
			cat = pheobe_cat_skeleton_new();
			pheobe_object_skeleton_set_cat(object, cat);
			g_object_unref(cat);
		}

		/* Handle Poke() D-Bus method invocations on the .renderer interface */
		g_signal_connect(renderer, "handle-poke", G_CALLBACK(on_renderer_poke),
				NULL); /* user_data */

		/* Export the object (@manager takes its own reference to @object) */
		g_dbus_object_manager_server_export(manager,
				G_DBUS_OBJECT_SKELETON(object));
		g_object_unref(object);
	}

	/* Export all objects */
	g_dbus_object_manager_server_set_connection(manager, connection);
}

static void on_name_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {
	g_print("Acquired the name %s\n", name);
}

static void on_name_lost(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {
	g_print("Lost the name %s\n", name);
}

gint main(gint argc, gchar *argv[]) {
	GMainLoop *loop;
	guint id;

	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);
	id = g_bus_own_name(G_BUS_TYPE_SESSION, GDBUS_RENDERER_INTERFACE_NAME,
			G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT
					| G_BUS_NAME_OWNER_FLAGS_REPLACE, on_bus_acquired,
			on_name_acquired, on_name_lost, loop, NULL);

	g_main_loop_run(loop);

	g_bus_unown_name(id);
	g_main_loop_unref(loop);

	return 0;
}
