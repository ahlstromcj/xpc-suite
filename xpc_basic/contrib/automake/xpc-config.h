#ifndef _INCLUDE_XPC_XPC_CONFIG_H
#define _INCLUDE_XPC_XPC_CONFIG_H 1
 
/* include/xpc/xpc-config.h. Generated automatically at end of configure. */
/* include/xpc/config.h.  Generated from config.h.in by configure.  */
/* include/xpc/config.h.in.  Generated from configure.ac by autoheader.  */

#ifndef XPC_VERSION
#define XPC_VERSION "1.1.0"
#endif
#ifndef XPC_API_VERSION
#define XPC_API_VERSION "1.1"
#endif



/* Define COVFLAGS=-fprofile-arcs -ftest-coverage if coverage support is
   wanted. */
#ifndef XPC_COVFLAGS
#define XPC_COVFLAGS 
#endif

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define on cygwin */
/* #undef CYGWIN */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define DBGFLAGS=-g -O0 -DDEBUG -NWIN32 -fno-inline if debug support is
   wanted. */
#ifndef XPC_DBGFLAGS
#define XPC_DBGFLAGS -O3 -DNWIN32
#endif

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#ifndef XPC_ENABLE_NLS
#define XPC_ENABLE_NLS 1
#endif

/* Define to 1 if you have `alloca', as a function or macro. */
#ifndef XPC_HAVE_ALLOCA
#define XPC_HAVE_ALLOCA 1
#endif

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#ifndef XPC_HAVE_ALLOCA_H
#define XPC_HAVE_ALLOCA_H 1
#endif

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the <ctype.h> header file. */
#ifndef XPC_HAVE_CTYPE_H
#define XPC_HAVE_CTYPE_H 1
#endif

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#ifndef XPC_HAVE_DCGETTEXT
#define XPC_HAVE_DCGETTEXT 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef XPC_HAVE_DLFCN_H
#define XPC_HAVE_DLFCN_H 1
#endif

/* have endian header */
#ifndef XPC_HAVE_ENDIAN_H
#define XPC_HAVE_ENDIAN_H 1
#endif

/* Define to 1 if you have the <errno.h> header file. */
#ifndef XPC_HAVE_ERRNO_H
#define XPC_HAVE_ERRNO_H 1
#endif

/* Define to 1 if the system has the type `errno_t'. */
/* #undef HAVE_ERRNO_T */

/* Define if the GNU gettext() function is already present or preinstalled. */
#ifndef XPC_HAVE_GETTEXT
#define XPC_HAVE_GETTEXT 1
#endif

/* Define to 1 if you have the `gettimeofday' function. */
#ifndef XPC_HAVE_GETTIMEOFDAY
#define XPC_HAVE_GETTIMEOFDAY 1
#endif

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef XPC_HAVE_INTTYPES_H
#define XPC_HAVE_INTTYPES_H 1
#endif

/* Define to 1 if you have the <libintl.h> header file. */
#ifndef XPC_HAVE_LIBINTL_H
#define XPC_HAVE_LIBINTL_H 1
#endif

/* Define to 1 if you have the <limits.h> header file. */
#ifndef XPC_HAVE_LIMITS_H
#define XPC_HAVE_LIMITS_H 1
#endif

/* Define to 1 if you have the <locale.h> header file. */
#ifndef XPC_HAVE_LOCALE_H
#define XPC_HAVE_LOCALE_H 1
#endif

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#ifndef XPC_HAVE_MALLOC
#define XPC_HAVE_MALLOC 1
#endif

/* Define to 1 if you have the <math.h> header file. */
#ifndef XPC_HAVE_MATH_H
#define XPC_HAVE_MATH_H 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef XPC_HAVE_MEMORY_H
#define XPC_HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <netdb.h> header file. */
#ifndef XPC_HAVE_NETDB_H
#define XPC_HAVE_NETDB_H 1
#endif

/* Define to 1 if you have the <netinet/in.h> header file. */
#ifndef XPC_HAVE_NETINET_IN_H
#define XPC_HAVE_NETINET_IN_H 1
#endif

/* Define to 1 if you have the <pthread.h> header file. */
#ifndef XPC_HAVE_PTHREAD_H
#define XPC_HAVE_PTHREAD_H 1
#endif

/* Define to 1 if the system has the type `ptrdiff_t'. */
#ifndef XPC_HAVE_PTRDIFF_T
#define XPC_HAVE_PTRDIFF_T 1
#endif

/* Define to 1 if you have the `select' function. */
#ifndef XPC_HAVE_SELECT
#define XPC_HAVE_SELECT 1
#endif

/* Define to 1 if you have the <setjmp.h> header file. */
#ifndef XPC_HAVE_SETJMP_H
#define XPC_HAVE_SETJMP_H 1
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#ifndef XPC_HAVE_STDARG_H
#define XPC_HAVE_STDARG_H 1
#endif

/* Define to 1 if stdbool.h conforms to C99. */
/* #undef HAVE_STDBOOL_H */

/* Define to 1 if you have the <stddef.h> header file. */
#ifndef XPC_HAVE_STDDEF_H
#define XPC_HAVE_STDDEF_H 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef XPC_HAVE_STDINT_H
#define XPC_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef XPC_HAVE_STDIO_H
#define XPC_HAVE_STDIO_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef XPC_HAVE_STDLIB_H
#define XPC_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef XPC_HAVE_STRERROR
#define XPC_HAVE_STRERROR 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef XPC_HAVE_STRINGS_H
#define XPC_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef XPC_HAVE_STRING_H
#define XPC_HAVE_STRING_H 1
#endif

/* Define to 1 if `st_blksize' is a member of `struct stat'. */
#ifndef XPC_HAVE_STRUCT_STAT_ST_BLKSIZE
#define XPC_HAVE_STRUCT_STAT_ST_BLKSIZE 1
#endif

/* Define to 1 if your `struct stat' has `st_blksize'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLKSIZE' instead. */
#ifndef XPC_HAVE_ST_BLKSIZE
#define XPC_HAVE_ST_BLKSIZE 1
#endif

/* Define to 1 if you have the <syslog.h> header file. */
#ifndef XPC_HAVE_SYSLOG_H
#define XPC_HAVE_SYSLOG_H 1
#endif

/* solaris endian */
/* #undef HAVE_SYS_ISA_DEFS_H */

/* Define to 1 if you have the <sys/select.h> header file. */
#ifndef XPC_HAVE_SYS_SELECT_H
#define XPC_HAVE_SYS_SELECT_H 1
#endif

/* Define to 1 if you have the <sys/socket.h> header file. */
#ifndef XPC_HAVE_SYS_SOCKET_H
#define XPC_HAVE_SYS_SOCKET_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef XPC_HAVE_SYS_STAT_H
#define XPC_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/sysctl.h> header file. */
#ifndef XPC_HAVE_SYS_SYSCTL_H
#define XPC_HAVE_SYS_SYSCTL_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef XPC_HAVE_SYS_TIME_H
#define XPC_HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef XPC_HAVE_SYS_TYPES_H
#define XPC_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <time.h> header file. */
#ifndef XPC_HAVE_TIME_H
#define XPC_HAVE_TIME_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef XPC_HAVE_UNISTD_H
#define XPC_HAVE_UNISTD_H 1
#endif

/* Define to 1 if the system has the type `_Bool'. */
/* #undef HAVE__BOOL */

/* Define if not on cygwin or mingw */
#ifndef XPC_LINUX
#define XPC_LINUX 1
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef XPC_LT_OBJDIR
#define XPC_LT_OBJDIR ".libs/"
#endif

/* Define on Mingw */
/* #undef MINGW */

/* Set NOERRLOG=-DXPC_NO_ERRORLOG if the user wants to disable error-logging.
   */
#ifndef XPC_NOERRLOG
#define XPC_NOERRLOG 
#endif

/* Set NONULLPTR=-DXPC_NO_NULLPTR if the user wants to disable null-pointer
   checking. */
#ifndef XPC_NONULLPTR
#define XPC_NONULLPTR 
#endif

/* Set NOTHISPTR=-DXPC_NO_THISPTR if the user wants to disable this-checking.
   */
#ifndef XPC_NOTHISPTR
#define XPC_NOTHISPTR 
#endif

/* Name of package */
#ifndef XPC_PACKAGE
#define XPC_PACKAGE "xpc"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef XPC_PACKAGE_BUGREPORT
#define XPC_PACKAGE_BUGREPORT "ahlstromcj@gmail.com"
#endif

/* Define to the full name of this package. */
#ifndef XPC_PACKAGE_NAME
#define XPC_PACKAGE_NAME "xpc"
#endif

/* Define to the full name and version of this package. */
#ifndef XPC_PACKAGE_STRING
#define XPC_PACKAGE_STRING "xpc 1.1"
#endif

/* Define to the one symbol short name of this package. */
#ifndef XPC_PACKAGE_TARNAME
#define XPC_PACKAGE_TARNAME "xpc"
#endif

/* Define to the home page for this package. */
#ifndef XPC_PACKAGE_URL
#define XPC_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef XPC_PACKAGE_VERSION
#define XPC_PACKAGE_VERSION "1.1"
#endif

/* Define PROFLAGS=-pg (gprof) or -p (prof) if profile support is wanted. */
#ifndef XPC_PROFLAGS
#define XPC_PROFLAGS 
#endif

/* Define to the type of arg 1 for `select'. */
#ifndef XPC_SELECT_TYPE_ARG1
#define XPC_SELECT_TYPE_ARG1 int
#endif

/* Define to the type of args 2, 3 and 4 for `select'. */
#ifndef XPC_SELECT_TYPE_ARG234
#define XPC_SELECT_TYPE_ARG234 (fd_set *)
#endif

/* Define to the type of arg 5 for `select'. */
#ifndef XPC_SELECT_TYPE_ARG5
#define XPC_SELECT_TYPE_ARG5 (struct timeval *)
#endif

/* Define STACKCHK=-fstack-check, if the user wants stack-checking. */
#ifndef XPC_STACKCHK
#define XPC_STACKCHK 
#endif

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#ifndef XPC_STDC_HEADERS
#define XPC_STDC_HEADERS 1
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifndef XPC_TIME_WITH_SYS_TIME
#define XPC_TIME_WITH_SYS_TIME 1
#endif

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#ifndef XPC_VERSION
#define XPC_VERSION "1.1.0"
#endif

/* Define on windows */
/* #undef WIN32 */

/* endian byte order */
/* #undef __BYTE_ORDER */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Bottom of header config */

 
/* once: _INCLUDE_XPC_XPC_CONFIG_H */
#endif
