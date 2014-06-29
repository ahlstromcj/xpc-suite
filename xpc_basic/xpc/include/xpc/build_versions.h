#ifndef XPC_BUILD_VERSIONS_H
#define XPC_BUILD_VERSIONS_H

/******************************************************************************
 * build_versions.h
 *------------------------------------------------------------------------*//**
 *
 * \file          build_versions.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       06/17/2006-06/21/2008
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides functions to display the compiler settings in force when the
 *    library was built.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* cbool_t and macro definitions       */
XPC_REVISION_DECL(build_versions)      /* show_xpc_build_versions_info()      */

/******************************************************************************
 * External functions
 *-----------------------------------------------------------------------------
 *
 *    Provided by the C module.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern void show_compiler_info (void);
extern void show_build_settings (void);
extern void show_have_macros (void);

EXTERN_C_END

#endif                                 /* XPC_BUILD_VERSIONS_H                */

/******************************************************************************
 * build_versions.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
