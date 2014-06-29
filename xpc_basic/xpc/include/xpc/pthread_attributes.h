#ifndef XPC_PTHREAD_ATTRIBUTES_H
#define XPC_PTHREAD_ATTRIBUTES_H

/******************************************************************************
 * pthread_attributes.h
 *------------------------------------------------------------------------*//**
 *
 * \file          pthread_attributes.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       05/01/2008-05/04/2008
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This utility provides global convenience functions for accessing the
 *    POSIX pthread_attr_t structure.
 *
 * \posix
 * \win32
 *    For the XPC library, we're going to employ the pthreads_w32 LGPL library
 *    to map the POSIX threads API onto Windows.  We will assume the authors
 *    and their colleagues can do a much better job of this mapping than we
 *    could.
 *
 *    Also see pthread_attributes.c for more documentation.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>              /* portability functions and macros    */

EXTERN_C_DEC                           /* pthreads are /not/ C++              */
#include <pthread.h>                   /* pthreads library functions          */
EXTERN_C_END

/******************************************************************************
 * Global functions (documented in pthread_attributes.c)
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t pthread_attributes_init (pthread_attr_t * attributes);
extern cbool_t pthread_attributes_initialize
(
   pthread_attr_t * attributes,
   int detachstate,
   int policy,
   int priority,
   int inherit,
   int scope,
   size_t stacksize,
   size_t stackguardsize
);
extern cbool_t pthread_attributes_get
(
   pthread_attr_t * attributes,
   int * detachstate,
   int * policy,
   int * priority,
   int * inherit,
   int * scope,
   size_t * stacksize,
   size_t * stackguardsize
);
extern void pthread_attributes_show (pthread_attr_t * attributes);
extern cbool_t pthread_attributes_set_stack_size
(
   pthread_attr_t * pattr,
   size_t stacksize
);
extern size_t pthread_attributes_get_stack_size (pthread_attr_t * pattr);
extern cbool_t pthread_attributes_set_detach_state
(
   pthread_attr_t * pattr,
   int s
);

EXTERN_C_END

#endif         // XPC_PTHREAD_ATTRIBUTES_H

/******************************************************************************
 * pthread_attributes.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
