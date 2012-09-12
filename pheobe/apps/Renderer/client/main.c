/*
 * main.c
 *
 *  Created on: Sep 12, 2012
 *      Author: buttonfly
 */

#include "gdbus-Renderer-generated.h"
#include "def-renderer.h"
#include "stdio.h"

#include <termios.h>
#include <unistd.h>

/* ---------------------------------------------------------------------------------------------------- */


typedef struct _CustomData {
  gint n_video;          /* Number of embedded video streams */
  gint n_audio;          /* Number of embedded audio streams */
  gint n_text;           /* Number of embedded subtitle streams */

  gint current_video;    /* Currently playing video stream */
  gint current_audio;    /* Currently playing audio stream */
  gint current_text;     /* Currently playing subtitle stream */
  GDBusObjectManager *manager;
  GMainLoop *main_loop;  /* GLib's Main Loop */
} CustomData;


static gboolean handle_keyboard(GIOChannel *source, GIOCondition cond,
		CustomData *data);
static void on_interface_proxy_properties_changed(
		GDBusObjectManagerClient *manager, GDBusObjectProxy *object_proxy,
		GDBusProxy *interface_proxy, GVariant *changed_properties,
		const gchar * const *invalidated_properties, gpointer user_data);
static void on_notify_name_owner(GObject *object, GParamSpec *pspec,
		gpointer user_data);
static void on_object_removed(GDBusObjectManager *manager, GDBusObject *object,
		gpointer user_data);
static void on_object_added(GDBusObjectManager *manager, GDBusObject *object,
		gpointer user_data);
static void print_objects(GDBusObjectManager *manager);


gint main(gint argc, gchar *argv[]) {
	CustomData data;

	GDBusObjectManager *manager;
	GMainLoop *loop;
	GError *error;
	gchar *name_owner;

	manager = NULL;
	loop = NULL;

	GIOChannel *io_stdin;

	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);

	error = NULL;
	manager = pheobe_object_manager_client_new_for_bus_sync(G_BUS_TYPE_SESSION,
			G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
			GDBUS_RENDERER_INTERFACE_NAME, GDBUS_RENDERER_OBJECT_PATH, NULL, /* GCancellable */
			&error);
	if (manager == NULL) {
		g_printerr("Error getting object manager client: %s", error->message);
		g_error_free(error);
		goto out;
	}
	data.manager = manager;

	name_owner = g_dbus_object_manager_client_get_name_owner(
			G_DBUS_OBJECT_MANAGER_CLIENT(manager));
	g_print("name-owner: %s\n", name_owner);
	g_free(name_owner);

	print_objects(manager);

	g_signal_connect(manager, "notify::name-owner",
			G_CALLBACK(on_notify_name_owner), NULL);
	g_signal_connect(manager, "object-added", G_CALLBACK(on_object_added),
			NULL);
	g_signal_connect(manager, "object-removed", G_CALLBACK(on_object_removed),
			NULL);
	g_signal_connect(manager, "interface-proxy-properties-changed",
			G_CALLBACK(on_interface_proxy_properties_changed), NULL);

	struct termios oldtio;
	struct termios newtio;
	tcgetattr(fileno(stdin), &oldtio);
	bzero(&newtio, sizeof(newtio));
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN]  = 1;

	tcflush(fileno(stdin), TCIFLUSH);
	tcsetattr(fileno(stdin), TCSANOW, &newtio);

#ifdef _WIN32
	io_stdin = g_io_channel_win32_new_fd (fileno (stdin));
#else
	io_stdin = g_io_channel_unix_new(fileno(stdin));
#endif

	g_io_add_watch(io_stdin, G_IO_IN, (GIOFunc) handle_keyboard, &data);

	g_main_loop_run(loop);

	out: if (manager != NULL)
		g_object_unref(manager);
	if (loop != NULL)
		g_main_loop_unref(loop);

	return 0;
}

static gboolean handle_keyboard(GIOChannel *source, GIOCondition cond, CustomData *data) {
	gchar ch = NULL;
	GList *objects;
	GList *l;
	GDBusInterface *interface;
	g_print("Object manager at %s\n",
			g_dbus_object_manager_get_object_path(data->manager));
	objects = g_dbus_object_manager_get_objects(data->manager);
	for (l = objects; l != NULL; l = l->next) {
		PheobeObject *object = PHEOBE_OBJECT(l->data);
		GList *interfaces;
				GList *ll;
				g_print(" - Object at %s\n",
						g_dbus_object_get_object_path(G_DBUS_OBJECT(object)));

				interfaces = g_dbus_object_get_interfaces(G_DBUS_OBJECT(object));
				for (ll = interfaces; ll != NULL; ll = ll->next) {
					interface = G_DBUS_INTERFACE(ll->data);
					g_print("   - Interface %s\n",
							g_dbus_interface_get_info(interface)->name);
				}
	}

	static gboolean toggle  = FALSE;
	if(g_io_channel_read_chars(source, &ch, 1, NULL, NULL) == G_IO_STATUS_NORMAL) {
		int index = (int)ch;
		g_print("0x%x\n", index);
		if(toggle) {
			pheobe_renderer_call_resume(interface,  NULL, NULL, NULL);
		}
		else {
			pheobe_renderer_call_pause(interface,  NULL, NULL, NULL);
		}
		toggle = !toggle;
	}
//	g_free(str);
	return TRUE;
}

static void print_objects(GDBusObjectManager *manager) {
	GList *objects;
	GList *l;

	g_print("Object manager at %s\n",
			g_dbus_object_manager_get_object_path(manager));
	objects = g_dbus_object_manager_get_objects(manager);
	for (l = objects; l != NULL; l = l->next) {
		PheobeObject *object = PHEOBE_OBJECT(l->data);
		GList *interfaces;
		GList *ll;
		g_print(" - Object at %s\n",
				g_dbus_object_get_object_path(G_DBUS_OBJECT(object)));

		interfaces = g_dbus_object_get_interfaces(G_DBUS_OBJECT(object));
		for (ll = interfaces; ll != NULL; ll = ll->next) {
			GDBusInterface *interface = G_DBUS_INTERFACE(ll->data);
			g_print("   - Interface %s\n",
					g_dbus_interface_get_info(interface)->name);

			/* Note that @interface is really a GDBusProxy instance - and additionally also
			 * an ExampleAnimal or ExampleCat instance - either of these can be used to
			 * invoke methods on the remote object. For example, the generated function
			 *
			 *  void example_animal_call_poke_sync (ExampleAnimal  *proxy,
			 *                                      gboolean        make_sad,
			 *                                      gboolean        make_happy,
			 *                                      GCancellable   *cancellable,
			 *                                      GError        **error);
			 *
			 * can be used to call the Poke() D-Bus method on the .Animal interface.
			 * Additionally, the generated function
			 *
			 *  const gchar *example_animal_get_mood (ExampleAnimal *object);
			 *
			 * can be used to get the value of the :Mood property.
			 */
			pheobe_renderer_call_set_uri(interface,
					"file:///home/buttonfly/Videos/MV/1.avi", NULL, NULL, NULL);
//			pheobe_renderer_call_play(interface, 1, NULL, NULL, NULL);
//			pheobe_renderer_call_stop(interface,  NULL, NULL, NULL);
//			pheobe_renderer_call_pause(interface,  NULL, NULL, NULL);
//			pheobe_renderer_call_resume(interface,  NULL, NULL, NULL);

		}
		g_list_foreach(interfaces, (GFunc) g_object_unref, NULL);
		g_list_free(interfaces);
	}
	g_list_foreach(objects, (GFunc) g_object_unref, NULL);
	g_list_free(objects);
}

static void on_object_added(GDBusObjectManager *manager, GDBusObject *object,
		gpointer user_data) {
	gchar *owner;
	owner = g_dbus_object_manager_client_get_name_owner(
			G_DBUS_OBJECT_MANAGER_CLIENT(manager));
	g_print("Added object at %s (owner %s)\n",
			g_dbus_object_get_object_path(object), owner);
	g_free(owner);
}

static void on_object_removed(GDBusObjectManager *manager, GDBusObject *object,
		gpointer user_data) {
	gchar *owner;
	owner = g_dbus_object_manager_client_get_name_owner(
			G_DBUS_OBJECT_MANAGER_CLIENT(manager));
	g_print("Removed object at %s (owner %s)\n",
			g_dbus_object_get_object_path(object), owner);
	g_free(owner);
}

static void on_notify_name_owner(GObject *object, GParamSpec *pspec,
		gpointer user_data) {
	GDBusObjectManagerClient *manager = G_DBUS_OBJECT_MANAGER_CLIENT(object);
	gchar *name_owner;

	name_owner = g_dbus_object_manager_client_get_name_owner(manager);
	g_print("name-owner: %s\n", name_owner);
	g_free(name_owner);
}

static void on_interface_proxy_properties_changed(
		GDBusObjectManagerClient *manager, GDBusObjectProxy *object_proxy,
		GDBusProxy *interface_proxy, GVariant *changed_properties,
		const gchar * const *invalidated_properties, gpointer user_data) {
	GVariantIter iter;
	const gchar *key;
	GVariant *value;
	gchar *s;

	g_print("Properties Changed on %s:\n",
			g_dbus_object_get_object_path(G_DBUS_OBJECT(object_proxy)));
	g_variant_iter_init(&iter, changed_properties);
	while (g_variant_iter_next(&iter, "{&sv}", &key, &value)) {
		s = g_variant_print(value, TRUE);
		g_print("  %s -> %s\n", key, s);
		g_variant_unref(value);
		g_free(s);
	}
}
