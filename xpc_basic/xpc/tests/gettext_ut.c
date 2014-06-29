/******************************************************************************
 * gettext_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          gettext_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       06/25/2008-08/23/2009
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    gettext_support.c/h module.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the gettext-related functions.  Also
 *          referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the gettext_support.c functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/cpu.h>                   /* macros for CPU support              */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/os.h>                    /* macros for OS support               */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

/******************************************************************************
 * gettext_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test case of the first test group merely provides some
 *    basic smoke-tests of the cpu.c module.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  init_gettext_support()
 *    -  gettext_false()
 *    -  gettext_true()
 *    -  gettext_boolean()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
gettext_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("gettext"), _("CPU Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xxxx()"))
      {
         is_biggie = xxxx();
         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  Big-endian:          %s\n",
               gettext_boolean(is_biggie)
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * gettext_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    Currently just a stub of a unit test.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  gettext()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
gettext_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("gettext"), _("xx")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_os_name()"))
      {
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * App description
 *------------------------------------------------------------------------*//**
 *
 *    Provides the application description.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME          "XPC library gettext test"

/******************************************************************************
 * Executable name
 *------------------------------------------------------------------------*//**
 *
 *    Provides the application executable name.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "gettext_ut"

/******************************************************************************
 * App version
 *------------------------------------------------------------------------*//**
 *
 *    Provides the application version.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_VERSION       0.1

/******************************************************************************
 * App base directory
 *------------------------------------------------------------------------*//**
 *
 *    Provides the application base directory.
 *
 *//*-------------------------------------------------------------------------*/

#define DEFAULT_BASE          "../test"

/******************************************************************************
 * App author
 *------------------------------------------------------------------------*//**
 *
 *    Provides the application author.
 *
 *//*-------------------------------------------------------------------------*/

#define DEFAULT_AUTHOR        "Chris Ahlstrom"

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the gettext_ut application.
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
      "gettext Test 0.1",                             /* nullptr for testin   */
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
            ok = unit_test_load(&testbattery, gettext_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, gettext_test_02_01);

               // ok = unit_test_load(&testbattery, gettext_test_02_yy);
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
 * gettext_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
