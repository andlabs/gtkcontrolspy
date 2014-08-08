// 8 august 2014
#include "gtkcontrolspy.h"

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

void dumpWidget(gpointer key, gpointer val, gpointer data)
{
	Widget *w = (Widget *) val;
	gint i;

	printf("%s : %s\n", w->Name, w->Derived);
	for (i = 0; i < w->nProperties; i++)
		if (w->Properties[i].Valid) {
			printf("\t%s %s", w->Properties[i].Name,
				g_type_tag_to_string(w->Properties[i].TypeTag));
			if (w->Properties[i].TypeTag == GI_TYPE_TAG_INTERFACE)
				printf("(%s %d)",
					w->Properties[i].TypeName,
					w->Properties[i].TypeType);
			printf("\n");
		}
}

int main(void)
{
	char *err;

	gtk_init(NULL, NULL);

	GtkWidget *wait;

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwin), "GTK+ Control Spy");
	gtk_window_resize(GTK_WINDOW(mainwin), 1024, 768);
	wait = gtk_spinner_new();
	gtk_container_add(GTK_CONTAINER(mainwin), wait);
	gtk_spinner_start(GTK_SPINNER(wait));
	gtk_widget_show_all(mainwin);

	err = collectWidgets(REPO, NULL);
	if (err != NULL)
		panic("error gathering widgets: %s", err);
	g_hash_table_foreach(widgets, dumpWidget, NULL);

//	gtk_main();
	return 0;
}
