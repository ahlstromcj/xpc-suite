#ifndef XPC_XWINSOCK_H
#define XPC_XWINSOCK_H

/******************************************************************************
 * xwinsock.h
 *------------------------------------------------------------------------*//**
 *
 * \file          xwinsock.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       05/04/2008-06/11/2008
 * \updates       2008-05-04 to 2010-06-09
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This header file provides a workaround for Microsoft's WinSock.h
 *    header madness.
 *
 * \win32
 *    The problem is that some files (and we haven't tracked them down)
 *    include the old WinSock 1 header file (winsock.h).  If this file gets
 *    included before windows.h is included, then, when windows.h is
 *    included, it includes winsock2.h, and this newer file attempts to
 *    redefine a lot of items (e.g. FD_SET) that are already defined in
 *    their old versions.
 *
 *    The result is a cascade of error messages.  With this file, the
 *    compilation aborts early and explains what is going on.
 *
 *    Therefore, include xwinsock.h in lieu of winsock2.h, please.
 *    For your own sanity.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32                           /* WIN32                               */

/******************************************************************************
 * See if we are targetting a legacy microsoft windows platform.
 *----------------------------------------------------------------------------*/

#if defined(_MSC_VER) || defined(_WIN32)

#define	_MSWINDOWS_

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0501
#undef	_WIN32_WINNT
#define	_WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINNT
#define	_WIN32_WINNT 0x0501
#endif

#endif   // defined(_MSC_VER) || defined(_WIN32)

/******************************************************************************
 * Older Microsoft SDKs require the windows.h header file first.
 *----------------------------------------------------------------------------*/

#if ! defined(_MSC_VER) || _MSC_VER < 1400
#include <windows.h>
#endif

/******************************************************************************
 * These Microsoft header files MUST be included first on newer SDK.
 *----------------------------------------------------------------------------*/

#include <ws2tcpip.h>
#include <winsock2.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <windows.h>
#endif

#endif                                 /* WIN32                               */

#endif                                 /* XPC_XWINSOCK_H                      */

/******************************************************************************
 * xwinsock.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
