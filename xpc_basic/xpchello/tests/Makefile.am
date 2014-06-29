#*****************************************************************************
# Makefile.am (xpchello/tests)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \application xpchello_test
# \author     	Chris Ahlstrom
# \date       	2009-04-05
# \updates     2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		This module provides an Automake makefile for the XPC 'hello' test
# 		applications.
#
#  	This makefile provides the skeleton needed to build the test directory
#  	using GNU autotools.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#****************************************************************************
# EXTRA_DIST
#----------------------------------------------------------------------------
#
#  	getopt_test.c is not ready and is not included at this time.
#
#		$(TESTS) is not included because it is derived files.
#
# 	TODO:  Makefile.bcc is not yet present, and Makefile.vc7 isn't ready.
#
#----------------------------------------------------------------------------

EXTRA_DIST = README test test_script

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------
#
#		top_srcdir    = ../../.. [xpc_suite directory]
#		builddir      = /home/ahlstrom/ca/mls/git/xpc_suite-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

cutlibdir = $(builddir)/xpccut/src/.libs

#******************************************************************************
# localdir
#------------------------------------------------------------------------------
#
# 	'localdir' is the normal system directory for installed localization files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#****************************************************************************
# XPCCUT_LIBS
#----------------------------------------------------------------------------
#
#		In the long run, we want to test linking to an installed unit-test
#		framework (XPCCUT), but for now we just point to the local built
#		version.
#
# 		XPCCUT_LIBS = @XPCCUT_LIBS@ is set up by PKG_CHECK_MODULES([XPCCUT]...
#		called in configure.ac.
#
# XPCCUT_LIBS = @XPCCUT_LIBS@
# Libtool error "libtool: link: cannot determine absolute directory name of
# ../../../xpccut/src/.libs" with this:
#
# XPCCUT_LIBS = -L$(top_srcdir)/xpccut/src/.libs -lxpccut
#
#----------------------------------------------------------------------------

XPCCUT_LIBS = -L$(cutlibdir) -lxpccut

#******************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#------------------------------------------------------------------------------
#
# 	'AM_CPPFLAGS' is the set of directories needed to access all of the
# 	XPC-suite library header files used in this project.
#
#------------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpccut/include \
 -I../include

#****************************************************************************
# The test programs to build
#----------------------------------------------------------------------------
#
# Warning:
#
#		It is very important to note that all of the applications need to
#		include the pthread library.  It gets linked in by the errorlogging
#		module, which includes a mutex object that can be used with the
#		--synch option.
#
#----------------------------------------------------------------------------

noinst_PROGRAMS =	xpchello_test

#****************************************************************************
# xpchello_test
#----------------------------------------------------------------------------
#
# 		Another option is to provide the following:
#
# 		   check_PROGRAMS = xpchello_test
#
#		This allows the makefile to support "make check".
#
#     	xpchello_test_LDADD = $(libmath) -lpthread -L.. -lxpccut
#			xpchello_test_DEPENDENCIES = ../src/xpc/libxpccut.a
#
#----------------------------------------------------------------------------

xpchello_test_SOURCES = xpchello_test.c
xpchello_test_LDADD = @LIBINTL@ -lpthread $(XPCCUT_LIBS)
xpchello_test_DEPENDENCIES =

#******************************************************************************
# Testing
#------------------------------------------------------------------------------
#
# 	See this link:
#
# 	   http://www.gnu.org/software/hello/manual/automake/Simple-Tests.html
#
#		The TEST variable is used in the "make check" command.  We add a
#		target to implement "make test", also.
#
#        testsubdir = test-results
#
#------------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT =
TESTS = test_script

test: check

#****************************************************************************
#  distclean
#----------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#*****************************************************************************
# Makefile.am (xpchello/tests)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#----------------------------------------------------------------------------
