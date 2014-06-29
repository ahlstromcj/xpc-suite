#*****************************************************************************
# Makefile.am (xpc/include)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc
# \author     	Chris Ahlstrom
# \date       	2008-02-26
# \update      2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the XPC library's
#     ./include directory.
#
# 		A stub Makefile.am for the include directory, which itself has
# 		subdirectories.
#
# 		In AUTOMAKE_OPTIONS, we don't currently use 'no-dependencies' and
# 		dist-bzip2.  dist-bzip2 is not available on older versions of autotools.
#
# 		In EXTRA_DIST and MAINTAINERCLEANFILES, we don't currently support a
# 		config.h.in (xpc-config.h is what we use).
#
# 		The $(xpcincludedir) symbol is defined in configure.ac.
#
#			xpcincludedir=$(includedir)/$(PACKAGE)-$(API_VERSION)
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#  Extra files in this directory
#-----------------------------------------------------------------------------

EXTRA_DIST =

#*****************************************************************************
#  Macros
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@
API_VERSION = @API_VERSION@
xpcincludedir = @xpcincludedir@
datarootdir = @datarootdir@
readmeincludedir=$(xpcincludedir)

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------

SUBDIRS = xpc

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------

DIST_SUBDIRS = $(SUBDIRS)

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-x.y if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(xpcincludedir) manually"

#******************************************************************************
# Makefile.am (include)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
