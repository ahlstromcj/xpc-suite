/******************************************************************************
 * cpu_os_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          cpu_os_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2008-06-22 to 2013-07-21
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    cpu.c and os.c modules.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the cpu.c and os.c functions.  Also
 *          referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the cpu_os.c functions.
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
 * cpu_os_test_01_01()
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
 *    -  xpc_is_big_endian()
 *    -  xpc_is_little_endian()
 *    -  xpc_cpu_model()
 *    -  xpc_is_16_bit()
 *    -  xpc_is_32_bit()
 *    -  xpc_is_lp32()
 *    -  xpc_is_ilp32()
 *    -  xpc_is_64_bit()
 *    -  xpc_is_lp64()
 *    -  xpc_is_ilp64()
 *    -  xpc_is_llp64()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
cpu_os_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("cpu_os"), _("CPU Smoke Test")
   );
   if (ok)
   {
      cbool_t is_biggie = false;
      cbool_t is_little = false;

      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_is_big_endian()"))
      {
         is_biggie = xpc_is_big_endian();

         /**
          * \todo
          *    Cross-check against other CPU and OS checks to make sure the
          *    results are consistent.
          */

#ifdef WIN32
#endif

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

      /*  2 */

      if (unit_test_status_next_subtest(&status, "xpc_is_little_endian()"))
      {
         is_little = xpc_is_little_endian();

         /**
          * \todo
          *    Cross-check against other CPU and OS checks to make sure the
          *    results are consistent.
          */

#ifdef WIN32
#endif

         ok = is_biggie == ! is_little;
         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  Little-endian:       %s\n",
               gettext_boolean(is_little)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "xpc_cpu_model(TBD)"))
      {
         const char * model = xpc_cpu_model();
         ok = not_nullptr(model);
         if (! xpccut_is_silent())
            fprintf(stdout, "  CPU model:           %s\n", model);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "xpc_is_16_bit(TBD)"))
      {
         cbool_t is_sixteen_bit = xpc_is_16_bit();
         ok = ! is_sixteen_bit;
         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  16-bit support:      %s\n",
               gettext_boolean(is_sixteen_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "xpc_is_32_bit()"))
      {
         cbool_t is_thirty_two_bit = xpc_is_32_bit();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_thirty_two_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  32-bit support:      %s\n",
               gettext_boolean(is_thirty_two_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "xpc_is_lp32()"))
      {
         cbool_t is_thirty_two_bit = xpc_is_lp32();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_thirty_two_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  LP32 data model:     %s\n",
               gettext_boolean(is_thirty_two_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "xpc_is_ilp32()"))
      {
         cbool_t is_thirty_two_bit = xpc_is_ilp32();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_thirty_two_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  ILP32 data model:    %s\n",
               gettext_boolean(is_thirty_two_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "xpc_is_64_bit()"))
      {
         cbool_t is_sixty_four_bit = xpc_is_64_bit();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_sixty_four_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  64-bit support:      %s\n",
               gettext_boolean(is_sixty_four_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "xpc_is_lp64()"))
      {
         cbool_t is_sixty_four_bit = xpc_is_lp64();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_sixty_four_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  LP64 data model:     %s\n",
               gettext_boolean(is_sixty_four_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "xpc_is_ilp64()"))
      {
         cbool_t is_sixty_four_bit = xpc_is_ilp64();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_sixty_four_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  ILP64 data model:    %s\n",
               gettext_boolean(is_sixty_four_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "xpc_is_llp64()"))
      {
         cbool_t is_sixty_four_bit = xpc_is_llp64();

         /*
          * Need an alternate test of some kind as corroboration.
          *
          *    ok = ! is_sixty_four_bit;
          */

         if (! xpccut_is_silent())
         {
            fprintf
            (
               stdout,
               "  LLP64 data model:    %s\n",
               gettext_boolean(is_sixty_four_bit)
            );
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * cpu_os_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test case of the second test group merely provides some
 *    basic smoke-tests of the os.c module.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_os_name()
 *    -  xpc_host_name()
 *    -  xpc_the_sizeof_it()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
cpu_os_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("cpu_os"), _("OS Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_os_name()"))
      {
         const char * os_name = xpc_os_name();
         ok = not_nullptr(os_name);
         if (! xpccut_is_silent())
            fprintf(stdout, "  OS name:             %s\n", os_name);

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "xpc_host_name()"))
      {
         const char * host_name = xpc_host_name();
         ok = not_nullptr(host_name);
         if (! xpccut_is_silent())
            fprintf(stdout, "  Host name:           %s\n", host_name);

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "xpc_the_sizeof_it()"))
      {
         ok = true;
         if (! xpccut_is_silent())
            xpc_the_sizeof_it();

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the cpu_os_ut application.
 *
 * \return
 *    Returns POSIX_SUCCESS (0) if the function succeeds.  Other values,
 *    including possible error-codes, are returned otherwise.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME          "XPC library cpu/os test"
#define XPC_TEST_NAME         "cpu_os_ut"
#define XPC_TEST_VERSION       0.1
#define DEFAULT_BASE          "../test"
#define DEFAULT_AUTHOR        "Chris Ahlstrom"

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
      "CPU/OS Test 0.1",                              /* nullptr for testin   */
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

      /**
       *    There are no application-specific options or help in this
       *    module.  Thus, it can provide a good test of the handling of the
       *    lack of such extras.  This is a good thing, since we found some
       *    problems in the XPCCUT library, even after all the pounding it
       *    has taken so far!
       */

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
            ok = unit_test_load(&testbattery, cpu_os_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, cpu_os_test_02_01);

               // ok = unit_test_load(&testbattery, cpu_os_test_02_yy);
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
 * cpu_os_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
