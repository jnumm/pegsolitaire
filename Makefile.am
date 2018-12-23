AM_CPPFLAGS = -DPACKAGE='"$(PACKAGE)"' -DVERSION='"$(PACKAGE_VERSION)"' \
              -DLOCALEDIR='"$(localedir)"' -DENABLE_NLS
AM_CFLAGS = -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wstrict-prototypes \
            -Wunused-macros -Wshadow -Wdouble-promotion \
            -Wno-overlength-strings \
            -fvisibility=hidden
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
  src/i18n.h \
  src/main.c \
  src/render.c \
  src/render.h \
  src/share.h
nodist_pegsolitaire_SOURCES = data.h
BUILT_SOURCES = data.h

EXTRA_DIST = \
  AUTHORS CHANGELOG COPYING README generate-data-header \
  data/hole.svg data/peg.svg data/pegsolitaire.glade \
  data/com.github.jnumm.pegsolitaire.desktop.in \
  data/com.github.jnumm.pegsolitaire.appdata.xml.in
CLEANFILES = \
  data.h \
  com.github.jnumm.pegsolitaire.desktop \
  com.github.jnumm.pegsolitaire.appdata.xml

# Embedding the required data files in the executable.
data.h: data/pegsolitaire.glade data/hole.svg data/peg.svg
	$(top_srcdir)/generate-data-header $^ >$@

# Rules for metadata translation using gettext.
desktopdir = $(datarootdir)/applications
desktop_DATA = com.github.jnumm.pegsolitaire.desktop
appdatadir = $(datarootdir)/metainfo
appdata_DATA = com.github.jnumm.pegsolitaire.appdata.xml

com.github.jnumm.pegsolitaire.desktop: data/com.github.jnumm.pegsolitaire.desktop.in
	$(MSGFMT) -v -c --statistics --desktop --template $< -d $(top_srcdir)/po -o $@

com.github.jnumm.pegsolitaire.appdata.xml: data/com.github.jnumm.pegsolitaire.appdata.xml.in
	$(MSGFMT) -v -c --statistics --xml --template $< -d $(top_srcdir)/po -o $@

# Files in dist_something_DATA will be installed to $(somethingdir).
dist_man_MANS = data/pegsolitaire.6

svgicondir = $(datarootdir)/icons/hicolor/scalable/apps
dist_svgicon_DATA = data/pegsolitaire.svg

helpdir = $(datarootdir)/gnome/help/pegsolitaire/C
dist_help_DATA = help/legal.xml help/pegsolitaire.xml

SUBDIRS = po
