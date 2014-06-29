/******************************************************************************
 * environment.c
 *------------------------------------------------------------------------*//**
 *
 * \file       environment.c
 * \author     Chris Ahlstrom
 * \date       2012-06-21
 * \updates    2012-06-23
 * \version    $Revision$
 *
 *    Provides safe replacements for the C environment functions.
 *    Also see the environment.h module.
 *
 * \note
 *    These are the C functions implemented herein.
 *
 *       -  xpc_get_environment().  A safer version of getenv().
 *       -  xpc_unsafe_getenv().    An easier version of the
 *                                  xpc_get_environment() function.
 *       -  xpc_put_environment().  A safer version of putenv() or setenv().
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/environment.h>           /* the environment support             */
#include <xpc/errorlogging.h>          /* errprint() family of macros         */
#include <xpc/gettext_support.h>       /* the i18n and marking macro          */
#include <xpc/xstrings.h>              /* C::xpc_errno_string()               */
XPC_REVISION(environment)

#ifdef __BORLANDC__
#include <stdlib.h>                    /* unsetenv() and setenv()             */
#endif

/******************************************************************************
 * xpc_get_environment()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a safe and informative wrapper for getenv().
 *
 * \param dest
 *    Provides a destination string for the environment variable.
 *
 * \param destsize
 *    Provides the length of the destination string, including the null
 *    terminator.
 *
 * \param varname
 *    Provides the name of the environment variable to be retrieved.
 *
 * \return
 *    Returns one of the the following:
 *       -  0, if the function succeeded.
 *       -  The length of the buffer size needed to retry to get the full
 *          environment variable, if the destination buffer was not big
 *          enough.  The destination buffer returns an empty string.
 *       -  INT_MAX, if the environment variable was not found.
 *          The destination buffer returns an empty string.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

size_t
xpc_get_environment
(
   char * dest,
   size_t destsize,
   const char * varname
)
{
   size_t result = INT_MAX;                     /* assume not found at first  */
   cbool_t ok = xpc_not_nullptr(dest, "xpc_get_environment(dest)");
   if (ok)
   {
      dest[0] = 0;
      ok = xpc_not_nullptr(varname, _("xpc_get_environment(varname)"));
   }
   if (ok)
   {
      ok = destsize > 0;
      if (ok)
      {
         ok = strlen(varname) > 0;
         if (! ok)
            xpc_errprint_func(_("zero-sized environment variable name"));
      }
      else
         xpc_errprint_func(_("zero-sized destination"));
   }
   if (ok)
   {

#ifdef _MSC_VER

      /**
       * \win32
       *    getenv_s() is not case-sensitive.  Also, getenv_s() returns 0 if
       *    successful, and an error code on failure.  However, not finding
       *    the environment variable yields a successful return value, but a
       *    0 value for \e needed.
       */

      size_t needed = destsize;                 /* used for retries           */
      errno_t errnum = getenv_s(&needed, dest, destsize, varname);
      ok = errnum == 0;
      if (ok)
         ok = needed != 0;

      if (ok)
      {
         result = 0;                            /* success!                   */
      }
      else
      {
         if (needed != 0)
         {
            char temp[256];
            if (xpc_errno_string(temp, sizeof(temp), errnum))
               xpc_errprintf("xpc_get_environment %s: '%s'", _("error"), temp);

            if (errnum == ERANGE)               /* buffer was too small       */
            {
               result = needed;
               xpc_warnprintf
               (
                  _("%s: increase destination size to %d"),
                  __func__, (int) needed
               );
            }
            else
               result = INT_MAX;
         }
         dest[0] = 0;                           /* empty string               */
      }

#else

      char * tmptr = getenv(varname);
      ok = test_nullptr(tmptr);
      if (ok)
      {
         size_t needed = strlen(tmptr) + 1;
         if (needed > destsize)
         {
            xpc_warnprintf
            (
               _("%s: increase destination size to %d"), __func__, (int) needed
            );
            result = needed;
         }
         else
         {
            cbool_t ok = xpc_string_copy(dest, destsize, tmptr);
            result = ok ? 0 : INT_MAX ;
         }
      }

#endif

   }
   if (result == 0)
      xpc_infoprintf("%s: got %s = '%s'", __func__, varname, dest);
   else
      xpc_warnprintf("%s: failed to get %s", __func__, varname);

   return result;
}

/******************************************************************************
 * xpc_unsafe_getenv()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a safe and informative wrapper for getenv(), that works more
 *    like getenv().
 *
 *    This version doesn't give the user a chance to reallocate, but is
 *    easier to work into existing amateurish code that doesn't bother to
 *    reallocate if getenv() fails.
 *
 *    This function replaces:
 *
 *       -  getenv()
 *
 * \param dest
 *    Provides a destination string.
 *
 * \param destsize
 *    Provides the length of the destination string.
 *
 * \param varname
 *    Provides the name of the environment variable to be retrieved.
 *
 * \return
 *    Returns a pointer to the destination if the function succeeded,
 *    otherwise it returns a null pointer.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

char *
xpc_unsafe_getenv
(
   char * dest,
   size_t destsize,
   const char * varname
)
{
   char * result = nullptr;
   size_t c = xpc_get_environment(dest, destsize, varname);
   if (c == 0)
      result = dest;

   return result;
}

/******************************************************************************
 * xpc_put_environment()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a safe and informative wrapper for putenv().
 *
 *    Like putenv(), this function always overwrites any existing value for
 *    that environment variable.
 *
 *    To delete an environment variable, pass in a zero-length string for
 *    the \a value parameters.
 *
 *    This function replaces:
 *
 *       -  putenv()
 *       -  putenv_s() [Microsoft]
 *       -  setenv()   [GNU]
 *       -  unsetenv() [GNU]
 *
 * \threadunsafe
 *    The getenv() and putenv()/setenv() functions are not thread-safe.
 *    Calls to these functions may need to be synchronized.
 *
 * \param varname
 *    Provides the name of the environment variable to be set or replaced.
 *
 * \param value
 *    The string value to which to set the environment variable.  If of zero
 *    length (but not a null pointer), the environment variable is deleted.
 *
 * \return
 *    Returns 'true' if the function parameters were valid and the function
 *    succeeded.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_put_environment
(
   const char * varname,
   const char * value
)
{
   cbool_t result = false;
   cbool_t ok = xpc_not_nullptr(varname, _("xpc_put_environment(varname)"));
   if (ok)
      ok = xpc_not_nullptr(value, _("xpc_put_environment(value)"));

   if (ok)
      ok = strlen(varname) > 0;

   if (! ok)
   {
      xpc_errprint_func(_("empty environment variable name"));
   }
   else
   {

#if defined __BORLANDC__
      char envspec[300];                        /* to make "varname=value"    */
#endif

      errno_t errnum;
      cbool_t clear_it = strlen(value) == 0;
      if (clear_it)
      {
         xpc_infoprintf
         (
            "%s:  %s '%s'",
            __func__, _("clearing environment variable"), varname
         );
      }

#if defined _MSC_VER

      errnum = _putenv_s(varname, value);
      ok = errnum == 0;
      if (ok)
         result = true;                         /* success!                   */

#elif defined __BORLANDC__

      ok = xpc_var_string_copycat
      (
         envspec, sizeof(envspec), 3, varname, "=", value
      );
      if (ok)
      {
         errnum = putenv(envspec);
         ok = errnum == 0;
         if (ok)
            result = true;                      /* success!                   */
      }

#else

      if (clear_it)
         errnum = unsetenv(varname);
      else
         errnum = setenv(varname, value, 1);    /* "1" == overwrite value     */

      ok = errnum != -1;
      if (ok)
         result = true;                         /* success!                   */

#endif

      else
      {
         char temp[128];
         errnum = errno;
         if (xpc_errno_string(temp, sizeof(temp), errnum))
            xpc_errprintf("xpc_put_environment %s: '%s'", _("error"), temp);
      }
   }
   return result;
}

/******************************************************************************
 * End of environment.c
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
