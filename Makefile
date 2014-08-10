CFILES = main.c introspect.c mainwin.c bindings.c pixbufeditor.c
HFILES = gtkcontrolspy.h

ALLFILES = $(CFILES) $(HFILES)

LIBS = \
	`pkg-config --cflags --libs gtk+-3.0 gobject-introspection-1.0` \

# -Wno-switch because why the hell
all:
	gcc -g -o gtkcontrolspy $(CFILES) -Wall -Wextra -pedantic -Wno-switch --std=c99 $(LIBS)
