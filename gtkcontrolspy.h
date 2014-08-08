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
};

struct Property {
	gboolean Valid;
	char *Name;
	GITypeTag TypeTag;
	char *InterfaceType;
};

extern GHashTable *widgets;

extern char *collectWidgets(char *, char *);
