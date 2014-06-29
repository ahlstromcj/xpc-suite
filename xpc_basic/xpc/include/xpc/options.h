#ifndef XPC_OPTIONS_H
#define XPC_OPTIONS_H

/******************************************************************************
 * options.h
 *------------------------------------------------------------------------*//**
 *
 * \file          options.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2011-07-17 to 2011-07-17
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Declares functions for making it easier to parse command-line options
 *    in a brute-force method.
 *
 *//*-------------------------------------------------------------------------*/

#include "macros.h"                    /* cbool_t                             */

/******************************************************************************
 * External functions
 *-----------------------------------------------------------------------------
 *
 *    Provided by the C module.
 *
 *----------------------------------------------------------------------------*/

extern cbool_t xpc_parse_flag
(
   int * argcp,
   const char * const argv [],
   const char * target
);
extern cbool_t xpc_parse_string
(
   const char ** value,
   int * argcp,
   int argc,
   const char * const argv [],
   const char * target
);

#endif                                       /* XPC_OPTIONS_H                 */

/******************************************************************************
 * options.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
