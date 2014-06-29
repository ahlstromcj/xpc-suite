/******************************************************************************
 * build_versions.c
 *------------------------------------------------------------------------*//**
 *
 * \file          build_versions.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2005-06-17
 * \updates       2012-06-02
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a function to show the various compiler settings used in
 *    building the current version of the library.
 *
 *    Note that there are other interesting settings in the "cpu" modules.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* C error-logging facility            */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
XPC_REVISION(build_versions)

/******************************************************************************
 * Undefined macros
 *------------------------------------------------------------------------*//**
 *
 *    Some macros may end up defined.  If you encounter one, add it to this
 *    list.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_HAVE_STDBOOL_H
#define XPC_HAVE_STDBOOL_H 0
#endif

/******************************************************************************
 * show_compiler_info()
 *------------------------------------------------------------------------*//**
 *
 *    If any output is allowed, shows the compiler and compiler settings
 *    used in the build.
 *
 * \unittests
 *    -  N/A.  A usage sample is present in the tests/errorlogging_ut.c
 *       module.
 *
 *//*-------------------------------------------------------------------------*/

void
show_compiler_info (void)
{
   if (! xpc_shownothing())
   {
      const char * platform = _("unknown");
      const char * compiler_name = _("unknown");
      int major_version = 0;
      int minor_version = 0;
      int patch_version = 0;

#ifdef __GNUC__
#ifdef __GNUG__
#error This is a C library, bucko!  No C++ allowed in here!
#else

      platform = "GNU";
      compiler_name = "gcc";
      major_version = __GNUC__;
      minor_version = __GNUC_MINOR__;
      patch_version = __GNUC_PATCHLEVEL__;

#endif      /* __GNUG__ */
#endif      /* __GNUC__ */

      fprintf
      (
         stdout,
         "  Compiler:                     %s %s %d.%d.%d\n",
         platform, compiler_name,
         major_version, minor_version, patch_version
      );
   }
}

/******************************************************************************
 * show_build_settings()
 *------------------------------------------------------------------------*//**
 *
 *    If any output is allowed, shows the macro settings used in the build.
 *
 * \unittests
 *    -  N/A.  A usage sample is present in the tests/errorlogging_ut.c
 *       module.
 *
 *//*-------------------------------------------------------------------------*/

void
show_build_settings (void)
{
   if (! xpc_shownothing())
   {
      fprintf
      (
         stdout,
         "  Operating system category:    %s\n"
         ,
         XPC_OS_CATEGORY
      );
   }
}

/******************************************************************************
 * TRUTH()
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert integer macros to "true" or "false".
 *
 *       # define TRUTH(x)  ((x) ? "true" : "false")
 *
 *//*-------------------------------------------------------------------------*/

#define TRUTH(x)  gettext_boolean(x)

/******************************************************************************
 * QUOTE()
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert integer macros to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#define QUOTE(x)  #x

/******************************************************************************
 * XPC_NOERRLOG_BIT
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert XPC_NOERRLOG to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NOERRLOG
#define XPC_NOERRLOG_BIT   "true"
#else
#define XPC_NOERRLOG_BIT   "false"
#endif

/******************************************************************************
 * XPC_NONULLPTR_BIT
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert XPC_NONULLPTR to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NONULLPTR
#define XPC_NONULLPTR_BIT   "true"
#else
#define XPC_NONULLPTR_BIT   "false"
#endif

/******************************************************************************
 * XPC_NOTHISPTR_BIT
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert XPC_NOTHISPTR to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NOTHISPTR
#define XPC_NOTHISPTR_BIT   "true"
#else
#define XPC_NOTHISPTR_BIT   "false"
#endif

/******************************************************************************
 * XPC_STACKCHK_BIT
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert XPC_NOTHISPTR to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_STACKCHK
#define XPC_STACKCHK_BIT   "true"
#else
#define XPC_STACKCHK_BIT   "false"
#endif

/******************************************************************************
 * XPC_PROFLAGS_BIT
 *------------------------------------------------------------------------*//**
 *
 *    Internal macro to convert XPC_NOTHISPTR to "true" or "false".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_PROFLAGS
#define XPC_PROFLAGS_BIT   "true"
#else
#define XPC_PROFLAGS_BIT   "false"
#endif

/******************************************************************************
 * show_have_macros()
 *------------------------------------------------------------------------*//**
 *
 *    If any output is allowed, shows the macro settings used in the build.
 *
 * \unittests
 *    -  N/A.  A usage sample is present in the tests/errorlogging_ut.c
 *       module.
 *
 *//*-------------------------------------------------------------------------*/

void
show_have_macros (void)
{
   if (! xpc_shownothing())
   {
      fprintf
      (
         stdout,
         "  XPC_API_VERSION:              %s\n"
         "  XPC_DBGFLAGS:                 %s\n"
         "  XPC_ENABLE_NLS:               %s\n"
         "  XPC_HAVE_CTYPE_H:             %s\n"
         "  XPC_HAVE_DCGETTEXT:           %s\n"
         "  XPC_HAVE_ENDIAN_H:            %s\n"
         "  XPC_HAVE_ERRNO_H:             %s\n"
         "  XPC_HAVE_GETTEXT:             %s\n"
         "  XPC_HAVE_GETTIMEOFDAY:        %s\n"
         "  XPC_HAVE_INTTYPES_H:          %s\n"
         ,
         XPC_API_VERSION,
         QUOTE(XPC_DBGFLAGS),
         TRUTH(XPC_ENABLE_NLS),
         TRUTH(XPC_HAVE_CTYPE_H),
         TRUTH(XPC_HAVE_DCGETTEXT),
         TRUTH(XPC_HAVE_ENDIAN_H),
         TRUTH(XPC_HAVE_ERRNO_H),
         TRUTH(XPC_HAVE_GETTEXT),
         TRUTH(XPC_HAVE_GETTIMEOFDAY),
         TRUTH(XPC_HAVE_INTTYPES_H)
      );
      fprintf
      (
         stdout,
         "  XPC_HAVE_LIBINTL_H:           %s\n"
         "  XPC_HAVE_LIMITS_H:            %s\n"
         "  XPC_HAVE_LOCALE_H:            %s\n"
         "  XPC_HAVE_MALLOC_H:            %s\n"
         "  XPC_HAVE_MEMORY_H:            %s\n"
         "  XPC_HAVE_NETDB_H:             %s\n"
         "  XPC_HAVE_PTHREAD_H:           %s\n"
         "  XPC_HAVE_PTRDIFF_T:           %s\n"
         "  XPC_HAVE_SELECT:              %s\n"
         "  XPC_HAVE_STDARG_H:            %s\n"
         ,
         TRUTH(XPC_HAVE_LIBINTL_H),
         TRUTH(XPC_HAVE_LIMITS_H),
         TRUTH(XPC_HAVE_LOCALE_H),
         TRUTH(XPC_HAVE_MALLOC),
         TRUTH(XPC_HAVE_MEMORY_H),
         TRUTH(XPC_HAVE_NETDB_H),
         TRUTH(XPC_HAVE_PTHREAD_H),
         TRUTH(XPC_HAVE_PTRDIFF_T),
         TRUTH(XPC_HAVE_SELECT),
         TRUTH(XPC_HAVE_STDARG_H)
      );
      fprintf
      (
         stdout,
         "  XPC_HAVE_STDBOOL_H:           %s\n"
         "  XPC_HAVE_STDDEF_H:            %s\n"
         "  XPC_HAVE_STDINT_H:            %s\n"
         "  XPC_HAVE_STDIO_H:             %s\n"
         "  XPC_HAVE_STDLIB_H:            %s\n"
         "  XPC_HAVE_STRERROR:            %s\n"
         "  XPC_HAVE_STRINGS_H:           %s\n"
         "  XPC_HAVE_STRING_H:            %s\n"
         "  XPC_...TRUCT_STAT_ST_BLKSIZE: %s\n"
         "  XPC_HAVE_ST_BLKSIZE:          %s\n"
         ,
         TRUTH(XPC_HAVE_STDBOOL_H),
         TRUTH(XPC_HAVE_STDDEF_H),
         TRUTH(XPC_HAVE_STDINT_H),
         TRUTH(XPC_HAVE_STDIO_H),
         TRUTH(XPC_HAVE_STDLIB_H),
         TRUTH(XPC_HAVE_STRERROR),
         TRUTH(XPC_HAVE_STRINGS_H),
         TRUTH(XPC_HAVE_STRING_H),
         TRUTH(XPC_HAVE_STRUCT_STAT_ST_BLKSIZE),
         TRUTH(XPC_HAVE_ST_BLKSIZE)
      );
      fprintf
      (
         stdout,
         "  XPC_HAVE_SYSLOG_H:            %s\n"
         "  XPC_HAVE_SYS_SYSCTL_H:        %s\n"
         "  XPC_HAVE_SYS_SELECT_H:        %s\n"
         "  XPC_HAVE_SYS_SOCKET_H:        %s\n"
         "  XPC_HAVE_SYS_STAT_H:          %s\n"
         "  XPC_HAVE_SYS_TIME_H:          %s\n"
         "  XPC_HAVE_SYS_TYPES_H:         %s\n"
         "  XPC_HAVE_TIME_H:              %s\n"
         "  XPC_HAVE_UNISTD_H:            %s\n"
         "  XPC_NOERRLOG:                 %s\n"
         ,
         TRUTH(XPC_HAVE_SYSLOG_H),
         TRUTH(XPC_HAVE_SYS_SYSCTL_H),
         TRUTH(XPC_HAVE_SYS_SELECT_H),
         TRUTH(XPC_HAVE_SYS_SOCKET_H),
         TRUTH(XPC_HAVE_SYS_STAT_H),
         TRUTH(XPC_HAVE_SYS_TIME_H),
         TRUTH(XPC_HAVE_SYS_TYPES_H),
         TRUTH(XPC_HAVE_TIME_H),
         TRUTH(XPC_HAVE_UNISTD_H),
         XPC_NOERRLOG_BIT
      );
      fprintf
      (
         stdout,
         "  XPC_NONULLPTR:                %s\n"
         "  XPC_NOTHISPTR:                %s\n"
         "  XPC_PACKAGE:                  %s\n"
         "  XPC_PACKAGE_BUGREPORT:        %s\n"
         "  XPC_PACKAGE_NAME:             %s\n"
         "  XPC_PACKAGE_STRING:           %s\n"
         "  XPC_PACKAGE_TARNAME:          %s\n"
         "  XPC_PACKAGE_VERSION:          %s\n"
         "  XPC_PROFLAGS:                 %s\n"
         "  XPC_SELECT_TYPE_ARG1:         %s\n"
         ,
         XPC_NONULLPTR_BIT,
         XPC_NOTHISPTR_BIT,
         XPC_PACKAGE,
         XPC_PACKAGE_BUGREPORT,
         XPC_PACKAGE_NAME,
         XPC_PACKAGE_STRING,
         XPC_PACKAGE_TARNAME,
         XPC_PACKAGE_VERSION,
         XPC_PROFLAGS_BIT,
         QUOTE(XPC_SELECT_TYPE_ARG1)
      );
      fprintf
      (
         stdout,
         "  XPC_SELECT_TYPE_ARG234:       %s\n"
         "  XPC_SELECT_TYPE_ARG5:         %s\n"
         "  XPC_STACKCHK:                 %s\n"
         "  XPC_STDC_HEADERS:             %s\n"
         "  XPC_TIME_WITH_SYS_TIME:       %s\n"
         "  XPC_VERSION:                  %s\n"
         ,
         QUOTE(XPC_SELECT_TYPE_ARG234),
         QUOTE(XPC_SELECT_TYPE_ARG5),
         XPC_STACKCHK_BIT,
         TRUTH(XPC_STDC_HEADERS),
         TRUTH(XPC_TIME_WITH_SYS_TIME),
         XPC_VERSION
      );
   }
}

/******************************************************************************
 * build_versions.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
