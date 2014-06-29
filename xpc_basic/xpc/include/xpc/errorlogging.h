#ifndef XPC_ERRORLOGGING_H
#define XPC_ERRORLOGGING_H

/******************************************************************************
 * errorlogging.h
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlogging.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2008-05-06 to 2011-11-25
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides basic error codes and strings logging that are usable in
 *    C/C++ code.
 *
 *    This module provides some very simple methods for showing console
 *    error messages.  It also provides error strings for the XPC library.
 *    This module is intended to simplify internationalization later.
 *
 * \note
 *    Many of the functions in this module can be disabled by including
 *    nullmacros.h in lieu of the errorlogging.h header file.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>             /* EXTERN_C_DEC and EXTERN_C_END          */
#include <stdio.h>                  /* FILE *                                 */
XPC_REVISION_DECL(errorlogging)     /* extern void show_errorlogging_info()   */

/******************************************************************************
 * __func__ functions
 *------------------------------------------------------------------------*//**
 *
 *    These macros encapsulate what has turned out to be a common idiom.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NO_ERRORLOG

#define xpc_errprint_func(x)
#define xpc_warnprint_func(x)
#define xpc_infoprint_func(x)
#define xpc_strerrprint_func(x, err)
#define xpc_strerrnoprint_func(x)

#else

#define xpc_errprint_func(x)            xpc_errprintex((x), __func__)
#define xpc_warnprint_func(x)           xpc_warnprintex((x), __func__)
#define xpc_infoprint_func(x)           xpc_infoprintex((x), __func__)
#define xpc_strerrprint_func(x, err)    xpc_strerrprintex((x), __func__, err)
#define xpc_strerrnoprint_func(x)       xpc_strerrnoprintex((x), __func__)

#endif   // XPC_NO_ERRORLOG

/******************************************************************************
 * Debug functions
 *-----------------------------------------------------------------------------
 *
 *    These functions are part of the errorlogging facilty.  However, if the
 *    code is not compiled for debugging mode, they provide no code at all.
 *    However, xpc_dbginfoprintf() must remain defined, since it has a variable
 *    number of arguments.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DEBUG                        /* DEBUG                                  */

#define xpc_dbginfoprint_func(x)           xpc_dbginfoprintex((x), __func__)

EXTERN_C_DEC
extern void xpc_dbginfoprint (const char * msg);
extern void xpc_dbginfoprintex (const char * msg, const char * label);
extern void xpc_print_debug (const char * msg);
extern void xpc_dbginfoprintf (const char * fmt, ...);
EXTERN_C_END

#else                               /* not DEBUG                              */

#define xpc_dbginfoprint(x)

EXTERN_C_DEC
extern void xpc_dbginfoprintf (const char * fmt, ...);
EXTERN_C_END

#define xpc_dbginfoprintex(x, y)
#define xpc_dbginfoprint_func(x)
#define xpc_print_debug(x)

#endif                              /* end of DEBUG                           */

/******************************************************************************
 * Print flag character functions
 *----------------------------------------------------------------------------
 *
 *    These functions make it possible for callers to use flag characters to
 *    adhere to the error-logging conventions in
 *    fprintf(xpc_logfile(),...) statements.
 *
 *    Normally, though, it is easier to rely on xpc_errprintf(),
 *    xpc_warnprintf(), xpc_dbginfoprintf(), and xpc_infoprintf().  The
 *    exception occurs when a multi-line message is desired, with marker
 *    characters on each line.
 *
 *//*-------------------------------------------------------------------------*/

EXTERN_C_DEC

extern const char * xpc_dbgmark (void);
extern const char * xpc_infomark (void);
extern const char * xpc_warnmark (void);
extern const char * xpc_errmark (void);
extern const char * xpc_usermark (void);
extern cbool_t xpc_usecolor_set (cbool_t flag);
extern cbool_t xpc_usecolor (void);

EXTERN_C_END

/******************************************************************************
 * xpc_errlevel_t
 *------------------------------------------------------------------------*//**
 *
 *    This enumeration controls the amount a information output by the
 *    errorlogging functions.
 *
 * \todo
 *    Probably want to rename this type as xpc_errlevel_t for consistency
 *    with our conventions, also prepend XPC_ to the names.
 *
 *//*-------------------------------------------------------------------------*/

typedef enum
{
   XPC_ERROR_LEVEL_NONE,            /**< --quiet [default for daemons?]       */
   XPC_ERROR_LEVEL_ERRORS,          /**< --errors [often the default]         */
   XPC_ERROR_LEVEL_WARNINGS,        /**< --warnings                           */
   XPC_ERROR_LEVEL_INFO,            /**< --info                               */
   XPC_ERROR_LEVEL_ALL,             /**< Removes the above restrictions       */
   XPC_ERROR_LEVEL_NOT_APPLICABLE   /**< In case of an error in the settings. */

} xpc_errlevel_t;

/******************************************************************************
 * Normal logging
 *-----------------------------------------------------------------------------
 *
 *    This section provides declarations of external functions.
 *
 * \todo
 *
 *    We need to consider hiding the functions that simply support the
 *    private functions or command-line options.  [One set of examples is
 *    xpc_open_logfile(), xpc_append_logfile(), and xpc_close_logfile().]
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_timestamps_set (cbool_t flag, cbool_t setbase);
extern cbool_t xpc_timestamps (void);
extern cbool_t xpc_errlevel_set (int errlevel);
extern cbool_t xpc_parse_errlevel (int argc, char * argv[]);
extern cbool_t xpc_parse_errlevel_nohelp (int argc, char * argv[]);
extern cbool_t xpc_parse_leveltoken (const char * errlevelstring);
extern cbool_t xpc_filename_check (const char * filename);
extern xpc_errlevel_t xpc_option_errlevel (const char * cmdswitch);
extern xpc_errlevel_t xpc_errlevel (void);
extern const char * xpc_errlevel_string (xpc_errlevel_t errlev);
extern void xpc_showerrlevel (void);
extern void xpc_showerr_version (void);
extern FILE * xpc_logfile (void);
extern cbool_t xpc_open_logfile (const char * logfilename);
extern cbool_t xpc_append_logfile (const char * logfilename);
extern cbool_t xpc_close_logfile (void);
extern cbool_t xpc_shownothing (void);
extern cbool_t xpc_showerrors (void);
extern cbool_t xpc_showwarnings  (void);
extern cbool_t xpc_showinfo (void);
extern cbool_t xpc_showall (void);
extern cbool_t xpc_showdebug (void);
extern void xpc_error_help (void);
extern void xpc_application_help (const char * helptext);

#ifdef XPC_NO_ERRORLOG

#define xpc_errprint(msg)                 xpc_no_op()
#define xpc_errprintf(msg, ...)           xpc_no_op()
#define xpc_errprintex(msg, label)        xpc_no_op()
#define xpc_warnprint(msg)                xpc_no_op()
#define xpc_warnprintf(msg, ...)          xpc_no_op()
#define xpc_warnprintex(msg, label)       xpc_no_op()
#define xpc_infoprint(msg)                xpc_no_op()
#define xpc_infoprintf(msg, ...)          xpc_no_op()
#define xpc_infoprintml(msg, ...)         xpc_no_op()
#define xpc_infoprintex(msg, label)       xpc_no_op()

extern xpc_inline void xpc_no_op () XPC_INLINE_CODE(static int i = 0)

#else

extern void xpc_errprint (const char * msg);
extern void xpc_errprintf (const char * fmt, ...);
extern void xpc_errprintex (const char * msg, const char * label);
extern void xpc_warnprint (const char * msg);
extern void xpc_warnprintf (const char * fmt, ...);
extern void xpc_warnprintex (const char * msg, const char * label);
extern void xpc_infoprint (const char * msg);
extern void xpc_infoprintf (const char * fmt, ...);
extern void xpc_infoprintml (const char * fmt, ...);
extern void xpc_infoprintex (const char * msg, const char * label);

#endif

extern void xpc_print (const char * msg);
extern void xpc_lkprintf (const char * fmt, ...);
extern void xpc_strerrprint (const char * errmsg, int errnum);
extern void xpc_strerrprintex
(
   const char * errmsg,
   const char * label,
   int errnum
);
extern int xpc_strerrnoprint (const char * errmsg);
extern int xpc_strerrnoprintex (const char * errmsg, const char * label);
extern int xpc_getlasterror (void);
extern const char * xpc_getlasterror_text (void);
extern const char * xpc_getlasterror_text_ex (int errorcode);
extern const char * xpc_getlasterror_text_r
(
   int errcode,
   char * textbuffer,
   size_t textbuffersize
);
extern void xpc_format_error_string
(
   const char * errmsg,
   const char * label,
   const char * syserr
);
extern cbool_t xpc_is_thisptr (const void * pointer, const char * funcname);
extern cbool_t xpc_not_nullptr (const void * pointer, const char * funcname);

EXTERN_C_END

/******************************************************************************
 * Buffering enumeration
 *------------------------------------------------------------------------*//**
 *
 *    This section provides declarations of more external values.
 *
 *    This enumeration provides constants used in modifying the kind of
 *    buffering used with stderr and stdout.
 *
 *//*-------------------------------------------------------------------------*/

enum
{
   XPC_ERROR_NOT_BUFFERED,       /**< Do not buffer; always flush characters  */
   XPC_ERROR_BLOCK_BUFFERED,     /**< Buffer by a block of data               */
   XPC_ERROR_LINE_BUFFERED       /**< Flush the buffer after each newline     */
};

/******************************************************************************
 * Daemon logging and other functions
 *-----------------------------------------------------------------------------
 *
 *    This section provides declarations of more external functions.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_syslogging_set (cbool_t flag);
extern int xpc_get_priority (xpc_errlevel_t errlev);
extern cbool_t xpc_syslogging (void);
extern cbool_t xpc_synchusage_set (cbool_t flag);
extern cbool_t xpc_synchusage (void);
extern cbool_t xpc_buffering_set (int btype);
extern void xpc_flush_error_log (void);

EXTERN_C_END

#endif         /* XPC_ERRORLOGGING_H */

/******************************************************************************
 * errorlogging.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
