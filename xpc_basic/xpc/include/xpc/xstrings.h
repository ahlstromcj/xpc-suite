#ifndef XPC_XSTRINGS_H
#define XPC_XSTRINGS_H

/******************************************************************************
 * xstrings.h
 *------------------------------------------------------------------------*//**
 *
 * \file          xstrings.h
 *
 *    Provides some C functions for analyzing and rebuilding strings.
 *
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2005-06-25
 * \updates       2012-06-22
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides string-handling functions that are safer or
 *    have more features than the C string library functions.  It also
 *    provides a C version of an std::string.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>              /* autoconf definitions                */
XPC_REVISION_DECL(xstrings)            /* extern void show_xstrings_info()    */

#if XPC_HAVE_ERRNO_H
#include <errno.h>                     /* declares errno, errno_t             */
#ifndef XPC_HAVE_ERRNO_T
typedef int errno_t;
#endif
#endif

#if XPC_HAVE_STDARG_H
#include <stdarg.h>                    /* declares va_list, etc.              */
#endif

/******************************************************************************
 * CASE_SENSITIVE and CASE_INSENSITIVE
 *------------------------------------------------------------------------*//**
 *
 *    These flags are simply more readable ways of specifying the type of
 *    matching for some of the string-comparison functions.
 *
 * \var CASE_INSENSITIVE
 *    The same as 'false'.
 *
 * \var CASE_SENSITIVE
 *    The same as 'true'.
 *
 *//*-------------------------------------------------------------------------*/

#define CASE_INSENSITIVE   false
#define CASE_SENSITIVE     true

/******************************************************************************
 * XPC_NATIVE_SLASH
 *------------------------------------------------------------------------*//**
 *
 *    This macro defines the default path-separator character for the
 *    platform.
 *
 * \win32
 *    This character is the backslash ('\').  Thanks, Bill.
 *
 * \posix
 *    This character is the forward slash ('/').  Thanks, Brian and Ken.
 *
 * \private
 *    This macro is exposed for unit testing.  Do not use it directly;
 *    instead, use the more versatile xpc_path_slash() function.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
#define XPC_NATIVE_SLASH    '/'
#else
#define XPC_NATIVE_SLASH    '\\'
#endif

/******************************************************************************
 * xpc_string_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides a very primitive C version of the C++ string class.
 *
 *    This type provides support for the xpc_string_n_cat_r() function.  It
 *    is mean to serve as an opaque structure to manage the concatenation or
 *    assignment of strings into a C character buffer.
 *
 *    Unlike the C++ string class, there's nothing dynamic about this
 *    structure.  The caller sets up a buffer of a definite known size using
 *    the xpc_string_init() function.  The string never expands beyond this
 *    limit.  Once xpc_string_n_cat_r() fills the buffer, nothing more can
 *    be added to the string represented by the buffer.
 *
 *    The caller is responsible for deallocating the buffer.
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   const char * str_buffer;   /**< Buffer, which the caller must provide      */
   char * str_scratch;        /**< Holds current pointer position during work */
   size_t str_limit;          /**< Size limit of the buffer, in bytes         */
   size_t str_length;         /**< The current length of the string, in bytes */

} xpc_string_t;

/******************************************************************************
 * UNIT_TEST_XPC_STRING_N_CAT_R_IMP
 *------------------------------------------------------------------------*//**
 *
 *    This macro allows the developer to turn on the unit tests for the
 *    tricky xpc_string_n_cat_r_impl() function.
 *
 *    This function is dangerous to use.  However, it is used privately by
 *    a public function which is fairly easy and safe to use, and so it
 *    needs to be tested at least once.
 *
 * \private
 *    This macro exposes xpc_string_n_cat_r_impl() for unit testing.  Do not
 *    enable it unless you modify that function.
 *
 *//*-------------------------------------------------------------------------*/

#define UNIT_TEST_XPC_STRING_N_CAT_R_IMP     1        /* testing enabled      */

/******************************************************************************
 * Global functions
 *-----------------------------------------------------------------------------
 *
 *    These functions are the public interface for the xstrings.c module.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

#ifdef USE_OBSOLETE_FUNCTION
extern int xpc_path_slash (const char * filespec);
#endif   // USE_OBSOLETE_FUNCTION

extern cbool_t xpc_string_not_void (const char * s);
extern cbool_t xpc_string_is_void (const char * s);
extern cbool_t xpc_strings_match (const char * p, const char * x);
extern cbool_t xpc_string_tokens_match (const char * p, const char * x);

/*
 * These functions are replaced, but still called internally.
 *
 * extern cbool_t xpc_string_n_copy_impl
 * (
 *    char * destination,
 *    const char * source,
 *    size_t n
 * );
 * extern cbool_t xpc_string_n_cat_deprecated
 * (
 *    char * destination,
 *    const char * source,
 *    size_t n
 * );
 *
 */

extern cbool_t xpc_string_n_cat_r
(
   xpc_string_t * mystring,
   const char * source
);

#if UNIT_TEST_XPC_STRING_N_CAT_R_IMP

extern size_t xpc_string_n_cat_r_impl
(
   char ** destination,
   const char * source,
   size_t n
);

#endif

extern cbool_t xpc_string_copy
(
   char * dest,
   size_t destsize,
   const char * source
);
extern cbool_t xpc_string_n_copy
(
   char * destination,
   size_t destsize,
   const char * source,
   size_t srccount
);
extern cbool_t xpc_string_nbytes_cat
(
   char * destination,
   size_t destsize,
   const char * source,
   size_t srccount
);
extern cbool_t xpc_var_string_copycat
(
   char * destination,
   size_t destsize,
   int stringcount,
   va_list vlist
);
extern cbool_t xpc_string_copycat
(
   char * destination,
   size_t destsize,
   int stringcount,
   ...
);
extern cbool_t xpc_string_cat
(
   char * destination,
   size_t destsize,
   const char * source
);
extern char * xpc_string_unsafe_cat
(
   char * destination,
   const char * source,
   size_t sourcesize
);
extern cbool_t xpc_string_n_cat
(
   char * destination,
   size_t destsize,
   int stringcount,
   ...
);
extern int xpc_sprintg
(
   char * destination,
   size_t destsize,
   const char * format,
   ...
);
extern int xpc_var_sprintf
(
   char * destination,
   size_t destsize,
   const char * format,
   va_list vlist
);
extern cbool_t xpc_errno_string
(
   char * destination,
   size_t destsize,
   errno_t errnum
);
extern int xpc_string_scanf
(
   const char * input,
   size_t inputsize,
   const char * format,
   ...
);
extern cbool_t xpc_string_n_compare
(
   const char * s1,
   const char * s2,
   size_t n
);
extern cbool_t xpc_string_equivalent
(
   const char * s1,
   const char * s2
);
extern cbool_t xpc_string_identical
(
   const char * s1,
   const char * s2
);
extern cbool_t xpc_string_match
(
   const char * s1,
   const char * s2,
   cbool_t matchcase
);
extern char * xpc_string_tokens
(
   char * source,
   const char * delimiters,
   char ** saveptr
);
extern size_t xpc_string_length (const char * source, size_t maxlen);
extern size_t xpc_strlen (const char * source);

/******************************************************************************
 * String-structure functions
 *----------------------------------------------------------------------------*/

extern cbool_t xpc_string_init
(
   xpc_string_t * pst,
   char * buffer,
   size_t size
);
extern cbool_t xpc_string_validate (xpc_string_t * pst);
extern const char * xpc_string_c_str (const xpc_string_t * pst);
extern size_t xpc_string_max (const xpc_string_t * pst);
extern size_t xpc_string_size (const xpc_string_t * pst);
extern const char * xpc_ip_to_string
(
   unsigned ipnumber,
   char * destination,
   size_t n,
   cbool_t usehex
);
extern unsigned xpc_string_to_ip (const char * strdata);

EXTERN_C_END

#endif                                    /* XPC_XSTRINGS_H                   */

/******************************************************************************
 * xstrings.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
