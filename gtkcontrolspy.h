// 8 august 2014
#include <gtk/gtk.h>
#include <girepository.h>
#include <stdarg.h>
#include <stdlib.h>

// introspect.c
typedef struct Widget Widget;
typedef struct Property Property;

struct Widget {
	char *Name;
	char *Derived;
	gint nProperties;
	Property *Properties;
	GType GType;
	gboolean Instantiable;
	GtkWidget *Grid;
	GtkWidget *GridScroller;
};

struct Property {
	gboolean Valid;
	char *Name;
	GtkWidget *Editor;
	char *BindTo;
};

extern GHashTable *widgets;

extern char *collectWidgets(char *, char *);

// mainwin.c
typedef struct MainWindow MainWindow;

extern MainWindow *newMainWindow(void);

// bindings.c
extern gboolean getBinding(GIPropertyInfo *, GtkWidget **, char **);
