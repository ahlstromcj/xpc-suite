#******************************************************************************
# Makefile.am (include/xpc)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc
# \author     	Chris Ahlstrom
# \date       	2008-02-26
# \update     	2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the XPC library's
#     ./include/xpc directory.
#
#------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# EXTRA_DIST
#------------------------------------------------------------------------------
#
# 		Note that we have to add "*.h" to ensure these files get distributed,
# 		since we had to move them from src/Makefile.am.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.h

#******************************************************************************
# Macros
#------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

xpcincludedir = @xpcincludedir@
datarootdir = @datarootdir@

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

xpccinclude_HEADERS = 		\
   build_versions.h			\
   cpu.h							\
   errorlog_macros.h			\
   errorlogging.h				\
   environment.h				\
   file_functions.h			\
   file_macros.h				\
   gettext_support.h			\
   integers.h					\
   macros.h						\
   nan_inf.h               \
	numerics.h					\
   options.h					\
   os.h							\
   parse_ini.h					\
   portable.h					\
   pthreader.h					\
   pthread_attributes.h		\
   syncher.h               \
   test_settings.h         \
   xwinsock.h              \
   xstrings.h

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
#     We'd like to remove /usr/local/include/xpc-1.1/xpc if it is
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
