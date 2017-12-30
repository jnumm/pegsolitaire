## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/pegsolitaire/glade
glade_DATA = pegsolitaire.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PEGSOLITAIRE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = pegsolitaire

pegsolitaire_SOURCES = \
    interface.c \
    interface.h \
    support.c \
    support.h \
	callbacks.c \
	callbacks.h \
    preimage.c \
    preimage.h \
    game.c \
    game.h \
    gridframe.c \
    gridframe.h \
	main.c

pegsolitaire_LDFLAGS = 

pegsolitaire_LDADD = $(PEGSOLITAIRE_LIBS)

EXTRA_DIST = $(glade_DATA) glade-strings.c

SUBDIRS = \
	pixmaps

##
## intltool-extract --type="gettext/glade" pegsolitaire.glade
## mv pegsolitaire.glade.h glade-strings.c
##

