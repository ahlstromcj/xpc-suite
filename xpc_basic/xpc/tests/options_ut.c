/******************************************************************************
 * options_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          options_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2011-07-17 to 2011-07-19
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    options_support.c/h module.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the options-related functions.  Also
 *          referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the options_support.c functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/options.h>               /* options functions                   */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

/******************************************************************************
 * options_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test case of the first test group merely provides some
 *    basic smoke-tests of the options.c module.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  init_options_support()
 *    -  options_false()
 *    -  options_true()
 *    -  options_boolean()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
options_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("options"), _("Options Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_parse_flag()"))
      {
         const char * const argv [2] =
         {
            "myapp", "--flag"
         };
         int argindex = 1;
         cbool_t ok = xpc_parse_flag(&argindex, argv, "--flag");
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "xpc_parse_string()"))
      {
         const int argc = 3;
         const char * const argv [3] =
         {
            "myapp", "--string", "perky"
         };
         int argindex = 1;
         const char * stringvalue;
         cbool_t ok = xpc_parse_string
         (
            &stringvalue, &argindex, argc, argv, "--string"
         );
         if (ok)
         {
            ok = strcmp(stringvalue, "perky") == 0;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * s_flag_loop()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a while-loop for processing command-line arguments.
 *
 *    Also provides a good example of writing such a loop.
 *
 *    The set of targets supported by this function is:
 *
 *       -  --alpha
 *       -  --beta
 *       -  --gamma
 *       -  --delta
 *
 * \param argc
 *    The total number of command-line arguments, used as a check to see if
 *    the value argument was provided.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 * \return
 *    The number of matching command-line arguments is returned.
 *
 *//*-------------------------------------------------------------------------*/

static int
s_flag_loop
(
   int argc,
   const char * const argv []
)
{
   int result = 0;
   if (not_nullptr(argv))
   {
      if (argc > 1)
      {
         int argindex = 1;             /* start with second argument          */
         while (argindex < argc)
         {
            if (xpc_parse_flag(&argindex, argv, "--alpha"))
            {
               result++;
            }
            else if (xpc_parse_flag(&argindex, argv, "--beta"))
            {
               result++;
            }
            else if (xpc_parse_flag(&argindex, argv, "--gamma"))
            {
               result++;
            }
            else if (xpc_parse_flag(&argindex, argv, "--delta"))
            {
               result++;
            }
            else
            {
               /*
                * No match on the set of targets occurred for the current
                * argument.  Therefore, we must increment the counter to the
                * next argument ourselves, instead of relying on
                * xpc_parse_flag() to do it.
                */

               argindex++;
            }
         }
      }
      else if (argc == 1)
         xpc_warnprint_func(_("Only one argument, nothing to parse"));
      else if (argc <= 0)
         xpc_errprint_func(_("One or more arguments are required"));
   }
   return result;
}

/******************************************************************************
 * options_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    A stub unit-test.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  options()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
options_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("options"), _("xpc_parse_flag()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Basic 1"))
      {
         const char * const argv [2] =
         {
            "myapp", "--flag"
         };
         int argcount = 1;
         cbool_t ok = xpc_parse_flag(&argcount, argv, "--flag");
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Basic 2"))
      {
         const char * const argv [2] =
         {
            "myapp", "-flag"
         };
         int argcount = 1;
         cbool_t ok = xpc_parse_flag(&argcount, argv, "-flag");
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Bad argument"))
      {
         const char * const argv [2] =
         {
            "myapp", "flag"
         };
         int argcount = 1;
         cbool_t ok = ! xpc_parse_flag(&argcount, argv, "-flag");
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "Bad target"))
      {
         const char * const argv [2] =
         {
            "myapp", "-flag"
         };
         int argcount = 1;
         cbool_t ok = ! xpc_parse_flag(&argcount, argv, "flag");
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "Null arg-count pointer"))
      {
         const char * const argv [2] =
         {
            "myapp", "-flag"
         };
         cbool_t ok = ! xpc_parse_flag(nullptr, argv, "flag");
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "Null arg-list pointer"))
      {
         int argcount = 1;
         cbool_t ok = ! xpc_parse_flag(&argcount, nullptr, "flag");
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "Null target pointer"))
      {
         const char * const argv [2] =
         {
            "myapp", "-flag"
         };
         int argcount = 1;
         cbool_t ok = ! xpc_parse_flag(&argcount, argv, nullptr);
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "Flag loop test 1"))
      {
         const int argc = 3;
         const char * const argv [3] =
         {
            "myapp", "--alpha", "--lima"     /* only --alpha is a match       */
         };
         cbool_t ok = s_flag_loop(argc, argv) == 1;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "Flag loop test 2"))
      {
         const int argc = 3;
         const char * const argv [3] =
         {
            "myapp", "--klone", "--lima"     /* no matches                    */
         };
         cbool_t ok = s_flag_loop(argc, argv) == 0;
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

#define XPC_APP_NAME          "XPC library options test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The executable name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "options_ut"

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
 *    This is the main routine for the options_ut application.
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
      "options Test 0.1",                             /* nullptr for testin   */
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
            ok = unit_test_load(&testbattery, options_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, options_test_02_01);

               // ok = unit_test_load(&testbattery, options_test_02_yy);
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
 * options_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
