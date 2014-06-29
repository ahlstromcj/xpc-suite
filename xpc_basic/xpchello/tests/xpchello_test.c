/******************************************************************************
 * xpchello_test.c
 *------------------------------------------------------------------------*//**
 *
 * \file          xpchello_test.c
 * \library       libxpccut (installed as a normal library)
 * \author        Chris Ahlstrom
 * \date          2009-04-05
 * \updates       2012-01-01
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the xpchello
 *    application.  It provides a demonstration of how to link to the XPCCUT
 *    library from within your own application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/unit_test.h>             /* unit_test_t structure               */

/******************************************************************************
 * xpchello_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test demonstrates the basics of the unit-test functions.
 *
 *    This functions is of the type unit_test_func_t; see that typedef for
 *    more information.
 *
 *    This first test is like an overview.  The detailed tests start in
 *    group 2.
 *
 * \test
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  unit_test_status_initialize()
 *    -  unit_test_status_pass()
 *    -  unit_test_status_fail()
 *    -  unit_test_status_is_verbose() [by user visualization only]
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
xpchello_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, "XPC Hello", _("Smoke Test")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "unit_test_status_pass()"))
         {
            unit_test_status_pass(&status, true);
            ok =
            (
               (status.m_Subtest_Error_Count == 0) &&
               (status.m_Failed_Subtest == 0)
            );
            if (! ok)
            {
               (void) unit_test_status_fail(&status);    /* create a failure  */
               if (unit_test_options_is_verbose(options))
               {
                  fprintf
                  (
                     stderr, "%s %s\n",
                     "unit_test_status_pass()", _("internal failure")
                  );
               }
            }
            unit_test_status_pass(&status, ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the xpchello_test application.
 *
 * \gnu
 *    The unit_test_initialize() function called here should set up the
 *    internationalization support for the application.
 *
 * \note
 *    Parse for any additional (non-unittest) arguments.  Don't try to find
 *    erroneous options in this loop.  If a valid option is found, then turn
 *    off the complaint flag to avoid error messages.  Note that a better
 *    way to do this work is to derive a class from unit-test and extend it
 *    to support the additional variables.  (In this case, it is likely that
 *    one would also want to extend the TestOptions class.
 *
 * \return
 *    Returns POSIX_SUCCESS (0) if the function succeeds.  Other values,
 *    including possible error-codes, are returned otherwise.
 *
 * \todo
 *    Some day, make this test application actually useful for something
 *    other than demonstration.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME             "XPC Hello Application"
#define XPC_TEST_NAME            "xpchello_test"
#define XPC_TEST_VERSION         1.0
#define DEFAULT_BASE             "../test"
#define DEFAULT_AUTHOR           "Chris Ahlstrom"

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   static const char * const additional_help =

"\n"
XPC_TEST_NAME "-specific options:\n"
"\n"
"None yet.\n"
/**
"--leak-check        Disable some of the 'destructor' calls.  This item can\n"
"                    be used when running valgrind (as an example).\n"
***/
"\n"
"This application is just a simple test-application to illustrate the very"
"basics of setting up a test-application that uses the XPC unit-test"
"library."
"\n"
"For an in-depth test application, see the unit-test application for the"
"unit-test library itself."
"\n"

   ;

   unit_test_t testbattery;                           /* uses default values */
   cbool_t ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPC_TEST_NAME,
      XPCCUT_VERSION_STRING(XPC_TEST_VERSION),
      additional_help
   );
   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      strcpy(gsBasename, DEFAULT_BASE);
      strcpy(gsAuthor, DEFAULT_AUTHOR);
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            const char * arg = arglist[currentarg];
            if (strcmp(arg, "--help") == 0)
            {
               // todo
            }
            currentarg++;
         }
      }
      if (ok)                           /* \note fails if --help specified */
      {
         if (load_the_tests)
         {
            /*
             * xpccut_infoprint(_("loading the tests"));
             */

            ok = unit_test_load(&testbattery, xpchello_test_01_01);
            if (ok)
            {
               /*
                * No reason to ever expect these loads to fail at run-time,
                * but let's use the return value as an artificial way to
                * lump all tests in one group together.
                *
                * ok = unit_test_load(&testbattery, xpchello_test_02_31);
                */
            }
         }
         if (ok)
         {
            ok = unit_test_run(&testbattery);
            if (! ok)
            {
               // nothing yet
            }
         }
         else
            xpccut_errprint(_("test-function load failed"));
      }
   }
   unit_test_destroy(&testbattery);
   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/******************************************************************************
 * xpchello_test.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
