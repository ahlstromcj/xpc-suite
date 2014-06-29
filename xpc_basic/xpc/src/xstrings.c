/******************************************************************************
 * xstrings.c
 *------------------------------------------------------------------------*//**
 *
 * \file          xstrings.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2006-07-26
 * \updates       2012-06-21
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Utilities for dealing with file-name string tokens in a simple way.
 *
 *    This module provides functions for manipulating strings.  Obviously,
 *    we don't want to duplicate the functionality of the C++ std::string
 *    class.  However, we do need convenient string concatenation in C code.
 *
 *    This module provides a C structure, xpc_string_t, which helps the
 *    caller keep track of concatenation operations.
 *
 *    Many other string functions are supplied that do not use xpc_string_t,
 *    but provide safe alternatives to some vulnerable string functions.
 *    These functions can be use in lieu of strcpy(), strcat(), and related
 *    functions, and will get rid of warnings that flawfinder finds.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* xpc_errprint() and other functions  */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/numerics.h>              /* atox() function                     */
#include <xpc/xstrings.h>              /* string utilities (DOS/UNIX form)    */
XPC_REVISION(xstrings)

#if XPC_HAVE_CTYPE_H
#include <ctype.h>                     /* isspace(), tolower() macros         */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* strncpy()                           */
#endif

#if XPC_HAVE_TIME_H
#include <time.h>                      /* ctime()                             */
#endif

#if defined _MSC_VER                   /* Microsoft compiler                  */
#include <wchar.h>                     /* _vsnwprintf_s()                     */
#include "vsscanf_win.h"               /* djinned up vsscanf_s() function     */

#endif

/******************************************************************************
 * X_STATIC
 *------------------------------------------------------------------------*//**
 *
 *    A private macro.
 *
 *    This macro is used in exposing a private implementation function for
 *    unit testing.
 *
 * \private
 *    See UNIT_TEST_XPC_STRING_N_CAT_R_IMP for more information.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

#if UNIT_TEST_XPC_STRING_N_CAT_R_IMP
#define X_STATIC
#else
#define X_STATIC static
#endif

X_STATIC size_t
xpc_string_n_cat_r_impl
(
   char ** destination,
   const char * source,
   size_t n
);

#ifdef USE_OBSOLETE_FUNCTION

   The function of the same name in file_functions.c is better.

/******************************************************************************
 * xpc_path_slash()
 *------------------------------------------------------------------------*//**
 *
 *    Returns the system-specific path separator.
 *
 *    This function provides the ability to handle forward slashes as well
 *    as back slashes.  It detects the first slash or backslash character in
 *    the string, if any, and returns the character to use as the
 *    path-component separator.
 *
 *    If no slash or backslash is found, or if no file specification is
 *    provided, the native slash for the environment is assumed.
 *
 * \return
 *    Returns either a slash ("/") or a backslash ("\") character.
 *
 * \note
 *    The issue of the slash character is quite tricky.  Do we want to force
 *    all new filenames to use the forward slash?  Thus, we're still working
 *    out the semantics of this little function.
 *
 * \obsolete
 *    This function had a small trick for efficiency.  Its result was a
 *    static value, and this value was set only once.  Thus, whatever
 *    happened on the first call was wired in for the duration of the
 *    application.  No changes in style were allowed during the run.
 *    However, this feature makes it difficult to handle arbitrary
 *    filenames, so it was dropped.
 *
 * \unittests
 *    xpc_filename_unit_test_01()
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_path_slash
(
   const char * filespec   /**< Option file specification to search inside    */
)
{
   int result = XPC_NATIVE_SLASH;               /* the default "slash"        */
   if (not_nullptr(filespec))                   /* is a filename provided?    */
   {
      if (xpc_strlen(filespec) > 0)             /* is the filename not empty? */
      {
         const char * slashptr = strchr(filespec, '/');
         if (not_nullptr(slashptr))
            result = '/';
         else
         {
            slashptr = strchr(filespec, '\\');
            if (not_nullptr(slashptr))
               result = '\\';
         }
      }
   }
   return result;
}

#endif   // USE_OBSOLETE_FUNCTION

/******************************************************************************
 * xpc_string_not_void()
 *------------------------------------------------------------------------*//**
 *
 *    Tests that a string is not empty and has non-space characters.
 *
 *    Provides essentially the opposite test that xpc_string_is_void()
 *    provides.
 *
 * \note
 *    There is another function, xpc_is_void_string(), that just checks
 *    the pointer and the length.  It is defined in the portable.c
 *    module.
 *
 * \return
 *    Returns 'true' if the pointer is valid, the string has a non-zero
 *    length, and is not just white-space.
 *
 *    The definition of white-space is provided by the isspace()
 *    function/macro.
 *
 * \unittests
 *    xpc_strings_unit_test_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_not_void
(
   const char * s       /**< The string pointer to check for emptiness        */
)
{
   cbool_t result = false;
   if (not_nullptr(s))
   {
      while (*s != 0)
      {
         if (! isspace(*s))
         {
            result = true;
            break;
         }
         s++;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_is_void()
 *------------------------------------------------------------------------*//**
 *
 *    Tests that a string is empty or has only white-space characters.
 *
 *    Meant to have essentially the opposite result of
 *    xpc_string_not_void().  The meaning of empty is special here, as it
 *    refers to a string being useless as a token:
 *
 *          -  The string pointer a nullptr, or
 *          -  The string is of zero length, or
 *          -  The string has only white-space characters in it, where the
 *             isspace() macro provides the definition of white-space
 *
 * \return
 *    Returns 'true' if the pointer is valid, the string is a null pointer,
 *    it has a zero length, or is only white-space.
 *
 * \unittests
 *    xpc_strings_unit_test_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_is_void
(
   const char * s       /**< The string pointer to check for emptiness        */
)
{
   cbool_t result = is_nullptr(s);
   if (! result)
      result = ! xpc_string_not_void(s);

   return result;
}

/******************************************************************************
 * xpc_strings_match()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings for a form of semantic equality.
 *
 *    The xpc_strings_match() function returns true if:
 *
 *       -  The primary and secondary comparison items are both empty as
 *          per xpc_string_not_void():
 *          -  Both are a nullptr
 *          -  Both are zero length
 *          -  Both have only whitespace characters
 *       -  The comparison items are otherwise identical in character
 *          content.
 *
 *    The biggest use for this function is in dealing with filenames as
 *    represented in the filenames.c module.
 *
 * \return
 *    Returns 'true' if both strings are "empty" as per the definition
 *    above, or if both strings are identical in characters, with the case
 *    of the characters being significant.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    xpc_strings_unit_test_03()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_strings_match
(
   const char * p,            /**< The primary string in the comparison       */
   const char * x             /**< The secondary string in the comparison     */
)
{
   cbool_t result;
   if (xpc_string_is_void(x))
      result = xpc_string_is_void(p);
   else
      result = (strcmp(p, x) == 0);

   return result;
}

/******************************************************************************
 * xpc_string_tokens_match()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two token strings for a form of semantic equality.
 *
 *    The xpc_string_tokens_match() function returns true if:
 *
 *       -  The primary and secondary comparison items are both not empty
 *          as per xpc_string_not_void().  This is contrary to the test in
 *          xpc_strings_match().
 *       -  The comparison items are otherwise identical in character
 *          content until the end of the second string.  If the second
 *          string is longer, the match fails.  This feature allows partial
 *          matching by abbreviating the secondary string.
 *
 *    The biggest use for this function is in dealing with command tokens
 *    where the user wants to be able to abbreviate the token for speed.
 *
 * \return
 *    Returns 'true' if both strings are not "empty", and if both strings
 *    are identical in characters, with the case of the characters being
 *    significant.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  xpc_strings_unit_test_08 ()
 *    -  xpc_strings_unit_test_09 ()
 *
 * \todo
 *    Have this function strip or ignore leading and trailing white-space,
 *    as long as there is a legal token in there somewhere.
 *
 * \todo
 *    Write a more powerful function that uses regular-expression matching.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_tokens_match
(
   const char * p,            /**< The primary string in the comparison       */
   const char * x             /**< The secondary string in the comparison     */
)
{
   cbool_t result = ! xpc_string_is_void(x) && ! xpc_string_is_void(p);
   if (result)
   {
      size_t plen = xpc_strlen(p);
      size_t xlen = xpc_strlen(x);
      result = xlen <= plen;
      if (result)
         result = (strncmp(p, x, xlen) == 0);
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_copy_impl()
 *------------------------------------------------------------------------*//**
 *
 *    A safe version of strncpy().
 *
 *    This version has the following features over strncpy():
 *
 *       -  Checks the pointer parameters for validity before using them.
 *       -  Checks that the source string had a non-zero length.
 *       -  Gives the caller an indication if the whole string was copied.
 *       -  Guarantees proper null termination, even if not all of the
 *          string could be copied.
 *       -  Provides error-log output to help the developer.
 *
 *    One might call it an obsessive-compulsive version of strncpy()!
 *
 * \warning
 *    -  This function is not built for speed.  It is built to be airtight.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the whole source
 *    string was able to be copied.  Otherwise, it returns 'false'.
 *
 * \unittests
 *    xpc_strings_unit_test_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_copy_impl
(
   char * destination,     /**< The destination for the copy operation.       */
   const char * source,    /**< The source string to copy.                    */
   size_t n                /**< Allocated size of the destination string.     */
)
{
   cbool_t result = false;
   if (not_nullptr_2(destination, source))
   {
      if (n > 0)
      {
         size_t slen = xpc_strlen(source);
         if (slen > 0)
         {
            (void) strncpy(destination, source, n);   /* use standard func    */
            destination[n-1] = 0;                     /* mandate termination  */
            if (slen < n)                             /* could all be copied? */
               result = true;
            else
               xpc_warnprint_func(_("destination too short"));
         }
         else
            xpc_warnprint_func(_("source is empty"));
      }
      else
         xpc_errprint_func(_("destination is zero-length"));
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_cat_deprecated()
 *------------------------------------------------------------------------*//**
 *
 *    An error-checked version of strncat().
 *
 *    Although strncat() is safer than strncpy(), it doesn't return any
 *    indication that the buffer may not have been long enough to hold the
 *    whole concatenation.
 *
 *    The destination and its size parameter apply to the whole destination
 *    buffer.  This function also does a sanity check on the destination.
 *    It checks the length of the destination via xpc_strlen().  If larger than
 *    specified, something is wrong, and false is returned with no action
 *    occurring.
 *
 * \warning
 *    -  Like strcat() or strncat(), this function relies on an initial
 *       setup, which would involve a call to strcpy(), strncpy(), or
 *       xpc_string_n_copy_impl().  <i>The caller must insure that there
 *       is a null terminator already in the destination</i>.
 *    -  The size parameter, \a n, includes the null terminator.
 *    -  This function is not built for speed.  It is built to be airtight.
 *       For a faster function, see xpc_string_n_cat_r_impl().  However,
 *       that function is meant purely for internal use.
 *
 * \bug
 *    This function provides no return of the amount of space remaining in
 *    the destination buffer; this return would make it more efficient for
 *    the caller.  For a function that does this, see xpc_string_n_cat_r_impl().
 *
 * \return
 *    Returns 'true' if the parameters were valid and the whole source
 *    string was able to be concatenated without causing the destination
 *    buffer to overflow.  Otherwise, it returns 'false'.  Even if false,
 *    partial string concatenation may have occurred.
 *
 * \unittests
 *    xpc_strings_unit_test_05()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_cat_deprecated
(
   char * destination,     /**< The destination for the copy operation        */
   const char * source,    /**< The source string to copy                     */
   size_t n                /**< Allocated size of the destination string      */
)
{
   cbool_t result = false;
   if (not_nullptr_2(destination, source))
   {
      if (n > 0)
      {
         size_t dlen = xpc_strlen(destination);
         size_t slen = xpc_strlen(source);
         if (dlen < (n - 1))                       /* n includes the null     */
         {
            if (slen > 0)
            {
               size_t m = n - dlen - 1;            /* deduct dest len + null  */
               (void) strncat(destination, source, m);
               if (m >= slen)                      /* could all be copied?    */
                  result = true;
               else
                  xpc_warnprint_func(_("destination too short"));
            }
            else
               xpc_warnprint_func(_("source empty"));
         }
         else
            xpc_errprint_func(_("destination already full"));
      }
      else
         xpc_errprint_func(_("destination is zero-length"));
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_cat_r()
 *------------------------------------------------------------------------*//**
 *
 *    A "repeatable" version of xpc_string_n_cat().
 *
 *    This function is meant to be called repeatedly to concatenate a series
 *    of strings into a character buffer.
 *
 *    This function requires that the string be set up first.  The caller
 *    must allocate a buffer, allocate a xpc_string_t structure, and then
 *    pass these two pointers, along with the size of the buffer, to the
 *    xpc_string_init() function.
 *
 * \usage
 *    -# The caller must allocate a character buffer of a known size.
 *    -# The caller must provide a xpc_string_t structure for the string.
 *    -# The caller must call xpc_string_init() with these items, to
 *       make the string ready for concatenation.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the whole source
 *    string was able to be concatenated.  Otherwise, 'false' is returned.
 *    As in xpc_string_n_cat(), there's no way to know if any characters
 *    were copied.
 *
 *    However, the xpc_string_t object can be examined, if desired.
 *
 * \todo
 *    Write the necessary accessor functions for xpc_string_t.
 *
 * \unittests
 *    xpc_strings_unit_test_05c()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_cat_r
(
   xpc_string_t * mystring,   /**< The "string" to concatenate the source to. */
   const char * source        /**< Source string to concatenate to mystring.  */
)
{
   cbool_t result = false;
   if (xpc_string_validate(mystring))           /* source checked in impl.    */
   {
      if (mystring->str_length < (mystring->str_limit - 1))
      {
         size_t count = xpc_string_n_cat_r_impl
         (
            &mystring->str_scratch,             /* must use scratch           */
            source, mystring->str_limit
         );
         if (count > 0)
         {
            result = count == xpc_strlen(source);     /* slows things down    */
            mystring->str_length += count;      /* we /must/ count it here    */
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_cat_r_impl()
 *------------------------------------------------------------------------*//**
 *
 *    A "repeatable" version of xpc_string_n_cat() for private use.
 *
 *    This function is meant to be called repeatedly to concatenate a series
 *    of strings into a character buffer.
 *
 *    This function fixes the "bug" of the xpc_string_n_cat() function by
 *    returning the amount of space copied into the caller's buffer.  It
 *    makes the function threadsafe.  It also makes the process more
 *    efficient (we think).
 *
 * \private
 *    However, the usage of this function is pretty demanding.  Therefore,
 *    an easier and safer public wrapper function, xpc_string_n_cat(), is
 *    provided.
 *
 * \usage
 *    -# <i>The caller must ensure that there is a null terminator already
 *       in the destination</i>, and this null terminator must be the first
 *       character in the buffer.   This means a statement such as
 *       <tt> destination[0] = 0; </tt> must be used.
 *    -# <i>The caller must ensure that a temporary pointer to the
 *       destination is provided, and passed to the function, because there
 *       is a side-effect on this pointer</i>.  The caller must pass the
 *       address of this temporary pointer to the function.
 *    -# The caller should make the first call using the full size of the
 *       buffer, which includes the null.
 *    -# The caller should examine the return value.  If it is one less than
 *       the full size of the buffer, no more concatenation is possible, and
 *       the last character of the destination will be a null character.
 *    -# If the return value is 0, nothing was copied, and no more
 *       concatenation is possible.
 *    -# If a non-zero is returned, then the caller should deduct the return
 *       value from the current amount of space in the buffer, and pass it
 *       to the next invocation of the function.
 *    -# The caller can also check that the return value was as large as
 *       expected.
 *    -# The caller must also beware of the side-effect on the pointer
 *       value.
 *
 *    A good example of the usage of this function can be found in
 *    xpc_addrinifo_flags_string().  The unit test noted below also covers
 *    all of the pitfalls of this function.  Because this function is so
 *    tricky, it is normally hidden using the UNIT_TEST_XPC_STRING_N_CAT_R_IMP
 *    macro, and is used privately as the implementation of
 *    xpc_string_n_cat_r().  Note that the caller can check if the number of
 *    characters copied was at least as many as the source provided, so that
 *    check is included here only for unit testing.
 *
 * \note
 *    This function also provides a good illustration of when to use
 *    xpc_dbginfoprint_func() versus xpc_errprint_func().
 *
 * \return
 *    Returns the number of characters copied into the caller's buffer if
 *    the parameters were valid.  Otherwise, zero (0) is returned.  The null
 *    terminator is \e not included in the returned value.  Also see the
 *    side-effect.
 *
 * \sideeffect
 *    The caller could also increment the destination pointer to speed up
 *    the function, but this function does that for the caller.
 *
 * \unittests
 *    xpc_strings_unit_test_05b() if UNIT_TEST_XPC_STRING_N_CAT_R_IMP is
 *    defined and is true (non-zero).
 *
 *//*-------------------------------------------------------------------------*/

X_STATIC size_t
xpc_string_n_cat_r_impl
(
   char ** destination, /**< [in,out] Points to pointer for copy operation.   */
   const char * source, /**< [in] The source string to copy.                  */
   size_t n             /**< Size of destination string including the null.   */
)
{
   size_t result = 0;
   if (not_nullptr_2(destination, source))
   {
      char * dptr = *destination;
      if (n > 0)                                   /* total amount available  */
      {
         const char * sptr = source;               /* copy the pointer        */
         if (n > 1)                                /* room for non-nulls?     */
         {
            size_t limit = n - 1;                  /* copy all but the null   */
            int ch;                                /* current source char     */
            while ((ch = *sptr++) != 0)
            {
               *dptr++ = (char) ch;                /* copy to destination     */
               if (++result == limit)              /* destination filled?     */
                  break;                           /* yes, it is              */
            }
         }
         *dptr = 0;                                /* force null termination  */
         *destination = dptr;                      /* side-effect             */

#if UNIT_TEST_XPC_STRING_N_CAT_R_IMP               /* only needed for testing */
         if (result > 0)
         {
            if (result < xpc_strlen(source))
               xpc_warnprint_func(_("destination too short"));
         }
         else
         {
            xpc_dbginfoprint_func(_("source empty"));
         }
#endif
      }
      else
      {
         xpc_dbginfoprint_func(_("destination is zero-length"));
      }
   }
   return result;
}

/******************************************************************************
 * xpc_impl_string_copy()
 *------------------------------------------------------------------------*//**
 *
 *    An internal function to assist other copying functions.
 *
 *    All parameters are assumed to have been guaranteed valid by the
 *    caller.
 *
 *    See the discussion at xpc_string_copy() for details.
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param source
 *    The null-terminated string to be copied.
 *
 * \param sourcelimit
 *    An optional number of characters to which to limit the source string.
 *    The caller does not need to guarantee that it is not longer than the
 *    actual length of the source; the copying stops at the null character.
 *    -  If 0, the limit is set to the length of the source.
 *    -  If greater than the actual length of the source, the limit is set
 *       to the length of the source.
 *
 * \return
 *    Returns 'true' if the parameters were valid, so that the whole source
 *    string could be copied (without truncation).  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_01() [indirectly]
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_13() [indirectly]
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_impl_string_copy
(
   char * destination,
   size_t destsize,
   const char * source,
   size_t sourcelimit
)
{
   cbool_t result = false;
   cbool_t ok = not_nullptr(destination);
   if (ok)
   {
      *destination = 0;
      ok = not_nullptr(source);
      if (ok)
         ok = destsize > 0;

      if (! ok)
         xpc_errprint_func(_("zero-sized destination"));
   }
   if (ok)
   {
      size_t length = xpc_strlen(source);          /* inefficient             */
      if ((sourcelimit > length) || (sourcelimit == 0))
         sourcelimit = length;

      if (sourcelimit <= INT_MAX)
      {

         /*
          * This code checks the length of the source string, and decides
          * if it can copy the whole string, or has to limit the string and
          * enforce the null terminator.
          */

#if defined _MSC_VER

         int rcode;
         size_t count = _TRUNCATE;                 /* overloaded parameter!!  */
         if (sourcelimit < destsize)               /* truncation impossible   */
         {
            destsize = sourcelimit + 1;
            count = sourcelimit;
         }
         rcode = strncpy_s                         /* guaranteed null byte    */
         (
            destination, destsize, source, count
         );
         if (rcode == 0)
            result = true;
         else
            xpc_warnprint_func(_("xpc_string_copy truncation"));

#else

         if (sourcelimit < destsize)               /* truncation impossible   */
         {
            destsize = sourcelimit + 1;
            result = true;
         }
         else                                      /* truncation definite     */
            xpc_warnprint_func(_("xpc_string_copy truncation"));

         (void) strncpy(destination, source, destsize);
         destination[destsize-1] = 0;

#endif

      }
      else
         xpc_errprint_func(_("xpc_string_copy source longer than INT_MAX"));
   }
   return result;
}

/******************************************************************************
 * xpc_string_copy()
 *------------------------------------------------------------------------*//**
 *
 *    A global function to make safer copies than does strncpy().
 *
 * \replaces
 *    -  strcpy() where the size of the destination is known.
 *    -  strncpy() where n is used for the size of the destination.
 *    -  strncpy_s() [Microsoft]
 *
 *    The problem with strncpy() is that, while it prevents writing beyond
 *    the buffer, it can return a string that does not have a null
 *    terminator character at the end, when a string gets truncated.
 *    The caller may get a nasty suprise when using this unexpectedly long
 *    string.
 *
 * \win32
 *    With the Microsoft compiler, strncpy_s() is used with the _TRUNCATE
 *    parameter.  It returns a code that indicates success or failure.
 *
 * \posix
 *    With other compilers, we use the dangerous strcpy() function, but
 *    guarantee that the result is valid by checking the source's strlen().
 *    If not, then we allow a truncated string to be returned, just as for
 *    the Microsoft compiler.  Less efficient, but very safe.
 *
 *    Note that xpc_string_copy() does \e not preserve the ordering of operands
 *    of strncpy().
 *
 *    Also note that this function is semantically \e different from
 *    strncpy().  In strncpy(), the size parameter is the number of bytes to
 *    copy.  In xpc_string_copy(), the size parameter is the number of bytes to
 *    limit the copy to.  The actual replacement for strncpy() is the new
 *    xpc_string_n_copy() function.
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters that can be copied, plus one.  The
 *    space needed for the terminating null character must be included in
 *    the size of the destination buffer.
 *
 * \param source
 *    The null-terminated string to be copied.
 *
 * \return
 *    Returns 'true' if the parameters were valid, so that the whole source
 *    string could be copied (without truncation).  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_copy
(
   char * destination,
   size_t destsize,
   const char * source
)
{
   cbool_t result = xpc_impl_string_copy(destination, destsize, source, 0);
   return result;
}

/******************************************************************************
 * xpc_string_n_copy()
 *------------------------------------------------------------------------*//**
 *
 *    Copies only the desired number of bytes from a string.
 *
 * \replaces
 *    -  strncpy(), where the size of the destination is known, and
 *       only the first n bytes of the source string need to be copied.
 *
 *    Note that this function is semantically \e different from
 *    strncpy().  In strncpy(), the size parameter is the number of bytes to
 *    copy.  In xpc_string_copy(), the size parameter is the number of bytes to
 *    limit the copy to.  The actual replacement for strncpy() is the new
 *    xpc_string_n_copy() function.
 *
 *    This function stops copying characters under the following conditions:
 *
 *       -  The destination becomes full.  The last character position in
 *          the destination will hold a null terminator.
 *       -  The source count has been reached.
 *       -  The null terminator in the source has been reached.
 *
 *    What does xpc_string_n_copy() give the caller that xpc_string_copy()
 *    does not?  It saves determining the size of the source via an internal
 *    call to strlen().
 *
 *    Note that xpc_string_n_copy() is implemented using xpc_string_copy().
 *
 * \todo
 *    Is it worth modifying all the client code of common::stringncopy() to
 *    use the newer, more standard ordering, for consistency?
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters that the destination can hold, plus
 *    one.  The space needed for the terminating null character must be
 *    included in this count.
 *
 * \param source
 *    The null-terminated string to be copied.
 *
 * \param srccount
 *    The maximum number of bytes to copy from the \e source.  If set to 0,
 *    then the actual length of the source string will be used.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_copy
(
   char * destination,
   size_t destsize,
   const char * source,
   size_t srccount
)
{
   cbool_t result = xpc_impl_string_copy
   (
      destination, destsize, source, srccount
   );
   return result;
}

/******************************************************************************
 * xpc_string_nbytes_cat()
 *------------------------------------------------------------------------*//**
 *
 *    Concatenates only the desired number of bytes from a string.
 *
 * \replaces
 *    -  strncat(), where the size of the destination is known, and
 *       only the first n bytes of the source string need to be concatenated.
 *
 *    This function stops concatenating characters under the following
 *    conditions:
 *
 *       -  The destination becomes full.  The last character position in
 *          the destination will hold a null terminator.
 *       -  The source count has been reached.
 *       -  The null terminator in the source has been reached.
 *
 *    Note that xpc_string_n_copy() is implemented using xpc_string_n_copy().
 *
 * \param destination
 *    The pointer to the area to receive the concatenation.  It must provide
 *    a null terminator byte.
 *
 * \param destsize
 *    The maximum number of characters that the destination can hold, plus
 *    one.  The space needed for the terminating null character must be
 *    included in this count.
 *
 * \param source
 *    The null-terminated string to be copied.
 *
 * \param srccount
 *    The maximum number of bytes to copy from the \e source.  If set to 0,
 *    then the actual length of the source string will be used.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_TBD()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_nbytes_cat
(
   char * destination,
   size_t destsize,
   const char * source,
   size_t srccount
)
{
   cbool_t result = false;
   char * newdest = strchr(destination, 0);
   size_t currentamount = newdest - destination;   /* what's already there?   */
   destsize -= currentamount;
   if (destsize > srccount)
   {
      result = xpc_impl_string_copy(newdest, destsize, source, srccount);
   }
   else
   {
      xpc_warnprintf
      (
         "%s: %s %d %s %s",
         __func__, _("destination to small to concatenate"),
         srccount, _("bytes of"), source
      );
   }
   return result;
}

/******************************************************************************
 * xpc_var_string_copycat()
 *------------------------------------------------------------------------*//**
 *
 *    A global function that combines strcpy() and strcat() with destination
 *    checking, but using a variable argument list [va_list] parameter.
 *
 * \replaces
 *    -  strcpy()
 *    -  strcat()
 *
 *    We side-step the unsafe function issue by writing our own loop --
 *    using the safe versions of string functions is overkill (slower).
 *
 *    This function stands on its own, but is also used by xpc_string_copycat().
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param stringcount
 *    The number of strings to be dealt with.  The first one is copied to
 *    the beginning of the buffer, and the rest are concatenated.
 *
 * \param vlist
 *    The list of null-terminated strings to be copied, then concatenated.
 *    See the discussion for the xpc_string_copycat() function.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Invalid parameters
 *    include:
 *       -  Null destination pointer
 *       -  Destination too small
 *       -  Any null source-string pointer
 *    Even if the parameters were invalid, the destination still can be used
 *    safely -- it might be empty or truncated, though.  The function tries
 *    to fill up the destination fully when truncation occurs.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_05() [indirect test]
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_var_string_copycat
(
   char * destination,
   size_t destsize,
   int stringcount,
   va_list vlist
)
{
   cbool_t result = not_nullptr(destination);
   if (result)
   {
      result = stringcount > 0;
      if (result)
      {
         result = (destsize > 0) && (destsize <= INT_MAX);
         if (result)
         {
            char * target = destination;
            int targetsize = (int) destsize;          /* INT_MAX vs UINT_MAX  */
            int i;
            target[0] = 0;                            /* create empty string  */
            for (i = 0; i < stringcount; i++)
            {
               const char * source = va_arg(vlist, const char *);
               if (not_nullptr(source))
               {
                  int slen = (int) xpc_strlen(source);
                  result = targetsize > slen;
                  if (! result)
                     slen = targetsize - 1;

                  (void) strncat(target, source, (size_t) slen);
                  if (result)
                  {
                     target += slen;
                     targetsize -= slen;
                  }
                  else
                  {
                     xpc_warnprintf
                     (
                        "%s '%s' %d %s %s",
                         _("string"), target, i+1, _("truncated in"),
                         __func__
                     );
                     break;
                  }
               }
               else
               {
                  xpc_errprintf
                  (
                     "%s %d %s %s",
                      _("string"), i+1, _("is null in"), __func__
                  );
                  result = false;
                  break;
               }
            }
         }
         else
         {
            destination[0] = 0;
            xpc_errprint_func(_("destination size out of range"));
         }
      }
      else
         xpc_errprint_func(_("bad string-count parameter"));
   }
   return result;
}

/******************************************************************************
 * xpc_string_copycat()
 *------------------------------------------------------------------------*//**
 *
 *    A global function that combines strcpy() and strcat() with destination
 *    checking.
 *
 * \replaces
 *    -  strcpy()
 *    -  strcat()
 *
 *    This function merely constructs a va_list structure and passes it to
 *    xpc_var_string_copycat().
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param stringcount
 *    The number of strings to be dealt with.  The first one is copied to
 *    the beginning of the buffer, and the rest are concatenated.  This
 *    number must match the number of string parameters that follow.
 *    It is necessary because va_arg() offers no way to determine that the
 *    end of the arguments have been reached.
 *
 * \param ...
 *    The null-terminated strings to be copied, then concatenated.
 *    If a string is simply empty, it is ignored, and processing continues
 *    with the rest of the strings.  If the string is a null pointer,
 *    processing is aborted, and a shorter string will result than if all
 *    the arguments were valid.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.  The function tries to fill up the
 *    destination fully when truncation occurs.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_05()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_copycat
(
   char * destination,
   size_t destsize,
   int stringcount,
   ...
)
{
   cbool_t result;
   va_list arguments;
   va_start(arguments, stringcount);
   result = xpc_var_string_copycat(destination, destsize, stringcount, arguments);
   va_end(arguments);
   return result;
}

/******************************************************************************
 * xpc_string_cat()
 *------------------------------------------------------------------------*//**
 *
 *    A global function that implements strcat() and strncat() safely.
 *
 * \replaces
 *    -  strcat(), where the caller has effectively created an empty initial
 *       string by setting destination[0] to a null character.
 *    -  strncat(), in case where the destination size is known, and the
 *       whole source string is to be concatenated to the end of the
 *       destination.  For copying only part of the source string, see the
 *       function xpc_string_n_cat().
 *
 *    It is like xpc_string_copycat(), except that the existing string, if any,
 *    in the destination, is skipped, so that concatenation can occur.
 *
 *    This function merely calls
 *
\verbatim
      xpc_string_n_cat(destination, destsize, 1, source);
\endverbatim
 *
 * \param destination
 *    The pointer to the area to receive the concatenation.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.  The space for the existing string in the buffer must also be
 *    included in this count.
 *
 * \param source
 *    The single null-terminated strings to be concatenated to the existing
 *    string.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.  The function tries to fill up the
 *    destination fully when truncation occurs.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_17()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_cat
(
   char * destination,
   size_t destsize,
   const char * source
)
{
   return xpc_string_n_cat(destination, destsize, 1, source);
}

/******************************************************************************
 * xpc_string_unsafe_cat()
 *------------------------------------------------------------------------*//**
 *
 *    A global function that implements strncat() exactly, except that it
 *    does check for null pointers.
 *
 * \replaces
 *    -  strcat(), where the destination size cannot be known.
 *    -  strncat(), where the destination size cannot be known.
 *    -  strcpy(), by setting destination[0] = 0 first
 *    -  strncpy(), by setting destination[0] = 0 first
 *
 *    Empirically, we have seen two use cases for strncat():
 *
 *       -  The size of the destination buffer is known, and the caller
 *          uses this size as the amount of data to concatenate.  This is an
 *          unsafe usage, and should be replaced with our xpc_string_cat()
 *          implementation.
 *       -  The size of the destination buffer is unknown, and the caller
 *          uses the size parameter as the number of source bytes.  This
 *          usage is also unsafe, but we can't do anything about it.  So we
 *          can use xpc_string_unsafe_cat() simply to shut the compiler up.
 *
 *    This function can also be used to replace strcat() and strcpy() in
 *    cases were we have no way of knowing the size of the destinations.
 *    We can support a number of use cases with this one "unsafe" function.
 *    Here are the use cases, assuming the following declarations:
 *
\verbatim
               char * target;
               const char * source;
               size_t sourcecount = X;
\endverbatim
 *
 *       -  Using as a direct replacement for strncat():
\verbatim
               char * result = xpc_string_unsafe_cat(target, source, sourcecount);
\endverbatim
 *       -  Using as a direct replacement for strcat():
\verbatim
               char * result = xpc_string_unsafe_cat(target, source, 0);
\endverbatim
 *       -  Using as a direct replacement for strncpy(), by setting up the
 *          target as an empty string first:
\verbatim
               char * result;
               target[0] = 0;
               result = xpc_string_unsafe_cat(target, source, sourcecount);
\endverbatim
 *       -  Using as a direct replacement for strcpy(), by setting up the
 *          target as an empty string first:
\verbatim
               char * result;
               target[0] = 0;
               result = xpc_string_unsafe_cat(target, source, 0);
\endverbatim
 *
 * \param destination
 *    The pointer to the area to receive the concatenation.  The caller is
 *    solely responsible for guaranteeing that the size of this buffer is
 *    sufficient for the result of the concatenation.
 *
 * \param source
 *    The single null-terminated strings to be concatenated to the existing
 *    string.
 *
 * \param sourcesize
 *    The maximum number of characters to be copied.  If set to 0, then
 *    strcat() is called instead of strncat().  Note that the caller must
 *    ensure there is room for the result in the destination.
 *
 * \return
 *    Returns the result of the strncat() call if the pointers were valid.
 *    Otherwise, it returns a null pointer.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_18()
 *
 *//*-------------------------------------------------------------------------*/

char * xpc_string_unsafe_cat
(
   char * destination,
   const char * source,
   size_t sourcesize
)
{
   char * result = nullptr;
   if (not_nullptr_2(destination, source))
   {
      if (sourcesize == 0)
      {
#ifdef _MSC_VER
#pragma warning ( suppress : 4996 )
#endif
         result = strcat(destination, source);
      }
      else
      {
#ifdef _MSC_VER
#pragma warning ( suppress : 4996 )
#endif
         result = strncat(destination, source, sourcesize);
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_cat()
 *------------------------------------------------------------------------*//**
 *
 *    A global function that implements strcat() and strncat() safely.
 *
 * \replaces
 *    -  strcat(), for multiple calls to this function.
 *    -  strncat(), for multiple calls to this function, where only the size
 *       of the destination is known.
 *
 *    It is like xpc_string_copycat(), except that the existing string, if
 *    any, in the destination, is skipped, so that concatenation can occur.
 *
 *    This function merely constructs a va_list structure and passes it to
 *    xpc_var_string_copycat().
 *
 * \param destination
 *    The pointer to the area to receive the concatenation.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.  The space for the existing string in the buffer must also be
 *    included in this count.
 *
 * \param stringcount
 *    The number of strings to be dealt with.  They are all concatenated.
 *    This number must match the number of string parameters that follow.
 *    It is necessary because va_arg() offers no way to determine that the
 *    end of the arguments have been reached.
 *
 * \param ...
 *    The null-terminated strings to be concatenated to the existing string.
 *    If a string is simply empty, it is ignored, and processing continues
 *    with the rest of the strings.  If the string is a null pointer,
 *    processing is aborted, and a shorter string will result than if all
 *    the arguments were valid.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Even if the parameters
 *    were invalid, the destination still can be used safely -- it might be
 *    empty or truncated, though.  The function tries to fill up the
 *    destination fully when truncation occurs.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_06()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_cat
(
   char * destination,
   size_t destsize,
   int stringcount,
   ...
)
{
   cbool_t result = not_nullptr(destination);
   if (result)
   {
      size_t length = xpc_strlen(destination);
      size_t remainder = destsize - length;
      char * newdestination = destination + length;
      va_list arguments;
      va_start(arguments, stringcount);
      result = xpc_var_string_copycat
      (
         newdestination, remainder, stringcount, arguments
      );
      va_end(arguments);
   }
   return result;
}

/******************************************************************************
 * xpc_sprintf()
 *------------------------------------------------------------------------*//**
 *
 *    A global function to make safer formatted output than sprintf().
 *
 * \replaces
 *    -  sprintf()
 *
 *    It uses vsnprintf_s() for Windows code, and vsnnprintf() otherwise.
 *    The differences in the two functions are accounted for in this
 *    implementation.
 *
 * \win32
 *    vsnprintf_s() returns the number of bytes stored, not counting the null
 *    character.  If an error occurred, including truncations, it returns -1.
 *
 * \gnu
 *    vsnprintf() returns the number of bytes stored, \e including the null
 *    character.  If the output was truncated due to lack of destination
 *    space, then the function returns the number of bytes that would have
 *    been written, \e not including the null byte.  [This gives the caller
 *    the opportunity to reallocate the correct amount of space, unlike
 *    sprintf_s().]  Thus, if the size returned is equal or greater to the
 *    supplied size, then an error occurred.  In this case, we throw that
 *    result away and return -1 instead.
 *
 * \usage
 *
\verbatim
      char temp[8];
      int rcode = xpc_sprintf(temp, sizeof(temp), "%x", value);
      if (rcode > 0)
         result += temp;
\endverbatim
 *
 * \note
 *    The C++ function common::stringnprintf() will call this one.
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param format
 *    The printf() format string needed to output the data.
 *
 * \param source
 *    The null-terminated string to be copied.
 *
 * \return
 *    Returns the number of bytes written to the destination.  If an error
 *    occurred, then -1 is returned.  The caller should check for the result
 *    being greater than 0 before using the string.
 *
 *    Perhaps we should use the snprintf() return value convention instead,
 *    though.  That would allow reallocation to be attempted.  However, we
 *    used sprintf() mostly with pre-sized buffers allocated on the stack.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_03()
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_sprintf
(
   char * destination,
   size_t destsize,
   const char * format,
   ...
)
{
   int result;
   va_list arguments;
   va_start(arguments, format);
   result = xpc_var_sprintf(destination, destsize, format, arguments);
   va_end(arguments);
   return result;
}

/******************************************************************************
 * xpc_var_sprintf()
 *------------------------------------------------------------------------*//**
 *
 *    A global function to make safer formatted output than vsnsprintf().
 *
 * \replaces
 *    -  vsnprintf()
 *    -  vsnprintf_s()
 *
 * \param destination
 *    The pointer to the area to receive the copy.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param format
 *    The printf() format string needed to output the data.
 *
 * \param vlist
 *    The standard argument structure representing the list of run-time
 *    arguments.
 *
 * \return
 *    Returns the number of bytes written to the destination.  If an error
 *    occurred, then -1 is returned.  The caller should check for the result
 *    being greater than 0 before using the string.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_03() [indirectly]
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_var_sprintf
(
   char * destination,
   size_t destsize,
   const char * format,
   va_list vlist
)
{
   int result = -1;
   cbool_t ok =
      not_nullptr_2(destination, format);

   if (ok)
   {
      if (destsize > 0)
      {

#if defined _MSC_VER
         result = vsnprintf_s(destination, destsize, _TRUNCATE, format, vlist);
         ok = result > 0;
         if (! ok)
         {
            char errmsg[128];
            (void) xpc_errno_string(errmsg, sizeof(errmsg), errno);
            xpc_errprintf(_("failed string: '%s'"), destination);
#else
         /*
          * A return value of size or more means that the output was
          * truncated.
          */

         result = vsnprintf(destination, destsize, format, vlist);
         ok = result > 0 && result < (int) destsize;
         if (! ok)
         {
            char * errmsg = result >= (int) destsize ?
               _("truncation error") : _("vsnprintf error");

#endif

            xpc_warnprintf
            (
               "%s: %s '%s'", __func__, _("failed with "), errmsg
            );
         }
      }
      else
      {
         destination[0] = 0;
         xpc_errprint_func(_("zero-sized destination"));
         ok = false;
      }
      if (! ok)
         result = -1;
   }
   return result;
}

/******************************************************************************
 * xpc_errno_string()
 *------------------------------------------------------------------------*//**
 *
 *    A global function to make safer copies of a system error message than
 *    does strerror().
 *
 * \replaces
 *    -  strerror()   [Borland]
 *    -  strerror_s() [Microsoft]
 *    -  strerror_r() [GNU]
 *
 *    It uses strerror_s() for Windows code, strerror() for the old Borland
 *    compiler, and strerror_r() otherwise.
 *
 *    The differences in the three functions are accounted for in this
 *    implementation.
 *
 *    Some functions [e.g. Microsoft's vsnprintf_s() when using the
 *    _TRUNCATE parameter] will set \e errno to 0 even when they return an
 *    error status.  In this case, we need to provide a non-confusing
 *    message, at least for Windows code.
 *
 * \param destination
 *    The pointer to the area to receive the system message.
 *
 * \param destsize
 *    The maximum number of characters to be copied, plus one.  The space
 *    needed for the terminating null character must be included in this
 *    count.
 *
 * \param errnum
 *    The errno variable to be translated to a message.
 *
 * \return
 *    Returns 'true' if no error occurred in the function.  The caller
 *    should check for the result being 'true' before using the destination
 *    string.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_07()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_errno_string
(
   char * destination,
   size_t destsize,
   errno_t errnum
)
{
   cbool_t result = not_nullptr(destination);
   if (result)
   {
      if (destsize > 0)
      {
#ifdef _MSC_VER

         /**
          * \win32
          *    The Windows version returns a 0 (success) when the errnum
          *    value is such that it yields an "Unknown error"!  In
          *    Microsoft's crtdefs.h, errno_t is an integer.  The values in
          *    Microsoft's errno.h go up to only a couple hundred.
          *    Nobody defines an ELAST value, so we will here, for now.
          */

#define ELAST  (EWOULDBLOCK + 1)    // WARNING:  Can change at vendor whim!

         if (errnum != 0)
         {
            int rcode = strerror_s(destination, destsize, errnum);
            result = (errnum >= 0) && (errnum < ELAST) ? (rcode == 0) : false ;
         }
         else
         {
            result = xpc_string_copy
            (
               destination, destsize, _("Possible truncation error")
            );
         }

#elif defined __BORLANDC__
         char * errmsg = strerror(errnum);
         result = xpc_string_copy(destination, destsize, errmsg);
#else
         /*
          * Actually the GNU-specific non-standard version of strerror_t.
          */

         (void) strerror_r((int) errnum, destination, destsize);
         result = true;
#endif
         if (! result)
         {
            errnum = errno;
            if (errnum != 0)
               result = xpc_errno_string(destination, destsize, errnum);  // recur
         }
      }
      else
      {
         result = false;
         destination[0] = 0;
         xpc_errprint_func(_("zero-sized destination"));
      }
   }
   return result;
}

/******************************************************************************
 * specifier_count()
 *------------------------------------------------------------------------*//**
 *
 *    Counts printf()-style format specifiers and does rudimentary
 *    error-checking.
 *
 *    The error checking consists only of checking for bare "%s" format
 *    specifiers.  These bare "%s" format specifiers are dangerous -- a
 *    number should be specified in order to reduce the chances of a buffer
 *    overrun.
 *
 * \private
 *    Internal helper function for xpc_string_scanf().
 *
 * \param format
 *    Provides the formatting string to be parsed.
 *
 * \return
 *    Returns the number of format specifiers.  If this number is 0, either
 *    there were none, or a null pointer was provided.  If this number is
 *    negative, the \e nth format specifier was the first invalid format
 *    specifier detected.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_08() [indirectly]
 *
 *//*-------------------------------------------------------------------------*/

static int
specifier_count (const char * format)
{
   int result = 0;
   if (not_nullptr(format))
   {
      const char * p = format;
      for (;;)                      // get an upper bound for the # of args
      {
         char c = *p++;
         if (c == 0)
            break;

         if (c == '%' && (*p != '*' && *p != '%'))
         {
            ++result;
            if (*p == 's')
            {
               xpc_warnprintf
               (
                  "%s:  %s %d %s '%s'",
                  __func__, _("Unsafe '%s' format specifier at position"),
                  result, _("in format string"), format
               );
               result = -result;
               break;
            }
         }
      }
      if (result == 0)
      {
         xpc_warnprintf
         (
            "%s:  %s '%s'",
            __func__, _("No format specifiers found in format string"),
            format
         );
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_scanf()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a safer replacement for sscanf().
 *
 * \replaces
 *    -  sscanf()
 *
 *    This function uses vsscanf().  However, for Visual Studio compilation,
 *    recall that this function is not included with Visual Studio, and so
 *    we use the implementation present in the vsscanf_win.c/h module.
 *
 * \param input
 *    The input string to be scanned for formatted values.
 *
 * \param inputsize
 *    The number of characters in the input buffer.  If this value is set to
 *    0, the inputsize is obtained by applying strlen() to the input.  If
 *    that value is 0, this function will fail.
 *
 * \param format
 *    The format expected for the input string of values.
 *
 * \param ...
 *    The variables to be assigned as per the format.
 *
 * \return
 *    Returns the number of input values assigned to the variables.  See
 *    sscanf(3) or Microsoft's documentation on sscanf_s().  If an error
 *    occurred, or the input runs out, EOF is returned.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_08()
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_string_scanf
(
   const char * input,
   size_t inputsize,
   const char * format,
   ...
)
{
   int result = EOF;
   cbool_t ok = not_nullptr_2(input, format);
   if (ok)
      ok = *input != 0;                   // ok = strlen(input) > 0

   if (ok)
   {
      int count = specifier_count(format);
      ok = count > 0;
   }
   else
      xpc_errprint_func(_("input string is empty"));

   if (ok)
   {
      va_list arguments;
      va_start(arguments, format);
      if (inputsize == 0)
         inputsize = xpc_string_length(input, inputsize);

      if (inputsize > 0)
      {
         /*
          * Funny as hell:  Microsoft deprecates this function, but provides
          * no replacement for it.  We have to implement our own.  We use the
          * same name and suppress the warning here.  The implementation used
          * is found in the vsscanf_win.c module.
          */

#ifdef _MSC_VER
#pragma warning ( suppress : 4996 )
#endif
         result = vsscanf(input, format, arguments);
         va_end(arguments);
         if (result == EOF)
         {
            char destination[128];
            int errnum = errno;
            if (errnum != 0)
               (void) xpc_errno_string(destination, sizeof(destination), errnum);
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_n_compare()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings, with a length option.
 *
 * \replaces
 *    -  strncmp()
 *
 *    The strings must be identical (including letter case) to the number of
 *    characters specified.
 *
 *    This function could call
 *
\verbatim
      stringcomparison(s1, s2, n, true)
\endverbatim
 *
 *    but we recoded the implementation here for efficiency.
 *
 * \param s1
 *    The first string to compare.
 *
 * \param s2
 *    The second string to compare.
 *
 * \param n
 *    The number of characters to compare at maximum.  If less than or equal
 *    to 0, all possible characters are compared, and this function is then
 *    equivalent to xpc_string_identical().
 *
 * \return
 *    Returns 'true' if the strings match to the given number of characters.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_22()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_n_compare
(
   const char * s1,
   const char * s2,
   size_t n
)
{
   cbool_t result = false;
   if (not_nullptr_2(s1, s2))
   {
      int c1, c2;
      size_t count = 0;
      result = true;
      for (;;)
      {
         c1 = *s1++;
         c2 = *s2++;
         if (c1 != 0 && c2 != 0)
         {
            if (c1 != c2)
            {
               result = false;
               break;
            }
            else if (n > 0)
            {
               count++;
               if (count == n)
                  break;
            }
         }
         else
         {
            if ((c1 != 0) || (c2 != 0))
               result = false;

            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_equivalent()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings, with case-insensitivity, returning only whether
 *    the strings are equivalent or not.
 *
 *    By "equivalent" is meant that the strings are the same token, ignoring
 *    the case of the characters.
 *
 * \replaces
 *    -  stricmp() == 0
 *    -  strcmpi() == 0
 *    -  _strcmpi() == 0
 *    -  strcasecmp() == 0
 *
 *    This function could call
 *
\verbatim
      stringcomparison(s1, s2, 0, false)
\endverbatim
 *
 *    but we recoded the implementation here for efficiency.
 *
 * \param s1
 *    The first string to compare.
 *
 * \param s2
 *    The second string to compare.
 *
 * \return
 *    Returns 'true' if the strings were not null pointers, and were exactly
 *    equal (with case-insensitivity).  If either or both strings were null
 *    pointers, then 'false' is returned.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_10()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_equivalent
(
   const char * s1,
   const char * s2
)
{
   cbool_t result = false;
   if (not_nullptr_2(s1, s2))
   {
      int c1, c2;
      result = true;
      for (;;)
      {
         c1 = *s1++;
         c2 = *s2++;
         if (c1 != 0 && c2 != 0)
         {
            c1 = tolower(c1) & 0xFF;      // 8 bits
            c2 = tolower(c2) & 0xFF;      // only
            if (c1 != c2)
            {
               result = false;
               break;
            }
         }
         else
         {
            if (c1 != c2)
               result = false;

            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_identical()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings, returning only whether the strings were
 *    identical (including having the same letter-case) or not.
 *
 * \replaces
 *    -  strcmp() == 0
 *
 *    This function is separately implemented for two reasons:
 *
 *       -# It is a very common use case to just see if two strings match
 *          exactly.
 *       -# It is very fast.
 *
 *    This function could call
 *
\verbatim
      stringcomparison(s1, s2, 0, true)
\endverbatim
 *
 *    but we recoded the implementation here for efficiency.
 *
 * \param s1
 *    The first string to compare.
 *
 * \param s2
 *    The second string to compare.
 *
 * \return
 *    Returns 'true' if the strings were not null pointers, and were exactly
 *    equal (with case-sensitivity).  If both strings are empty (they
 *    contain only a null terminator), then 'true' is returned.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_11()
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_06() [indirect test]
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_07() [indirect test]
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_identical
(
   const char * s1,
   const char * s2
)
{
   cbool_t result = false;
   if (not_nullptr_2(s1, s2))
   {
      int c1, c2;
      result = true;
      for (;;)
      {
         c1 = *s1++;
         c2 = *s2++;
         if (c1 != c2)
         {
            result = false;
            break;
         }
         else if (c1 == 0 || c2 == 0)
            break;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_match()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings, with only a case-matching option, returning
 *    only whether the strings match for all characters in the shorter
 *    string.
 *
 *    This function provides a nice way to perform matches of case-sensitive
 *    or case-insensitive matches on abbreviations (shortening) of commands.
 *
 * \replaces
 *    -  strcmp()
 *    -  stricmp()
 *    -  _stricmp() [Microsoft]
 *    -  strcasecmp() [GNU]
 *
 * \param s1
 *    The first string to compare.
 *
 * \param s2
 *    The second string to compare.
 *
 * \param matchcase
 *    If 'true', the normal string comparison is made.  If 'false', a
 *    case-insensitive comparison is made.
 *
 * \return
 *    Returns 'true' if the strings were not null pointers, and were exactly
 *    equal (with case-insensitivity if specified).
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_12()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_match
(
   const char * s1,
   const char * s2,
   cbool_t matchcase
)
{
   cbool_t result = false;
   if (not_nullptr_2(s1, s2))
   {
      int c1, c2;
      cbool_t convert = ! matchcase;
      const char * p1 = s1;
      const char * p2 = s2;
      result = true;
      for (;;)
      {
         c1 = *p1++;
         c2 = *p2++;
         if (c1 != 0 && c2 != 0)
         {
            if (convert)
            {
               c1 = tolower(c1) & 0xFF;      /* 8 bits   */
               c2 = tolower(c2) & 0xFF;
            }
            if (c1 != c2)
            {
               result = false;
               break;
            }
         }
         else
         {
            /*
             * At least one of the strings has ended.  Either they matched
             * up to this point, or one of the strings was empty.  In the
             * latter case, falsify the result.
             */

            if (((*s1 == 0) || (*s2 == 0)) && (*s1 != *s2))
               result = false;

            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_string_tokens()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a replacement for strtok(), with the added safety of separate
 *    character-buffer scratchpads.
 *
 * \replaces
 *    -  strtok()
 *    -  strtok_s() [Microsoft]
 *    -  strtok_r() [GNU]
 *
 * \param source
 *    The string to be tokenized.  This string will be messed up as this
 *    function performs its work.  After the first call to xpc_string_tokens(),
 *    the caller must then pass a null pointer, to tell xpc_string_tokens() to
 *    process the next token.
 *
 * \param delimiters
 *    The list of valid token delimiters.
 *
 * \param saveptr
 *    Provides a scratchpad for the strtok_r() or strtok_s() functions to
 *    use.
 *
 * \return
 *    Returns the current token, or a null poniter if there are no more
 *    tokens.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_09_23()
 *
 *//*-------------------------------------------------------------------------*/

char *
xpc_string_tokens
(
   char * source,                         /* after first use, pass a null!    */
   const char * delimiters,
   char ** saveptr
)
{
   char * result = nullptr;
   cbool_t ok = not_nullptr_2
   (
      delimiters, saveptr                 /* do NOT test 'source'             */
   );

#if defined __BORLANDC__                  /* for the old Builder 4 compiler!  */

   if (ok)
      result = strtok(source, delimiters);

#elif defined _MSC_VER                    /* Microsoft compiler               */

   if (ok)
      result = strtok_s(source, delimiters, saveptr);

#else

   if (ok)
      result = strtok_r(source, delimiters, saveptr);

#endif

   return result;
}

/******************************************************************************
 * xpc_string_length()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a replacement for strlen(), with the added safety of
 *    of limiting the range of the search.
 *
 * \replaces
 *    -  strlen()
 *    -  strnlen() [GNU]
 *    -  strnlen() [Microsoft]
 *
 * \param source
 *    The string to be counted.
 *
 * \param maxlen
 *    The length beyond which the null terminator is not scanned.
 *
 * \return
 *    Returns the length of the string, or maxlen if there is no null
 *    terminator within that length, or if the string pointer is null.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_xx_yy()
 *
 *//*-------------------------------------------------------------------------*/

size_t
xpc_string_length (const char * source, size_t maxlen)
{
   size_t result = maxlen;
   if (not_nullptr(source))
      result = strnlen(source, maxlen);

   return result;
}

/******************************************************************************
 * xpc_strlen()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a replacement for strlen(), with the added safety of
 *    of limiting the range of the search with a default maximum length of
 *    1024.
 *
 * \replaces
 *    -  strlen()
 *    -  strnlen() [GNU]
 *    -  strnlen() [Microsoft]
 *
 * \warning
 *    This function is meant only to shut flawfinder up when we cannot know
 *    the maximum length of the string.  Thus, it is only a stop-gap, and
 *    eventually all instances should be replaced with actual checks.
 *
 * \param source
 *    The string to be counted.
 *
 * \return
 *    Returns the length of the string, or 1024 if there is no null
 *    terminator within that length, or if the string pointer is null.
 *
 * \unittests
 *    -  xpc_strings_ut.c: xpc_strings_unit_test_xx_yy()
 *
 *//*-------------------------------------------------------------------------*/

size_t
xpc_strlen (const char * source)
{
   size_t result = 1024;
   if (not_nullptr(source))
      result = strnlen(source, 1024);

   return result;
}

/******************************************************************************
 * String-structure functions
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * xpc_string_init()
 *------------------------------------------------------------------------*//**
 *
 *    Initializes a xpc_string_t structure, for simple string concatenation.
 *
 *    This function assigns the pointer to the string buffer that is managed
 *    by the caller.  The caller also provides the size of the buffer,
 *    including the null terminator, and this size is saved.  Then length of
 *    the string is initialized to zero (0), and the first byte of the
 *    buffer is set to the null terminator, as required by the private
 *    function xpc_string_n_cat_r_impl().
 *
 *    Once this function is called, the caller can safely use the
 *    xpc_string_n_cat_r() function.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    xpc_strings_unit_test_05c() [indirect]
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_init
(
   xpc_string_t * pst,  /**< Primitive management structure for the string    */
   char * buffer,       /**< The buffer created (and destroyed) by the caller */
   size_t size          /**< Size of the buffer; includes room for the null   */
)
{
   cbool_t result = false;
   if (is_thisptr(pst))
   {
      pst->str_length = 0;                /* reflect that the string is empty */
      pst->str_buffer = buffer;           /* log the location of the buffer   */
      pst->str_scratch = buffer;          /* log the scratch location         */
      if (not_nullptr(buffer))
      {
         pst->str_scratch[0] = 0;         /* make sure it is null-terminated  */
         pst->str_limit = size;           /* log the size limit of the buffer */
         if (size == 0)
            xpc_errprint_func(_("invalid buffer size"));
         else
            result = true;
      }
      else
         pst->str_limit = 0;              /* log the size limit of the buffer */
   }
   return result;
}

/******************************************************************************
 * xpc_string_validate()
 *------------------------------------------------------------------------*//**
 *
 *    Verifies that a xpc_string_t structure has been initialized.
 *
 *    This function checks the string-buffer pointer and string-buffer size.
 *    It is not foolproof, but helps.
 *
 * \return
 *    Returns 'true' if the parameters were valid.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    xpc_strings_unit_test_05c() [indirect]
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_string_validate
(
   xpc_string_t * pst   /**< Primitive management structure for the string    */
)
{
   cbool_t result = false;
   if (is_thisptr(pst))
   {
      result = not_nullptr(pst->str_buffer);
      if (result)
         result = pst->str_limit > 0;
   }
   return result;
}

/******************************************************************************
 * xpc_string_c_str()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the string pointer held by a xpc_string_t structure.
 *
 *    This function is merely an accessor for the buffer pointer in the
 *    xpc_string_t structure.  Since this is C, an accessor function is not
 *    necessary, and the caller could simply access the string pointer
 *    directly.  However, pretending the structure is opaque and relying on
 *    the accessor makes the interface more clear.
 *
 *    It is not foolproof, but helps.
 *
 * \return
 *    Returns the pointer to the string that was set up by the
 *    xpc_string_init() call.
 *
 * \unittests
 *    xpc_strings_unit_test_05c() [indirect]
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_string_c_str
(
   const xpc_string_t * pst      /**< Management structure for the string     */
)
{
   const char * result = nullptr;
   if (is_thisptr(pst))
      result = pst->str_buffer;

   return result;
}

/******************************************************************************
 * xpc_string_max()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the maximum string length held by a xpc_string_t structure.
 *
 *    This function is merely an accessor for the buffer size in the
 *    xpc_string_t structure.  Pretending the structure is opaque and
 *    relying on the accessor makes the interface more clear.
 *
 * \return
 *    Returns the maximum size of the string that was set up by the
 *    xpc_string_init() call.  Zero (0) is returned if the parameter is not
 *    valid.
 *
 * \unittests
 *    xpc_strings_unit_test_05c() [indirect]
 *
 *//*-------------------------------------------------------------------------*/

size_t
xpc_string_max
(
   const xpc_string_t * pst      /**< Management structure for the string     */
)
{
   size_t result = 0;
   if (is_thisptr(pst))
      result = pst->str_limit;               /* the size of the buffer        */

   return result;
}

/******************************************************************************
 * xpc_string_size()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the current string length held by a xpc_string_t structure.
 *
 *    This function is merely an accessor for the string length in the
 *    xpc_string_t structure.  Pretending the structure is opaque and
 *    relying on the accessor makes the interface more clear.
 *
 * \return
 *    Returns the maximum size of the string that was set up by the
 *    xpc_string_init() call.  Zero (0) is returned if the parameter is not
 *    valid.
 *
 * \unittests
 *    xpc_strings_unit_test_05c() [indirect]
 *
 *//*-------------------------------------------------------------------------*/

size_t
xpc_string_size
(
   const xpc_string_t * pst      /**< Management structure for the string     */
)
{
   size_t result = 0;
   if (is_thisptr(pst))
      result = pst->str_length;            /* current length of the string    */

   return result;
}

/******************************************************************************
 * xpc_ip_to_string()
 *------------------------------------------------------------------------*//**
 *
 *    Conversions between the string forms and the integer forms of
 *    IP addresses.
 *
 *    xpc_ip_to_string() accepts integers ranging from 0x0 to 0xffffffff, and
 *    returns strings of one of the following forms:
 *
 *          -  10.2.3.6
 *          -  192.134.22.125
 *          -  0.0.0.0
 *
 *    If the 'usehex' parameter is true:
 *
 *          -  0xa.2.3.6
 *          -  0xca.fe.1f.2
 *          -  0x0.0.0.0
 *
 *    The caller can skip the '0x' if it is not wanted.
 *
 *    This function is meant only to convert integer IP's to numeric
 *    strings.  No DNS lookups are involved.
 *
 * \warning
 *    This function handles only 32-bit (IPv4) addresses, which contain 4
 *    octets.  Also, currently this routine assumes that the IP address is
 *    in L-to-R (left to right) order.  In other words, the integer and
 *    string representations are in the same order:  "0x7f000001" is
 *    "0x7f.0.0.1" or "127.0.0.1".  Not sure if this will become an issue!
 *    In any case, while not deprecated, this function is only of limited
 *    usefulness, even if we eventually upgrade it to handle IPv6 addresses.
 *
 * \posix
 *    The function inet_ntop(3) can also be used to convert an address to a
 *    string, where the address is "(void *) &sin->sin_addr.s_addr, sin
 *    being a struct sockaddr_in or struct sockaddr_in6.  It supercedes
 *    inet_ntoa(). See "man 3 inet_ntop" for more information.  Another
 *    option is to use getnameinfo() with the NI_NUMERICHOST flag.
 *
 * \return
 *    Returns the caller's string pointer if the parameters were valid and
 *    the whole source string was able to be copied.  Otherwise, it returns
 *    a null pointer (nullptr).
 *
 * \unittests
 *    xpc_strings_unit_test_06()
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_ip_to_string
(
   unsigned ipnumber,   /**< 32-bit IP address integer in L-to-R order        */
   char * destination,  /**< The location to which to write the string        */
   size_t n,            /**< The length of the destination                    */
   cbool_t usehex       /**< If 'true', convert to hexadecimal octets         */
)
{
   const char * result = nullptr;
   if (not_nullptr(destination) && (n > 0))
   {
      cbool_t ok;
      if (usehex)
         ok = xpc_string_n_copy_impl(destination, "0x", n);
      else
      {
         ok = true;
         destination[0] = 0;                 /* prep for concatenation        */
      }
      if (ok)
      {
         char temp[8];
         int octet;
         unsigned mask = 0xFF000000;
         cbool_t ok = true;
         for (octet = 0; octet < 4; octet++, mask >>= 8)
         {
            unsigned curr = ipnumber & mask;
            int bitshift = (3 - octet) * 8;
            curr >>= bitshift;
            if (octet != 0)
            {
               if (! xpc_string_n_cat_deprecated(destination, ".", n))
               {
                  ok = false;
                  break;
               }
            }
            sprintf(temp, usehex ? "%x" : "%d", curr);
            if (! xpc_string_n_cat_deprecated(destination, temp, n))
            {
               ok = false;
               break;
            }
         }
         if (ok)
            result = destination;
      }
   }
   else
      xpc_errprint_func(_("invalid buffer"));

   return result;
}

/******************************************************************************
 * xpc_string_to_ip()
 *------------------------------------------------------------------------*//**
 *
 *    Converts a string in IP octet format to an IP-address integer.
 *
 *    xpc_string_to_ip() performs the inverse calculation of
 *    xpc_ip_to_string().  However, it doesn't need to have all 4 octets
 *    provided.  If less than 4 are given, then those values are assumed to
 *    be the "least significant".  Thus, the functions are not inverses in
 *    the true sense of "inverse".
 *
 *    If the string begins with "0x", then the digits are translated as
 *    hexadecimal values.
 *
 *    This function is meant only to convert octet-format strings to IP
 *    address integers.  No DNS lookups are involved.
 *
 *    This function also attempts to validate that the address is numeric.
 *    The validation is not perfect, however.
 *
 * \warning
 *    -  This function handles only 32-bit (IPv4) addresses, which contain 4
 *       octets.  See the POSIX section that follows for an alternative
 *       function.  However, the function does allow for IPv6 colons in the
 *       IP address.
 *    -  This function assumes decimal (base 10) octets unless the string
 *       begins with "0x".
 *
 * \posix
 *    The function inet_pton(3) can be used to convert a string (in
 *    dotted-quad format, 'ddd.ddd.ddd.ddd') to a struct in_addr.  (This
 *    function also handles AF_INET6 addresses.)  Another option is to use
 *    getaddrinfo() with the AI_NUMERICHOST flag.
 *
 * \win32
 *    The function inet_addr() can be used for a similar conversion (the
 *    return value is an in_addr_t).  This function is also available in
 *    POSIX (actually BSD 4.3), but is considered obsolete.
 *
 * \note
 *    The IP address as a string is in L-to-R (left to right) order.  On a
 *    little-endian machine (x86), the IP address as an integer is in the
 *    same order: "0x7f000001" is "0x7f.0.0.1" or "127.0.0.1".
 *
 * \convention
 *    The XPC convention for both port and IP is that parameters and return
 *    values are always in <b> host </b> byte order.
 *
 * \return
 *    Returns the IP-address integer if the function succeeded.  Otherwise
 *    0, a legal IP address, is returned.  Hence, there is no easy way to
 *    check if the function actually failed.  The best way is to check for
 *    an INADDR_ANY string before calling this function.
 *
 * \unittests
 *    xpc_strings_unit_test_07()
 *
 * \todo
 *    Update this function to be sure it returns 0 for bad numeric IPs, and
 *    add the requisite unit tests.
 *
 *//*-------------------------------------------------------------------------*/

unsigned
xpc_string_to_ip
(
   const char * strdata    /**< String to convert to an IP-address integer    */
)
{
   unsigned result = 0x0;
   if (not_nullptr(strdata))
   {
      cbool_t isnumericip = strdata[0] != 0;
      if (isnumericip)
      {
         size_t count;
         if (strncmp(strdata, "0x", 2) == 0)
            count = strspn(strdata+2, ".0123456789abcdefABCDEF:");
         else
            count = strspn(strdata, ".0123456789:");

         isnumericip = count > 0;
      }
      if (isnumericip)
      {
         int octet = 0;
         cbool_t usehex = (strdata[0] == '0') && (strdata[1] == 'x');
         unsigned curr;
         if (usehex)
            strdata += 2;

         while (octet < 4)
         {
            curr = usehex ? atox(strdata) : (unsigned) atoi(strdata);
            if (octet != 0)
               result <<= 8;

            result += curr;
            while (isxdigit(*strdata))
               strdata++;

            if (*strdata == '.')
               strdata++;
            else
               break;

            octet++;
         }
      }
      else
      {
         xpc_dbginfoprint_func(_("host name is not numeric"));
      }
   }
   return result;
}

/******************************************************************************
 * xstrings.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
