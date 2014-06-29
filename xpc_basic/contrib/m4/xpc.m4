#******************************************************************************
# xpc.m4
#------------------------------------------------------------------------------
#
# \file        xpc.m4
# \library     libxpc
# \author      Chris Ahlstrom
# \version     $Revision$
# \date        02/26/2008-02/26/2008
# \license     $XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake m4 macro for the XPC library.
#
#     Configure paths for libxpc.  Adapted from ogg.m4 from the following
#     source.
#
#        Jack Moffitt <jack@icecast.org> 10-21-2000
#        Shamelessly stolen from Owen Taylor and Manish Singh
#
#------------------------------------------------------------------------------

dnl AM_PATH_XPC([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Test for libxpc, and define XPC_CFLAGS and XPC_LIBS.  The definition
dnl starts here, and goes to the end of the file.
dnl
dnl Note that libpthread is also tested by this macro; any application built
dnl with the XPC library needs pthreads.

AC_DEFUN([AM_PATH_XPC],
[dnl
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(xpc,[  --with-xpc=prefix   Prefix where libxpc is installed (optional)], xpc_prefix="$withval", xpc_prefix="")

AC_ARG_WITH(xpc-libraries,[  --with-xpc-libraries=dir   Directory where libxpc library is installed (optional)], xpc_libraries="$withval", xpc_libraries="")

AC_ARG_WITH(xpc-includes,[  --with-xpc-includes=dir   Directory where libxpc header files are installed (optional)], xpc_includes="$withval", xpc_includes="")

AC_ARG_ENABLE(xpctest, [  --disable-xpctest       Do not try to build and run a test program (option not yet supported)],, enable_xpctest=yes)

  if test "x$xpc_libraries" != "x" ; then
    XPC_LIBS="-L$xpc_libraries"
  elif test "x$xpc_prefix" != "x" ; then
    XPC_LIBS="-L$xpc_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    XPC_LIBS="-L$prefix/lib"
  fi

  XPC_LIBS="$XPC_LIBS -lpthread -lxpc"

  if test "x$xpc_includes" != "x" ; then
    XPC_CFLAGS="-I$xpc_includes"
  elif test "x$xpc_prefix" != "x" ; then
    XPC_CFLAGS="-I$xpc_prefix/include"
  elif test "x$prefix" != "xNONE"; then
    XPC_CFLAGS="-I$prefix/include"
  fi

  AC_MSG_CHECKING(for libxpc)
  no_xpc=""

  if test "x$enable_xpctest" = "xyes" ; then
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $XPC_CFLAGS"
    LIBS="$LIBS $XPC_LIBS"
dnl
dnl Now check if the installed libxpc is sufficiently new.
dnl
      rm -f conf.xpctest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xpc/macros.h>

int main ()
{
  system("touch conf.xpctest");
  return 0;
}

],, no_xpc=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
  fi

  if test "x$no_xpc" = "x" ; then
     AC_MSG_RESULT(yes)
     ifelse([$1], , :, [$1])     
  else
     AC_MSG_RESULT(no)
     if test -f conf.xpctest ; then
       :
     else
       echo "* Could not run libxpc test program, determining why..."
       CFLAGS="$CFLAGS $XPC_CFLAGS"
       LIBS="$LIBS $XPC_LIBS"
       AC_TRY_LINK([
#include <stdio.h>
#include <xpc/macros.h>
],     [ return 0; ],
       [ echo "* Test program compiled, but did not run. The (run-time) linker"
       echo "* did not find libxpc, or found the wrong version."
       echo "* If it did not find libxpc, set the LD_LIBRARY_PATH environment"
       echo "* variable, or edit /etc/ld.so.conf to point to the installed"
       echo "* location. Then run ldconfig, if that is required on your system."
       echo "* If an old version is installed, remove it, although modifying"
       echo "* LD_LIBRARY_PATH may get things to work"],
       [ echo "* The test program failed to compile or link. See config.log"
       echo "* for the exact error.  This can mean libxpc ($XPC_LIBS) was"
       echo "* installed incorrectly, moved, or was uninstalled. The value of"
       echo "CFLAGS is ($CFLAGS.)"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
     XPC_CFLAGS=""
     XPC_LIBS=""
     ifelse([$2], , :, [$2])
  fi
  AC_SUBST(XPC_CFLAGS)
  AC_SUBST(XPC_LIBS)
  rm -f conf.xpctest
])

#******************************************************************************
# End of xpc.m4
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 et syntax=m4
#------------------------------------------------------------------------------
