dnl ***************************************************************************
dnl xpc_mingw32.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file          xpc_mingw32.m4
dnl \library       XPC
dnl \author        Chris Ahlstrom
dnl \date          2008-03-04
dnl \update        2012-01-08
dnl \version       $Revision$
dnl \license       $XPC_SUITE_GPL_LICENSE$
dnl
dnl   Sets up for using MingW.
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_MINGW32],
[
   dnl AC_CANONICAL_HOST
   dnl AC_EXEEXT

   AC_MSG_CHECKING([for target system])
   CYGWIN=
   MINGW32=
   case $host_os in
      *cygwin* ) CYGWIN=1 ;;
      *mingw* ) MINGW32=1 ;;
   esac

   AR=ar
   if test "x$MINGW" = "xyes"; then

      # Fix for the debian distribution of mingw.  Note that we can assume
      # the other executables are found as well.

      if test -x "/usr/i586-mingw32msvc/bin/ar";then
         AR="/usr/i586-mingw32msvc/bin/ar"
      fi
      if test -x "/usr/i586-mingw32msvc/bin/ranlib";then
         RANLIB="/usr/i586-mingw32msvc/bin/ranlib"
      fi

      # Fix for the gentoo distribution of mingw

      if test -x "/opt/xmingw/bin/i386-mingw32msvc-ar";then
         AR="/opt/xmingw/bin/i386-mingw32msvc-ar"
      fi

      if test -x "/opt/xmingw/bin/i386-mingw32msvc-ranlib";then
         RANLIB="/opt/xmingw/bin/i386-mingw32msvc-ranlib"
      fi

      if test -x "/opt/xmingw/bin/i386-mingw32msvc-strip";then
         STRIP="/opt/xmingw/bin/i386-mingw32msvc-strip"
      fi
   fi
   AC_SUBST(AR)

   dnl Checks for system services

   if test "x${CYGWIN}" = "xyes"; then
      AC_DEFINE([CYGWIN], [1], [Define on cygwin])
      AC_MSG_RESULT(cygwin)
   else
      if test "x${MINGW}" = "xyes"; then
         AC_DEFINE([MINGW], [1], [Define on Mingw])
         WIN32=1
         AC_DEFINE([WIN32], [1], [Define on windows])
         LIBS="$LIBS -lws2_32 -lgdi32"
         AC_MSG_RESULT([mingw])
      else
         LINUX=1
         export LINUX
         AC_DEFINE([LINUX], [1], [Define if not on cygwin or mingw])
         AC_MSG_RESULT()
      fi
   fi
])

dnl ***************************************************************************
dnl xpc_mingw32.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
