/******************************************************************************
 * errorlogging_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlogging_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2008-05-06
 * \updates       2012-07-23
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    errorlogging.c module.  It provides a demonstration of how to use
 *    the errorlogging utility.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the error-logging.c
 *                   functions.  Also referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the errorlogging.c functions.
 *       -  Group 3. Thread unit-tests of the errorlogging.c functions,
 *                   using the "synch" functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/pthread_attributes.h>    /* pthread attributes functions        */
#include <xpc/pthreader.h>             /* pthreader functions                 */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

#ifdef XPC_HAVE_SYS_STAT_H
#include <sys/stat.h>                  /* stat() or _stat()                   */
#endif

/******************************************************************************
 * g_do_leak_check
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to disable some "destructor" calls, in order to test
 *    leakage mechanisms.
 *
 *    This variable is normally false.  It can be set to true using the
 *    --leak-check option (which should not be confused with valgrind's
 *    option of the same name).
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t g_do_leak_check = false;

/******************************************************************************
 * errorlogging_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test group provides some basic smoke-tests of the
 *    errorlogging.c module.
 *
 *    If the --silent option was used (but /not/ the --quiet option), then
 *    the smoke tests are not performed, and this test passes anyway.  Some
 *    of the functions are more thoroughly tested, nonetheless, in later
 *    tests in this unit-test application.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_errlevel()
 *    -  xpc_errlevel_set()
 *    -  xpc_errprint()
 *    -  xpc_warnprint()
 *    -  xpc_infoprint()
 *    -  xpc_infoprintf()
 *    -  xpc_print()
 *    -  xpc_dbginfoprint()
 *    -  xpc_print_debug()
 *    -  xpc_errprintex()
 *    -  xpc_warnprintex()
 *    -  xpc_infoprintex()
 *    -  xpc_dbginfoprintex()
 *    -  xpc_print()
 *    -  xpc_dbginfoprint()
 *    -  xpc_lkprintf().  Not yet tested.
 *
 *    Since this is a smoke test, it is not useful for regression testing.
 *    But the "print" functions here do not make any settings.
 *
 *    They also do not output anything that can be checked easily.  The only
 *    way to test them directly would be to output them to a log file.
 *
 *    A test like this is performed in errorlogging_test_02_03().
 *
 *    However, we're not sure we want to go to all of this trouble to see if
 *    output is properly filtered.
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("errorlogging"), _("Smoke Test")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Overall appearance"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_ALL);
            f = xpc_errlevel();
            xpc_errprint(_("This is an error-level message"));
            xpc_warnprint(_("This is a warning-level message"));
            xpc_infoprint(_("This is an information-level message"));
            xpc_print(_("This is an unconditional message"));
            xpc_dbginfoprint(_("This is a debug-level message"));
            xpc_print_debug(_("This is another debug-level message"));
            ok = f == XPC_ERROR_LEVEL_ALL;
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Overall appearance ex"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_ALL);
            f = xpc_errlevel();
            xpc_errprintex(_("error-level message"), _("TESTING"));
            xpc_warnprintex(_("warning-level message"), _("TESTING"));
            xpc_infoprintex(_("information-level message"), _("TESTING"));
            xpc_dbginfoprintex(_("debug-level message"), _("TESTING"));
            ok = f == XPC_ERROR_LEVEL_ALL;
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "xpc_errprintf()"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
            f = xpc_errlevel();
            xpc_errprint(_("This is an error-level message."));
            xpc_errprintf
            (
               "This is a plain-format multiline message showing some values.\n"
               "Note the lack of the question-markers in this plain format.\n"
               "\n"
               "Values:\n"
               "\n"
               "   First:     %d\n"
               "  Second:     %d\n"
               "   Third:     %d\n"
               "\n"
               ,
               1, 2, 3
            );
            ok = f == XPC_ERROR_LEVEL_INFO;
            unit_test_status_pass(&status, ok);
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "xpc_warnprintf()"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
            f = xpc_errlevel();
            xpc_warnprint(_("This is an warning-level message."));
            xpc_warnprintf
            (
               "This is a plain-format multiline message showing some values.\n"
               "Note the lack of the exclamation-markers in this plain format.\n"
               "\n"
               "Values:\n"
               "\n"
               "   First:     %d\n"
               "  Second:     %d\n"
               "   Third:     %d\n"
               "\n"
               ,
               1, 2, 3
            );
            ok = f == XPC_ERROR_LEVEL_INFO;
            unit_test_status_pass(&status, ok);
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "xpc_infoprintf()"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
            f = xpc_errlevel();
            xpc_infoprint(_("This is an information-level message."));
            xpc_infoprintf
            (
               "This is a plain-format multiline message showing some values.\n"
               "Note the lack of the asterisk markers in this plain format.\n"
               "\n"
               "Values:\n"
               "\n"
               "   First:     %d\n"
               "  Second:     %d\n"
               "   Third:     %d\n"
               "\n"
               ,
               1, 2, 3
            );
            ok = f == XPC_ERROR_LEVEL_INFO;
            unit_test_status_pass(&status, ok);
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "xpc_infoprintml()"))
      {
         if (! xpccut_is_silent())
         {
            xpc_errlevel_t e = xpc_errlevel();     /* get run-time level      */
            xpc_errlevel_t f;                      /* for second level        */
            xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
            f = xpc_errlevel();
            xpc_infoprint(_("This is an information-level message"));
            xpc_infoprintml
            (
               "This is an info-format multiline message showing some values.\n"
               "Note the presence of the asterisk markers in this format.\n"
               "\n"
               "Values:\n"
               "\n"
               "   First:     %d\n"
               "  Second:     %d\n"
               "   Third:     %d\n"
               "\n"
               ,
               1, 2, 3
            );
            ok = f == XPC_ERROR_LEVEL_INFO;
            unit_test_status_pass(&status, ok);
            xpc_errlevel_set(e);                   /* restore run-time level  */
         }
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    xpc_synchusage_set() function.
 *
 *    There's not much that can cause an error in this function.  The only
 *    possibility of error is if the atexit() call fails.
 *
 *    If you run this test with the --info option, you should see a note
 *    about atexit() being called.
 *
 *    If you run this test under valgrind, there should be no memory leaks.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_synchusage_set()
 *    -  xpc_synchusage()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 1, _("errorlogging"), _("xpc_synchusage_set()")
   );
   if (ok)
   {
      cbool_t original_synch_usage = xpc_synchusage();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "set to false"))
      {
         ok = xpc_synchusage_set(false);
         if (ok)
            ok = ! xpc_synchusage();

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "set to true"))
      {
         ok = xpc_synchusage_set(true);
         if (ok)
            ok = xpc_synchusage();

         unit_test_status_pass(&status, ok);
      }

      (void) xpc_synchusage_set(original_synch_usage);
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_02()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    xpc_usecolor_set() function.
 *
 *    There's not much that can cause an error in this function.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_usecolor_set()
 *    -  xpc_usecolor()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 2, _("errorlogging"), _("xpc_usecolor_set()")
   );
   if (ok)
   {
      cbool_t original_use_color = xpc_usecolor();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "set to false"))
      {
         ok = xpc_usecolor_set(false);
         if (ok)
            ok = ! xpc_usecolor();

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "set to true"))
      {
         ok = xpc_usecolor_set(true);

#ifdef WIN32

         /*
          * You're supposed to be able to get the DOS console to show colors
          * by loading an ANSI.SYS file, but I've had no luck loading it.
          */

         if (! ok)
            ok = ! xpc_usecolor();
#else
         if (ok)
            ok = xpc_usecolor();
#endif

         unit_test_status_pass(&status, ok);
      }
      (void) xpc_usecolor_set(original_use_color);
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_03()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC log-file functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_logfile()
 *    -  xpc_close_logfile()
 *    -  xpc_open_logfile()
 *    -  xpc_append_logfile()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_03 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 3, _("errorlogging"), _("Log file")
   );
   if (ok)
   {
      FILE * original_logfile = xpc_logfile();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "stdin"))
      {
         /*
          * Trying to set the log-file to 'stdin' is illegal.
          */

         cbool_t not_ok = ! xpc_open_logfile("stdin");
         if (not_ok)
            not_ok = ! xpc_open_logfile("STDIN");

         if (not_ok)
            not_ok = xpc_logfile() == original_logfile;

         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "stdout"))
      {
         /*
          * Trying to set the log-file to 'stdout' is legal.
          */

         cbool_t okay = xpc_open_logfile("stdout");
         if (okay)
            okay = xpc_logfile() == stdout;

         okay = xpc_open_logfile("STDOUT");
         if (okay)
            okay = xpc_logfile() == stdout;

         unit_test_status_pass(&status, okay);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "stderr"))
      {
         /*
          * Trying to set the log-file to 'stdout' is legal.
          */

         cbool_t okay = xpc_open_logfile("stderr");
         if (okay)
            okay = xpc_logfile() == stderr;

         okay = xpc_open_logfile("STDERR");
         if (okay)
            okay = xpc_logfile() == stderr;

         unit_test_status_pass(&status, okay);
      }

      /*  4 */

      /**
       * \todo
       *    Need a better way to distinguish between certain Win32 and POSIX
       *    constructs for Standard C functions and for file differences
       *    such as DOS-style line endings.
       */

#define LOG_FILENAME    "mylog.txt"
#ifdef POSIX
#define STAT_T          struct stat
#define STATFUNC(x, y)  stat((x), (y))
#else
#define STAT_T          struct _stat
#define STATFUNC(x, y)  _stat((x), (y))
#endif

      if (unit_test_status_next_subtest(&status, LOG_FILENAME))
      {
         /**
          * First, we need to remove any version of the log-file left behind
          * from previous runs.  We don't care if the deletion succeeds,
          * since it may fail if the file doesn't exist.  We just plow
          * ahead.
          */

         cbool_t okay;
         (void) unlink(LOG_FILENAME);
         okay = xpc_open_logfile(LOG_FILENAME);
         if (okay)
         {
            /**
             * The log-file subtest saves the current error level, and then
             * sets it so that output can occur even if the caller specified
             * the --quiet (error-logging) or --silent (unit-test)
             * command-line options.  Then, the output is directed to a log
             * file, and the resulting log file is checked for both size and
             * contents.
             *
             * \todo
             *    Handle the case where the caller has specified a log file
             *    on the command line; save the log-file name, close the
             *    caller's log file, and then re-open it for appending; all
             *    this probably constitutes a feature upgrade for the
             *    error-logging module!
             */

            xpc_errlevel_t el = xpc_errlevel();   /* get current value */
            okay = xpc_errlevel_set(XPC_ERROR_LEVEL_ERRORS);
            if (okay)
            {
               STAT_T statusret;
               int retcode;
               xpc_print("123456789");                /* write to log file */
               okay = xpc_logfile() != stderr;
               if (okay)
                  okay = xpc_close_logfile();
               else
                  xpc_errprint(_("log-file == stderr"));

               retcode = STATFUNC(LOG_FILENAME, &statusret);
               if (okay && (retcode == 0))
               {
                  size_t size = (size_t) statusret.st_size;
                  if (size > 0)
                  {
#ifdef POSIX
                     size_t expected = 12;
#else
                     size_t expected = 13;
#endif
                     okay = size == expected;
                     if (! okay)
                        xpc_errprint(_("log-file not the expected size"));
                  }
                  else
                     xpc_errprint(_("log-file not written"));
               }
               else
                  xpc_errprint(_("stat() function failed"));

            }
            if (okay)
               okay = xpc_errlevel_set(el);           /* restore setting   */
         }
         unit_test_status_pass(&status, okay);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "xpc_append_logfile()"))
      {
         cbool_t okay;
         okay = xpc_append_logfile(LOG_FILENAME);
         if (okay)
         {
            xpc_errlevel_t el = xpc_errlevel();       /* get current value */
            okay = xpc_errlevel_set(XPC_ERROR_LEVEL_ERRORS);
            if (okay)
            {
               STAT_T statusret;
               int retcode;
               xpc_print("123456789");                /* write to log file */
               okay = xpc_logfile() != stderr;
               if (okay)
                  okay = xpc_close_logfile();
               else
                  xpc_errprint(_("log-file == stderr"));

               retcode = STATFUNC(LOG_FILENAME, &statusret);
               if (okay && (retcode == 0))
               {
                  size_t size = (size_t) statusret.st_size;
                  if (size > 0)
                  {
#ifdef POSIX
                     size_t expected = 2*12;
#else
                     size_t expected = 2*13;
#endif
                     okay = size == expected;
                     if (! okay)
                        xpc_errprint(_("log-file not the expected size"));
                  }
                  else
                     xpc_errprint(_("log-file not written"));
               }
               else
                  xpc_errprint(_("stat() function failed"));

            }
            if (okay)
               okay = xpc_errlevel_set(el);           /* restore setting   */
         }
         unit_test_status_pass(&status, okay);
      }

#if 0
      /*  x */

      if (unit_test_status_next_subtest(&status, "xxx"))
      {
         /**
          *    There's a lot to check here.  We want the test to run
          *    properly no matter what command-line arguments, including
          *    --log, may have been used to change the log-file.

         cbool_t is_standard =
         (
            (original_logfile == stderr) ||
            (original_logfile == stdout) ||
            (original_logfile == stdin)
         );
         cbool_t closed = xpc_close_logfile();
         ok = xpc_logfile() == stderr;
         unit_test_status_pass(&status, ok);
          */
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "next"))
      {
         unit_test_status_pass(&status, ok);
      }
#endif

      // (void) xpc_usecolor_set(original_use_color);
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_04()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC output-marking functions.
 *
 * \warning
 *    These tests would involve exposing a number of internal macros (e.g.
 *    COLOR_STR_INFO) or duplicating the macros in the present unit test.
 *    At this time, we don't know if there is a good approach to these
 *    unit-tests, and so they are not yet implemented.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_dbgmark()
 *    -  xpc_infomark()
 *    -  xpc_warnmark()
 *    -  xpc_errmark()
 *    -  xpc_usermark()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_04 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 4, _("errorlogging"), _("xpc_xxxmark()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "fake test()"))
      {
         if (unit_test_options_show_progress(options))
         {
            fprintf
            (
               stdout, "! %s:\n%s",
               _("no unit-tests for the following functions"),
               "- xpc_dbgmark()\n"
               "- xpc_infomark()\n"
               "- xpc_warnmark()\n"
               "- xpc_errmark()\n"
               "- xpc_usermark()\n"
            );
         }
         unit_test_status_pass(&status, true);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_05()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC output-time-stamping functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_timestamps()
 *    -  xpc_timestamps_set()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_05 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 5, _("errorlogging"), _("xpc_timestamps()")
   );
   if (ok)
   {
      cbool_t original_use_timestamps = xpc_timestamps();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "set to false"))
      {
         cbool_t ts;
         xpc_timestamps_set(false, false);
         ts = xpc_timestamps();
         ok = ! ts;
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "set to true"))
      {
         cbool_t ts;
         xpc_timestamps_set(true, false);
         ts = xpc_timestamps();
         ok = ts;
         unit_test_status_pass(&status, ok);
      }

      xpc_timestamps_set(original_use_timestamps, false);

      /*  3 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_timestamps() == original_use_timestamps;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_06()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level setter and getter functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_errlevel()
 *    -  xpc_errlevel_set()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_06 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 6, _("errorlogging"), _("xpc_errlevel()")
   );
   if (ok)
   {
      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_NONE"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NONE);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_NONE;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ERRORS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ERRORS);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ERRORS;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_WARNINGS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_WARNINGS);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_WARNINGS;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_INFO"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_INFO;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ALL"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ALL);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ALL;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if
      (
         unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_NOT_APPLICABLE")
      )
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NOT_APPLICABLE);
         if (ok)
         {
            xpccut_errprint(_("handling for bad error-level failed"));
         }
         else
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ALL;    /* from previous sub-test  */
         }
         unit_test_status_pass(&status, ok);
      }

      /* Prep for final sub-test */

      xpc_errlevel_set(original_errlevel);

      /*  7 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_07()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level token parser.
 *
 *    Also the the unit-test for the xpc_option_errlevel() function.  That
 *    function is called by xpc_parse_leveltoken().  Nonetheless, we provide
 *    the semi-redundant unit-tests for the xpc_option_errlevel() function.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_parse_leveltoken()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_07 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 7, _("errorlogging"), _("xpc_parse_leveltoken()")
   );
   if (ok)
   {
      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "--quiet"))
      {
         ok = xpc_parse_leveltoken("--quiet");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_NONE;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "--errors"))
      {
         ok = xpc_parse_leveltoken("--errors");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ERRORS;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "--warnings"))
      {
         ok = xpc_parse_leveltoken("--warnings");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_WARNINGS;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "--warn"))
      {
         ok = xpc_parse_leveltoken("--warn");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_WARNINGS;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "--info"))
      {
         ok = xpc_parse_leveltoken("--info");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_INFO;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "--information"))
      {
         ok = xpc_parse_leveltoken("--information");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_INFO;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "--verbose"))
      {
         ok = xpc_parse_leveltoken("--verbose");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ALL;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "--all"))
      {
         ok = xpc_parse_leveltoken("--all");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_ALL;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "--bogus"))
      {
         xpc_errlevel_t saved_level = xpc_errlevel();
         if (xpccut_is_silent())
         {
            ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NONE);
            if (ok)
            {
               xpc_errlevel_t errlev = xpc_errlevel();
               ok = errlev == XPC_ERROR_LEVEL_NONE;
            }
         }
         if (ok)
         {
            ok = xpc_parse_leveltoken("--bogus");
            if (ok)
            {
               xpccut_errprint(_("handling for bad error-level failed"));
            }
            else
            {
               xpc_errlevel_t errlev;
               if (xpccut_is_silent())
                  ok = xpc_errlevel_set(saved_level);

               errlev = xpc_errlevel();
               ok = errlev == XPC_ERROR_LEVEL_ALL; /* from earlier sub-test   */
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "--silent"))
      {
         ok = xpc_parse_leveltoken("--quiet");
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == XPC_ERROR_LEVEL_NONE;
         }
         unit_test_status_pass(&status, ok);
      }

      /**
       *    What about XPC_ERROR_LEVEL_NOT_APPLICABLE?  Any way to make that
       *    one happen?
       */

      /* Prep for next sub-test. */

      ok = xpc_errlevel_set(original_errlevel);

      /* 11 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "nullptr"))
      {
         ok = ! xpc_parse_leveltoken(nullptr);
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_errlevel();
            ok = errlev == original_errlevel;
         }
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "verify original again"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_08()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level token parser.
 *
 *    Also see the unit-test for the xpc_parse_leveltoken() function.  We
 *    provide the semi-redundant unit-tests for both functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_option_errlevel()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_08 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 8, _("errorlogging"), _("xpc_option_errlevel()")
   );
   if (ok)
   {
      /*
       * The xpc_option_errlevel() only returns an error-level; it does not
       * modify the global error-level.  Nonetheless, it doesn't hurt to
       * verify that it didn't get changed by someone slipping in that
       * functionality.
       */

      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "--quiet"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--quiet");
         ok = errlev == XPC_ERROR_LEVEL_NONE;
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "--errors"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--errors");
         ok = errlev == XPC_ERROR_LEVEL_ERRORS;
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "--warnings"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--warnings");
         ok = errlev == XPC_ERROR_LEVEL_WARNINGS;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "--warn"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--warn");
         ok = errlev == XPC_ERROR_LEVEL_WARNINGS;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "--info"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--info");
         ok = errlev == XPC_ERROR_LEVEL_INFO;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "--information"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--information");
         ok = errlev == XPC_ERROR_LEVEL_INFO;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "--verbose"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--verbose");
         ok = errlev == XPC_ERROR_LEVEL_ALL;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "--all"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--all");
         ok = errlev == XPC_ERROR_LEVEL_ALL;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "--daemon"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--daemon");
         ok = errlev == XPC_ERROR_LEVEL_NONE;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "--bogus"))
      {
         xpc_errlevel_t saved_level = xpc_errlevel();

         /*
          * Quieting the output isn't really necessary here, since the
          * function will emit no error-log mesage for this test case.
          * However, what the heck.
          */

         if (xpccut_is_silent())
         {
            ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NONE);
            if (ok)
            {
               xpc_errlevel_t errlev = xpc_errlevel();
               ok = errlev == XPC_ERROR_LEVEL_NONE;
            }
         }
         if (ok)
         {
            xpc_errlevel_t errlev = xpc_option_errlevel("--bogus");
            if (xpccut_is_silent())
               ok = xpc_errlevel_set(saved_level);

            ok = errlev == XPC_ERROR_LEVEL_NOT_APPLICABLE;
         }
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "--silent"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--quiet");
         ok = errlev == XPC_ERROR_LEVEL_NONE;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "--debug"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel("--debug");
#ifdef DEBUG
         ok = errlev == XPC_ERROR_LEVEL_INFO;
#else
         ok = errlev == XPC_ERROR_LEVEL_ERRORS;
#endif
         unit_test_status_pass(&status, ok);
      }

      /* Prep for next sub-test. */

      ok = xpc_errlevel_set(original_errlevel);

      /* 13 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "nullptr"))
      {
         xpc_errlevel_t errlev = xpc_option_errlevel(nullptr);
         ok = errlev == XPC_ERROR_LEVEL_NOT_APPLICABLE;
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "verify original again"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * unit_test_s_filename_check()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a helper function to indirectly test the static function
 *    s_filename_check().
 *
 * \param option_name
 *    Provides the option name to be checked.
 *
 * \test
 *    s_filename_check().  Tested implicitly.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
unit_test_s_filename_check
(
   const char * option_name      /**< Name of option (--log or --append).     */
)
{
   /*
    * This sub-test provides a too-small argument count and some other
    * bogus information about the file-name.
    */

   cbool_t not_ok;
   char * test_argv[4];
   char option[32];
   test_argv[1] = option;
   test_argv[2] = nullptr;
   test_argv[3] = nullptr;
   strncpy(option, option_name, sizeof(option));
   option[32-1] = 0;
   not_ok = ! xpc_parse_errlevel_nohelp(2, test_argv);
   if (not_ok)
   {
      test_argv[2] = ".";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   if (not_ok)
   {
      test_argv[2] = "..";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   if (not_ok)
   {
      test_argv[2] = "./";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   if (not_ok)
   {
      test_argv[2] = "../";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   if (not_ok)
   {
      test_argv[2] = ".\\";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   if (not_ok)
   {
      test_argv[2] = "..\\";
      not_ok = ! xpc_parse_errlevel_nohelp(3, test_argv);
   }
   return not_ok;
}

/******************************************************************************
 * errorlogging_test_02_09()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the "internal"
 *    XPC command-line token parser.
 *
 *    This test is a bit difficult to implement for a couple of reasons:
 *
 *       -# There are a lot of command-line options to test.
 *       -# All of the possible sub-tests provide a large number of
 *          side-effects on the status of the error-logging facility, so
 *          there are a lot of internal data items that would need to be
 *          saved and restored.
 *
 *    For now, we punt on these issues, and just test the "--help" option
 *    the handling of null pointers, the handling on no arguments, and the
 *    handling of options (e.g. "--log") that require an argument.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_parse_errlevel_nohelp()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_09 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 9, _("errorlogging"), _("xpc_parse_errlevel_nohelp()")
   );
   if (ok)
   {
      char * test_argv[4];
      test_argv[0] = "errorlogging";
      test_argv[1] = nullptr;
      test_argv[2] = nullptr;
      test_argv[3] = nullptr;

      /*  1 */

      if (unit_test_status_next_subtest(&status, "nullptr"))
      {
         cbool_t not_ok = ! xpc_parse_errlevel_nohelp(1, nullptr);
         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "argc == 0"))
      {
         cbool_t not_ok = ! xpc_parse_errlevel_nohelp(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "argc == 1"))
      {
         ok = xpc_parse_errlevel_nohelp(1, test_argv);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "--version"))
      {
         cbool_t not_ok;
         test_argv[1] = "--version";
         not_ok = ! xpc_parse_errlevel_nohelp(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "--help"))
      {
         cbool_t not_ok;
         test_argv[1] = "--help";
         not_ok = ! xpc_parse_errlevel_nohelp(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "--log bogus"))
      {
         /*
          * This sub-test provides a too-small argument count and some other
          * bogus information about the file-name.
          */

         cbool_t not_ok = unit_test_s_filename_check("--log");
         unit_test_status_pass(&status, not_ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "--append bogus"))
      {
         cbool_t not_ok = unit_test_s_filename_check("--append");
         unit_test_status_pass(&status, not_ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "--non-existent-option"))
      {
         test_argv[1] = "--non-existent-option";
         ok = xpc_parse_errlevel_nohelp(2, test_argv);
         unit_test_status_pass(&status, ok);
      }

      /*  x */

      if (unit_test_status_next_subtest(&status, "No further testing"))
      {
         if (unit_test_options_is_verbose(options))
         {
            xpc_infoprint_func
            (
               _("there's a lot more to test in xpc_parse_errlevel_nohelp()")
            );
         }
         unit_test_status_pass(&status, true);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_10()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the external
 *    XPC command-line token parser.
 *
 *    This test is essentially identical to errorlogging_test_02_09(), and
 *    the two tests should be kept in synchrony.  If this becomes too much
 *    of a hassle, we could write an internal helper testing function that
 *    accepts a function pointer, assigned to either xpc_parse_errlevel() or
 *    xpc_parse_errlevel_nohelp().
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_parse_errlevel()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_10 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 10, _("errorlogging"), _("xpc_parse_errlevel()")
   );
   if (ok)
   {
      char * test_argv[4];
      test_argv[0] = "errorlogging";
      test_argv[1] = nullptr;
      test_argv[2] = nullptr;
      test_argv[3] = nullptr;

      /*  1 */

      if (unit_test_status_next_subtest(&status, "nullptr"))
      {
         cbool_t not_ok = ! xpc_parse_errlevel(1, nullptr);
         unit_test_status_pass(&status, not_ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "argc == 0"))
      {
         cbool_t not_ok = ! xpc_parse_errlevel(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "argc == 1"))
      {
         ok = xpc_parse_errlevel(1, test_argv);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "--version"))
      {
         cbool_t not_ok;
         test_argv[1] = "--version";
         not_ok = ! xpc_parse_errlevel(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "--help"))
      {
         cbool_t not_ok;
         test_argv[1] = "--help";
         not_ok = ! xpc_parse_errlevel(0, test_argv);
         unit_test_status_pass(&status, not_ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "--log bogus"))
      {
         /*
          * This sub-test provides a too-small argument count and some other
          * bogus information about the file-name.
          */

         cbool_t not_ok = unit_test_s_filename_check("--log");
         unit_test_status_pass(&status, not_ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "--append bogus"))
      {
         cbool_t not_ok = unit_test_s_filename_check("--append");
         unit_test_status_pass(&status, not_ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "--non-existent-option"))
      {
         test_argv[1] = "--non-existent-option";
         ok = xpc_parse_errlevel(2, test_argv);
         unit_test_status_pass(&status, ok);
      }

      /*  x */

      if (unit_test_status_next_subtest(&status, "No further testing"))
      {
         if (unit_test_options_is_verbose(options))
         {
            xpc_infoprint_func
            (
               _("there's a lot more to test in xpc_parse_errlevel()")
            );
         }
         unit_test_status_pass(&status, true);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_11()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level string
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_errlevel_string()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_11 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 11, _("errorlogging"), _("xpc_errlevel_string()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "ERRORS"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_ERRORS);
         ok = strcmp(token, _("errors")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", XPC_ERROR_LEVEL_ERRORS, token);

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "WARNINGS"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_WARNINGS);
         ok = strcmp(token, _("warnings")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", XPC_ERROR_LEVEL_WARNINGS, token);

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "INFO"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_INFO);
         ok = strcmp(token, _("info")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", XPC_ERROR_LEVEL_INFO, token);

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "ALL"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_ALL);
         ok = strcmp(token, _("verbose")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", XPC_ERROR_LEVEL_ALL, token);

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "NONE"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_NONE);
         ok = strcmp(token, _("quiet")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", XPC_ERROR_LEVEL_NONE, token);

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "NOT_APPLICABLE"))
      {
         const char * token = xpc_errlevel_string(XPC_ERROR_LEVEL_NOT_APPLICABLE);
         ok = strcmp(token, _("N/A")) == 0;
         if (unit_test_options_is_verbose(options))
         {
            fprintf
            (
               stdout, "  Token %d == %s\n",
               XPC_ERROR_LEVEL_NOT_APPLICABLE, token
            );
         }

         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "unknown"))
      {
         const char * token = xpc_errlevel_string((xpc_errlevel_t) 999);
         ok = strcmp(token, _("unknown")) == 0;
         if (unit_test_options_is_verbose(options))
            fprintf(stdout, "  Token %d == %s\n", 999, token);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_12()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level show-functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_showerrlevel()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_12 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 12, _("errorlogging"), _("xpc_showerrlevel()")
   );
   if (ok)
   {
      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_NONE"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NONE);
         if (ok)
         {
            if (unit_test_options_show_progress(options))
               xpc_showerrlevel();
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ERRORS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ERRORS);
         if (ok)
         {
            if (unit_test_options_show_progress(options))
               xpc_showerrlevel();
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_WARNINGS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_WARNINGS);
         if (ok)
         {
            if (unit_test_options_show_progress(options))
               xpc_showerrlevel();
         }
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_INFO"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
         if (ok)
         {
            if (unit_test_options_show_progress(options))
               xpc_showerrlevel();
         }
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ALL"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ALL);
         if (ok)
         {
            if (unit_test_options_show_progress(options))
               xpc_showerrlevel();
         }
         unit_test_status_pass(&status, ok);
      }

      /* Prep for final sub-test */

      xpc_errlevel_set(original_errlevel);

      /*  7 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_13()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level show-functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_showerrlevel()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_13 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 13, _("errorlogging"), _("xpc_showxxx()")
   );
   if (ok)
   {
      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_NONE"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_NONE);
         if (ok)
            ok = xpc_shownothing();

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ERRORS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ERRORS);
         if (ok)
            ok = xpc_showerrors();

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_WARNINGS"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_WARNINGS);
         if (ok)
            ok = xpc_showwarnings();

         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_INFO"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
         if (ok)
            ok = xpc_showinfo();

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_ALL"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_ALL);
         if (ok)
            ok = xpc_showall();

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LEVEL_INFO"))
      {
         ok = xpc_errlevel_set(XPC_ERROR_LEVEL_INFO);
         if (ok)
         {
            cbool_t isdebug = xpc_showdebug();

#if defined DEBUG && ! defined XPC_NO_ERRORLOG
            ok = isdebug;
#else
            ok = ! isdebug;
#endif

         }
         unit_test_status_pass(&status, ok);
      }

      /* Prep for final sub-test */

      xpc_errlevel_set(original_errlevel);

      /*  7 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_14()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    XPC error-level show-functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_showerrlevel()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_14 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 14, _("errorlogging"), _("xpc_showerr_version()")
   );
   if (ok)
   {
      xpc_errlevel_t original_errlevel = xpc_errlevel();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "show version"))
      {
         xpc_showerr_version();
         unit_test_status_pass(&status, true);
      }

      /* Prep for final sub-test */

      xpc_errlevel_set(original_errlevel);

      /*  2 */

      if (unit_test_status_next_subtest(&status, "set to original"))
      {
         ok = xpc_errlevel() == original_errlevel;
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_15()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    xpc_syslogging_set() function.
 *
 *    There's not much that can cause an error in this function.  The only
 *    possibility of error is if the atexit() call fails.
 *
 *    If you run this test with the --info option, you should see a note
 *    about atexit() being called.
 *
 *    If you run this test under valgrind, there should be no memory leaks.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_syslogging_set()
 *    -  xpc_syslogging()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_15 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 15, _("errorlogging"), _("xpc_syslogging_set()")
   );
   if (ok)
   {
      cbool_t original_synch_usage = xpc_syslogging();

      /*  1 */

      if (unit_test_status_next_subtest(&status, "set to false"))
      {
         ok = xpc_syslogging_set(false);
         if (ok)
            ok = ! xpc_syslogging();

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "set to true"))
      {
         ok = xpc_syslogging_set(true);
         if (ok)
            ok = xpc_syslogging();

         unit_test_status_pass(&status, ok);
      }

      (void) xpc_syslogging_set(original_synch_usage);
   }
   return status;
}

/******************************************************************************
 * XPC_ACCESS
 *------------------------------------------------------------------------*//**
 *
 *    An internal macro for the access() function.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_ACCESS
#ifdef WIN32
#define XPC_ACCESS      _access
#else
#define XPC_ACCESS      access
#endif
#endif

/******************************************************************************
 * errorlogging_test_02_16()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    xpc_getlasterror() function.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_getlasterror()
 *    -  xpc_getlasterror_text()
 *    -  xpc_getlasterror_text_ex() [indirect test]
 *    -  xpc_getlasterror_text_r()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_16 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 16, _("errorlogging"), _("xpc_getlasterror()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "errno and perror()"))
      {
         int rcode = XPC_ACCESS("no-such-file.pdq", F_OK);
         ok = not_posix_success(rcode);
         if (ok)
         {
            if (unit_test_options_is_verbose(options))
            {
               int errcode = xpc_getlasterror();
               fprintf
               (
                  stdout, "%s: '%d'\n",
                  _("  The errno value set by the access() function was"),
                  errcode
               );
               perror(_("  The system string for this error is"));
            }
         }
         else
            xpc_errprintex(_("how can this file exist?"), "no-such-file.pdq");

         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "xpc_getlasterror_text()"))
      {
         int rcode = XPC_ACCESS("no-such-file.pdq", F_OK);
         ok = not_posix_success(rcode);
         if (ok)
         {
            const char * error_text = xpc_getlasterror_text();
            ok = not_nullptr(error_text);
            if (ok && unit_test_options_is_verbose(options))
            {
               char * error_text_2;
               char * error_text_3;
               char msg[512];
               int error_number = xpc_getlasterror();

               /*
                * This test doesn't perform as expected.  While the perror()
                * call above works for errors from the access() function,
                * that strerror() family of calls used in
                * xpc_getlasterror_text() yield an empty message string,
                * which you can see if the --verbose option is used.
                */

               fprintf
               (
                  stdout, "  %s:\n     '%s'\n",
                  _("The error string for the error is"), error_text
               );

               /*
                * This call works.  The message returned via the pointer is
                * as expected.
                */

               error_text_2 = strerror(error_number);
               fprintf
               (
                  stdout, "  %s:\n     '%s'\n",
                  _("The strerror() string for the error is"), error_text_2
               );

               /*
                * This call works.  The message returned via the pointer is
                * as expected.  But the actual message buffer is of zero
                * length!  The two fprintf() calls could show the
                * difference, but sometimes the buffer has junk in it,
                * instead of being empty.
                */

               error_text_3 = strerror_r(error_number, msg, 512);
               fprintf
               (
                  stdout, "  %s:\n     '%s'\n",
                  _("The strerror_r() string for the error is"), error_text_3
               );

               /*
                * fprintf
                * (
                *    stdout, "  %s:\n     '%s'\n",
                *    _("The strerror_r() buffer contains"), msg
                * );
                */
            }
         }
         else
            xpc_errprintex(_("how can this file exist?"), "no-such-file.pdq");

         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "xpc_getlasterror_text_r()"))
      {
         int rcode = XPC_ACCESS("no-such-file.pdq", F_OK);
         ok = not_posix_success(rcode);
         if (ok)
         {
            char text[128];
            const char * errmsg = xpc_getlasterror_text_r(rcode, text, 128);
            int errcode = xpc_getlasterror();
            ok = strlen(errmsg) > 0;
            if (unit_test_options_is_verbose(options))
            {
               fprintf
               (
                  stdout, "  %d: %s:\n     '%s'\n",
                  errcode, _("The xpc_getlasterror_text_r() string is"), errmsg
               );
            }
            if (ok)
            {
               errmsg = xpc_getlasterror_text_r(rcode, text, 4);
               ok = strlen(errmsg) == 3;
               if (unit_test_options_is_verbose(options))
               {
                  fprintf
                  (
                     stdout, "  %d: %s:\n     '%s'\n",
                     errcode, _("Truncated to 3, the string is"), errmsg
                  );
               }
            }
         }
         else
            xpc_errprintex(_("how can this file exist?"), "no-such-file.pdq");

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_17()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    output-buffering-setting functions.
 *
 *    This test validates the range-checking of the function and the success
 *    of each of the calls.
 *
 *    This test does not bother trying to figure out the original setting of
 *    buffering, and then trying to restore it after the test.  Thus, it may
 *    affect the visual appearance of the tests that follow.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_buffering_set()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_17 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 17, _("errorlogging"), _("xpc_buffering_set()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "lower limit - 2"))
      {
         ok = ! xpc_buffering_set(XPC_ERROR_NOT_BUFFERED-2);
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "lower limit - 1"))
      {
         ok = ! xpc_buffering_set(XPC_ERROR_NOT_BUFFERED-1);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_NOT_BUFFERED"))
      {
         ok = xpc_buffering_set(XPC_ERROR_NOT_BUFFERED);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_BLOCK_BUFFERED"))
      {
         ok = xpc_buffering_set(XPC_ERROR_BLOCK_BUFFERED);
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "XPC_ERROR_LINE_BUFFERED"))
      {
         ok = xpc_buffering_set(XPC_ERROR_LINE_BUFFERED);
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "upper limit + 1"))
      {
         ok = ! xpc_buffering_set(XPC_ERROR_LINE_BUFFERED+1);
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "upper limit + 2"))
      {
         ok = ! xpc_buffering_set(XPC_ERROR_LINE_BUFFERED+2);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_18()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    pointer-testing functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_is_thisptr()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_02_18 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 18, _("errorlogging"), _("xpc_is_thisptr()")
   );
   if (ok)
   {
      const char * testptr = "illinois";

      /*  1 */

      if (unit_test_status_next_subtest(&status, "good pointer, good func-name"))
      {
         ok = xpc_is_thisptr(testptr, __func__);
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "good pointer, bad func-name"))
      {
         ok = xpc_is_thisptr(testptr, nullptr);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "bad pointer, good func-name"))
      {
         ok = ! xpc_is_thisptr(nullptr, __func__);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "bad pointer, bad func-name"))
      {
         ok = ! xpc_is_thisptr(nullptr, nullptr);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * errorlogging_test_02_19()
 *------------------------------------------------------------------------*//**
 *
 *    This test group/case provides basic unit-tests of the
 *    null-pointer-testing functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_is_thisptr()
 *
 *//*-------------------------------------------------------------------------*/

// TODO add 02_20 for testing the new function xpc_get_priority

static unit_test_status_t
errorlogging_test_02_19 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 19, _("errorlogging"), _("xpc_not_nullptr()")
   );
   if (ok)
   {
      const char * testptr = "south carolina";

      /*  1 */

      if (unit_test_status_next_subtest(&status, "good pointer, good func-name"))
      {
         ok = xpc_not_nullptr(testptr, __func__);
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "good pointer, bad func-name"))
      {
         ok = xpc_not_nullptr(testptr, nullptr);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "bad pointer, good func-name"))
      {
         ok = ! xpc_not_nullptr(nullptr, __func__);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "bad pointer, bad func-name"))
      {
         ok = ! xpc_not_nullptr(nullptr, nullptr);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * plain_string_thread_function()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an example of how to write a simple thread function.
 *
 * \obsolete
 *    Currently not used in any unit-test.
 *
 * \return
 *    Returns the \a thread_id parameter.
 *
 *//*-------------------------------------------------------------------------*/

void *
plain_string_thread_function
(
   void * thread_id     /**< An integer parameter identifying the thread.     */
)
{
   if (! xpccut_is_silent())
   {
      const char * s = (const char *) thread_id;
      if (not_nullptr(s) && (strlen(s) > 0))
      {
         int index;
         for (index = 0; index < 10; index++)
         {
            fprintf(stderr, "%s", s);
            xpc_ms_sleep(1);
         }
      }
      else
         xpc_errprint(_("no string for thread to print"));
   }
   return thread_id;
}

/******************************************************************************
 * string_thread_function()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an example of how to write a simple thread function.
 *
 * \return
 *    Returns the \a thread_id parameter.
 *
 *//*-------------------------------------------------------------------------*/

void *
string_thread_function
(
   void * thread_id     /**< An integer parameter identifying the thread.     */
)
{
   if (! xpccut_is_silent())
   {
      const char * s = (const char *) thread_id;
      if (not_nullptr(s) && (strlen(s) > 0))
      {
         int index;
         for (index = 0; index < 5; index++)
         {
            xpc_errprint(s);
            xpc_ms_sleep(1);
         }
      }
      else
         xpc_errprint(_("no string for thread to print"));
   }
   return thread_id;
}

/******************************************************************************
 * string_1
 *------------------------------------------------------------------------*//**
 *
 *    A string for the first thread.
 *
 *//*-------------------------------------------------------------------------*/

static const char * string_1 =
 "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1"
   ;

/******************************************************************************
 * string_2
 *------------------------------------------------------------------------*//**
 *
 *    A string for the second thread.
 *
 *//*-------------------------------------------------------------------------*/

static const char * string_2 =
 "2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2"
   ;

/******************************************************************************
 * errorlogging_test_03_01()
 *------------------------------------------------------------------------*//**
 *
 *    This test group provides threaded smoke-test unit-tests of the
 *    errorlogging module.
 *
 * \note
 *    This test is considered interactive, since it really doesn't have the
 *    expected effect at this time, needs to be evaluated by a human, and
 *    puts out too much output anyway.
 *
 * \win32
 *    In a DOS console, this test takes 300 ms.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \test
 *    -  xpc_synchusage_set()
 *    -  xpc_synchusage()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
errorlogging_test_03_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 1, _("errorlogging"), _("visible synch'ing")
   );
   if (ok)
      ok = unit_test_options_is_interactive(options);

   if (ok)
   {
      pthread_attr_t x_attributes;
      pthread_t thread_1;
      pthread_t thread_2;
      cbool_t original_synch_usage = xpc_synchusage();
      ok = pthread_attributes_init(&x_attributes);

      /*  1 */

      (void) xpc_synchusage_set(false);
      if (unit_test_status_next_subtest(&status, "--no-synch"))
      {
         xpc_infoprint(_("output syncronization is turned off"));
         if (ok)
            ok = ! xpc_synchusage();

         if (ok)
         {
            void * stp;
            thread_1 = pthreader_create
            (
               &x_attributes,
               string_thread_function,
               (void *) string_1
            );
            thread_2 = pthreader_create
            (
               &x_attributes,
               string_thread_function,
               (void *) string_2
            );
            stp = pthreader_join(thread_1);
            if (is_nullptr(stp))
            {
               xpc_errprint(_("join 1 returned null"));
               ok = false;
            }
            else
               xpc_infoprint(_("thread 1 done"));

            stp = pthreader_join(thread_2);
            if (is_nullptr(stp))
            {
               xpc_errprint(_("join 2 returned null"));
               ok = false;
            }
            else
               xpc_infoprint(_("thread 2 done"));
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      (void) xpc_synchusage_set(true);
      if (unit_test_status_next_subtest(&status, "--synch"))
      {
         void * stp;
         xpc_infoprint(_("output syncronization is turned on"));
         if (ok)
            ok = xpc_synchusage();

         thread_1 = pthreader_create
         (
            &x_attributes,
            string_thread_function,
            (void *) string_1
         );
         thread_2 = pthreader_create
         (
            &x_attributes,
            string_thread_function,
            (void *) string_2
         );
         stp = pthreader_join(thread_1);
         if (is_nullptr(stp))
         {
            xpc_errprint(_("join 1 returned null"));
            ok = false;
         }
         else
            xpc_infoprint(_("thread 1 done"));

         stp = pthreader_join(thread_2);
         if (is_nullptr(stp))
         {
            xpc_errprint(_("join 2 returned null"));
            ok = false;
         }
         else
            xpc_infoprint(_("thread 2 done"));

         unit_test_status_pass(&status, ok);
      }
      (void) xpc_synchusage_set(original_synch_usage);
   }
   return status;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the errorlogging_ut application.
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
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME          "XPC library error-logging test"
#define XPC_TEST_NAME         "errorlogging_ut"
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
   static const char * const additional_help =

"\n"
XPC_TEST_NAME "-specific options:\n"
"\n"
" --leak-check          Disable some of the 'destructor' calls.  This item can\n"
"                       be used when running valgrind (as an example). NOT YET\n"
"                       READY.\n"
" --show-settings       Show compiler settings and other macro values.\n"

   ;

   unit_test_t testbattery;                           /* uses default values  */
   cbool_t ok = xpc_parse_errlevel(argc, argv);       /* cool feature         */
   ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPC_TEST_NAME,
      "Error-Logging Test 0.1",     /* XPC_VERSION_STRING(XPC_TEST_VERSION),  */
      additional_help
   );
   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      cbool_t load_interactive = false;               /* for now, anywya      */
      strcpy(gsBasename, DEFAULT_BASE);
      strcpy(gsAuthor, DEFAULT_AUTHOR);
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            const char * arg = arglist[currentarg];
            if (strcmp(arg, "--leak-check") == 0)
            {
               g_do_leak_check = true;
               xpccut_infoprint(_("memory leakage enabled"));
            }
            if (strcmp(arg, "--show-settings") == 0)
            {
               /*
                * Show some informative macro values using these functions
                * from the build_versions.c/h module.
                */

               show_have_macros();
               show_build_settings();
               show_compiler_info();
               ok = false;
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

            ok = unit_test_load(&testbattery, errorlogging_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, errorlogging_test_02_01);
               (void) unit_test_load(&testbattery, errorlogging_test_02_02);
               (void) unit_test_load(&testbattery, errorlogging_test_02_03);
               (void) unit_test_load(&testbattery, errorlogging_test_02_04);
               (void) unit_test_load(&testbattery, errorlogging_test_02_05);
               (void) unit_test_load(&testbattery, errorlogging_test_02_06);
               (void) unit_test_load(&testbattery, errorlogging_test_02_07);
               (void) unit_test_load(&testbattery, errorlogging_test_02_08);
               (void) unit_test_load(&testbattery, errorlogging_test_02_09);
               (void) unit_test_load(&testbattery, errorlogging_test_02_10);
               (void) unit_test_load(&testbattery, errorlogging_test_02_11);
               (void) unit_test_load(&testbattery, errorlogging_test_02_12);
               (void) unit_test_load(&testbattery, errorlogging_test_02_13);
               (void) unit_test_load(&testbattery, errorlogging_test_02_14);
               (void) unit_test_load(&testbattery, errorlogging_test_02_15);
               (void) unit_test_load(&testbattery, errorlogging_test_02_16);
               (void) unit_test_load(&testbattery, errorlogging_test_02_17);
               (void) unit_test_load(&testbattery, errorlogging_test_02_18);
               ok = unit_test_load(&testbattery, errorlogging_test_02_19);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, errorlogging_test_03_01);
               // ok = unit_test_load(&testbattery, errorlogging_test_03_yy);
            }
            if (ok)
            {
               /**
                * Right now, the interactive tests are not loaded be default.
                * This is because they are painful to test, and we're not so
                * concerned with them right now.
                *
                * If you really want to run them, add the --interactive option
                * to the command-line.
                */

               if (load_interactive)
               {
                  /* No interactive tests at this time. */
               }
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
 * errorlogging_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
