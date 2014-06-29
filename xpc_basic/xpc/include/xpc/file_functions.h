#if ! defined XPC_FILE_FUNCTIONS_H
#define XPC_FILE_FUNCTIONS_H

/******************************************************************************
 * file_functions.h
 *------------------------------------------------------------------------*//**
 *
 * \file    file_functions.h
 *
 *    Provides the declarations for safe replacements for the various C
 *    file functions.
 *
 * \author  Chris Ahlstrom
 * \date    2012-06-09
 * \updates 2012-06-23
 * \version $Revision$
 *
 *    Also see the file_functions.c module.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>              /* define cbool_t, etc.                */

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* file-name info <stdlib.h>           */
#elif defined __BORLANDC__
#include <stdio.h>                     /* file-name info for old compiler     */
#endif

#if XPC_HAVE_FCNTL_H
#include <fcntl.h>                     /* open(), O_RDONLY, etc.              */
#endif

#if ! defined _MSC_VER                 /* non-Microsoft compiler              */
#include <fcntl.h>                     /* open()                              */
#endif

/******************************************************************************
 * Global function declarations in the C namespace
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t is_file_name_good (const char * filename);
extern cbool_t is_file_mode_good (const char * mode);
extern cbool_t is_path_rooted (const char * filespec);
extern cbool_t xpc_file_access (const char * targetfile, int mode);
extern cbool_t xpc_file_exists (const char * targetfile);
extern cbool_t xpc_file_readable (const char * targetfile);
extern cbool_t xpc_file_writable (const char * targetfile);
extern cbool_t xpc_file_accessible (const char * targetfile);
extern cbool_t xpc_file_executable (const char * targetfile);
extern cbool_t xpc_file_is_directory (const char * targetfile);
extern FILE * xpc_file_open (const char * filename, const char * mode);
extern FILE * xpc_file_open_for_read (const char * filename);
extern FILE * xpc_file_open_for_write (const char * filename);
extern FILE * xpc_file_create_for_write (const char * filename);
extern cbool_t xpc_file_close (FILE * filehandle, const char * filename);
extern cbool_t xpc_file_delete (const char * filename);
extern int xpc_path_slash (const char * filespec);
extern int xpc_platform_slash (void);
extern cbool_t xpc_directory_create (const char * dirname);
extern cbool_t xpc_directory_delete (const char * filename);
extern cbool_t xpc_directory_delete_recursive (const char * dirname);
extern off_t xpc_file_handle_seek (int filehandle, off_t offset, int whence);
extern cbool_t xpc_file_copy
(
   const char * newfile,
   const char * oldfile
);
extern int xpc_file_handle_open (const char * filename, int flags, int mode);
extern int xpc_file_handle_close (int filehandle, const char * filename);

EXTERN_C_END

#endif            /* XPC_FILE_FUNCTIONS_H   */

/******************************************************************************
 * End of file_functions.h
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
