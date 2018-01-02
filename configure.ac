AC_INIT([pegsolitaire], [0.0.4])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AM_MAINTAINER_MODE([enable])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$PACKAGE"], [GETTEXT package name])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])

PKG_CHECK_MODULES([GTK], [gtk+-2.0 librsvg-2.0])

AC_CONFIG_FILES([Makefile po/Makefile.in])

AC_OUTPUT
