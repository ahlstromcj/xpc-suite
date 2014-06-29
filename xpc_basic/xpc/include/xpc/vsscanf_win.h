#if ! defined XPC_VSSCANF_H
#define XPC_VSSCANF_H

/******************************************************************************
 * vsscanf_win.h
 *------------------------------------------------------------------------*//**
 *
 * \file    vsscanf_win.h
 *
 *    Provides the declarations for safe replacements for the vsscanf() and
 *    vsscanf_s() functions that are missing from Visual Studio.
 *
 * \author  Chris Ahlstrom
 * \date    2011-06-21
 * \updates 2012-06-11
 * \version $Revision$
 *
 *    See the vsscanf_win.c module for much more information.
 *
 *//*-------------------------------------------------------------------------*/

#include <stdarg.h>                    /* va_list and friends                 */
#include <xpc/portable.h>              /* define EXTERN_C_DEC                 */

/******************************************************************************
 * Global function declarations in the C namespace
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern int vsscanf
(
   const char * buffer,
   const char * format,
   va_list argptr
);

EXTERN_C_END

#endif            /* XPC_VSSCANF_H     */

/******************************************************************************
 * End of vsscanf_win.h
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
