#******************************************************************************
# Makefile.am (xpchello/include/xpc)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpchello application
# \author     	Chris Ahlstrom
# \date       	2008-05-09
# \updates     2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the xpchello
#     application's ./include/xpc directory.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# Package macros
#------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@
XPC_API_VERSION = @XPC_API_VERSION@
xpcincludedir = @xpcincludedir@
datarootdir = @datarootdir@

#******************************************************************************
# EXTRA_DIST
#
# 		Note that we have to add "*.h" to ensure these files get distributed,
# 		since we had to move them from src/Makefile.am.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.h

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
# 	This define is necessary to cause the proper subdirectory to be made
# 	during installation.
#
#------------------------------------------------------------------------------

xpccincludedir=$(xpcincludedir)/xpc

#******************************************************************************
# Header files
#------------------------------------------------------------------------------
#
#		Causes this file to be installed in /usr/local/include/xpc-1.1,
#		not /usr/local/include/xpc-1.1/xpc.  Not sure we care about that right
#		now.
#
#------------------------------------------------------------------------------

xpcinclude_HEADERS = tiny_macros.h

#******************************************************************************
# Makefile.am (xpchello/include/xpc)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
