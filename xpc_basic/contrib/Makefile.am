#******************************************************************************
# Makefile.am (contrib)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	XPC
# \author     	Chris Ahlstrom
# \date       	2008-02-26
# \update      2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This makefile provides mostly just a way to make sure the contrib
#     files are included in the 'dist' target.
#
#     The files in the contrib directory are mostly configuration files that
#     users of vim, emacs, bash, splint, indent, and mrxvt might find useful.
#
#     See the README file for more information.
#
#------------------------------------------------------------------------------

#******************************************************************************
# Automatically included:
#
# 		ChangeLog Makefile.am Makefile.in README
#
#------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile

PACKAGE = @PACKAGE@
VERSION = @VERSION@
API_VERSION = @API_VERSION@
datarootdir = @datarootdir@

#******************************************************************************
# Other data
#
# 	During installation, we want to install xpc.m4 to
# 	/usr/local/share/aclocal, so that other projects can use it.
#
#------------------------------------------------------------------------------

aclocaldir = $(datadir)/aclocal
aclocal_DATA = xpc.m4

#******************************************************************************
# install-data-local is replaced by aclocal_DATA above.
#------------------------------------------------------------------------------

#******************************************************************************
# EXTRA_DIST
#------------------------------------------------------------------------------

EXTRA_DIST =					\
	README

#******************************************************************************
# XPC/contrib/Makefile.am
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
