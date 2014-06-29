#*****************************************************************************
# Makefile.am (xpc++/include)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc++
# \author     	Chris Ahlstrom
# \date       	2010-05-30
# \updates     2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the XPC++ library's
#     ./include directory.
#
#  	This makefile provides the skeleton needed to build the
#  	xpccut++ project directory using GNU autotools.
#
#     This file is an Automake include-level Makefile.
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#	Extra files in the top-level directory
#-----------------------------------------------------------------------------

EXTRA_DIST =

#*****************************************************************************
#  Macros
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------

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
#     We'd like to remove /usr/local/include/xpc-1.0 if it is
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
