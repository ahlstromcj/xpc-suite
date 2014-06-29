dnl ***************************************************************************
dnl xpc_mingw32.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_mingw32.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	03/04/2008-03/04/2008
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   Tests for usage of the Sun WorkShop compiler.
dnl
dnl   Modifies CFLAGS to suit.
dnl
dnl   A good starting point for learning to use Mingw (formerly "Mingw32)
dnl   to build Windows application on Linux is
dnl
dnl   http://www.wxwidgets.org/wiki/index.php/Cross-Compiling_Under_Linux
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_MINGW32],
[

   AC_MSG_CHECKING(for Sun WorkShop compiler) 
   if $CC -V 2>&1 | egrep 'WorkShop Compilers 4.2' > /dev/null ; then
      AC_MSG_RESULT(4.2)
      CFLAGS="${CFLAGS} -instances=global -features=anachronisms"
   elif $CC -V 2>&1 | egrep 'WorkShop Compilers 5.0' > /dev/null ; then
      AC_MSG_RESULT(5.0)
      CFLAGS="${CFLAGS} -instances=global -features=anachronisms,no%conststrings"
   elif $CC -V 2>&1 | egrep 'Sun WorkShop 6' > /dev/null ; then
      AC_MSG_RESULT(Forte6)
      CFLAGS="${CFLAGS} -instances=global -features=anachronisms,no%conststrings"
   else
      AC_MSG_RESULT(no)
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
