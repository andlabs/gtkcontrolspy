// 9 august 2014
#include "gtkcontrolspy.h"

GtkWidget *mainwin;

struct MainWindow {
	GtkWidget *window;
	GtkWidget *layout;
	GtkWidget *widgetList;
	GtkListStore *widgetListStore;
	GtkWidget *widgetScroller;
	GtkWidget *canvas;
	GtkWidget *properties;
	GtkWidget *current;
};

// TODO is adjusting val legal here?
static void prepareWidgetUIStuff(gpointer val, gpointer data)
{
	Widget *w = (Widget *) val;
	MainWindow *m = (MainWindow *) data;
	gint i;
	GtkTreeViewColumn *col;
	GtkTreeIter iter;

	printf("%s : %s\n", w->Name, w->Derived);
	// don't add things like GtkWidget itself or GtkBin or other abstract types to the list of widgets the user can choose from
	if (w->Instantiable) {
		gtk_list_store_append(m->widgetListStore, &iter);
		gtk_list_store_set(m->widgetListStore, &iter,
			0, w->Name,
			1, w,
			-1);
	}
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
	gtk_notebook_append_page(GTK_NOTEBOOK(m->properties),
		w->ViewScroller,
		gtk_label_new(w->Name));
}

static void changeWidget(GtkTreeSelection *sel, gpointer data)
{
	MainWindow *m = (MainWindow *) data;
	GtkTreeIter iter;
	Widget *w;

	if (gtk_tree_selection_get_selected(sel, NULL, &iter) == FALSE)
		return;		// no selection
	// TODO before the above?
	if (m->current != NULL)
		gtk_container_remove(GTK_CONTAINER(m->canvas), m->current);
	gtk_tree_model_get(GTK_TREE_MODEL(m->widgetListStore), &iter, 1, &w, -1);
	m->current = GTK_WIDGET(g_object_new(w->GType, NULL));
	gtk_container_add(GTK_CONTAINER(m->canvas), m->current);
	gtk_widget_show_all(m->current);
}

MainWindow *newMainWindow(void)
{
	MainWindow *m;
	GtkTreeViewColumn *col;

	m = g_new0(MainWindow, 1);

	m->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(m->window), "GTK+ Control Spy");
	gtk_window_resize(GTK_WINDOW(m->window), 1024, 768);
	g_signal_connect(m->window, "destroy", gtk_main_quit, NULL);		// TODO safe?

	m->layout = gtk_grid_new();
	m->widgetListStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	m->widgetList = gtk_tree_view_new_with_model(GTK_TREE_MODEL(m->widgetListStore));
	col = gtk_tree_view_column_new_with_attributes("", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(m->widgetList), col);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m->widgetList), FALSE);
	g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(m->widgetList)), "changed", G_CALLBACK(changeWidget), m);
	m->widgetScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(m->widgetScroller), GTK_SHADOW_IN);
	// don't allow horizontal scrolling
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m->widgetScroller), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(m->widgetScroller), m->widgetList);
	m->canvas = gtk_layout_new(NULL, NULL);
	m->properties = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(m->properties), TRUE);

	gtk_widget_set_vexpand(m->widgetScroller, TRUE);
	gtk_widget_set_valign(m->widgetScroller, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(m->layout),
		m->widgetScroller, NULL,
		GTK_POS_TOP, 1, 2);
	gtk_widget_set_hexpand(m->canvas, TRUE);
	gtk_widget_set_halign(m->canvas, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(m->canvas, TRUE);
	gtk_widget_set_valign(m->canvas, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(m->layout),
		m->canvas, m->widgetScroller,
		GTK_POS_RIGHT, 1, 1);
	gtk_widget_set_hexpand(m->properties, TRUE);
	gtk_widget_set_halign(m->properties, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(m->properties, TRUE);
	gtk_widget_set_valign(m->properties, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(m->layout),
		m->properties, m->canvas,
		GTK_POS_BOTTOM, 1, 1);

	g_ptr_array_foreach(widgets, prepareWidgetUIStuff, m);

	gtk_container_add(GTK_CONTAINER(m->window), m->layout);
	gtk_widget_show_all(m->window);

	return m;
}
