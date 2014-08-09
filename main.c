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

// TODO is adjusting val legal here?
void dumpWidget(gpointer val, gpointer data)
{
	Widget *w = (Widget *) val;
	GtkNotebook *properties = (GtkNotebook *) data;
	gint i;
	GtkTreeViewColumn *col;

	printf("%s : %s\n", w->Name, w->Derived);
	w->Model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	for (i = 0; i < w->nProperties; i++)
		if (w->Properties[i].Valid) {
			GtkTreeIter iter;

			gtk_list_store_append(w->Model, &iter);
			gtk_list_store_set(w->Model, &iter, 0, w->Properties[i].Name, -1);
			printf("\t%s ", w->Properties[i].Name);
			if (w->Properties[i].Pointer)
				printf("*");
			printf("%s", g_type_tag_to_string(w->Properties[i].TypeTag));
			if (w->Properties[i].TypeTag == GI_TYPE_TAG_INTERFACE)
				printf("(%s %d)",
					w->Properties[i].TypeName,
					w->Properties[i].TypeType);
			printf("\n");
		}
	w->View = gtk_tree_view_new_with_model(GTK_TREE_MODEL(w->Model));
	col = gtk_tree_view_column_new_with_attributes("Property", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(w->View), col);
	col = gtk_tree_view_column_new_with_attributes("Value", gtk_cell_renderer_text_new(), "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(w->View), col);
	w->ViewScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(w->ViewScroller), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(w->ViewScroller), w->View);
	// keep a ref so we can add/remove tabs
	g_object_ref_sink(w->ViewScroller);
	gtk_notebook_append_page(properties,
		w->ViewScroller,
		gtk_label_new(w->Name));
}

int main(void)
{
	char *err;

	gtk_init(NULL, NULL);

	err = collectWidgets(REPO, NULL);
	if (err != NULL)
		panic("error gathering widgets: %s", err);

	GtkWidget *layout;
	GtkWidget *widgetList, *widgetScroller;
	GtkWidget *canvas;
	GtkWidget *properties;

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwin), "GTK+ Control Spy");
	gtk_window_resize(GTK_WINDOW(mainwin), 1024, 768);

	layout = gtk_grid_new();
	widgetList = gtk_tree_view_new();
	widgetScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(widgetScroller), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(widgetScroller), widgetList);
	canvas = gtk_layout_new(NULL, NULL);
	properties = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(properties), TRUE);

	gtk_widget_set_vexpand(widgetScroller, TRUE);
	gtk_widget_set_valign(widgetScroller, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(layout),
		widgetScroller, NULL,
		GTK_POS_TOP, 1, 2);
	gtk_widget_set_hexpand(canvas, TRUE);
	gtk_widget_set_halign(canvas, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(canvas, TRUE);
	gtk_widget_set_valign(canvas, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(layout),
		canvas, widgetScroller,
		GTK_POS_RIGHT, 1, 1);
	gtk_widget_set_hexpand(properties, TRUE);
	gtk_widget_set_halign(properties, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(properties, TRUE);
	gtk_widget_set_valign(properties, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(layout),
		properties, canvas,
		GTK_POS_BOTTOM, 1, 1);

	g_ptr_array_foreach(widgets, dumpWidget, properties);

	gtk_container_add(GTK_CONTAINER(mainwin), layout);
	gtk_widget_show_all(mainwin);

	gtk_main();
	return 0;
}
