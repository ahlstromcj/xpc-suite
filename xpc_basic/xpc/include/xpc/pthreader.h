#ifndef XPC_PTHREADER_H
#define XPC_PTHREADER_H

/******************************************************************************
 * pthreader.h
 *------------------------------------------------------------------------*//**
 *
 * \file          pthreader.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       05/03/2008-05/17/2008
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This utility provides global convenience functions for accessing the
 *    POSIX interface
 *
 * \posix
 * \win32
 *    For the XPC library, we're going to employ the pthreads_w32 LGPL library
 *    to map the POSIX threads API onto Windows.  We will assume the authors
 *    and their colleagues can do a much better job of this mapping than we
 *    could.
 *
 *    Also see pthreader.c and pthread_attributes.c for more documentation.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>              /* portability functions and macros    */

EXTERN_C_DEC                           /* pthreads are /not/ C++              */
#include <pthread.h>                   /* pthreads library functions          */
EXTERN_C_END

/******************************************************************************
 * pthreader_func_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides a convenient type definition for a POSIX thread function.
 *
 *//*-------------------------------------------------------------------------*/

typedef void * (* pthreader_func_t) (void *);

/******************************************************************************
 * Global functions (documented in pthread_attributes.c)
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern pthread_t pthreader_null_thread (void);
extern cbool_t pthreader_is_null_thread (pthread_t th);
extern cbool_t pthreader_is_valid_thread (pthread_t th);
extern cbool_t pthreader_is_cancelled (void * rvalue);
extern pthread_t pthreader_create
(
   const pthread_attr_t * thread_attr,
   pthreader_func_t thread_callback,
   void * thread_data
);
extern void pthreader_yield (void);
extern void * pthreader_join (pthread_t th);
extern cbool_t pthreader_cancel (pthread_t th);
extern int pthreader_detach (pthread_t th);
extern cbool_t pthreader_set_cancel
(
   cbool_t cancelenable,
   cbool_t cancelasync
);

EXTERN_C_END

#endif         // XPC_PTHREADER_H

/******************************************************************************
 * pthreader.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
