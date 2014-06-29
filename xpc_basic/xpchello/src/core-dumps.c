/******************************************************************************
 * core-dumps.c
 *------------------------------------------------------------------------*//**
 *
 * \file          core-dumps.c
 * \library       xpc_suite "Core Dumps" application
 * \author        Chris Ahlstrom
 * \date          2011-02-09
 * \update        2013-07-20
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides examples of pointer usage, and what you can
 *    get away with in C code.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/xpc-config.h>
#include <xpc/tiny_macros.h>

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* declares printf(), stdout, etc.     */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* declares strcmp(), etc.             */
#endif

/******************************************************************************
 * helptext
 *------------------------------------------------------------------------*//**
 *
 *    Provides a discussion of this application.
 *
 *//*-------------------------------------------------------------------------*/

const char * const helptext =
"\n"
"core-dump version 1.1.0\n"
"\n"
"Usage:  core-dumps\n"
"\n"
"'core-dumps' serves simply as a demonstration of core dumps.  To allow a\n"
"core dump to be made, issue the command 'ulimit -c 1024' in the current\n"
"shell first.\n"
"\n"
"Inspired by\n"
"\n"
"   http://www.ffnn.nl/pages/articles/linux/gdb-gnu-debugger-intro.php\n"
"\n"
"Feel free to mail me at <mailto:ahlstromcj@gmail.com>.\n"
"\n"
;

/******************************************************************************
 * array()
 *------------------------------------------------------------------------*//**
 *
 *    This function shows ...
 *
 * \param use_it
 *
 *    If true, actually causes the core dump.
 *
 * \return
 *    Returns 'true' (non-zero) if the function succeeds.
 *
 *//*-------------------------------------------------------------------------*/

int
array (int use_it)
{
   char buffer[16];
   if (use_it)
   {
      buffer[80000] = 'a';
      printf("%32s\n", buffer);        // prevent warning about non-usage
   }
   return ! use_it;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the core-dump application.
 *
 * \return
 *    Returns POSIX_SUCCESS (0) if the function succeeds.  Other values,
 *    including possible error-codes, are returned otherwise.
 *
 *//*-------------------------------------------------------------------------*/

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   if (argc > 1)
   {
      if (strcmp(argv[1], "--help") == 0)
         printf("%s", helptext);
   }
   else
   {
      int ok = array(true);
      if (ok)
      {
         return 0;
      }
      else
         return 1;
   }
}

/******************************************************************************
 * core-dumps.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
