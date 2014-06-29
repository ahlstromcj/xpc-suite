/******************************************************************************
 * file_functions.c
 *------------------------------------------------------------------------*//**
 *
 * \file    file_functions.c
 *
 *    Provides the implementations for safe replacements for the various C
 *    file functions.
 *
 * \author  Chris Ahlstrom
 * \date    2012-06-09
 * \updates 2012-06-11
 * \version $Revision$
 *
 *    Newer compilers issue warning for unsafe versions of file-handling
 *    functions such as <tt>fopen()</tt>.  This module contains safe
 *    versions of these functions, as well as some convenience function for
 *    testing file status.
 *       _CRT_SECURE_NO_WARNINGS.
 *
 *    Also see the file_functions.h module and the the xstrings.c module for
 *    more information.
 *
 *//*-------------------------------------------------------------------------*/

#include <ctype.h>                     /* toupper()                           */
#include <xpc/file_macros.h>           /* file_functions support macros       */
#include <xpc/file_functions.h>        /* file utility module                 */
#include <xpc/errorlogging.h>          /* errprint family of functions        */
#include <xpc/gettext_support.h>       /* the i18n and marking macro          */
#include <xpc/xstrings.h>              /* xpc_string_copy(), etc.             */

#include <sys/types.h>
#include <sys/stat.h>

#if defined _MSC_VER                   /* Microsoft compiler                  */

#if defined WIN32
#include <xpc/xwinsock.h>              /* safely include winsock2.h           */
#endif

#include <io.h>                        /* _access_s()                         */
#include <share.h>                     /* _SH_DENYNO                          */
#elif defined __BORLANDC__             /* Borland C/C++ Builder 4 compiler    */
#include <io.h>                        /* _access_s()                         */
#else                                  /* for us, just gcc at the moment      */
#include <unistd.h>                    /* access()                            */
#endif                                 /* _MSC_VER                            */

/******************************************************************************
 * is_file_name_good()
 *------------------------------------------------------------------------*//**
 *
 *    Verifies that a file-name pointer is legal.
 *
 *    The following checks are made:
 *
 *       -  The pointer is not null.
 *       -  The file-name is not empty.
 *       -  The file-name is not one of the following:
 *          -  "stdout"
 *          -  "stdin"
 *          -  "stderr"
 *          -  "?"
 *
 * \replaces
 *    -  Does not replace any function, but is a helper function that is
 *       worthwhile to expose publicly.
 *
 * \param filename
 *    Provides the file name to be verified.
 *
 * \return
 *    Returns 'true' if the file-name is valid.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
is_file_name_good (const char * filename)
{
   cbool_t result = xpc_not_nullptr(filename, _("bad file-name pointer"));
   if (result)
   {
      result = *filename != 0;
      if (result)
      {
         result = !
         (
            xpc_string_identical(filename, "stdout") ||
            xpc_string_identical(filename, "stdin")  ||
            xpc_string_identical(filename, "stderr") ||
            xpc_string_identical(filename, "?")
         );
      }
      if (! result)
         xpc_errprint_func(_("file-name invalid"));
   }
   return result;
}

/******************************************************************************
 * is_file_mode_good()
 *------------------------------------------------------------------------*//**
 *
 *    Verifies that a file-mode pointer represents a legal value.
 *
 *    The following checks of the file-mode string are made:
 *
 *       -  The mode pointer is not null.
 *       -  The mode is not empty.
 *       -  The mode is one of the following:
 *          -  r, w, a
 *          -  r+, w+, a+
 *          -  rb, wb, ab
 *          -  rb+, wb+, ab+
 *          -  r+b, w+b, a+b
 *          -  rt, wt, at
 *          -  rt+, wt+, at+
 *          -  r+t, w+t, a+t
 *
 *    Non-standard extensions are not allowed.
 *
 * \param mode
 *    Provides the file-opening mode string to be verified.
 *
 * \return
 *    Returns 'true' if the file-name is valid.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
is_file_mode_good (const char * mode)
{
   cbool_t result = xpc_not_nullptr(mode, _("bad file-mode pointer"));
   if (result)
   {
      /*
       * Accept only valid characters here.  Note that this test will
       * reject empty strings as well.
       */

      result = mode[0] == 'r' || mode[0] == 'w' || mode[0] == 'a';
      if (result)
      {
         if (mode[1] != 0)
         {
            result = mode[1] == '+' || mode[1] == 'b' || mode[1] == 't';
            if (result)
            {
               if (mode[2] != 0)
               {
                  result = mode[2] == '+' || mode[2] == 'b' || mode[2] == 't';
                  if (result)
                     result = mode[3] == 0;
               }
            }
         }
      }
      if (! result)
         xpc_errprint_func(_("file-mode invalid"));
   }
   return result;
}

/******************************************************************************
 * xpc_impl_file_error()
 *------------------------------------------------------------------------*//**
 *
 *    Checks for a file error, reporting it to the error-log if there is
 *    one.
 *
 * \private
 *    This function is visible and used only in this module, and assumes
 *    that the pointer parameters have been checked by the caller.
 *
 * \param filename
 *    Provides the name of the file to be handled.
 *
 * \param mode
 *    Provides the mode of usage for the file, which can be one of the
 *    file-opening mode, or the number of the function in which the failure
 *    occurred.
 *
 * \param errnum
 *    Provides the error code.  If 0, there is no error to report.
 *
 * \return
 *    Returns the status of the \a errnum variable.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_03() [indirectly]
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_impl_file_error
(
   const char * filename,
   const char * mode,
   int errnum
)
{
   cbool_t result = (errnum == 0);
   if (! result)
   {
      char temp[128];
      if (xpc_errno_string(temp, sizeof(temp), errnum))
      {
         xpc_errprintf
         (
            "file error: '%s' for '%s', mode/function '%s'",
            temp, filename, mode
         );
      }
   }
   return result;
}

/******************************************************************************
 * xpc_impl_file_warning()
 *------------------------------------------------------------------------*//**
 *
 *    Checks for a file error, reporting it to the error-log if there is
 *    one.
 *
 *    This function is similar to xpc_impl_file_error() but reports a warning
 *    instead of an error.  Some "failures" (e.g. a file not being
 *    readable), aren't really errors, but just conditions that the caller
 *    has to adjust for.
 *
 * \param filename
 *    Provides the name of the file to be handled.
 *
 * \param mode
 *    Provides the mode of usage for the file, which can be one of the
 *    file-opening mode, or the number of the function in which the failure
 *    occurred.
 *
 * \param errnum
 *    Provides the error code.  If 0, there is no warning to report.
 *    As far as we know, if -1, the file was not present, so we will warn
 *    about that.
 *
 * \return
 *    Returns the status of the \a errnum variable.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_03() [indirectly]
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_impl_file_warning
(
   const char * filename,
   const char * mode,
   int errnum
)
{
   cbool_t result = (errnum == 0);
   if (! result)
   {
      if (errnum == (-1))              // also possible on Windows???
      {
         xpc_warnprintf
         (
            "file not found: '%s', mode/function '%s'", filename, mode
         );
      }
      else
      {
         char temp[128];
         if (xpc_errno_string(temp, sizeof(temp), errnum))
         {
            xpc_warnprintf
            (
               "file warning: '%s' for '%s', mode/function '%s'",
               temp, filename, mode
            );
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_file_access()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for the desired access modes.
 *
 *    The following modes are defined, and can be OR'd:
 *
 *       -  POSIX Value Windows  Meaning
 *       -  F_OK    0   0x00     Existence
 *       -  X_OK    1   N/A      Executable
 *       -  W_OK    2   0x04     Writable
 *       -  R_OK    4   0x02     Readable
 *
 * \note
 *    Although a file not
 *
 * \win32
 *    Windows does not provide a mode to check for executability.
 *
 * \replaces
 *    -  access()
 *    -  _access() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \param mode
 *    Provides the mode of the file to check for.
 *    This value should be in the cross-platform set of file-mode's for the
 *    various versions of the fopen() function.
 *
 * \return
 *    Returns 'true' if the requested modes are all supported for the file.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_03()
 *
 *//*-------------------------------------------------------------------------*/

#define ERRNO_FILE_DOESNT_EXIST     2  /* true in Windows and Linux  */

cbool_t
xpc_file_access (const char * filename, int mode)
{
   cbool_t result = is_file_name_good(filename);
   if (result)
   {

#ifdef _MSC_VER

      /**
       * Passing in X_OK here on Windows 7 yields a debug assertion!
       * For now, we just have to return false if that value is part of the
       * mode mask.
       */

      if (mode & X_OK)
      {
         xpc_errprint_func(_("cannot test X_OK (executable bit) on Windows"));
         result = false;
      }
      else
      {
         int errnum = S_ACCESS(filename, mode);
         result = xpc_impl_file_warning(filename, __func__, errnum);
      }

#else

      int errnum = S_ACCESS(filename, mode);
      result = xpc_impl_file_warning(filename, __func__, errnum);

#endif

   }
   return result;
}

/******************************************************************************
 * xpc_file_exists()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for existence.
 *
 * \replaces
 *    -  access()
 *    -  _access() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \return
 *    Returns 'true' if the file exists.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_exists (const char * filename)
{
   return xpc_file_access(filename, F_OK);
}

/******************************************************************************
 * xpc_file_readable()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for readability.
 *
 * \replaces
 *    -  access()
 *    -  _access() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \return
 *    Returns 'true' if the file is readable.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_readable (const char * filename)
{
   return xpc_file_access(filename, R_OK);
}

/******************************************************************************
 * xpc_file_writable()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for writability.
 *
 * \replaces
 *    -  access()
 *    -  _access() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \return
 *    Returns 'true' if the file is writable.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_writable (const char * filename)
{
   return xpc_file_access(filename, W_OK);
}

/******************************************************************************
 * xpc_file_accessible()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for readability and writability.
 *
 *    An even stronger test than xpc_file_exists.  At present, we see no need to
 *    distinguish read and write permissions.  We assume the file is
 *    accessible only if the file has both permissions.
 *
 * \replaces
 *    -  access()
 *    -  _access() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \return
 *    Returns 'true' if the file is readable and writable.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_accessible (const char * filename)
{
   return xpc_file_access(filename, R_OK|W_OK);
}

/******************************************************************************
 * xpc_file_executable()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file for the ability to be executed.
 *
 * \replaces
 *    -  stat()
 *    -  _stat() [Microsoft]
 *
 * \param filename
 *    Provides the name of the file to be checked.
 *
 * \return
 *    Returns 'true' if the file exists.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_executable (const char * filename)
{
   cbool_t result = is_file_name_good(filename);
   if (result)
   {
      stat_t statusbuf;
      int statresult = S_STAT(filename, &statusbuf);
      if (statresult == 0)                           // a good file handle?
      {
#if defined _MSC_VER
         result = (statusbuf.st_mode & _S_IEXEC) != 0;
#elif defined __BORLANDC__
         result = (statusbuf.st_mode & _S_IEXEC) != 0;
#else
         result =
         (
            ((statusbuf.st_mode & S_IXUSR) != 0) ||
            ((statusbuf.st_mode & S_IXGRP) != 0) ||
            ((statusbuf.st_mode & S_IXOTH) != 0)
         );
#endif
      }
      else
      {
         result = false;
         xpc_errprintf("%s: stat %s '%s'", __func__, _("failed on"), filename);
      }
   }
   return result;
}

/******************************************************************************
 * xpc_file_is_directory()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a file to see if it is a directory.
 *
 *    This function is also used in the function of the same name
 *    in fileutilities.cpp.
 *
 * \replaces
 *    -  stat()
 *    -  _stat() [Microsoft]
 *
 * \param filename
 *    Provides the name of the directory to be checked.
 *
 * \return
 *    Returns 'true' if the file is a directory.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_04()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t xpc_file_is_directory (const char * filename)
{
   cbool_t result = is_file_name_good(filename);
   if (result)
   {
      stat_t statusbuf;
      int statresult = S_STAT(filename, &statusbuf);
      if (statresult == 0)                           // a good file handle?
      {
#ifdef _MSC_VER
         result = (statusbuf.st_mode & _S_IFDIR) != 0;
#else
         result = (statusbuf.st_mode & S_IFDIR) != 0;
#endif
      }
      else
      {
         result = false;
         xpc_errprintf
         (
            "%s: stat %s '%s'", __func__, _("failed on"), filename
         );
      }
   }
   return result;
}

/******************************************************************************
 * xpc_file_open()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file in the desired operating mode.
 *
 * \replaces
 *    -  fopen()
 *    -  fopen_s() [Microsoft]
 *
 * \note
 *    Both the Microsoft and GNU "fopen" functions provide an errno-style
 *    error code when the function fails.  The Microsoft function fopen_s()
 *    returns the error-code directly.  The fopen(3) function returns a null
 *    pointer, and sets the value of 'errno' to indicate the error.
 *
 * \param filename
 *    Provides the name of the file to be opened.
 *
 * \param mode
 *    Provides the mode of the file.  See the is_file_mode_good() function.
 *    This value should be in the cross-platform set of file-mode's for the
 *    various versions of the fopen() function.
 *
 * \return
 *    Returns the file pointer if the function succeeded.  Otherwise, a null
 *    pointer is returned.  The caller must check this value before
 *    proceeding to use the file-handle.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_05()
 *
 *//*-------------------------------------------------------------------------*/

FILE * xpc_file_open (const char * filename, const char * mode)
{
   FILE * filehandle = nullptr;
   if (is_file_name_good(filename) && is_file_mode_good(mode))
   {
#ifdef _MSC_VER

      int errnum = (int) S_FOPEN(&filehandle, filename, mode);
      if (errnum != 0)
         filehandle = nullptr;

#else

      int errnum = 0;
      filehandle = S_FOPEN(filename, mode);
      if (is_nullptr(filehandle))
         errnum = errno;

#endif

      (void) xpc_impl_file_error(filename, mode, errnum);
   }
   return filehandle;
}

/******************************************************************************
 * xpc_file_open_for_read()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for binary reading.
 *
 *    This function calls xpc_file_open(filename, "rb").
 *
 * \replaces
 *    -  fopen()
 *
 * \param filename
 *    Provides the name of the file to be opened.
 *
 * \return
 *    Returns the file pointer if the function succeeded.  Otherwise, a null
 *    pointer is returned.  The caller must check this value before
 *    proceeding to use the file-handle.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

FILE * xpc_file_open_for_read (const char * filename)
{
   FILE * filehandle = 0;
   if (xpc_file_readable(filename))
      filehandle = xpc_file_open(filename, "rb");   // open for reading only

   return filehandle;
}

/******************************************************************************
 * xpc_file_open_for_write()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for binary writing (and reading) in update-able style.
 *
 *    The intent of this function is to always overwrite the contents of the
 *    file if it exists.
 *
 *    In other word, always start over with the file.
 *
 * \note
 *    This function use to call xpc_file_open(filename, "r+b") if the file already
 *    exists, and xpc_file_open(filename, "w+b") if it does not already exist.
 *
 *    In light of Alex's review comments, then, it seems that we can
 *    simplify this and just use the latter ("w+b") call, so that the file
 *    is created, or, if it exists, it is truncated.
 *
 * \replaces
 *    -  fopen()
 *
 * \param filename
 *    Provides the name of the file to be opened.
 *
 * \return
 *    Returns the file pointer if the function succeeded.  Otherwise, a null
 *    pointer is returned.  The caller must check this value before
 *    proceeding to use the file-handle.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

FILE * xpc_file_open_for_write (const char * filename)
{
   return xpc_file_open(filename, "w+b");  // create or truncate for updating
}

/******************************************************************************
 * xpc_file_create_for_write()
 *------------------------------------------------------------------------*//**
 *
 *    Recreates a file for binary writing.
 *
 *    This function calls xpc_file_open(filename, "wb").
 *
 * \replaces
 *    -  fopen()
 *
 * \note
 *    You might expect that this function should fail if the file already
 *    exists.  Indeed, that would be a nice function to have.  However,
 *    this function supports legacy code, and we can't modify what it does
 *    at this time.
 *
 * \param filename
 *    Provides the name of the file to be opened.
 *
 * \return
 *    Returns the file pointer if the function succeeded.  Otherwise, a null
 *    pointer is returned.  The caller must check this value before
 *    proceeding to use the file-handle.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

FILE * xpc_file_create_for_write (const char * filename)
{
   return xpc_file_open(filename, "wb");                  // start all over again
}

/******************************************************************************
 * xpc_file_close()
 *------------------------------------------------------------------------*//**
 *
 *    Closes a file opened by the xpc_file_open() functions.
 *
 * \replaces
 *    -  fclose()
 *
 * \param filehandle
 *    Provides the file-handle to be closed.
 *
 * \param filename
 *    Provides the name of the file to be closed.  Used only for error
 *    reporting.  If you don't care about it, pass in a null pointer or an
 *    empty string.
 *
 * \return
 *    Returns 'true' if the close operation succeeded.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_05()
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t xpc_file_close (FILE * filehandle, const char * filename)
{
   cbool_t result = xpc_not_nullptr(filehandle, _("xpc_file_close null handle"));
   if (result)
   {
      int rcode = fclose(filehandle);
      result = xpc_impl_file_error(filename, __func__, rcode);
   }
   else
   {
      if (not_nullptr(filename) && (*filename != 0))  /* strlen(filename) > 0 */
         xpc_errprintex(filename, _("File"));
   }
   return result;
}

/******************************************************************************
 * file_delete()
 *------------------------------------------------------------------------*//**
 *
 *    Deletes a file based on its filename.
 *
 *    This function first makes sure that the filename is valid, using the
 *    is_file_name_good() function.
 *
 * \replaces
 *    -  _unlink() [Microsoft]
 *    -  unlink() [GNU]
 *
 * \param filename
 *    Provides the name of the file to be delete.
 *
 * \return
 *    Returns 'true' if the delete operation succeeded.  It also returns
 *    'true' if the file did not exist in the first place.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_05()
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t file_delete (const char * filename)
{
   cbool_t result = xpc_not_nullptr(filename, _("file_delete null handle"));
   if (result)
      result = is_file_name_good(filename);

   if (result)
   {
      result = xpc_file_exists(filename);
      if (result)
      {

#ifdef WIN32
         int rcode;
         if (xpc_file_is_directory(filename))
            rcode = rmdir(filename);
         else
            rcode = S_UNLINK(filename);

#else
         int rcode = S_UNLINK(filename);
#endif

         if (rcode == (-1))
            result = xpc_impl_file_error(filename, __func__, errno);
      }
      else
      {
         /*
          * The file doesn't exist, so we don't need to delete it.  But we
          * need to tell the caller that it is gone, instead of failing.
          */

         result = true;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_platform_slash()
 *------------------------------------------------------------------------*//**
 *
 *    Returns the operating-system specific slash character.
 *
 * \return
 *    For Windows, a backslash is returned.  Otherwise, a forward
 *    slash is returned.
 *
 *//*-------------------------------------------------------------------------*/

int xpc_platform_slash (void)
{
#ifdef WIN32
   return '\\';                        // backslash is default
#else
   return '/';                         // forward slash is default
#endif
}

/******************************************************************************
 * xpc_path_slash()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a filename to see if it contains forward slashes or
 *    backslashes, and returns the path separator that applies.
 *
 * \param filespec
 *    The file-specification to scan for a slash character.
 *
 * \return
 *    Whichever slash is found first is returned.  If there is no slash,
 *    then, for Windows, a backslash is returned.  Otherwise, a forward
 *    slash is returned.
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_path_slash (const char * filespec)
{
   int result = xpc_platform_slash();
   if (is_file_name_good(filespec))
   {
      const char * slashptr = strchr(filespec, result);  // look for default
      if (is_nullptr(slashptr))                          // not found
      {
         int nextchar = result == '/' ? '\\' : '/' ;
         slashptr = strchr(filespec, nextchar);          // look for alternate
         if (not_nullptr(slashptr))
            result = nextchar;
      }
   }
   return result;
}

/******************************************************************************
 * is_path_rooted()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a filename to see if starts from the 'root' directory.
 *
 *    A root directory starts at the top of the file system.  In many cases,
 *    we want to avoid messing with the root directory.  Here are some
 *    examples:
 *
 *       -  /var/run
 *       -  C:\var\run\myapp
 *       -  C:/var/run/yourapp
 *       -  \\myhost\share\...
 *       -  //myhost/share/...
 *
 * \param filespec
 *    The file-specification to scan for a "rootitude".
 *
 * \return
 *    If the directory represents an absolute path (from root), then 'true'
 *    is returned.  Otherwise 'false' is returned.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
is_path_rooted (const char * filespec)
{
   int result = false;
   if (is_file_name_good(filespec))
   {
      int target = xpc_platform_slash();
      if (*filespec == target)
         result = true;
      else
      {
         target = target == '/' ? '\\' : '/' ;
         if (*filespec == target)
            result = true;
         else
         {
            int first = toupper(filespec[0]);
            if ((first >= 'A') && (first <= 'Z') && (filespec[1] == ':'))
               result = true;
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_directory_create()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a directory based on its directory name.
 *
 *    This function is a super-charged turbo version of the mkdir() and
 *    _mkdir() functions.  It makes sure that the whole sequences of
 *    directories in a path-name are created, if they don't exist already.
 *
 *    In addition, the path-name parameter is massaged in whatever way is
 *    necessary, such as removing any terminating backslash.
 *
 *    For now, since at least _mkdir() can handle either the '/' or the '\',
 *    we don't worry about converting to the '\' for DOS-like stuff.
 *
 *    The input pathname is in the form of something like
 *
 *         "C:\dir0\dir1\dir2\...\dirn"      or
 *
 *         "C:\dir0\dir1\dir2\...\dirn\"
 *
 *    The code works by temporarily converting each '\' character in
 *    sequence to a null, and seeing if the resulting entity (drive or
 *    directory) exists.  If not, the attempt is made to create it.  If it
 *    succeeds, the null is converted back to a '\', and the next
 *    subdirectory is worked on, until all are done.
 *
 *    Added the ability to handle forward slashes as well as back slashes.
 *    See the xpc_path_slash() function above.
 *
 * \replaces
 *    -  _mkdir() [Microsoft]
 *    -  mkdir() [GNU].
 *
 * \param filename
 *    Provides the name of the file to be delete.
 *
 * \return
 *    Returns 'true' if the create operation succeeded.  It also returns
 *    'true' if the directory already exists.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

cbool_t xpc_directory_create (const char * dirname)
{
   cbool_t result = is_file_name_good(dirname);
   if (result)
   {
      if (xpc_file_exists(dirname))              // directory already exists?
         return true;
   }
   if (result)
   {
      char currdir[F_MAX_PATH];             // A Windows limitation
      cbool_t more = true;
      int slash = xpc_path_slash(dirname);       // determine path separator
      char * endptr = &currdir[0];           // start at the beginning
      char * nextptr;                        // just what it says!
      char * termination;
      (void) xpc_string_copy(currdir, sizeof(currdir), dirname);
      termination = strchr(endptr, '\0');
      do
      {
         nextptr = strchr(endptr, slash);    // find next slash/backslash
         if (nextptr == 0)                   // if still not found ...
         {
            more = false;                    // ... this will be last subdir ...
            if (endptr < termination)        // ... and if not yet at end ...
               nextptr = termination;        // ... this will be the end
         }
         if (nextptr != 0)
         {
            /*
             * For absolute UNIX paths, do not remove the first slash.
             */

            if (nextptr > &currdir[0])
               *nextptr = 0;                 // convert to a null

            if (! xpc_file_exists(currdir))      // see if subdirectory exists
            {
               int retcode;

#if defined POSIX
               mode_t pathmode = 0755;       // rwxr-xr-x
               retcode = F_MKDIR(currdir, pathmode);
#else
               retcode = F_MKDIR(currdir);
#endif
               if (retcode == -1)
               {
                  more = false;
                  result = false;
               }
            }
            if (more)
            {
               *nextptr = (char) slash;      // convert back to slash/backslash
               endptr = nextptr + 1;         // point just past it
            }
         }

      } while (more);
   }
   return result;
}

/******************************************************************************
 * xpc_directory_delete()
 *------------------------------------------------------------------------*//**
 *
 *    Deletes a directory based on its directory name.
 *
 *    This function first makes sure that the name is valid, using the
 *    is_file_name_good() function.
 *
 * \replaces
 *    -  _rmdir() [Microsoft]
 *    -  rmdir() [GNU].  Note, however, that file_delete() can be called
 *       and will work with both files and directories.
 *
 * \param filename
 *    Provides the name of the file to be delete.
 *
 * \return
 *    Returns 'true' if the delete operation succeeded.  It also returns
 *    'true' if the directory did not exist in the first place.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_directory_delete (const char * filename)
{
   cbool_t result = xpc_not_nullptr(filename, _("file_delete null handle"));
   if (result)
      result = is_file_name_good(filename);

   if (result)
      if (xpc_file_exists(filename))

   if (result)
   {
      int rcode = rmdir(filename);
      if (rcode == (-1))
         result = xpc_impl_file_error(filename, __func__, errno);
   }
   return result;
}

/******************************************************************************
 * xpc_directory_delete_recursive()
 *------------------------------------------------------------------------*//**
 *
 *    Deletes a directory from bottom to top.
 *
 *    This function first makes sure that the name is valid, using the
 *    is_file_name_good() function.  It will also refuse to remove a root
 *    directory (a directory name starting with '/' or something like
 *    'C:\').
 *
 * \param filename
 *    Provides the name of the file to be delete.
 *
 * \return
 *    Returns 'true' if the delete operation(s) succeeded.  It also returns
 *    'true' if the directory did not exist in the first place.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

cbool_t xpc_directory_delete_recursive (const char * dirname)
{
   cbool_t result = is_file_name_good(dirname);
   if (result)
      result = ! is_path_rooted(dirname);

   if (result)
   {
      result = xpc_directory_delete(dirname);    // delete final sub-directory
      if (result)
      {
         char currdir[F_MAX_PATH];
         int slash = xpc_path_slash(dirname);    // determine path separator
         if (xpc_string_copy(currdir, sizeof(currdir), dirname))
         {
            for (;;)
            {
               char * slasher = strrchr(currdir, slash);
               if (not_nullptr(slasher))
               {
                  while (*slasher == slash)  // handle consecutive slashes
                     *slasher-- = 0;

                  if (strlen(currdir) > 0)
                  {
                     if
                     (
                        (strcmp(currdir, ".") != 0) &&
                        (strcmp(currdir, "..") != 0)
                     )
                     result = xpc_directory_delete(currdir);
                     if (! result)
                        break;
                  }
                  else
                     break;
               }
               else
                  break;

            }
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_file_handle_seek()
 *------------------------------------------------------------------------*//**
 *
 *    A cross-type wrapper for the various lseek() functions.
 *
 * \replaces
 *    -  lseek()
 *    -  _lseek() [Microsoft]
 *
 * \param filehandle
 *    Provides the filehandle of the open file to seek.
 *
 * \param offset
 *    Provides the offset into the file from the position indicated by the
 *    \a whence parameter.
 *
 * \param whence
 *    Provides the origin within the file from which the offset is to be
 *    made.  The standard lseek() values are used:
 *
 *       -  SEEK_SET.  The origin is the beginning of the file.
 *       -  SEEK_CUR.  The origin is the current position within the file.
 *       -  SEEK_END.  The origin is the end of the file.
 *
 *    If the offset gets set past the end of the file, the filesize does not
 *    actually change until (more) data is written.
 *
 * \return
 *    Returns the new file position from the beginning of the file.  If the
 *    function fails for some reason, then -1L is returned.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_08()
 *
 *//*-------------------------------------------------------------------------*/

off_t xpc_file_handle_seek (int filehandle, off_t offset, int whence)
{
   off_t result = (off_t) (-1);
   cbool_t ok = filehandle != -1;
   if (ok)
   {
      /*
       * Although the lseek() function will catch these errors, we want to
       * emit our own messages and not even make the call.
       */

      ok = (whence == SEEK_SET) || (whence == SEEK_CUR) || (whence == SEEK_END);
      if (! ok)
         xpc_errprint_func(_("bad file-origin parameter"));
   }
   else
      xpc_errprint_func(_("bad file-handle"));

   if (ok)
   {
      result = S_LSEEK(filehandle, offset, whence);
      if (result == -1L)
         (void) xpc_impl_file_error(__func__, _("unknown file"), errno);
   }
   return result;
}

/******************************************************************************
 * xpc_file_copy()
 *------------------------------------------------------------------------*//**
 *
 *    Copies a file to a file with a different name.
 *
 *    The file names can be absolute, or they can be relative.  This
 *    function will refuse to copy a file onto itself.  (This check is
 *    simply a check for string-equivalence of the names; the check does not
 *    try to figure out if the paths resolve to the same file-name and
 *    path).
 *
 * \param newfile
 *    The full file path-specification for the new copy of the file.
 *
 * \param oldfile
 *    The full file path-specification for the input file to be copied.
 *
 * \note
 *    The two parameters are in \e reverse order versus the extant
 *    common::xpc_file_copy() function.  The order here is more consistent with
 *    other source/destination functions.
 *
 * \return
 *    Returns 'true' if all of the parameters were valid, and all of the
 *    operations succeeded.  Returns 'false' otherwise.
 *
 * \todo
 *    Consider the use of fflush() to flush the user-space buffers provided
 *    by the C library, and then fsync() to have the kernel flush the file
 *    to disk, followed by fsync() on the directory itself to finish the
 *    process.  See the Linux man pages [fflush(3), sync(2), and fsync(2)].
 *
 * \unittests
 *    -  TBD.c: unit_test_10_05()
 *    -  TBD.c: unit_test_10_06()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_copy
(
   const char * newfile,
   const char * oldfile
)
{
   cbool_t result = false;
   if (not_nullptr_2(oldfile, newfile))
   {
#ifdef _MSC_VER
      result = ! xpc_string_equivalent(newfile, oldfile);    // Windows
#else
      result = ! xpc_string_identical(newfile, oldfile);     // UNIX/Linux
#endif

      if (result)
      {
         FILE * input = xpc_file_open_for_read(oldfile);
         if (not_nullptr(input))
         {
            cbool_t okinput = false;
            cbool_t okoutput = false;
            FILE * output = xpc_file_create_for_write(newfile);
            if (not_nullptr(output))
            {
               int ci;
               int co;
               while ((ci = fgetc(input)) != EOF)
               {
                  co = fputc(ci, output);
                  if (co == EOF)
                     break;
               }
               okoutput = xpc_file_close(output, newfile);
            }
            okinput = xpc_file_close(input, oldfile);
            result = okinput && okoutput;
         }
      }
      else
         xpc_warnprint_func(_("filenames are equivalent"));
   }
   return result;
}

/******************************************************************************
 * xpc_file_handle_open()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file in the desired operating mode, returning a handle instead
 *    of a file pointer.
 *
 * \replaces
 *    -  open()
 *    -  _open() [Microsoft]
 *
 *    Here are the file-mode flags in our supported subset:
 *
 *       -  O_RDWR.  Open for reading and writing.
 *       -  O_RDONLY.  Open for reading only.
 *       -  O_WRONLY.  Open for writing only.  Not supported on Windows.
 *       -  O_APPEND.  Open so that each write goes to the end of the file.
 *       -  O_CREAT.  Create the file if it does not exist.
 *       -  O_TRUNC.  Open the file and truncate it.
 *
 * \warning
 *    There are way too many file modes in this function, in Windows or
 *    otherwise, to commit to checking them all for a cross-platform subset
 *    at this time.
 *
 * \param filename
 *    Provides the name of the file to be opened.
 *
 * \param flags
 *    Provides flags to determine how the file is handled.  Some of the
 *    flags common to Windows and UNIX/Linux are:
 *
\verbatim
          LINUX       Windows
         ========    =========
         O_RDWR      _O_RDWR
         O_RDONLY    _O_RDONLY
         O_WRONLY    _O_WRONLY (doesn't function on Windows)
         O_APPEND    _O_APPEND
         O_CREAT     _O_CREAT
         O_TRUNC     _O_TRUNC
\endverbatim
 *
 *    Best to stick with that limited set.  Probably better to use
 *    xpc_file_open() instead, unless you really need the extra features.
 *
 * \param permissions
 *    Provides the permissions of the file.  The useful values are:
 *
\verbatim
          LINUX       Windows
         ========    =========
                     _S_IREAD
                     _S_IWRITE
         S_IRUSR
         S_IWUSR
         S_IXUSR
\endverbatim
 *
 *    There are a lot more, but we'll document/fix this mess later.
 *    For now, you can pass \a permissions = 0, which sets the following
 *    values for you:
 *
 *       -  Linux:    S_IRUSR |  S_IWUSR
 *       -  Windows: _S_IREAD | _S_IWRITE
 *
 * \return
 *    Returns the file handle if the function succeeded.  Otherwise, a -1 is
 *    returned.  The caller must check this value before proceeding to use
 *    the file-handle.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_07()
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_file_handle_open (const char * filename, int flags, int permissions)
{
   int filehandle = -1;
   if (is_file_name_good(filename))
   {
      int errnum;

#ifdef _MSC_VER

      if (permissions == 0)
         permissions = _S_IREAD | _S_IWRITE;    /* permit read and write      */

      errnum = (int) S_OPEN
      (
         &filehandle, filename, flags,
         _SH_DENYNO,                            /* permit read/write sharing  */
         permissions
      );
      if (errnum != 0)
         filehandle = -1;

#else

      errnum = 0;
      if (permissions == 0)
         permissions = S_IRUSR | S_IWUSR;       /* permit read and write      */

      filehandle = S_OPEN(filename, flags, permissions);
      if (filehandle == -1)
         errnum = errno;

#endif

      (void) xpc_impl_file_error(filename, __func__, errnum);
   }
   return filehandle;
}

/******************************************************************************
 * xpc_file_handle_close()
 *------------------------------------------------------------------------*//**
 *
 *    Closes an open file based on an open file-handle.
 *
 * \replaces
 *    -  close()
 *    -  _close() [Microsoft]
 *
 * \param filehandle
 *    Provides the file-handle of the open file to be closed.
 *
 * \param filename
 *    Provides the name of the file to be closed, or some other relevant
 *    message, such as the context in which this function was called.  Used
 *    only for error reporting.  If you don't care about it, pass in a null
 *    pointer or an empty string.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the operation
 *    succeeded.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  TBD.c: unit_test_10_07()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_file_handle_close (int filehandle, const char * filename)
{
   cbool_t result = filehandle != -1;
   if (result)
   {
      int errnum = S_CLOSE(filehandle);
      if (errnum == -1)
         errnum = errno;

      (void) xpc_impl_file_error(filename, __func__, errnum);
      result = false;
   }
   else
   {
      if (is_nullptr(filename) || (*filename == 0))   /* strlen(filename) == 0*/
         filename = _("unspecified file");

      xpc_errprintf
      (
         "%s: %s %s",
         __func__, _("uninitialized file handle for"), filename
      );
   }
   return result;
}

/******************************************************************************
 * End of file_functions.c
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
