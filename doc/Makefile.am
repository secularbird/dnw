# Samsung USB bulk-transfer utility
# Copyright (C) 2011, Michel Stempin <michel.stempin@wanadoo.fr>
# All rights reserved.
#
# Makefile configuration - processed by automake.
#
# $Id: Makefile.am 214 2011-03-09 23:36:14Z michel.stempin@wanadoo.fr $
#

man_MANS=dnw.1
EXTRA_DIST=dnw.pod
DISTCLEANFILES = dnw.1

dnw.1: dnw.pod
	pod2man -c "Samsung USB Bulk Transfer Utility" -r "$(PACKAGE_NAME)-$(PACKAGE_VERSION)" $< > $@
