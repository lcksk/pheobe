#include "gdbus-sampleplayer-generated.h"
#include "def.h"

static GDBusObjectManagerServer* manager = NULL;

static void on_bus_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {
	sampleplayerSampleplayerSkeleton *object;
	guint n;

	g_print("Acquired a message bus connection\n");

	/* Create a new org.freedesktop.DBus.ObjectManager rooted at /example/Animals */
	manager = g_dbus_object_manager_server_new(
			GDBUS_OBJECT_PATH);

//  for (n = 0; n < 10; n++)
//    {
//      gchar *s;
//      ExampleAnimal *animal;
//
//      /* Create a new D-Bus object at the path /example/Animals/N where N is 000..009 */
//      s = g_strdup_printf ("/example/Animals/%03d", n);
//      object = example_object_skeleton_new (s);
//      g_free (s);
//
//      /* Make the newly created object export the interface
//       * org.gtk.GDBus.Example.ObjectManager.Animal (note
//       * that @object takes its own reference to @animal).
//       */
//      animal = example_animal_skeleton_new ();
//      example_animal_set_mood (animal, "Happy");
//      example_object_skeleton_set_animal (object, animal);
//      g_object_unref (animal);
//
//      /* Cats are odd animals - so some of our objects implement the
//       * org.gtk.GDBus.Example.ObjectManager.Cat interface in addition
//       * to the .Animal interface
//       */
//      if (n % 2 == 1)
//        {
//          ExampleCat *cat;
//          cat = example_cat_skeleton_new ();
//          example_object_skeleton_set_cat (object, cat);
//          g_object_unref (cat);
//        }
//
//      /* Handle Poke() D-Bus method invocations on the .Animal interface */
//      g_signal_connect (animal,
//                        "handle-poke",
//                        G_CALLBACK (on_animal_poke),
//                        NULL); /* user_data */
//
//      /* Export the object (@manager takes its own reference to @object) */
//      g_dbus_object_manager_server_export (manager, G_DBUS_OBJECT_SKELETON (object));
//      g_object_unref (object);
//    }

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

gint main(gint argc, gchar* argv[]) {

	GMainLoop* loop;
	guint id;

	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);
	id = g_bus_own_name(G_BUS_TYPE_SESSION,
			GDBUS_NAME,
			G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT
					| G_BUS_NAME_OWNER_FLAGS_REPLACE, on_bus_acquired,
			on_name_acquired, on_name_lost, loop, NULL);

	g_main_loop_run(loop);
	g_bus_unown_name(id);
	g_main_loop_unref(loop);

	return 0;
}
