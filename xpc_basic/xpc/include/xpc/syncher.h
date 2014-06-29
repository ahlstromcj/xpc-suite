#ifndef XPC_SYNCHER_H
#define XPC_SYNCHER_H

/******************************************************************************
 * syncher.h
 *------------------------------------------------------------------------*//**
 *
 * \file          syncher.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2008-04-29
 * \updates       2012-05-28
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This utility provides a basic locking mechanism that works under POSIX
 *    or Win32.
 *
 * \win32
 *    It does not use the CRITICAL_SECTION for Windows.  Instead, we rely on
 *    a port of the POSIX pthreads API to Windows:
 *
 *       http://sourceware.org/pthreads-win32/
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>                 /* portability functions and macros */

EXTERN_C_DEC                              /* pthreads are /not/ C++           */
#include <pthread.h>                      /* pthreads library functions       */
EXTERN_C_END

#define XPC_SYNCHER     pthread_mutex_t   /* pthreads mutex data type         */

/******************************************************************************
 * xpc_syncher_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides a very light wrapper for the syncher-object.
 *
 *    This wrapper includes the syncher-object, and a boolean to indicate if
 *    is already owned.
 *
 * \win32
 *    The Win32 CRITICAL_SECTION is always recursive.  It can be entered
 *    almost any number of times, as long as it is left an equal number of
 *    times.
 *
 * \posix
 *    The POSIX mutex in normally 'fast'.  There are two other flavors, for
 *    a total of three:
 *
 *       -# Fast.  The mutex is locked, but if the same thread tries to lock
 *          it again, deadlock occurs.
 *       -# Error-checked.  If the mutex is locked again, the locking
 *          function returns an error code.
 *       -# Recursive.  The mutex can be locked multiple times, and
 *          unlocking must be done the same number of times.
 *
 *    See the xpc_syncher_create() function for more information.
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   /**
    *    Defines a platform-dependent "critical-section" synchronization
    *    object.
    *
    * \posix
    *    The syncher-object is implemented by a POSIX-threads mutex,
    *    pthread_mutex_t.
    *
    * \win32
    *    The syncher-object is implemented by a Win32 CRITICAL_SECTION
    *    structure.
    */

   XPC_SYNCHER m_Syncher;

   /**
    *    Indicates if the syncher-object is currently owned by a thread.
    *
    *    Generally, a syncher-object object has to be un-owned before it
    *    can be destroyed.  This flag helps detect such a situation.  Note
    *    that it is effectively locked when being modified.
    */

   cbool_t m_Syncher_Owned;

   /**
    *    Indicates that the syncher-object is in a usable state.
    *
    *    It is not clear that such a variable is really necessary, but we
    *    can run a lot of unit-tests to find out.
    */

   cbool_t m_Syncher_Ready;

} xpc_syncher_t;

/******************************************************************************
 * Global functions
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_syncher_enter (xpc_syncher_t * xs);
extern cbool_t xpc_syncher_tryenter (xpc_syncher_t * xs);
extern cbool_t xpc_syncher_leave (xpc_syncher_t * xs);
extern cbool_t xpc_syncher_create (xpc_syncher_t * xs, cbool_t recursive);
extern cbool_t xpc_syncher_destroy (xpc_syncher_t * xs);

EXTERN_C_END

#endif         // XPC_SYNCHER_H

/******************************************************************************
 * syncher.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
