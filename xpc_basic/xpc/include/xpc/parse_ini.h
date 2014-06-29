#ifndef XPC_PARSE_INI_H
#define XPC_PARSE_INI_H

/******************************************************************************
 * parse_ini.h
 *------------------------------------------------------------------------*//**
 *
 * \file          parse_ini.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-07-14 to 2011-03-20
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a way to set options using a DOS/Windows INI-style
 *    configuration file.
 *
 *----------------------------------------------------------------------------*/

#include <xpc/macros.h>             /* EXTERN_C_DEC and EXTERN_C_END          */

/******************************************************************************
 * Global functions
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_is_INI_line (const char * line);
extern char ** xpc_argv_from_INI
(
   const char * filespec,
   const char * section,
   int * argc_return,
   char ** buffer_return
);
extern void xpc_delete_argv (char ** argv, char * buffer);
extern FILE * xpc_write_INI_header (const char * filespec, const char * section);
extern cbool_t xpc_append_INI_header (FILE * fhandle, const char * section);
extern cbool_t xpc_append_argv_to_INI (FILE * fhandle, int argc, char ** argv);
extern cbool_t xpc_write_INI_item
(
   FILE * fhandle,
   const char * option,
   const char * value
);
extern cbool_t xpc_write_INI_footer (FILE * fhandle, const char * filespec);
extern cbool_t xpc_argv_to_INI
(
   const char * filespec,
   const char * section,
   int argc,
   char ** argv                  // const char * argv []
);
extern cbool_t xpc_parse_boolean
(
   int argc,
   char ** argv,
   int argi,
   const char * optionname,
   cbool_t * boolresult
);

EXTERN_C_END

#endif      // XPC_PARSE_INI_H

/******************************************************************************
 * parse_ini.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
