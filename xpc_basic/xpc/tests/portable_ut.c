/******************************************************************************
 * portable_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          portable_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2008-06-25 to 2011-07-17
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    portable_support.c/h module.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the portable-related functions.  Also
 *          referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the portable_support.c functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/portable.h>              /* macros for portable support         */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

/******************************************************************************
 * portable_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test case of the first test group merely provides some
 *    basic smoke-tests of the portable.c module.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  init_portable_support()
 *    -  portable_false()
 *    -  portable_true()
 *    -  portable_boolean()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
portable_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("portable"), _("Portable Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_boolean_string()"))
      {
         const char * truth = xpc_boolean_string(true);
         if (! xpccut_is_silent())
         {
            fprintf(stdout, "  Truth string:        %s\n", truth);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * portable_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    A stub unit-test.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  portable()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
portable_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 1, _("portable"), "xpc_boolean_string()"
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_boolean_string()"))
      {
         const char * truth = xpc_boolean_string(true);
         if (! xpccut_is_silent())
         {
            fprintf(stdout, "  Truth string:        %s\n", truth);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME          "XPC library portable test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The executable name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "portable_ut"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The version of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_VERSION       0.1

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The base-directory of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define DEFAULT_BASE          "../test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The author of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define DEFAULT_AUTHOR        "Chris Ahlstrom"

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the portable_ut application.
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
   unit_test_t testbattery;                           /* uses default values  */
   cbool_t ok = xpc_parse_errlevel(argc, argv);       /* cool feature         */
   ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPC_TEST_NAME,                                  /* nullptr for testin   */
      "Portable Test 0.1",                            /* nullptr for testin   */
      nullptr                                         /* no added help        */
   );
   if (ok)
   {
      /*
       * int argcount = argc;
       * char ** arglist = argv;
       */

      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      strcpy(gsBasename, DEFAULT_BASE);
      strcpy(gsAuthor, DEFAULT_AUTHOR);

      /*
       * if (argcount > 1)
       * {
       *    int currentarg = 1;
       *    while (currentarg < argcount)
       *    {
       *       const char * arg = arglist[currentarg];
       *       if (strcmp(arg, "--leak-check") == 0)
       *       {
       *          g_do_leak_check = true;
       *          xpccut_infoprint(_("memory leakage enabled"));
       *       }
       *       currentarg++;
       *    }
       * }
       */

      if (ok)                           /* \note fails if --help specified */
      {
         if (load_the_tests)
         {
            ok = unit_test_load(&testbattery, portable_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, portable_test_02_01);

               // ok = unit_test_load(&testbattery, portable_test_02_yy);
            }
         }
         if (ok)
            ok = unit_test_run(&testbattery);
         else
            xpccut_errprint(_("test function load failed"));
      }
   }
   unit_test_destroy(&testbattery);
   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/******************************************************************************
 * portable_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
