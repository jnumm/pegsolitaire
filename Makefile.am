AM_CPPFLAGS = -DLOCALEDIR='"$(localedir)"' -DPKGDATADIR='"$(pkgdatadir)"'
AM_CFLAGS = -Wall -Wextra -pedantic
AM_LDFLAGS = -Wl,--as-needed

bin_PROGRAMS = pegsolitaire
# $(AM_CPPFLAGS) need to be explicitly added here.
pegsolitaire_CPPFLAGS = $(AM_CPPFLAGS) $(GTK_CFLAGS)
pegsolitaire_LDADD = $(GTK_LIBS)
pegsolitaire_SOURCES = \
  src/callbacks.c \
  src/callbacks.h \
  src/game.c \
  src/game.h \
  src/gridframe.c \
  src/gridframe.h \
  src/i18n.h \
  src/main.c \
  src/preimage.c \
  src/preimage.h

# Files in dist_something_DATA will be installed to $(somethingdir).
dist_pkgdata_DATA = src/hole.svg src/peg.svg src/pegsolitaire.glade

dist_doc_DATA = README COPYING AUTHORS ChangeLog NEWS

pixmapsdir = $(datarootdir)/pixmaps
dist_pixmaps_DATA = pegsolitaire.png

helpdir = $(datarootdir)/gnome/help/pegsolitaire/C
dist_help_DATA = help/legal.xml help/pegsolitaire.xml

EXTRA_DIST = pegsolitaire.desktop.in
CLEANFILES = pegsolitaire.desktop
desktopdir = $(datarootdir)/applications
desktop_DATA = pegsolitaire.desktop
pegsolitaire.desktop: pegsolitaire.desktop.in
	$(MSGFMT) -c --statistics --verbose --desktop \
	--template $< -d $(top_srcdir)/po -o $@

SUBDIRS = po
