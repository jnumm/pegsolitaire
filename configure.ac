AC_INIT([pegsolitaire], [0.0.4])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall foreign])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_HEADER_STDC


GETTEXT_PACKAGE=pegsolitaire
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])


PKG_CHECK_MODULES(GTK, [gtk+-3.0])


AC_CONFIG_FILES([
Makefile
src/Makefile
src/pixmaps/Makefile
po/Makefile.in
help/Makefile
])

AC_OUTPUT
