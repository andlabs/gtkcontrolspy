// 8 august 2014
#include <gtk/gtk.h>
#include <girepository.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *mainwin;

void panic(char *fmt, ...)
{
	char *msg;
	GtkWidget *alert;
	va_list ap;

	va_start(ap, fmt);
	msg = g_strdup_vprintf(fmt, ap);
	va_end(ap);
	alert = gtk_message_dialog_new(GTK_WINDOW(mainwin), GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"%s", msg);
	gtk_widget_show_all(gtk_message_dialog_get_image(GTK_MESSAGE_DIALOG(alert)));
	gtk_dialog_run(GTK_DIALOG(alert));
	exit(EXIT_FAILURE);
}

#define REPO "Gtk"

int main(void)
{
	GError *err = NULL;

	gtk_init(NULL, NULL);

	GtkWidget *wait;

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwin), "GTK+ Control Spy");
	gtk_window_resize(GTK_WINDOW(mainwin), 1024, 768);
	wait = gtk_spinner_new();
	gtk_container_add(GTK_CONTAINER(mainwin), wait);
	gtk_spinner_start(GTK_SPINNER(wait));
	gtk_widget_show_all(mainwin);

	gint i, n;

	if (g_irepository_require(NULL, REPO, NULL, 0, &err) == NULL)
		panic("error loading GTK+ introspection repository: %s", err->message);
	n = g_irepository_get_n_infos(NULL, REPO);
	for (i = 0; i < n; i++) {
		GIBaseInfo *info;
		GIRegisteredTypeInfo *reg;
		GIObjectInfo *obj;
		GType gtype;
		gint ip, np;

		info = g_irepository_get_info(NULL, REPO, i);
		if (g_base_info_get_type(info) != GI_INFO_TYPE_OBJECT)
			continue;
		reg = (GIRegisteredTypeInfo *) info;
		obj = (GIObjectInfo *) info;
		gtype = g_registered_type_info_get_g_type(reg);
		if (!g_type_is_a(gtype, GTK_TYPE_WIDGET))
			continue;
		printf("%s\n", g_object_info_get_type_name(obj));
		np = g_object_info_get_n_properties(obj);
		for (ip = 0; ip < np; ip++) {
			GIPropertyInfo *prop;
			GIBaseInfo *propbase;
			GITypeInfo *type;
			GITypeTag typetag;

			prop = g_object_info_get_property(obj, ip);
			propbase = (GIBaseInfo *) prop;
			type = g_property_info_get_type(prop);
			typetag = g_type_info_get_tag(type);
			printf("\t%s %s", g_base_info_get_name(propbase), g_type_tag_to_string(typetag));
			if (typetag == GI_TYPE_TAG_INTERFACE)
				printf("(%s)", g_base_info_get_name(g_type_info_get_interface(type)));
			printf("\n");
		}
	}

//	gtk_main();
	return 0;
}
