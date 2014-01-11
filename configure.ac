dnl Samsung USB bulk-transfer utility
dnl
dnl Copyright (C) 211, Michel Stempin <michel.stempin@wanadoo.fr>
dnl All rights reserved.
dnl
dnl Configuration file - processed by autoconf.
dnl
dnl $Id: configure.ac 214 2011-03-09 23:36:14Z michel.stempin@wanadoo.fr $
dnl

AC_INIT([dnw],[1.0.0])
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_SRCDIR([src/dnw.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([include/config.h])
AC_PREFIX_DEFAULT([/usr/local])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Checks for libraries
AC_CHECK_LIB([usb], [usb_open])

dnl Checks for header files
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T

dnl Checks for library functions
AC_CHECK_FUNCS([setlocale strdup])

dnl i18n
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.13.1)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([m4/Makefile])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_OUTPUT
