## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po \
	help

pegsolitairedocdir = ${datadir}/doc/pegsolitaire
pegsolitairedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(pegsolitairedoc_DATA) \
             intltool-update.in \
             intltool-extract.in \
             intltool-merge.in

# Copy all the spec files. Of course, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

