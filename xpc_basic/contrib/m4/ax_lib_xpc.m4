# ===========================================================================
# ax_lib_xpc.m4i, Chris Ahlstrom, 2013-07-24 to 2013-07-27
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_XPC([ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   This macro will check for the existence of libxpc
#   (http://xpc_suite.sourceforge.net/). It does this by checking for the
#   header file errorloging.h and the xpc library object file. A --with-libxpc
#   option is supported as well. The following output variables are set with
#   AC_SUBST:
#
#     XPC_CPPFLAGS
#     XPC_LDFLAGS
#     XPC_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(XPC_CPPFLAGS)
#     AM_LDFLAGS = $(XPC_LDFLAGS)
#     myprogram_LDADD = $(XPC_LIBS)
#
#   Additionally, the following C preprocessor symbol will be defined with
#   AC_DEFINE if libxpc seems to be available.
#
#     HAVE_LIBXPC_H
#
# WARNING
#
#   At present, "xpc-1.1" is hardwired as part of the paths set up in this
#   macro.
#
# LICENSE
#
#   Copyright (c) 2013 Chris Ahlstrom <ahlstromcj@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_LIB_XPC],
[
  AH_TEMPLATE([HAVE_LIBXPC_H], [Define if libxpc header is available])
  AC_MSG_CHECKING(whether libxpc is present and to be used)
  AC_ARG_WITH([libxpc],
    [  --with-libxpc=DIR       prefix for xpc library files and headers],
    [
       if test "$withval" = "no"; then
         want_xpc="no"
         ac_xpc_path=""
       elif test "$withval" = "yes"; then
         want_xpc="yes"
         ac_xpc_path="/usr"
         if test ! -d $ac_xpc_path/include/xpc-1.1 ; then
            ac_xpc_path="/usr/local"
         fi
       else
         want_xpc="yes"
         ac_xpc_path="$withval"
       fi
    ],
    [
       want_xpc="yes"
       ac_xpc_path="/usr"
       if test ! -d $ac_xpc_path/include/xpc-1.1 ; then
          ac_xpc_path="/usr/local"
       fi
    ]
  )
  if test "x$want_xpc" = "xyes"; then
      AC_SUBST(XPC_CPPFLAGS, [-I$ac_xpc_path/include/xpc-1.1])
      AC_SUBST(XPC_LDFLAGS, [-L$ac_xpc_path/lib/xpc-1.1])
      AC_SUBST(XPC_LIBS, [-lxpc])
      AC_DEFINE([HAVE_LIBXPC_H])
      echo "... yes"
  else
      echo "... no"
      echo "Error! libxpc support cannot be configured."
  fi
])

dnl ***************************************************************************
dnl ax_lib_xpc.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
