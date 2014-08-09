CFILES = main.c introspect.c mainwin.c
HFILES = gtkcontrolspy.h

ALLFILES = $(CFILES) $(HFILES)

LIBS = \
	`pkg-config --cflags --libs gtk+-3.0 gobject-introspection-1.0` \

all:
	gcc -g -o gtkcontrolspy $(CFILES) -Wall -Wextra -pedantic --std=c99 $(LIBS)
