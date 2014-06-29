#****************************************************************************
# Makefile.am (xpc++/tests)
#----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc++
# \author     	Chris Ahlstrom
# \date       	2010-05-30
# \updates     2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#		This is a Makefile for the tests directory for the
#  	xpc++ library project.
#
#  	This module provides an automake makefile for the unit-test, developer
#  	tests, or integration test applications.  This makefile provides the
#  	items needed to build the test applications using GNU autotools.
#
#----------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#****************************************************************************
# 	EXTRA_DIST
#----------------------------------------------------------------------------
#
#	Add any extra distributable files here, so they are added to the tar
#	package.
#
#----------------------------------------------------------------------------

EXTRA_DIST = test test_script

#******************************************************************************
# Items from configure.ac
#-------------------------------------------------------------------------------
#
#  XPCCUT_LIBS is defined by the PKG_CHECK_MODULES macro.  The xpccut
#  library must be installed officially in order to be used to build the
#  unit-test application of the xpc library.
#
#-------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

XPC_API_MAJOR = @XPC_API_MAJOR@
XPC_API_MINOR = @XPC_API_MINOR@
XPC_API_PATCH = @XPC_API_PATCH@
XPC_API_VERSION = @XPC_API_VERSION@

XPCCUT_LIBS = @XPCCUT_LIBS@
XPC_LIBS = @XPC_LIBS@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------
#
# 	Not needed, really, since we won't be installing the unit tests.  Also,
# 	we want to link to local versions, not installed versions, of the XPC
# 	libraries.
#
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
# localedir
#------------------------------------------------------------------------------
#
# 	'localedir' is the normal system directory for installed localization
#  files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------
#
#  'cutlibdir' and 'xpcclibdir' locate the XPC libraries as built within the
#  source tree (whether a normal build or out-of-source build).
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

cutlibdir = $(builddir)/xpccut/src/.libs
cutpplibdir = $(builddir)/xpccut++/src/.libs
xpcclibdir = $(builddir)/xpc/src/.libs
xpcpplibdir = $(builddir)/xpc++/src/.libs

#****************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#----------------------------------------------------------------------------
#
#	Specifies the header-file locations for uninstalled libraries in the XPC
#	source tree.
#
#----------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpccut/include \
 -I$(top_srcdir)/xpccut++/include \
 -I$(top_srcdir)/xpc/include \
 -I$(top_srcdir)/xpc++/include \
 -I../../include

#******************************************************************************
# libmath
#------------------------------------------------------------------------------
#
# 		One day, we got errors about sqrt() undefined, which we fixed by
# 		adding -lm.  Then one day we got errors about various items in
# 		sys/stat.h being multiply-defined, and it turned out to be the -lm.
#
# 		We make it (an empty) define for how to handle it more easily.
#
#------------------------------------------------------------------------------

libmath = -lm

#****************************************************************************
# Project-specific library files
#----------------------------------------------------------------------------
#
#	Specifies the names of the libraries to be linked with the test
#	application.
#
# 	The ordering of libraries is important, to avoid unresolved externals.
# 	If library libA needs library libB, then -lB should come after -lA.
#
# 	In some cases, libraries have to be included more than once.
# 	This is a minor limitation in the GNU linker.
#
#----------------------------------------------------------------------------

libraries = \
 -L$(cutlibdir) -lxpccut \
 -L$(cutpplibdir) -lxpccut++ \
 -L$(xpcclibdir) -lxpc \
 -L$(xpcpplibdir) -lxpc++

#****************************************************************************
# Project-specific dependency files
#----------------------------------------------------------------------------
#
#  Provdies the specific list of dependencies, to assure that the make
#  detects all changes, if they are available.
#
#  Since this is our own library and test application, we will assume
#  the xpccut-1.0 project has already been built.
#
#----------------------------------------------------------------------------

dependencies = \
 $(cutlibdir)/libxpccut.a \
 $(cutpplibdir)/libxpccut++.a \
 $(xpcclibdir)/libxpc.a \
 $(xpcpplibdir)/libxpc++.a

#****************************************************************************
# The program(s) to build, but not install
#----------------------------------------------------------------------------

noinst_PROGRAMS = xpcpp_unit_test experiments

#****************************************************************************
# xpcpp_unittest_SOURCES
#----------------------------------------------------------------------------
#
#  Note that we assume the developer has also downloaded and built, in this
#  same directory tree, the C libraries, xpc and xpccut, and the C++ library
#  xpccut++.  If a bug is found, the developer will need both sets of source
#  code anyway.
#
#----------------------------------------------------------------------------

xpcpp_unit_test_SOURCES = xpcpp_unit_test.cpp
xpcpp_unit_test_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
xpcpp_unit_test_DEPENDENCIES = $(dependencies)

#****************************************************************************
# experiments_SOURCES
#----------------------------------------------------------------------------
#
#  Note that we assume the developer has also downloaded and built, in this
#  same directory tree, the C libraries, xpc and xpccut, and the C++ library
#  xpccut++.  If a bug is found, the developer will need both sets of source
#  code anyway.
#
#----------------------------------------------------------------------------

experiments_SOURCES = experiments.cpp info.cpp infolist.cpp
experiments_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
experiments_DEPENDENCIES = $(dependencies)

#****************************************************************************
# TESTS
#----------------------------------------------------------------------------
#
#     This variable is used in the "make check" command.  We add a target to
#     implement "make test", also.
#
#----------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT =
TESTS = test_script

test: check

#******************************************************************************
#  distclean
#------------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#****************************************************************************
# Makefile.am (xpc++/tests)
#----------------------------------------------------------------------------
# Local Variables:
# End:
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
