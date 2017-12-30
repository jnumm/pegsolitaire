gladedir = $(pkgdatadir)/glade
dist_glade_DATA = pegsolitaire.glade

AM_CPPFLAGS = \
  -DLOCALEDIR=\"$(localedir)\" \
  -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall -Wextra -pedantic
AM_LDFLAGS = -Wl,--as-needed

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

pegsolitaire_CPPFLAGS = $(AM_CPPFLAGS) $(GTK_CFLAGS)
pegsolitaire_LDADD = $(GTK_LIBS)

SUBDIRS = pixmaps
