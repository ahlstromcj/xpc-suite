#ifndef XPC_TINY_MACROS_H
#define XPC_TINY_MACROS_H

/******************************************************************************
 * tiny_macros.h
 *------------------------------------------------------------------------*//**
 *
 * \file          tiny_macros.h
 * \library       xpchello application
 * \author        Chris Ahlstrom
 * \date          2008-05-09
 * \updates       2011-12-31
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides macros to support the special features of the XPC xpchello
 *    application.
 *
 * \warning
 *    This module is a very tiny subset of the macros.h module included in
 *    the XPC library, libxpc.
 *
 *    Please do not include this file in any code outside of the xpchello
 *    project.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32

#error "Windows not yet supported by the xpchello application."

#include "../w32/xpc-config.h"         /* defines all available HAVE macros   */
#ifdef _INCLUDE_XPC_XPC_CONFIG_H
#error Picking up UNIX version of xpc-config.h, check your solution settings.
#endif

#else                                  /* if not Win32                        */

#include <xpc/xpc-config.h>            /* defines all available HAVE macros   */

#endif                                 /* End of Win32                        */

#if XPC_HAVE_UNISTD_H
#include <unistd.h>                    /* here, gives us _POSIX_VERSION       */
#endif

/******************************************************************************
 * WIN32
 *------------------------------------------------------------------------*//**
 *
 *    We want to pepper our code with "WIN32", not "_WIN32".
 *
 *//*-------------------------------------------------------------------------*/

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

/******************************************************************************
 * POSIX
 *------------------------------------------------------------------------*//**
 *
 *    We want this macro defined in POSIX-style functions can be used.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef _POSIX_VERSION

#ifndef POSIX
#define POSIX                    1
#endif

#ifdef WIN32
#error WIN32 and POSIX macros defined simultaneously
#endif

#endif

/******************************************************************************
 * POSIX_SUCCESS
 *------------------------------------------------------------------------*//**
 *
 *    POSIX_SUCCESS is returned from a string function when it has processed
 *    a message that is not an error.
 *
 *    It is also a non-error value for most POSIX-conformant functions.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef POSIX_SUCCESS
#define POSIX_SUCCESS              0
#endif

/******************************************************************************
 * POSIX_ERROR
 *------------------------------------------------------------------------*//**
 *
 *    POSIX_ERROR is returned from a string function when it has processed an
 *    error.
 *
 *    It indicates that an error is in force.  Normally, the caller
 *    then uses this indicator to set a class-based error message.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef POSIX_ERROR
#define POSIX_ERROR              (-1)
#endif

#ifndef __cplusplus

/******************************************************************************
 * false
 *------------------------------------------------------------------------*//**
 *
 *    false is zero (0).
 *
 *//*-------------------------------------------------------------------------*/

#ifndef false
#define false         0
#endif

/******************************************************************************
 * true
 *------------------------------------------------------------------------*//**
 *
 *    true is one (1).  We are all one.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef true
#define true          1
#endif

#endif

/******************************************************************************
 * gettext() support
 *------------------------------------------------------------------------*//**
 *
 *    We were going to try to support gettext() in every library but
 *    xpchello, but, if building from the xpc_suite root, then all libraries
 *    need to support it, since they all use the _() macro in their code.
 *    Therefore, they all must get a valid declaration of gettext() from the
 *    libintl.h header file.
 *
 *//*-------------------------------------------------------------------------*/

#if (XPC_ENABLE_NLS == 1)

#ifndef ENABLE_NLS
#define ENABLE_NLS  1
#endif

#define _(msg)             gettext (msg)
#define gettext_noop(msg)  msg
#define N_(msg)            gettext_noop (msg)

#else                               /* no XPC_ENABLE_NLS defined             */

/******************************************************************************
 * _()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a placeholder for future support of gettext() in the
 *    unit-test library.
 *
 *    Right now, this is too much to worry about.
 *
 *//*-------------------------------------------------------------------------*/

#define _(msg)             (msg)

/******************************************************************************
 * N_()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a placeholder for future support of gettext() in the
 *    unit-test library.
 *
 *    Right now, this is too much to worry about.
 *
 *//*-------------------------------------------------------------------------*/

#define N_(msg)             msg

#endif                              /* End of XPC_ENABLE_NLS                 */

/******************************************************************************
 * nullptr
 *------------------------------------------------------------------------*//**
 *
 *    This macro replaces the oft-disparaged \a NULL macro.
 *
 *    See the documentation for the macro.h module for full details.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef nullptr                     /* nullptr define starts                  */
#ifdef __cplusplus                  /* C++                                    */
#define nullptr            0        /* for fopen() and generic pointers       */
#else                               /* C                                      */
#define nullptr   ((void *) 0)      /* try this on for C size                 */
#endif                              /* C/C++                                  */
#endif                              /* nullptr define ends                    */

/******************************************************************************
 * __func__ macro
 *------------------------------------------------------------------------*//**
 *
 *    GCC provides three magic variables which hold the name of the current
 *    function, as a string.  The first of these is `__func__', which is
 *    part of the C99 standard.  `__FUNCTION__' is another name for
 *    `__func__'.  `__PRETTY_FUNCTION__' is the decorated version of the
 *    function name.
 *
 *    Visual Studio defines only __FUNCTION__, so a definition is provided
 *    below.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX                                          /* POSIX                */

#ifndef __func__
#if __STDC_VERSION__ < 199901L
#if __GNUC__ >= 2
#define __func__        __FUNCTION__                  /* bald func names      */
#else
#define __func__        "<unknown>"
#endif
#endif
#endif

#else                                                 /* Win32                */

#ifndef __func__
#define __func__        __FUNCTION__
#endif

#endif                                                /* end of POSIX/Win32   */

#endif                                                /* XPC_TINY_MACROS_H    */

/******************************************************************************
 * tiny_macros.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
