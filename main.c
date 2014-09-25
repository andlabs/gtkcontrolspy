// 8 august 2014
#include "gtkcontrolspy.h"

// #qo pkg-config: gtk+-3.0 gobject-introspection-1.0

void panic(char *fmt, ...)
{
	char *msg;
	GtkWidget *alert;
	va_list ap;

	va_start(ap, fmt);
	msg = g_strdup_vprintf(fmt, ap);
	va_end(ap);
	alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"%s", msg);
	gtk_widget_show_all(gtk_message_dialog_get_image(GTK_MESSAGE_DIALOG(alert)));
	gtk_dialog_run(GTK_DIALOG(alert));
	exit(EXIT_FAILURE);
}

#define REPO "Gtk"

int main(void)
{
	char *err;

	gtk_init(NULL, NULL);

	err = collectWidgets(REPO, NULL);
	if (err != NULL)
		panic("error gathering widgets: %s", err);

	newMainWindow();

	gtk_main();
	return 0;
}
