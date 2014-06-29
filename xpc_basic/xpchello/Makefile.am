#*****************************************************************************
# Makefile.am (xpchello)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \application	xpchello subproject
# \author     	Chris Ahlstrom
# \date       	2008-03-06
# \update     	2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#		This is a project "root directory" makefile for the xpchello project.
#
#  	This makefile provides the skeleton needed to build the
#  	xpchello project directory using GNU autotools.
#
# \references
# 		-	http://olympus.het.brown.edu/cgi-bin/info2www?(gettext)
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------
#
#		Always use Automake in foreign mode (adding foreign to
#		AUTOMAKE_OPTIONS in Makefile.am). Otherwise, it requires too many
#		boilerplate files from the GNU coding standards that aren't useful to
#		us. 
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# EXTRA_DIST
#-----------------------------------------------------------------------------

EXTRA_DIST = README

#*****************************************************************************
#	Packaging.
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------
#
#     GNU automake assigns, for us: LIBTOOL_DEPS = @LIBTOOL_DEPS@
#
#-----------------------------------------------------------------------------

# libtool: $(LIBTOOL_DEPS)
#    $(SHELL) ./scripts/config.status --recheck

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------
#
#		We don't provide our own version of the intl/ sub-directory.  The
#		canonical order is:
#
#  		SUBDIRS = doc intl lib src po
#
#		The xpchello project, as a minimalist demonstration/tutorial project,
#		doesn't handle the whole set of targets.
#
#-----------------------------------------------------------------------------

SUBDIRS = include src tests

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------
#
#	This section is recommended by:
#
#		http://olympus.het.brown.edu/cgi-bin/info2www?(gettext)Makefile
#
#-----------------------------------------------------------------------------

DISTFILES = ABOUT-NLS
DIST_SUBDIRS = $(SUBDIRS)

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------
#
#		According to the reference above, other actions that need to be
#		handled here are:
#
#			installdirs
#			install
#			uninstall
#			clean
#			distclean
#
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All build items completed * * * * *"

#*****************************************************************************
# Makefile.am (xpchello)
#-----------------------------------------------------------------------------
# Local Variables:
# End:
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#-----------------------------------------------------------------------------
