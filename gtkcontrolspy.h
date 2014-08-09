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
	GtkWidget *View;
	GtkWidget *ViewScroller;
	GtkListStore *Model;
	gboolean Instantiable;
};

struct Property {
	gboolean Valid;
	char *Name;
	GITypeTag TypeTag;
	gboolean Pointer;
	// if TypeTag == GI_TYPE_TAG_INTERFACE
	char *TypeName;
	GIInfoType TypeType;
};

extern GPtrArray *widgets;

extern char *collectWidgets(char *, char *);

// mainwin.c
typedef struct MainWindow MainWindow;

extern MainWindow *newMainWindow(void);
