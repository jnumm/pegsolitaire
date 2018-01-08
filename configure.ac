AC_INIT([pegsolitaire], [0.1.0])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AM_MAINTAINER_MODE([enable])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

dnl Add here the compilation flags that might cause an error on some compilers.
AX_APPEND_COMPILE_FLAGS([-Wmissing-variable-declarations])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])

PKG_CHECK_MODULES([GTK], [gtk+-2.0 gmodule-2.0 librsvg-2.0])

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
