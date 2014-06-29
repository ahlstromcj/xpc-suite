#*****************************************************************************
# Makefile.am (xpc++)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc++
# \author     	Chris Ahlstrom
# \date       	2010-05-30
# \update      2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#		This is a project "root directory" makefile for the xpc++ project
#		of the XPC library suite.
#
#  	This makefile provides the skeleton needed to build the
#  	xpc++ project directory using GNU autotools.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# EXTRA_DIST
#-----------------------------------------------------------------------------
#
#		Extra files in the top-level directory.
#
#		The reference says to use "DISTFILES = ABOUT-NLS".  But vim doesn't
#		highlight that as a keyword, and we don't see reference to it elsewhere
#		on the web.
#
#-----------------------------------------------------------------------------

EXTRA_DIST =

#*****************************************************************************
#	Packaging.
#-----------------------------------------------------------------------------
#
#	This section is recommended by:
#
#		http://olympus.het.brown.edu/cgi-bin/info2www?(gettext)Makefile
#
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

XPC_API_MAJOR = @XPC_API_MAJOR@
XPC_API_MINOR = @XPC_API_MINOR@
XPC_API_PATCH = @XPC_API_PATCH@
XPC_API_VERSION = @XPC_API_VERSION@

XPC_LT_CURRENT = @XPC_LT_CURRENT@
XPC_LT_REVISION = @XPC_LT_REVISION@
XPC_LT_AGE = @XPC_LT_AGE@

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
# Local project directories
#------------------------------------------------------------------------------
#
#		top_srcdir    = ../../.. [xpc_suite directory]
#		builddir      = /home/ahlstrom/ca/mls/git/xpc_suite-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#******************************************************************************
# localedir
#------------------------------------------------------------------------------
#
#  'localedir' is the normal system directory for installed localization
#  files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------
#
# 		GNU automake does this assignment for us.
# 
# 			LIBTOOL_DEPS = @LIBTOOL_DEPS@
#
#-----------------------------------------------------------------------------

libtool: $(LIBTOOL_DEPS)
	$(SHELL) $(top_srcdir)/scripts/config.status --recheck

version = $(XPC_API_MAJOR):$(XPC_API_MINOR):$(XPC_API_PATCH)

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------

SUBDIRS = include src tests w32

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------

DIST_SUBDIRS = $(SUBDIRS)

#*****************************************************************************
# Tests
#-----------------------------------------------------------------------------
#
#		Uncomment these items only if your project does not support "make
#		check" or "make test".
#
# .PHONY: test check
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# make coverage (or gcov) [NOT READY]
#-----------------------------------------------------------------------------

coverage: gcov

gcov:
	./tests/unit_test_test
	cd tests
	gcov -b -f *.c > output.log
	cd ../src
	gcov -b -f -o .libs *.c > output.log
	@echo "-----------------------------------"
	@echo "Examine the cumulative data in:"
	@echo "   src/output.log"
	@echo "   tests/output.log"
	@echo "   src/*.c.gcov"
	@echo "   tests/*.c.gcov"
	@echo "-----------------------------------"

#*****************************************************************************
# make profiling (or gprof) [NOT READY]
#-----------------------------------------------------------------------------

profiling: gprof

gprof:
	./tests/xpcpp_unit_test

#*****************************************************************************
# make test
#-----------------------------------------------------------------------------

test: check

#******************************************************************************
# install-exec-hook
#------------------------------------------------------------------------------
#
#		We want our libraries stored in lib/xpc-1.0, not just lib.
#
# 			install-exec-hook:
#				mkdir -p $(xpclibdir)
#				mv $(libdir)/libxpccut* $(xpclibdir)
#
#		However, we need to add the installation path to either
#		/etc/ld.so.conf or to a file in the directory /etc/ld.so.conf.d, and
#		then run ldconfig.
#
#		We can assume that the user is running "make install" as root.
#
#------------------------------------------------------------------------------

install-exec-hook:
	$(top_srcdir)/scripts/ld_config.sh $(xpclibdir)

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All xpc++ build items completed * * * * *"

#****************************************************************************
# Makefile.am (libxpc++ top-level)
#----------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
