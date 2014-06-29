#ifndef XPC_ENVIRONMENT_H
#define XPC_ENVIRONMENT_H

/******************************************************************************
 * environment.h
 *------------------------------------------------------------------------*//**
 *
 * \file          environment.h
 * \author        Chris Ahlstrom
 * \date          2012-06-21
 * \updates       2012-06-23
 * \version       $Revision$
 *
 *    Provides declarations for replacements for C environment-variable
 *    functions.
 *
 *    Also see the environment.c module.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/integers.h>              /* INT_MAX, needed by all callers      */
#include <xpc/portable.h>              /* define cbool_t, etc.                */
XPC_REVISION_DECL(environment)         /* extern void show_environment_info() */

/******************************************************************************
 * Global function declarations in the C namespace
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern size_t xpc_get_environment
(
   char * destination,
   size_t destsize,
   const char * varname
);
extern char * xpc_unsafe_getenv
(
   char * destination,
   size_t destsize,
   const char * varname
);
extern cbool_t xpc_put_environment
(
   const char * varname,
   const char * value
);

EXTERN_C_END

#endif            /* XPC_ENVIRONMENT_H   */

/******************************************************************************
 * End of environment.h
 *----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
