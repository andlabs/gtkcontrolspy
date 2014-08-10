// 10 august 2014
#include "gtkcontrolspy.h"

G_DEFINE_TYPE(PixbufEditor, pixbufEditor, GTK_TYPE_FILE_CHOOSER_BUTTON)

// TODO derive?
static void pixbufEditor_file_set(GtkFileChooserButton *fcb, gpointer data)
{
	PixbufEditor *e = PIXBUF_EDITOR(data);
	GdkPixbuf *pixbuf;
	GError *err = NULL;

	pixbuf = gdk_pixbuf_new_from_file(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcb)), &err);
	if (pixbuf == NULL) {
		GtkWidget *alert;

		alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
			"Error loading pixbuf: %s", err->message);
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
			"The pixbuf has not been changed.");
		gtk_widget_show_all(gtk_message_dialog_get_image(GTK_MESSAGE_DIALOG(alert)));
		gtk_dialog_run(GTK_DIALOG(alert));
		gtk_widget_destroy(alert);
		return;
	}
	g_object_set(e, PIXBUF_EDITOR_PROPERTY, pixbuf, NULL);
}

static void pixbufEditor_init(PixbufEditor *e)
{
	GtkFileFilter *filter;

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pixbuf_formats(filter);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(e), filter);
	g_signal_connect(e, "file-set", G_CALLBACK(pixbufEditor_file_set), e);
	if (e->pixbuf == NULL)
		e->pixbuf = PIXBUF_EDITOR_GET_CLASS(e)->missing;
}

static void pixbufEditor_dispose(GObject *obj)
{
	if (PIXBUF_EDITOR(obj)->pixbuf != PIXBUF_EDITOR_GET_CLASS(PIXBUF_EDITOR(obj))->missing)
		g_object_unref(PIXBUF_EDITOR(obj)->pixbuf);
	G_OBJECT_CLASS(pixbufEditor_parent_class)->dispose(obj);
}

static void pixbufEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(pixbufEditor_parent_class)->finalize(obj);
}

static GParamSpec *pixbufEditorProperties[2];

static void pixbufEditor_setProperty(GObject *obj, guint id, const GValue *value, GParamSpec *pspec)
{
	PixbufEditor *e = (PixbufEditor *) obj;

	if (id != 1) {
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, pspec);
		return;
	}
	if (e->pixbuf != PIXBUF_EDITOR_GET_CLASS(e)->missing)
		g_object_unref(e->pixbuf);
	e->pixbuf = (GdkPixbuf *) g_value_get_object(value);
}

static void pixbufEditor_getProperty(GObject *obj, guint id, GValue *value, GParamSpec *pspec)
{
	PixbufEditor *e = (PixbufEditor *) obj;

	if (id != 1) {
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, pspec);
		return;
	}
	g_value_set_object(value, e->pixbuf);
}

static void pixbufEditor_class_init(PixbufEditorClass *class)
{
	GError *err = NULL;

	G_OBJECT_CLASS(class)->dispose = pixbufEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = pixbufEditor_finalize;
	G_OBJECT_CLASS(class)->set_property = pixbufEditor_setProperty;
	G_OBJECT_CLASS(class)->get_property = pixbufEditor_getProperty;

	class->missing = gtk_icon_theme_load_icon(
		gtk_icon_theme_get_default(),
		"image-missing",
		GTK_ICON_SIZE_SMALL_TOOLBAR,
		GTK_ICON_LOOKUP_USE_BUILTIN | GTK_ICON_LOOKUP_GENERIC_FALLBACK,
		&err);
	if (class->missing == NULL) {
		// TODO
	}

	pixbufEditorProperties[1] = g_param_spec_object(
		PIXBUF_EDITOR_PROPERTY,
		PIXBUF_EDITOR_PROPERTY,
		"Current pixbuf.",
		GDK_TYPE_PIXBUF,
		G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(G_OBJECT_CLASS(class), 2, pixbufEditorProperties);
}

GtkWidget *newPixbufEditor(void)
{
	return (GtkWidget *) g_object_new(PIXBUF_EDITOR_TYPE, NULL);
}
