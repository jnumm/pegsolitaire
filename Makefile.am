AM_CPPFLAGS = -DLOCALEDIR='"$(localedir)"' -DPKGDATADIR='"$(pkgdatadir)"'
AM_CFLAGS = -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wstrict-prototypes \
            -Wunused-macros -Wshadow -Wdouble-promotion
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
  src/preimage.h \
  src/share.h

# Files in dist_something_DATA will be installed to $(somethingdir).
dist_pkgdata_DATA = data/hole.svg data/peg.svg data/pegsolitaire.glade

dist_man_MANS = data/pegsolitaire.6
dist_doc_DATA = CHANGELOG COPYING

svgicondir = $(datarootdir)/icons/hicolor/scalable/apps
dist_svgicon_DATA = data/pegsolitaire.svg

helpdir = $(datarootdir)/gnome/help/pegsolitaire/C
dist_help_DATA = help/legal.xml help/pegsolitaire.xml

EXTRA_DIST = AUTHORS README

# Rules for metadata translation using gettext.
EXTRA_DIST += \
  data/pegsolitaire.desktop.in \
  data/com.github.jnumm.pegsolitaire.appdata.xml.in
CLEANFILES = \
  pegsolitaire.desktop \
  com.github.jnumm.pegsolitaire.appdata.xml

desktopdir = $(datarootdir)/applications
desktop_DATA = pegsolitaire.desktop
appdatadir = $(datarootdir)/metainfo
appdata_DATA = com.github.jnumm.pegsolitaire.appdata.xml

pegsolitaire.desktop: data/pegsolitaire.desktop.in
	$(MSGFMT) -v -c --statistics --desktop --template $< -d $(top_srcdir)/po -o $@

com.github.jnumm.pegsolitaire.appdata.xml: data/com.github.jnumm.pegsolitaire.appdata.xml.in
	$(MSGFMT) -v -c --statistics --xml --template $< -d $(top_srcdir)/po -o $@

SUBDIRS = po
