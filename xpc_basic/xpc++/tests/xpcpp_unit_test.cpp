/******************************************************************************
 * xpcpp_unit_test.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          xpcpp_unit_test.cpp
 * \library       libxpc++
 * \author        Chris Ahlstrom
 * \date          2010-05-30
 * \updates       2014-04-22
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the xpc++ library
 *    module.
 *
 * \note
 *    It all of these unit-tests, it is important to understand that a
 *    status coming up invalid [i.e. status.valid() == false] is \e not a
 *    test failure.
 *
 *    If status.valid() returns 'false', and you want to be sure that it is
 *    not an error, you will (currently) have to check
 *    xpc::cut_status::m_Is_Valid and xpc::cut_status::m_Can_Run separately.
 *    No separate public accessors are (yet) provided.
 *
 *//*-------------------------------------------------------------------------*/

#include <stdexcept>                   /* std::logic_error                    */
#include <iostream>                    /* std::cout and std::cerr             */
#include <xpc/binstring.hpp>           /* xpc::binstring class                */
#include <xpc/cut.hpp>                 /* xpc::cut unit-test class            */
#include <xpc/errorlog.hpp>            /* xpc::errorlog class                 */
#include <xpc/initree.hpp>             /* xpc::initree class                  */
#include <xpc/stringmap.hpp>           /* xpc::stringmap class                */
#include <xpc/rowset.hpp>              /* xpc::rowset class                   */
#include <xpc/systemtime.hpp>          /* xpc::systemtime class               */

/******************************************************************************
 * gs_do_leak_check
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to disable some "destructor" calls, in order to test
 *    leakage mechanisms.
 *
 *    This variable is normally false.  It can be set to true using the
 *    --leak-check option (which should not be confused with valgrind's
 *    option of the same name).
 *
 *    This is an element specific to this unit-test application.
 *
 *//*-------------------------------------------------------------------------*/

static bool gs_do_leak_check = false;

/******************************************************************************
 * xpcpp_unit_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a basic "smoke test" for the XPC C++ classes.
 *
 *    A smoke test is a test of the basic functionality of the object or
 *    function.  It is an easy test that makes sure the code has basic
 *    integrity.
 *
 *    This test is about the simplest unit test function that can be
 *    written.
 *
 * \group
 *    1. Smoke tests.
 *
 * \case
 *    1. Basic smoke test.
 *
 * \note
 *    It all of these unit-tests, it is important to understand that a
 *    status coming up invalid [i.e. status.valid() == false] is \e not a
 *    test failure -- it only indicates that the status object is invalid \e
 *    or that the test is not allowed to run.
 *
 * \tests
 *    -  xpc::cutxxx()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_01_01 (const xpc::cut_options & options)
{
   xpc::cut_status status(options, 1, 1, "XPC++", _("Basic Smoke Test"));
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("cut_status::pass()"))
            status.pass();
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::stringmap class.
 *
 * \group
 *    2. xpc::stringmap
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::stringmap()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_02_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 2, 1, "xpc::stringmap", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("xpc::stringmap()"))
            status.pass();
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_03_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::rowset class.
 *
 * \group
 *    3. xpc::rowset
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::rowset()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_03_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 3, 1, "xpc::rowset", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("xpc::rowset()"))
            status.pass();
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_04_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::systemtime class.
 *
 * \group
 *    4. xpc::systemtime
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::systemtime()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_04_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 1, "xpc::systemtime", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("xpc::systemtime(false)"))
         {
            xpc::systemtime s(false);              /* do not set current time */
            ok = s.is_reset();
            status.pass();
         }
         if (status.next_subtest("xpc::systemtime(true)"))
         {
            xpc::systemtime s(true);               /* set the current time    */
            ok = ! s.is_reset();
            status.pass();
         }
         if (status.next_subtest("xpc::systemtime()"))
         {
            xpc::systemtime s;                     /* set the current time    */
            ok = ! s.is_reset();
            status.pass();
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_04_02()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::systemtime class.
 *
 * \group
 *    4. xpc::systemtime
 *
 * \case
 *    2. duration()
 *
 * \tests
 *    -  xpc::systemtime::duration()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_04_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 2, "xpc::systemtime", _("duration()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         if (status.next_subtest("xpc::systemtime::duration()"))
         {
            xpc::systemtime s;
            xpc_ms_sleep(500);
            double duration = s.duration();
            ok = duration >= 0.5;
            if (options.show_values())
            {
               std::cout
                  << "  Expected duration 0.5 sec.  Actual:  "
                  << duration
                  << std::endl
                  ;
            }
            status.pass();
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_04_03()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::systemtime class.
 *
 * \group
 *    4. xpc::systemtime
 *
 * \case
 *    3. operator <()
 *
 * \tests
 *    -  xpc::systemtime::operator <()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_04_03 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 4, 3, "xpc::systemtime", _("operator <()")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         if (status.next_subtest("xpc::systemtime::operator <()"))
         {
            xpc::systemtime s;
            xpc_ms_sleep(100);
            xpc::systemtime s2;
            ok = s < s2;
            status.pass(ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_05_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::errorlog class.
 *
 * \group
 *    5. xpc::errorlog
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::errorlog()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_05_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 1, "xpc::errorlog", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::errorlog e;                          /* construct by default    */
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("xpc::errorlog::synchusage()"))
         {
            ok = e.synchusage() == false;
            status.pass(ok);
         }
         if (status.next_subtest("xpc::errorlog::errlevel()"))
         {
#ifdef XPC_NO_ERRLOG
            ok = e.errlevel() == XPC_ERROR_LEVEL_NONE;
#else
            ok = e.errlevel() == XPC_ERROR_LEVEL_ERRORS;
#endif
            status.pass();
         }
         if (status.next_subtest("xpc::errorlog::logfile()"))
         {
            ok = e.logfile() == stderr;
            status.pass();
         }
         if (status.next_subtest("xpc::errorlog::timestamps()"))
         {
            ok = e.timestamps() == false;
            status.pass();
         }
         if (status.next_subtest("xpc::errorlog::syslogging()"))
         {
            ok = e.syslogging() == false;
            status.pass();
         }
         if (status.next_subtest("xpc::errorlog::usecolor()"))
         {
#ifdef XPC_USE_COLORS
            ok = e.usecolor() == false;
#else
            status.pass();
#endif
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_05_02()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::errorlog class.
 *
 * \group
 *    5. xpc::errorlog
 *
 * \case
 *    2. Macro constructionb
 *
 * \tests
 *    -  xpc::errorlog()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_05_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 5, 2, "xpc::errorlog", _("Macro construction")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         /*
          * The xpc::mainlog() function provides a reference to a hidden
          * xpc::errorlog object that is always available [once mainlog()
          * first gets called, that is].  In the main() function, we set the
          * main errorlog object according to the command-line arguments.
          *
          *    const xpc::errorlog e = xpc::mainlog();
          *
          * Instead of accessing this object directly, macros are provided
          * to make the calls more like the original C error-logging
          * functions and macros.
          */

         const std::string x = "x";
         const std::string y = "y";
         const int err = 100;

         if (status.next_subtest("xpc::errorlog macros"))
         {
            log_errprint(x);
            log_errprintf("The variadic string is %s", x.c_str());
            log_errprintex(x, y);
            log_warnprint(x);
            log_warnprintf("The variadic string is %s", x.c_str());
            log_warnprintex(x, y);
            log_infoprint(x);
            log_infoprintf("The variadic string is %s", x.c_str());
            log_infoprintex(x, y);
            log_strerrprint(x, err);
            log_strerrprintex(x, y, err);
            log_strerrnoprint(x);
            log_strerrnoprintex(x, y);
            log_errprint_func(x);
            log_warnprint_func(x);
            log_infoprint_func(x);
            log_strerrprint_func(x, err);
            log_strerrnoprint_func(x);
            ok = true;
            status.pass(ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_06_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::binstring class.
 *
 * \group
 *    6. xpc::binstring
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::binstring()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_06_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 6, 1, "xpc::binstring", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         // xpc::cut::show(options, _("No values to show in this test"));

         if (status.next_subtest("xpc::binstring default"))
         {
            xpc::binstring bs;                     /* construct by default    */
            ok = bs.size() == 0;
            if (ok)
               ok = bs.length() == 0;

            if (ok)
               ok = bs.empty();

            if (ok)
               ok = std::string(bs) == "";

            if (ok)
               ok = bs.is_binary();

            status.pass(ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_06_02()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::binstring class.
 *
 * \group
 *    6. xpc::binstring
 *
 * \case
 *    2. String constructor
 *
 * \tests
 *    -  xpc::binstring(const std::string &))
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_06_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 6, 2, "xpc::binstring", _("String constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         // xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("xpc::binstring('hello')"))
         {
            std::string s("Hello");
            xpc::binstring bs(s);
            ok = bs.size() == 5;
            if (ok)
               ok = bs.length() == 5;

            if (ok)
               ok = ! bs.empty();

            if (ok)
               ok = std::string(bs) == "Hello";

            if (ok)
               ok = bs.is_binary();

            if (ok)
            {
               std::string hexversion = xpc::string_as_hex(std::string(bs));
               if (options.show_values())
               {
                  std::cout << hexversion;
               }
            }
            status.pass(ok);
         }
         if (status.next_subtest("xpc::binstring('Ctrl-A to E')"))
         {
            std::string s("\x01\x02\x03\x04\x05");
            xpc::binstring bs(s);
            ok = bs.size() == 5;
            if (ok)
               ok = bs.length() == 5;

            if (ok)
               ok = ! bs.empty();

            if (ok)
               ok = std::string(bs) == "\x01\x02\x03\x04\x05";

            if (ok)
               ok = bs.is_binary();

            if (ok)
            {
               std::string hexversion = xpc::string_as_hex(std::string(bs));
               if (options.show_values())
               {
                  std::cout << hexversion;
               }
            }
            status.pass(ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_07_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::initree class.
 *
 * \group
 *    7. xpc::initree
 *
 * \case
 *    1. Default constructor
 *
 * \tests
 *    -  xpc::initree()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_07_01 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 7, 1, "xpc::initree", _("Default constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         if (status.next_subtest("xpc::initree default"))
         {
            /*
             * The constructors of initree always create an unnamed section,
             * so there is always at least one section in the initree.
             */

            xpc::initree it;                     /* construct by default    */
            ok = it.size() == 1;

            if (ok)
               ok = ! it.empty();

            status.pass(ok);
         }

         /* MORE TESTS NEEDED */

      }
   }
   return status;
}

/******************************************************************************
 * xpcpp_unit_test_07_02()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the xpc::initree class.
 *
 * \group
 *    7. xpc::initree
 *
 * \case
 *    2. Principal constructor
 *
 * \tests
 *    -  xpc::initree()
 *
 * \param options
 *    Provides the command-line options for the unit-test application.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
xpcpp_unit_test_07_02 (const xpc::cut_options & options)
{
   xpc::cut_status status
   (
      options, 7, 2, "xpc::initree", _("Principle constructor")
   );
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         if (status.next_subtest("INI file smoke test"))
         {
            xpc::initree it("Smoke", "initree.ini");
            ok = it.size() > 0;
            if (ok)
            {
               if (options.is_verbose())
                  xpc::show("Unit Test 07.02", it);
            }

            status.pass(ok);
         }
      }
   }
   return status;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the xpcpp_unit_test application.
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

#define XPCPP_APP_NAME           "XPC++ library"
#define XPCPP_TEST_NAME          "xpcpp_unit_test"
#define XPCPP_TEST_VERSION       1.1.0
#define DEFAULT_BASE             "../tests"
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
XPCPP_TEST_NAME "-specific options:\n"
"\n"
" --leak-check       Turn on some leak-checking code.\n"

   ;

   /*
    * Note that the XPCCUT_VERSION_STRING macro is defined in the
    * unit_test.h module of the XPCCUT unit-test C library.
    */

   xpc::cut testbattery
   (
      argc, argv,
      std::string(XPCPP_TEST_NAME),
      std::string(XPCCUT_VERSION_STRING(XPCPP_TEST_VERSION)),
      std::string(additional_help)
   );
   bool ok = testbattery.valid();
   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      cbool_t load_the_tests = true;
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            std::string arg = arglist[currentarg];
            if (arg == "--no-leak-check")
            {
               gs_do_leak_check = false;
            }
            currentarg++;
         }

         /*
          * The xpc::mainlog() function provides a reference to a hidden
          * xpc::errorlog object that is always available [once mainlog()
          * first gets called, that is].  Here, we set the
          * main errorlog object according to the command-line arguments.
          */

         ok = log_parse(argc, argv);
      }
      if (ok && load_the_tests)
      {
         ok = testbattery.load(xpcpp_unit_test_01_01);
         if (ok)
         {
            // (void) testbattery.load(xpcpp_unit_test_01_02);
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_02_01);
            if (ok)
            {
               // (void) testbattery.load(xpcpp_unit_test_02_02);
            }
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_03_01);
            if (ok)
            {
               // (void) testbattery.load(xpcpp_unit_test_03_02);
            }
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_04_01);
            if (ok)
            {
               ok = testbattery.load(xpcpp_unit_test_04_02);
               if (ok)
                  ok = testbattery.load(xpcpp_unit_test_04_03);
            }
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_05_01);
            if (ok)
            {
               (void) testbattery.load(xpcpp_unit_test_05_02);
            }
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_06_01);
            if (ok)
            {
               (void) testbattery.load(xpcpp_unit_test_06_02);
            }
         }
         if (ok)
         {
            ok = testbattery.load(xpcpp_unit_test_07_01);
            if (ok)
            {
               (void) testbattery.load(xpcpp_unit_test_07_02);
            }
         }
      }
      if (ok)
         ok = testbattery.run();
      else
         xpccut_errprint(_("load of test functions failed"));
   }
   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/******************************************************************************
 * xpcpp_unit_test.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
