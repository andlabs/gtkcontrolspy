// 9 august 2014
#include "gtkcontrolspy.h"

GtkWidget *mainwin;

struct MainWindow {
	GtkWidget *window;
	GtkWidget *layout;
	GtkWidget *widgetList;
	GtkListStore *widgetListStore;
	GtkTreeModel *model;
	GtkWidget *widgetScroller;
	GtkWidget *canvas;
	GtkWidget *properties;
	GtkWidget *current;
};

static void prepareWidgetUIStuff(gpointer key, gpointer val, gpointer data)
{
	Widget *w = (Widget *) val;
	MainWindow *m = (MainWindow *) data;
	gint i;

	printf("%s : %s\n", w->Name, w->Derived);

	// don't add things like GtkWidget itself or GtkBin or other abstract types to the list of widgets the user can choose from
	if (w->Instantiable) {
		GtkTreeIter iter;

		gtk_list_store_append(m->widgetListStore, &iter);
		gtk_list_store_set(m->widgetListStore, &iter,
			0, w->Name,
			1, w,
			-1);
	}

	w->Grid = gtk_grid_new();
	w->Values = g_new0(GtkWidget *, w->nProperties);
	for (i = 0; i < w->nProperties; i++)
		if (w->Properties[i].Valid) {
			GtkWidget *label;

			label = gtk_label_new(w->Properties[i].Name);
			gtk_grid_attach_next_to(GTK_GRID(w->Grid),
				label, NULL,
				GTK_POS_BOTTOM, 1, 1);
			w->Values[i] = gtk_entry_new();
			gtk_grid_attach_next_to(GTK_GRID(w->Grid),
				w->Values[i], label,
				GTK_POS_RIGHT, 1, 1);
		}

	w->GridScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(w->GridScroller), GTK_SHADOW_IN);
	// TODO 3.8 and lower
	gtk_container_add(GTK_CONTAINER(w->GridScroller), w->Grid);
	// keep a ref so we can add/remove tabs
	g_object_ref_sink(w->GridScroller);
}

static void changeWidget(GtkTreeSelection *sel, gpointer data)
{
	MainWindow *m = (MainWindow *) data;
	GtkTreeIter iter;
	Widget *w;
	gint i, n;

	if (gtk_tree_selection_get_selected(sel, NULL, &iter) == FALSE)
		return;		// no selection
	// TODO before the above?
	if (m->current != NULL)
		gtk_container_remove(GTK_CONTAINER(m->canvas), m->current);
	n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(m->properties));
	for (i = 0; i < n; i++)			// repeatedly delete the (new) first page to clear them all
		gtk_notebook_remove_page(GTK_NOTEBOOK(m->properties), 0);
	gtk_tree_model_get(GTK_TREE_MODEL(m->model), &iter, 1, &w, -1);
	m->current = GTK_WIDGET(g_object_new(w->GType, NULL));
	gtk_container_add(GTK_CONTAINER(m->canvas), m->current);
	gtk_widget_show_all(m->current);
	while (w != NULL) {
		// add the tab
		// show everything because GtkNotebook is an example of "buggy for backwards-compatibilty purposes" and requires that a child widget be visible before a tab can be switched to (see the documentation of gtk_notebook_set_current_page())
		gtk_widget_show_all(w->GridScroller);
		gtk_notebook_append_page(GTK_NOTEBOOK(m->properties),
			w->GridScroller,
			gtk_label_new(w->Name));

		// bind the properties
		for (i = 0; i < w->nProperties; i++)
			if (w->Properties[i].Valid)
				g_object_bind_property(
					m->current, w->Properties[i].Name,
					w->Values[i], "text",
					G_BINDING_BIDIRECTIONAL);

		// next parent
		w = (Widget *) g_hash_table_lookup(widgets, w->Derived);
	}
	gtk_widget_show_all(m->properties);		// refresh
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
	m->model = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(m->widgetListStore));
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(m->model), 0, GTK_SORT_ASCENDING);
	m->widgetList = gtk_tree_view_new_with_model(m->model);
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

	g_hash_table_foreach(widgets, prepareWidgetUIStuff, m);

	gtk_container_add(GTK_CONTAINER(m->window), m->layout);
	gtk_widget_show_all(m->window);

	return m;
}
