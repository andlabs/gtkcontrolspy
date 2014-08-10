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

// pixbufeditor.c
#define PIXBUF_EDITOR_TYPE (pixbufEditor_get_type())
#define PIXBUF_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PIXBUF_EDITOR_TYPE, PixbufEditor))
#define IS_PIXBUF_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PIXBUF_EDITOR_TYPE))
#define PIXBUF_EDITOR_CLASS(class) (G_TYPE_CHECK_CLASS_CAST((class), PIXBUF_EDITOR_TYPE, PixbufEditorClass))
#define IS_PIXBUF_EDITOR_CLASS(class) (G_TYPE_CHECK_CLASS_TYPE((class), PIXBUF_EDITOR_TYPE))
#define PIXBUF_EDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PIXBUF_EDITOR_TYPE, PixbufEditorClass))

typedef struct PixbufEditor PixbufEditor;
typedef struct PixbufEditorClass PixbufEditorClass;

struct PixbufEditor {
	GtkFileChooserButton parent_instance;
	GdkPixbuf *pixbuf;
};

struct PixbufEditorClass {
	GtkFileChooserButtonClass parent_class;
	GdkPixbuf *missing;
};

extern GType pixbufEditor_get_type(void);

#define PIXBUF_EDITOR_PROPERTY "current-pixbuf"

extern GtkWidget *newPixbufEditor(void);
