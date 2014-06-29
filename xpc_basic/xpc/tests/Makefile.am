#******************************************************************************
# Makefile.am (xpc/tests)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc
# \author     	Chris Ahlstrom
# \date       	2008-05-02
# \update      2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		This module provides an Automake makefile for the thread and other
# 		XPC library test applications.
#
#  	This makefile provides the skeleton needed to build the test
#  	directory using GNU autotools.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#******************************************************************************
#  EXTRA_DIST
#------------------------------------------------------------------------------
#
#  getopt_test.c is not ready and is not included at this time.
#	$(TESTS) is not included because it is derived files.
#
#------------------------------------------------------------------------------

EXTRA_DIST = dl_leaks.supp make-tests README

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
xpcclibdir = $(builddir)/xpc/src/.libs

#******************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#------------------------------------------------------------------------------
#
# 	'AM_CPPFLAGS' is the set of directories needed to access all of the
# 	XPC-suite unit-test library header files used in this project.
#
#		Defines where to look for header files.
#
#			-	If we build from the xpc_suite root directory, then $top_srcdir
#           is "../.."
#			-	If we build from the xpc directory, then it is "..".
#			-	If we enter a directory such as "debug", and do "../configure",
#				then it is "../../.."
#
#		"../include" is where the xpc header files are located, and it must
#		be defined even if $top_srcdir is "../..".  So we need two entries,
#		to handle the two possible build situations.
#
#		By convention, we include the "xpc" subdirectory in the C/C++ #include
#		directives, so it is not needed here.
#
#------------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpccut/include \
 -I$(top_srcdir)/xpc/include \
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
#	These files are the ones built in the source tree, not the installed
#	ones.
#
#  Sometimes one has to change the order of the libraries in this list.
#
#----------------------------------------------------------------------------

libraries = $(libmath) -L$(cutlibdir) -lxpccut -L$(xpcclibdir) -lxpc

#****************************************************************************
# Project-specific dependency files
#----------------------------------------------------------------------------
#
#  Provdies the specific list of dependencies, to assure that the make
#  detects all changes, if they are available.
#
#  Since this is our own library and test application, we will assume
#  the xpccut-1.1 project has already been built.
#
#----------------------------------------------------------------------------

dependencies = $(cutlibdir)/libxpccut.a $(xpcclibdir)/libxpc.a

#******************************************************************************
# The test programs to build
#------------------------------------------------------------------------------
#
# Warning:
#
#		It is very important to note that all of the application need to
#		include the pthread library.  It gets linked in by the errorlogging
#		module, which includes a mutex object that can be used with the
#		--synch option.
#
# Not yet ready to add to the list:
#
#	xpc_strings_ut:  It needs to be upgraded to the newest unit-test library.
#
#------------------------------------------------------------------------------

noinst_PROGRAMS = cpu_os_ut errorlogging_ut numerics_ut options_ut parse_ini_ut portable_ut syncher_thread_ut

#******************************************************************************
# xpc_strings_ut
#------------------------------------------------------------------------------
#
# xpc_strings_ut_SOURCES = xpc_strings_ut.c
# xpc_strings_ut_LDADD = $(libraries) -lpthread
# xpc_strings_ut_DEPENDENCIES = $(dependencies)
#
#------------------------------------------------------------------------------

#******************************************************************************
# cpu_os_ut
#------------------------------------------------------------------------------

cpu_os_ut_SOURCES = cpu_os_ut.c
cpu_os_ut_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
cpu_os_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# errorlogging_ut
#------------------------------------------------------------------------------

errorlogging_ut_SOURCES = errorlogging_ut.c
errorlogging_ut_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
errorlogging_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# numerics_ut
#------------------------------------------------------------------------------
#
#     Note the extra library, -lm, needed for the sqrt() function.
#
#------------------------------------------------------------------------------

numerics_ut_SOURCES = numerics_ut.c
numerics_ut_LDADD = @LIBINTL@ -lpthread -ldl -lm $(libraries)
numerics_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# options_ut
#------------------------------------------------------------------------------
#
#     Note the extra library, -lm, needed for the sqrt() function.
#
#------------------------------------------------------------------------------

options_ut_SOURCES = options_ut.c
options_ut_LDADD = @LIBINTL@ -lpthread -ldl -lm $(libraries)
options_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# parse_ini_ut
#------------------------------------------------------------------------------
#
#     Note the extra library, -lm, needed for the sqrt() function.
#
#------------------------------------------------------------------------------

parse_ini_ut_SOURCES = parse_ini_ut.c
parse_ini_ut_LDADD = @LIBINTL@ -lpthread -ldl -lm $(libraries)
parse_ini_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# portable_ut
#------------------------------------------------------------------------------
#
#     Note the extra library, -lm, needed for the sqrt() function.
#
#------------------------------------------------------------------------------

portable_ut_SOURCES = portable_ut.c
portable_ut_LDADD = @LIBINTL@ -lpthread -ldl -lm $(libraries)
portable_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# syncher_thread_ut
#------------------------------------------------------------------------------

syncher_thread_ut_SOURCES = syncher_thread_ut.c
syncher_thread_ut_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
syncher_thread_ut_DEPENDENCIES = $(dependencies)

#******************************************************************************
# xpc_strings_ut
#------------------------------------------------------------------------------
#
# Not yet ready to add to the list; it needs to be upgraded to the newest
# unit-test library.
#
# xpc_strings_ut_SOURCES = xpc_strings_ut.c
# xpc_strings_ut_LDADD = @LIBINTL@ -lpthread -ldl $(libraries)
# xpc_strings_ut_DEPENDENCIES = $(dependencies)
#
#------------------------------------------------------------------------------

#******************************************************************************
# Testing
#------------------------------------------------------------------------------
#
# 	See this link:
#
# 	   http://www.gnu.org/software/hello/manual/automake/Simple-Tests.html
#
#		The TEST variable is used in the "make check" command.  We add a
#		target to implement "make test", also.  testsubdir = test-results is
#		not yet in use.  The GNU test enivronment defines 'srcdir'; for this
#		test as run in the out-of-source 'debug' directory, this macro is
#		defined as '../../..'.  Search for 'srcdir' in the parse_ini_ut.c
#		module.
#
#------------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT =
TESTS = test_script

test: check

#******************************************************************************
#  distclean
#------------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#******************************************************************************
# Makefile.am (xpc/tests)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# 	vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
