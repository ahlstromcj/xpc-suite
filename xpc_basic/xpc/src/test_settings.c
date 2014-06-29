/******************************************************************************
 * test_settings.c
 *------------------------------------------------------------------------*//**
 *
 * \file          test_settings.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2012-06-02
 * \updates       2012-06-23
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Makes various settings to make it easier to locate resources in
 *    unit-testing.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/environment.h>           /* xpc_get_environment()               */
#include <xpc/errorlogging.h>          /* showinfo(), etc.                    */
#include <xpc/gettext_support.h>       /* _() macro                           */
#include <xpc/file_functions.h>        /* xpc_file_exists(), etc.             */
#include <xpc/file_macros.h>           /* define F_GETCWD, etc.               */
#include <xpc/test_settings.h>         /* these functions                     */
#include <xpc/xstrings.h>              /* xpc_string_n_cat(), etc             */

#if XPC_HAVE_UNISTD_H
#include <unistd.h>                    /* chdir()                             */
#else                                  /* probably Windows                    */
#include <io.h>                        /* _chdir()                            */
#endif                                 /* WIN32 versus POSIX                  */

/******************************************************************************
 * get_current_directory()
 *------------------------------------------------------------------------*//**
 *
 *    A wrapper for getcwd() and other such functions.  It obtains
 *    the current working directory in the application.
 *
 *    Since this function makes a copy of stack data, it is thread-safe.
 *    However, the caller mustdelete the copy when finished with it, by
 *    freeing the pointer that is returned.
 *
 * \return
 *    Returns a pointer to the internally-allocated buffer containing the
 *    name of the current directory.  If this function fails, then a null
 *    pointer is returned.
 *
 *//*-------------------------------------------------------------------------*/

const char *
get_current_directory ()
{
   char temp[F_MAX_PATH];
   char * result = nullptr;
   char * cwd = F_GETCWD(temp, F_MAX_PATH);        /* get current directory   */
   if (not_nullptr(cwd))
   {
      size_t length = strlen(cwd);                 /* don't use "result"      */
      if (length > 0)
      {
         char * destination = malloc(length + 1);  /* (plus null)             */
         if (not_nullptr(destination))
         {
            cbool_t good = xpc_string_copy(destination, length + 1, cwd);
            if (good)
            {
               result = destination;
               xpc_infoprintf("%s: %s", _("current directory"), destination);
            }
         }
         else
            xpc_errprint_func(_("allocation failed"));
      }
      else
         xpc_errprint_func(_("empty directory name returned"));
   }
   else
      xpc_errprint_func(_("could not get"));

   return result;
}

/******************************************************************************
 * free_current_directory()
 *------------------------------------------------------------------------*//**
 *
 *    A wrapper for free().
 *
 * \param dir
 *    The directory name buffer to be freed.
 *
 *//*-------------------------------------------------------------------------*/

void
free_current_directory (const char * dir)
{
   if (not_nullptr(dir))
      free((char *) dir);
}

/******************************************************************************
 * set_current_directory()
 *------------------------------------------------------------------------*//**
 *
 *    A wrapper replacement for chdir().  It sets the current working
 *    directory in the application.
 *
 * \param path
 *    The path to which the current working directory should be set.
 *
 * \return
 *    If this function is provided valid parameters, and succeeds, then
 *    'true' is returned.  Otherwise, false is returned.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
set_current_directory (const char * path)
{
   cbool_t result = not_nullptr(path) && (*path != 0);   /* strlen(path) > 0  */
   if (result)
   {
      int rcode = F_CHDIR(path);
      result = rcode == POSIX_SUCCESS;
      if (result)
      {
         if (xpc_showinfo())
         {
            const char * tempptr = get_current_directory();
            free_current_directory(tempptr);
         }
      }
      else
         xpc_errprint_func(_("failed"));
   }
   else
      xpc_errprint_func(_("null pointer or empty path"));

   return result;
}

/******************************************************************************
 * get_relative_directory()
 *------------------------------------------------------------------------*//**
 *
 *    Looks for a partial directory path in the current directory, and
 *    returns a string for the relative path needed to get to it from the
 *    current directory.
 *
 *    Since this function makes a copy of stack data, it is thread-safe.
 *    However, the caller has to delete the copy when finished with it, by
 *    freeing the pointer that is returned.
 *
 * \example
 *    Assume the current directory is
 *    <tt>~/bin/.hudson/jobs/mysolution/projects/libraries/myproj/tests</tt>
 *    and that we want to find the "projects" portion.  The returned string
 *    will be "../../../".
 *
 * \param portion
 *    Provides the name of the subdirectory for which to search.  The search
 *    is case-sensitive.
 *
 * \param directory
 *    If provided, this directory is the one searched for the sub-portion.
 *    Otherwise, the current working directory is obtained and used.
 *
 * \return
 *    Returns a pointer to the internally-allocated buffer containing the
 *    name of the relative directory.  If this function fails, then a null
 *    pointer is returned.
 *
 *//*-------------------------------------------------------------------------*/

const char *
get_relative_directory (const char * portion, const char * directory)
{
   char temp[F_MAX_PATH];
   char * result = nullptr;
   char * cwd;
   if (not_nullptr(directory))
   {
      xpc_string_copy(temp, sizeof(temp), directory);
      cwd = &temp[0];
   }
   else
      cwd = F_GETCWD(temp, sizeof(temp));       /* get current directory      */

   if (not_nullptr_2(portion, cwd) && strlen(portion) > 0)
   {
      const char * onedot = "./";
      const char * twodot = "../";
      int tokencount = 0;
      int matchcount = -1;
      char resulttemp[F_MAX_PATH];
      char * slashes = "/\\";
      char * beginning;
      size_t length;
      if (not_nullptr(strchr(cwd, '\\')))
      {
         onedot = ".\\";
         twodot = "..\\";
      }
      resulttemp[0] = 0;
      beginning = strtok(cwd, slashes);
      while (not_nullptr(beginning))
      {
         tokencount++;
         if (strcmp(beginning, portion) == 0)
            matchcount = tokencount;

         beginning = strtok(NULL, slashes);
      }
      if (matchcount != (-1))
      {
         int dotcount = tokencount - matchcount;
         if (dotcount == 0)
            xpc_string_copy(resulttemp, sizeof(resulttemp), onedot);
         else
         {
            int i;
            for (i = 0; i < dotcount; i++)
               xpc_string_cat(resulttemp, sizeof(resulttemp), twodot);
         }
      }
      length = strlen(resulttemp);
      if (length > 0)
      {
         char * destination = malloc(length + 1);        /* length plus null  */
         if (not_nullptr(destination))
         {
            cbool_t good = xpc_string_copy(destination, length + 1, resulttemp);
            if (good)
            {
               result = destination;
               xpc_infoprintf("%s: %s", _("relative directory"), destination);
            }
         }
         else
            xpc_errprint_func(_("allocation failed"));
      }
      else
         xpc_warnprint_func(_("sub-directory name not found"));
   }
   else
      xpc_errprint_func(_("could not get directory"));

   return result;
}

/******************************************************************************
 * TEST_DIR_BASE
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating a test directory.
 *
 *//*-------------------------------------------------------------------------*/

#define TEST_DIR_BASE         "test"      // used if null test_dir_base given

/******************************************************************************
 * TEST_DIR_BASE2
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating a test directory.
 *
 *    We need to account for this new convention for the name of the test
 *    directory.
 *
 *//*-------------------------------------------------------------------------*/

#define TEST_DIR_BASE2        "tests"     // the new version, not yet used

/******************************************************************************
 * DEBUG_DIR
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating the debug directory.
 *
 *//*-------------------------------------------------------------------------*/

#define DEBUG_DIR             "debug"

/******************************************************************************
 * RELEASE_DIR
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating a release directory.
 *
 *//*-------------------------------------------------------------------------*/

#define RELEASE_DIR           "release"

/******************************************************************************
 * VISUAL_STUDIO_DIR
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating a project directory.
 *
 *    The location of this directory is as per Visual Studio conventions.
 *    It means "no changes, we're already in the main project directory".
 *
 *//*-------------------------------------------------------------------------*/

#define VISUAL_STUDIO_DIR     ""

/******************************************************************************
 * SOURCE_DIR_BASE
 *------------------------------------------------------------------------*//**
 *
 *    Internal value indicating a source-code directory.
 *
 *//*-------------------------------------------------------------------------*/

#define SOURCE_DIR            "src"

/******************************************************************************
 * set_test_directory_ex()
 *------------------------------------------------------------------------*//**
 *
 *    This function checks the current directory, and adjusts it so that the
 *    Win32 applications will find the same files as the Linux applications,
 *    whether the application is running under the Visual C++ debugger or
 *    not.
 *
 *    If the current directory has "release" or "rebug" in it, it is almost
 *    certain that the executable is running outside of the Visual Studio
 *    environment.  In this case, we need to change the directory to
 *    "../../test" to match where the Linux make puts the executables.
 *
 *    Otherwise, it is likely that the executable is running inside the
 *    Visual Studio environment.  In this case, the current directory is the
 *    win32 or vs10 directory, so we need to change the directory to "../test".
 *
 *    If the current directory has "src" in it at the end, then it is
 *    probably an application built in the old automake location, the \e src
 *    directory itself.
 *
 *    A newer convention is to call the directory "tests", so this code
 *    has to handle that case, too.
 *
 * \param base
 *    If not null, this item should point to a base directory for the test
 *    application.  If null, then the default base directory is provided --
 *    it is a directory in the current project directory for the test
 *    application, name TEST_DIR_BASE ("test").  But even if that directory
 *    is found, it can be overridden by the new convention, TEST_DIR_BASE2
 *    ("tests").
 *
 * \return
 *    Returns 'true' under the following conditions:
 *       -  If the directory setting was made successfully.
 *       -  If the current directory was already the desired directory.
 *    Otherwise, 'false' is returned.
 *
 * \followup
 *    We need to be able to handle odd cases, such as
 *    "/home/me/src/libs/src/myproject/src" and
 *    "/home/me/tests/mytestapp/src", more robustly; right now, this stuff
 *    works only for project that have only a main directory, and a "src",
 *    "debug", "release", or "test"/"tests" directory as immediate children
 *    of that main directory.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
set_test_directory_ex (const char * base)
{
   cbool_t result = false;
   const char * currentdir = get_current_directory();
   if (not_nullptr(currentdir))
   {
      char * testptr;
#ifdef WIN32
      char * slash = "\\";
#else
      char * slash = "/";
#endif
      if (is_nullptr(base))
         base = TEST_DIR_BASE;

      /**
       * First see if we're already likely to be in the test or tests
       * directory.  If we are, we will not make any changes.  Currently,
       * this doesn't handle the case where we are in a subdirectory of
       * test.  It also doesn't handle anything other than 'test' or
       * 'tests'.
       */

      testptr = strstr(currentdir, base);          /* look for "test"         */
      if (not_nullptr(testptr))                    /* it was found            */
      {
         /*
          * If we found "test", then we see if "tests" exists.  If so, we'll
          * use that one.  Cases like "tests_new" will fail.
          */

         if (not_nullptr(strstr(currentdir, TEST_DIR_BASE2)))
            base = TEST_DIR_BASE2;
      }
      else
         base = TEST_DIR_BASE2;                    /* freshest default        */

      /*
       * At this point, base is either what the caller passed, "test", or
       * "tests".  testptr points to either what the caller passed, "test",
       * "tests", or nothing.
       */

      if (is_nullptr(testptr))                     /* no test(s) dir found    */
      {
         char temp[F_MAX_PATH];
         testptr = strstr(currentdir, DEBUG_DIR);
         if (not_nullptr(testptr))
            testptr = "..";
         else
         {
            testptr = strstr(currentdir, RELEASE_DIR);
            if (not_nullptr(testptr))
               testptr = "..";
            else
            {
               testptr = strstr(currentdir, SOURCE_DIR);
               if (not_nullptr(testptr))
                  testptr = "..";                  /* some build in "src"     */
               else
                  testptr = VISUAL_STUDIO_DIR;     /* ""                      */
            }
         }
         temp[0] = 0;                              /* empty it                */
         if (strlen(testptr) > 0)                  /* add ".." plus slash     */
            (void) xpc_string_n_cat(temp, sizeof(temp), 2, testptr, slash);

         xpc_string_cat(temp, sizeof(temp), base); /* add "test" or "tests"   */
         testptr = &temp[0];                       /* point to new directory  */
         if (xpc_file_exists(testptr))
            result = set_current_directory(testptr);
         else
         {
            result = true;
            xpc_warnprint(_("no test(s) directory exists, ignoring it"));
         }
      }
      else                                         /* already in "test(s)"    */
      {
         cbool_t is_nested;
         char temp[F_MAX_PATH];
         temp[0] = 0;
         xpc_string_n_cat(temp, sizeof(temp), 3, base, slash, DEBUG_DIR);
         testptr = strstr(currentdir, temp);       /* "tests/debug"           */
         is_nested = not_nullptr(testptr);
         if (! is_nested)
         {
            temp[0] = 0;
            xpc_string_n_cat(temp, sizeof(temp), 3, base, slash, RELEASE_DIR);
            testptr = strstr(currentdir, temp);    /* "tests/release"         */
            is_nested = not_nullptr(testptr);
         }
         if (! is_nested)
         {
            temp[0] = 0;
            xpc_string_n_cat(temp, sizeof(temp), 3, base, slash, SOURCE_DIR);
            testptr = strstr(currentdir, temp);    /* "tests/src"             */
            is_nested = not_nullptr(testptr);
         }
         if (is_nested)
            result = set_current_directory("..");
         else
         {
            result = true;
            xpc_infoprint_func(_("no need to change directories"));
         }
      }
      free_current_directory(currentdir);
   }
   return result;
}

/******************************************************************************
 * set_test_directory()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a default version of set_test_directory().
 *
 *    This function simply calls set_test_directory_ex() with a null pointer
 *    parameter.  Thus it assumes that the name of our test directory is
 *    actually "test" (or "tests"; anything else that starts with "tests"
 *    will fail).
 *
 * \return
 *    Returns 'true' if the directory setting was made successfully, or if
 *    the current directory was already the desired directory.  Otherwise,
 *    'false' is returned.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
set_test_directory ()
{
   return set_test_directory_ex(nullptr);
}

/******************************************************************************
 * find_in_source_file()
 *------------------------------------------------------------------------*//**
 *
 *    Searches for a file in the in-source directory in order to access test
 *    resources.
 *
 *    This function solves the case where we have generated a GNU automake
 *    "out-of-source" build, but still need to find test-files resources
 *    located in the "in-source" directory for the project.
 *
 *    For example, assume we're in an out-of-source debug directory such as
 *    <tt>~/xpc-suite/debug/xpc/tests</tt>, and the xpc-test executable
 *    needs to find this file: <tt>~/xpc-suite/xpc/tests/test.ini</tt>.
 *
 *    This function needs to move up one level, save the project name
 *    ("xpc"), move up again to "debug", and move up one more time to
 *    "xpc-suite".  Then it needs to move down until it finds the in-source
 *    "xpc" directory.
 *
 *    If we're already in the in-source directory, we have to find out that
 *    fact by moving up to the "xpc-suite" directory and noting that "xpc"
 *    was directly below it.
 *
 * \note
 *    Alternatively, we could have the make process copy the file to the
 *    out-of-source locations.
 *
 * \param filename
 *    The name of the test-resource to find.
 *
 * \param base
 *    Provides the highest directory that should be touched.  This is needed
 *    in the case where we're already in-source.  If null, then there's a
 *    danger of going up too high.
 *
 * \return
 *    Returns the path to the test-resource, if available.  Otherwise, a
 *    null pointer is returned.  The caller must free this pointer when
 *    finished.
 *
 *//*-------------------------------------------------------------------------*/

const char *
find_in_source_file (const char * filename, const char * base)
{
   const char * result = nullptr;
   cbool_t ok = not_nullptr(filename);
   if (ok)
   {
      size_t length = 0;

      if (length > 0)
      {
         char * destination = malloc(length + 1);  /* (plus null)             */
         if (not_nullptr(destination))
         {
            //
            // more to come!!!!!!!!!!!!!!!!!!!!!!!!!
            //

            cbool_t good = xpc_string_copy(destination, length + 1, "DUMMY");
            if (good)
            {
               result = destination;
               xpc_infoprintf("%s: %s", _("file specification"), destination);
            }
         }
         else
            xpc_errprint_func(_("allocation failed"));
      }
   }
   return result;
}

/******************************************************************************
 * make_filespec_from_env()
 *------------------------------------------------------------------------*//**
 *
 *    If the result is non-zero, the caller will have to reallocate, but
 *    this function does not provide the buffer-size that is needed.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
make_filespec_from_env
(
   char * dest,
   size_t destsize,
   const char * envvariable,
   const char * optionalfile
)
{
   size_t count = xpc_get_environment(dest, destsize, envvariable);
   cbool_t result = count == 0;
   if (result)
   {
      if (not_nullptr(optionalfile))
      {
         int slash = xpc_path_slash(optionalfile);
         char temp[2];
         temp[0] = (char) slash;
         temp[1] = 0;
         result = xpc_string_cat(dest, destsize, temp);
         if (result)
            result = xpc_string_cat(dest, destsize, optionalfile);

         if (result)
            xpc_infoprint_func(dest);
      }
   }
   return result;
}



/******************************************************************************
 * test_settings.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
