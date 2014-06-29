#*****************************************************************************
# Makefile.am (top man)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpc suite
# \author     	Chris Ahlstrom
# \date       	2008-08-09
# \update     	2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This makefile provides the skeleton needed to build the XPC's
#     man-page directory using GNU autotools.
#
#     Unlike the library and header directories, this one is standard.
#
#     dist-bzip2 is not available on older versions of autotools.
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#  EXTRA_DIST
#-----------------------------------------------------------------------------

EXTRA_DIST = $(man_MANS)

#*****************************************************************************
# The documentation to build
#-----------------------------------------------------------------------------

man_MANS = \
 errorlogging.1 \
 xpccut.1 \
 xpccut++.1

# stringmap.1

# dist_man_MANS = $(man_MANS)

#*****************************************************************************
# End of man/Makefile.am
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#-----------------------------------------------------------------------------
