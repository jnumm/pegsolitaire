package = pegsolitaire
version = 0.2
appid = com.github.jnumm.pegsolitaire

src = $(wildcard src/*.c)
obj = $(src:.c=.o)
pofiles = $(wildcard po/*.po)
mofiles = $(pofiles:.po=.mo)

INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m644
prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/share/man
localedir ?= $(prefix)/share/locale
desktopdir ?= $(prefix)/share/applications
appdatadir ?= $(prefix)/share/metainfo
svgicondir ?= $(prefix)/share/icons/hicolor/scalable/apps
helpdir ?= $(prefix)/share/gnome/help/$(package)/C

deps = gtk+-3.0 gmodule-2.0 librsvg-2.0

# Flags that are more or less required to build
pkg_CPPFLAGS = -I. -DPACKAGE='"$(package)"' -DVERSION='"$(version)"' \
	-DLOCALEDIR='"$(localedir)"' -DENABLE_NLS
pkg_CFLAGS = -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wstrict-prototypes \
	-Wunused-macros -Wshadow -Wdouble-promotion -Wno-overlength-strings \
	-fvisibility=hidden $(shell pkg-config --cflags $(deps))
pkg_LDLIBS = -Wl,--as-needed $(shell pkg-config --libs $(deps))

# Non-essential default flags
CFLAGS ?= -O2 -g

.PHONY: all clean install update-po
all: pegsolitaire $(mofiles) $(appid).desktop $(appid).appdata.xml

%.o: %.c data.h
	$(CC) $(pkg_CPPFLAGS) $(CPPFLAGS) $(pkg_CFLAGS) $(CFLAGS) -o $@ -c $<
pegsolitaire: $(obj)
	$(CC) $(pkg_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(pkg_LDLIBS) $(LDLIBS)

# Embedding the required data files in the executable.
data.h: data/pegsolitaire.glade data/hole.svg data/peg.svg
	./generate-data-header $^ > $@

$(appid).desktop: data/$(appid).desktop.in po/LINGUAS
	msgfmt -c --desktop --template $< -o $@ -d po
$(appid).appdata.xml: data/$(appid).appdata.xml.in po/LINGUAS
	msgfmt -c --xml --template $< -o $@ -d po
po/LINGUAS:
	echo $(notdir $(basename $(pofiles))) > $@
%.mo: %.po
	msgfmt -c -o $@ $<
# The first -k disables all default keywords, because we don't want the
# value for Icon in the desktop file to be translated.
pegsolitaire.pot:
	xgettext -o $@ -f po/POTFILES.in \
		-k -k_ -kN_ -kName -kGenericName -kComment -kKeywords \
		--add-comments=TRANSLATORS: \
		--package-name=$(package) --package-version=$(version) \
		--msgid-bugs-address=https://github.com/jnumm/pegsolitaire/issues
update-po: $(addsuffix -update,$(pofiles))
%.po-update: pegsolitaire.pot
	msgmerge --update $*.po $<

clean:
	rm -f pegsolitaire $(obj) data.h $(appid).desktop $(appid).appdata.xml \
		$(mofiles) pegsolitaire.pot po/LINGUAS

install: all $(addsuffix -install,$(mofiles))
	mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(desktopdir) \
		$(DESTDIR)$(appdatadir) $(DESTDIR)$(svgicondir) \
		$(DESTDIR)$(mandir)/man6 $(DESTDIR)$(helpdir)
	$(INSTALL_PROGRAM) pegsolitaire $(DESTDIR)$(bindir)
	$(INSTALL_DATA) $(appid).desktop $(DESTDIR)$(desktopdir)
	$(INSTALL_DATA) $(appid).appdata.xml $(DESTDIR)$(appdatadir)
	$(INSTALL_DATA) data/pegsolitaire.svg $(DESTDIR)$(svgicondir)
	$(INSTALL_DATA) data/pegsolitaire.6 $(DESTDIR)$(mandir)/man6
	$(INSTALL_DATA) help/* $(DESTDIR)$(helpdir)
%.mo-install: %.mo
	mkdir -p $(DESTDIR)$(localedir)/$(notdir $*)/LC_MESSAGES
	$(INSTALL_DATA) $< $(DESTDIR)$(localedir)/$(notdir $*)/LC_MESSAGES/$(package).mo
