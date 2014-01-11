# Samsung USB bulk-transfer utility
# Copyright (C) 2011 Michel Stempin <michel.stempin@wanadoo.fr>
# All rights reserved.
#
# Makefile configuration - processed by automake.
#
# $Id: Makefile.am 214 2011-03-09 23:36:14Z michel.stempin@wanadoo.fr $
#

# The list of programs that are built.
bin_PROGRAMS = dnw

# The source files of the 'dnw' program.
dnw_SOURCES = dnw.c ${top_srcdir}/include/gettext.h

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

# Make sure the gettext.h include file is found.
AM_CPPFLAGS = -I. -I$(top_srcdir) -I$(top_srcdir)/include -I/opt/local/include/libusb-1.0 -Wall

# Link time dependencies.
LDADD = @LIBINTL@  -L/opt/local/lib -lusb-1.0
