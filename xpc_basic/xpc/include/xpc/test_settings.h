#ifndef XPC_TEST_SETTINGS_H
#define XPC_TEST_SETTINGS_H

/******************************************************************************
 * test_settings.h
 *------------------------------------------------------------------------*//**
 *
 * \file          test_settings.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \dates         2012-06-02
 * \updates       2012-06-23
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides some directory-management tools useful in testing situations
 *    where the executable is located in a different directory relative to
 *    its test files.  This situation can occur when running code outside of
 *    Visual Studio, or when running code in a GNU automake out-of-source
 *    build.
 *
 *    An "out-of-source" build is one where the make-files have been
 *    recreated in a directory outside of the project, such as in a "debug"
 *    or "release" directory.
 *
 *----------------------------------------------------------------------------*/

#include <xpc/portable.h>

/******************************************************************************
 * External functions:
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern const char * get_current_directory ();
extern const char * get_relative_directory
(
   const char * portion,
   const char * directory
);
extern void free_current_directory (const char * d);
extern cbool_t set_current_directory (const char * path);
extern cbool_t set_test_directory ();
extern cbool_t set_test_directory_ex (const char * testdir);
extern const char * find_in_source_file
(
   const char * filename,
   const char * base
);
extern cbool_t make_filespec_from_env
(
   char * dest,
   size_t destsize,
   const char * envvariable,
   const char * optionalfile
);

EXTERN_C_END


#endif      // XPC_TEST_SETTINGS_H

/******************************************************************************
 * test_settings.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
