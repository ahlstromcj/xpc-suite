#******************************************************************************
# Makefile.am (include/xpc)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc++
# \author     	Chris Ahlstrom
# \date       	2010-05-30
# \update     	2014-04-21
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the XPC++ library's
#     ./include/xpc directory.
#
#  	It is useful for installing header files in a subdirectory of the
#  	project include directory, /usr/include, /usr/local/include, etc.
#
# \note
#     The normal and re-named configuration header files created are:
#
#        -  config.h
#        -  xpc-config.h
#
#     Note the second name -- it is not xpccut-config.h, as you might
#     expect.  All suite projects share the same configuration.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#	Macros
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

#******************************************************************************
# Installed directories.
#------------------------------------------------------------------------------

prefix = @prefix@
libdir = @xpclibdir@
datadir = @datadir@
datarootdir = @datarootdir@
xpclibdir = @xpclibdir@
xpcdocdir = @xpcdocdir@
xpcdoxygendir = @xpcdoxygendir@
xpcincludedir = @xpcincludedir@

#******************************************************************************
# EXTRA_DIST
#
# 		Note that we have to add "*.h" to ensure these files get distributed,
# 		since we had to move them from src/Makefile.am.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.hpp *.h

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
#  This define is necessary to cause the proper subdirectory to be made
#  during installation.  'xpcincludedir' is defined in the configure.ac
#  script.
#
#  Without this value, the header files are stored in
#  "$(prefix)/include/xpc-1.0" instead of "$(prefix)/include/xpc-1.0/xpc".
#  Also, automake needs xpccincludedir for the xpccinclude_HEADERS
#  statement.
#
#------------------------------------------------------------------------------

pkgincludedir=$(xpcincludedir)/xpc
xpccincludedir=$(xpcincludedir)/xpc

#******************************************************************************
# Do not distribute the configure-generated config.h (but it turns out it is
# distributed during the install, which is okay.)
#
# dist-hook:
# 	rm -f $(distdir)/config.h
#
#------------------------------------------------------------------------------

xpccinclude_HEADERS =	\
   averager.hpp			\
   binstring.hpp        \
   errorlog.hpp			\
	initree.hpp				\
   map_helpers.hpp      \
   rowset.hpp				\
   stringmap.hpp        \
   systemtime.hpp

#******************************************************************************
# Installing xpc-config.h
#------------------------------------------------------------------------------
#
#     This is now done in the xpc_basic/include/xpc make-file.
#
#------------------------------------------------------------------------------

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-1.0/xpc if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(pkgincludedir) manually"

#******************************************************************************
# Makefile.am (include/xpc)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
