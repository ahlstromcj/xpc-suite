#ifndef XPC_PORTABLE_H
#define XPC_PORTABLE_H

/*******************************************************************************
 * portable.h
 *------------------------------------------------------------------------*//**
 *
 * \file          portable.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2005-06-26
 * \updates       2012-08-11
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides some miscellaneous portable functions.
 *
 *    This file should include ../config.h for configurations built using
 *    automake (for Linux/UNIX) or ../w32/config.h for configuration built
 *    for Windows.
 *
 *    In other words, "xpc-config.h" below should be the one needed for
 *    your project and for the current operating system.
 *
 *    For example, for Windows compilation, the proper config.h is located
 *    in the w32 sub-directory for the project.  Like the normal autoconf
 *    config.h, it contains a number of "HAVE" macros.  However, these
 *    macros have to be edited by hand, as there seems to be no easy
 *    automake facility for Windows.
 *
 * \note
 *    There are different kinds of portability:
 *
 *    -  <b>Data portability</b>:
 *    -  <b>Portability between CPUs</b>:
 *    -  <b>Portability between compilers</b>:
 *    -  <b>Network (data structure) portability</b>:
 *    -  <b>Functional portability</b>:
 *    -  <b>Language portability (internationalization)</b>:
 *
 * \note
 *    Additional header files for portability are provided, and this is a
 *    good place to summarize them.  It should be obvious that "portability"
 *    can cover a lot of ground.
 *
 *    -  <b> atomix.h: </b> Provides atomic operations on the x86 processors
 *       under Linux and Windows.
 *    -  <b> cond_t.h: </b> Provides POSIX condition variables, as well as a
 *       provisional (and somewhat simple) implementation under Windows.
 *    -  <b> config.h: </b> Provides a set of macros that describe the
 *       feature set of the current host environment.  Created by the
 *       "configure" command.
 *    -  <b> xpc-config.h: </b> The same file as config.h, but renamed for
 *       direct installation into /usr/include.
 *    -  <b> cpu.h: </b> Provides definitions of CPU properties such as
 *       integer width and byte order.
 *    -  <b> critex_t.h: </b> Provides a wrapper for Windows critical
 *       sections, and an implementation of them using POSIX mutexes.
 *    -  <b> file_macros.h: </b> Provides the maximum sizes of filename
 *       components, host names, and UNC share names.
 *    -  <b> filer.h: </b> Provides uniform and convenient functions to
 *       perform common file operations, hiding the slight differences in
 *       standard C functions between Linux and Windows compilers.
 *    -  <b> ftwex.h: </b> Provides bit-checking functions for use with
 *       stat() and _stat().
 *    -  <b> ftw_macros.h: </b> Provides FTW macros for Windows builds.
 *    -  <b> gettext_support.h: </b> Provides the _() and N_() macros to
 *       support marking text output for language translation.
 *    -  <b> integers.h: </b> Provides definitions for standard 8-, 16-,
 *       32-, and 64-bit integer types.
 *    -  <b> macros.h: </b> Provides a number of macros useful in testing
 *       pointers, handles, socket descriptors, and file descriptors.
 *    -  <b> mutex_t.h: </b> Provides either Windows mutexes or POSIX
 *       mutexes.
 *    -  <b> nan_inf.h: </b> Provides functions to deal with non-numbers,
 *       infinity, and floating-point comparisons on IEEE floating-point
 *       hardware.
 *    -  <b> os.h: </b> Provides basic macros whose exact values depend on
 *       the operating system.
 *    -  <b> portable.h: </b> This file; it provides a mixed-bag of
 *       functions, currently mostly functions dealing with time. It even
 *       provides some conditional C++ support.
 *    -  <b> semaphore_t.h: </b>  Provides either Windows semaphores or POSIX
 *       semaphores.
 *    -  <b> socket_t.h: </b> Reconciles the differences in socket types and
 *       values on Linux versus Windows.
 *    -  <b> sock_wrapper.h: </b>  Provides either Windows sockets or POSIX
 *       sockets.
 *    -  <b> thread_macros.h: </b> Provides thread macros to help keep the
 *       threading code less cluttered with conditional defines.
 *    -  <b> ftw_w32.h: </b>  Provides a Windows implementation of the BSD
 *       ftw() [file-tree walker] function.
 *    -  <b> xthread.h: </b>  Provides either Windows threads or POSIX
 *       threads in a straight-forward set of functions.
 *
 * \note
 *    This macro is needed to define NAN in C code (as opposed to C++ code).
 *    However, defining it here doesn't work.  So we're going to try
 *    -std=c99 or -std=gnu99.
 *
\verbatim
            #define __USE_ISOC99   1
\endverbatim
 *
 * \note
 *    The header file os.h also describes some portability issues.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>             /* support for special XPC features       */

#if XPC_HAVE_TIME_H
#include <time.h>                   /* clock_t                                */
#endif

#if XPC_HAVE_SYS_TIME_H             /* see the OS's config.h header           */
#include <sys/time.h>               /* struct timeval and gettimeofday()      */
#endif

XPC_REVISION_DECL(portable)         /* extern void show_xpc_portable_info();  */

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    Since this module has Win32-specific constructs included in it, and
 *    we've set Doxyfile to pre-define only POSIX, Doxygen cannot generate
 *    some of the documentation in this file unless we artificially define
 *    WIN32 while Doxygen is processing this file.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define WIN32
#endif

/******************************************************************************
 * ISO C++ issues
 *------------------------------------------------------------------------*//**
 *
 *    This section covers certain C++ compiler issues.
 *
 * TYPE_NAME:
 *
 *    The support for 'typename' is variable between g++ and Microsoft C++.
 *    Microsoft even introduced code-breaking changes between Visual Studio
 *    .NET 2002 and 2003.  To try to get around those issues, we introduce a
 *    macro.  Ugly as hell.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __cplusplus

#if defined _MSC_VER          /* works with VS .NET 2003 (might not with 2002)*/
#define TYPE_NAME typename
#else
#define TYPE_NAME
#endif

#endif

/******************************************************************************
 * POSIX / BSD / Win32 Wedgies
 *------------------------------------------------------------------------*//**
 *
 *    This set of macros reconciles the names of functions and structures
 *    that are otherwise the same in function and number and type of
 *    arguments.  They try to map all APIs to POSIX, at least for the
 *    handful of functions we need in this project.
 *
 *    See macros.h and os.h for more information.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32
#include <winsock2.h>               /* needed to declare struct timeval (!)   */
#endif

/******************************************************************************
 * Portable C functions
 *-----------------------------------------------------------------------------
 *
 *    Global and portable functions for some common, basic tasks.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_is_empty_string (const char * s);
extern const char * xpc_boolean_string (cbool_t value);
extern cbool_t xpc_make_ms_timeval (struct timeval * tv, unsigned long ms);
extern void xpc_us_sleep (unsigned long us);
extern void xpc_us_sleep_poll (unsigned long us);
extern void xpc_ms_sleep (unsigned long ms);
extern double xpc_cpu_time (clock_t c1, clock_t c2);
extern cbool_t xpc_get_microseconds (struct timeval * c);
extern double xpc_double_time_ptr (struct timeval * c);
extern double xpc_double_time (struct timeval c);
extern cbool_t xpc_time_fix (struct timeval * c);
extern unsigned long xpc_time_add (struct timeval * c1, struct timeval c2);
extern unsigned long xpc_time_subtract (struct timeval * c1, struct timeval c2);
extern unsigned long xpc_time_difference_us
(
   struct timeval c1,
   struct timeval c2
);
extern unsigned long xpc_time_difference_ms
(
   struct timeval c1,
   struct timeval c2
);
extern double xpc_time_difference (struct timeval c1, struct timeval c2);
extern double xpc_time_difference_ptr                          /* deprecated  */
(
   struct timeval * c1,
   struct timeval * c2
);
extern void xpc_stopwatch_start (void);
extern double xpc_stopwatch_duration (void);
extern double xpc_stopwatch_lap (void);
extern const char * xpc_current_date ();

EXTERN_C_END

#endif                              /* XPC_PORTABLE_H                         */

/******************************************************************************
 * portable.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
