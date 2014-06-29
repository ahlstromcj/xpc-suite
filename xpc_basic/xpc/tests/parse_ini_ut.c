/******************************************************************************
 * parse_ini_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          parse_ini_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \dates         2010-07-17
 * \update        2012-06-24
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    parse_ini.c/h module.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the INI parsing functions.  Also
 *          referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the parse_ini.c functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/cpu.h>                   /* macros for CPU support              */
#include <xpc/environment.h>           /* xpc_get_environment(), etc.         */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/file_functions.h>        /* xpc_file_exists() etc.              */
#include <xpc/os.h>                    /* macros for OS support               */
#include <xpc/parse_ini.h>             /* xpc_create_argv_from_INI() etc.     */
#include <xpc/test_settings.h>         /* make_filespec_from_env()            */
#include <xpc/unit_test.h>             /* unit_test_t structure               */
#include <xpc/xstrings.h>              /* xpc_string_copy(), etc.             */

/******************************************************************************
 * show_arguments()
 *------------------------------------------------------------------------*//**
 *
 *    A helper function to dump the command-line arguments to the screen.
 *
 * \param section
 *    The section name from which the arguments came.
 *
 * \param argc
 *    The number of arguments on the command-line.
 *
 * \param argv
 *    The array of command-line arguments.
 *
 *    show_arguments (int argc, const char * argv [])
 *
 *//*-------------------------------------------------------------------------*/

static void
show_arguments (const char * section, int argc, char ** argv)
{
   int i;
   if (xpc_showinfo())
   {
      xpc_infoprintf("[ %s ]", section);
      for (i = 0; i < argc; i++)
      {
         xpc_infoprintf
         (
            "   %s %d: '%s'", _("Arg"), i, argv[i]
         );
      }
   }
}

/******************************************************************************
 * parse_ini_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test case of the first test group merely provides some
 *    basic smoke-tests of the parse_ini.c module.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_argv_from_INI()
 *    -  xpc_delete_argv()
 *
 *//*-------------------------------------------------------------------------*/

#define INI_FILE    "parse_ini_test.ini"

static unit_test_status_t
parse_ini_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   char ini_filename[512];
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("Parse INI"), _("Smoke Test")
   );
   if (ok)
   {
      char temp[64];
      ok = xpc_string_copy(temp, sizeof(temp), "xpc/tests/");
      if (ok)
         ok = xpc_string_cat(temp, sizeof(temp), INI_FILE);

      if (ok)
      {
         ok = make_filespec_from_env      /* srcdir defined by 'make test'    */
         (
            ini_filename, sizeof(ini_filename), "srcdir", temp
         );
         if (! ok)
         {
            ok = xpc_file_exists(INI_FILE);
            if (ok)
            {
               ok = xpc_string_copy(ini_filename, sizeof(ini_filename), INI_FILE);
            }
            else
            {
               xpc_errprintf
               (
      "The environment variable 'srcdir' is not defined.  Nor can we\n"
      "  find the test INI file ('%s') for parsing without it.\n"
      "  In the context of 'make check', that variable will be equal to\n"
      "  'top_srcdir', which is the main xpc_suite directory."
                  ,
                  INI_FILE
               );
            }
         }

      }
   }
   if (ok)
   {
      /*  1 */

      int argc;
      char * buffer;
      char ** argv = xpc_argv_from_INI
      (
         ini_filename, "Smoke Test A", &argc, &buffer
      );
      if (unit_test_status_next_subtest(&status, "Smoke Test A"))
      {
         ok = not_nullptr(argv);
         if (ok)
            ok = not_nullptr(buffer);

         if (ok)
            ok = argc == 17;

         if (ok)
         {
            ok = strcmp(argv[0], ini_filename) == 0;
            if (ok)
               ok = strcmp(argv[1], "--ALPHA") == 0 ;
            if (ok)
               ok = strcmp(argv[2], "alpha") == 0 ;
            if (ok)
               ok = strcmp(argv[3], "--BETA") == 0 ;
            if (ok)
               ok = strcmp(argv[4], "beta") == 0 ;
            if (ok)
               ok = strcmp(argv[5], "--GAMMA") == 0 ;
            if (ok)
               ok = strcmp(argv[6], "gamma") == 0 ;
            if (ok)
               ok = strcmp(argv[7], "--DELTA") == 0 ;
            if (ok)
               ok = strcmp(argv[8], "delta") == 0 ;
            if (ok)
               ok = strcmp(argv[9], "--EPSILON") == 0 ;
            if (ok)
               ok = strcmp(argv[10], "--ZETA") == 0 ;
            if (ok)
               ok = strcmp(argv[11], "zeta") == 0 ;
            if (ok)
               ok = strcmp(argv[12], "--ETA") == 0 ;
            if (ok)
               ok = strcmp(argv[13], "--THETA") == 0 ;
            if (ok)
               ok = strcmp(argv[14], "true") == 0 ;
            if (ok)
               ok = strcmp(argv[15], "--IOTA") == 0 ;
            if (ok)
               ok = strcmp(argv[16], "false") == 0 ;
         }
         show_arguments("Smoke Test A", argc, argv);
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Smoke Test A Saved"))
      {
         ok = xpc_argv_to_INI
         (
            "parse_ini_test.sav", "Smoke Test B", argc, argv
         );
         unit_test_status_pass(&status, ok);
      }

      xpc_delete_argv(argv, buffer);   // delete sub-test 1 and 2 leftovers

      /* 3 */

      /*
       * Note that we wrote the section name as "Smoke Test B" above, so we
       * have to read that same section here, otherwise, only argv[0] is
       * retrieved.
       */

      argv = xpc_argv_from_INI
      (
         "parse_ini_test.sav", "Smoke Test B", &argc, &buffer
      );
      if (unit_test_status_next_subtest(&status, "Smoke Test B"))
      {
         ok = not_nullptr(argv);
         if (ok)
            ok = not_nullptr(buffer);

         if (ok)
            ok = argc == 17;

         if (ok)
         {
            ok = strcmp(argv[0], "parse_ini_test.sav") == 0;
            if (ok)
               ok = strcmp(argv[1], "--ALPHA") == 0 ;
            if (ok)
               ok = strcmp(argv[2], "alpha") == 0 ;
            if (ok)
               ok = strcmp(argv[3], "--BETA") == 0 ;
            if (ok)
               ok = strcmp(argv[4], "beta") == 0 ;
            if (ok)
               ok = strcmp(argv[5], "--GAMMA") == 0 ;
            if (ok)
               ok = strcmp(argv[6], "gamma") == 0 ;
            if (ok)
               ok = strcmp(argv[7], "--DELTA") == 0 ;
            if (ok)
               ok = strcmp(argv[8], "delta") == 0 ;
            if (ok)
               ok = strcmp(argv[9], "--EPSILON") == 0 ;
            if (ok)
               ok = strcmp(argv[10], "--ZETA") == 0 ;
            if (ok)
               ok = strcmp(argv[11], "zeta") == 0 ;
            if (ok)
               ok = strcmp(argv[12], "--ETA") == 0 ;
            if (ok)
               ok = strcmp(argv[13], "--THETA") == 0 ;
            if (ok)
               ok = strcmp(argv[14], "true") == 0 ;
            if (ok)
               ok = strcmp(argv[15], "--IOTA") == 0 ;
            if (ok)
               ok = strcmp(argv[16], "false") == 0 ;
         }
         show_arguments("Smoke Test B", argc, argv);
         unit_test_status_pass(&status, ok);
      }

      /* 4 */

      if (unit_test_status_next_subtest(&status, "xpc_parse_boolean()"))
      {
         cbool_t boolresult = false;         // important to set it
         int argi = 1;

         /*
          * There is no "--NOT-ALPHA" argument in the list, and certainly
          * not at the argi = 1 index, and so the result is not usable.
          */

         cbool_t usable = xpc_parse_boolean
         (
            argc, argv, argi, "--NOT-ALPHA", &boolresult
         );
         ok = ! usable;
         if (ok)
         {
            /*
             * Parse the "--ALPHA alpha" pair.  The result should be true,
             * paradoxically, because we're handling a non-boolean value,
             * but xpc_parse_boolean() thinks the value is the first
             * non-option argument, and returns the default value of a
             * boolean option -- 'true'!
             */

            usable = xpc_parse_boolean
            (
               argc, argv, argi, "--ALPHA", &boolresult
            );
            ok = usable;
            if (ok)
               ok = boolresult;
         }
         if (ok)
         {
            /*
             * Parse the "--ETA --THETA" pair.  The --ETA option obviously
             * has no value, so we can treat it as a boolean, and return the
             * default value of a boolean option -- 'true'.
             */

            argi = 12;
            usable = xpc_parse_boolean
            (
               argc, argv, argi, "--ETA", &boolresult
            );
            ok = usable;
            if (ok)
               ok = boolresult;
         }
         if (ok)
         {
            /*
             * Parse the "--THETA true" pair.
             */

            argi = 15;
            usable = xpc_parse_boolean
            (
               argc, argv, argi, "--IOTA", &boolresult
            );
            ok = usable;
            if (ok)
               ok = ! boolresult;
         }
         if (ok)
         {
            /*
             * Parse the "--IOTA false" pair.
             */

            argi = 13;
            usable = xpc_parse_boolean
            (
               argc, argv, argi, "--THETA", &boolresult
            );
            ok = usable;
            if (ok)
               ok = boolresult;
         }
         unit_test_status_pass(&status, ok);
      }

      /*
       * Now verify that sections can be found.
       */

      /*  5 */

      argv = xpc_argv_from_INI
      (
         ini_filename, "Smoke Test C", &argc, &buffer
      );
      if (unit_test_status_next_subtest(&status, "Smoke Test C"))
      {
         ok = not_nullptr(argv);
         if (ok)
            ok = not_nullptr(buffer);

         if (ok)
            ok = argc == 13;

         if (ok)
         {
            ok = strcmp(argv[0], ini_filename) == 0;
            if (ok)
               ok = strcmp(argv[1], "--FILE") == 0 ;
            if (ok)
               ok = strcmp(argv[2], INI_FILE) == 0 ;
            if (ok)
               ok = strcmp(argv[3], "--LIBRARY") == 0 ;
            if (ok)
               ok = strcmp(argv[4], "xpc") == 0 ;
            if (ok)
               ok = strcmp(argv[5], "--AUTHOR") == 0 ;
            if (ok)
               ok = strcmp(argv[6], "Chris Ahlstrom") == 0 ;
            if (ok)
               ok = strcmp(argv[7], "--UPDATES") == 0 ;
            if (ok)
               ok = strcmp(argv[8], "2010-07-17 to 2010-07-18") == 0 ;
            if (ok)
               ok = strcmp(argv[9], "--VERSION") == 0 ;
            if (ok)
               ok = strcmp(argv[10], "$Revision$") == 0 ;
            if (ok)
               ok = strcmp(argv[11], "--LICENSE") == 0 ;
            if (ok)
               ok = strcmp(argv[12], "$XPC_SUITE_GPL_LICENSE$") == 0 ;
         }
         show_arguments("Smoke Test C", argc, argv);
         unit_test_status_pass(&status, ok);
      }
      xpc_delete_argv(argv, buffer);   // delete sub-test 3 and 4 leftovers
   }
   else
      unit_test_status_pass(&status, false);

   return status;
}

/******************************************************************************
 * parse_ini_02_01()
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
parse_ini_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("xxxx"), _("xx")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xxxx()"))
      {
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

#define XPC_APP_NAME          "XPC library INI Parsing test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The executable name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "parse_ini_ut"

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
 *    This is the main routine for the parse_ini_ut application.
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
      "Parse-INI Test 0.1",                           /* nullptr for testin   */
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
            ok = unit_test_load(&testbattery, parse_ini_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, parse_ini_02_01);

               // ok = unit_test_load(&testbattery, parse_ini_02_yy);
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
 * parse_ini_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
