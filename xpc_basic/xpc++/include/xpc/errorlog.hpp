#ifndef XPC_ERRORLOG_HPP
#define XPC_ERRORLOG_HPP

/******************************************************************************
 * errorlog.hpp
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlog.hpp
 * \library       xpc++
 * \author        Chris Ahlstrom
 * \date          2010-09-11
 * \updates       2012-07-08
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides basic error codes and strings logging that are usable in
 *    C++ code.
 *
 *    This module is an elaborated version of xpc/errorlogging.c that
 *    provides a way to use multiple error-logs, each with their own
 *    settings.
 *
 *    It provides an xpc::errorlog class that contains members and output
 *    functions.
 *
 *    It also provides a static xpc::errorlog instance,
 *    accessible with the xpc::main_log() function, which always exists, and
 *    is a replacement for the global C functions and variables that
 *    control error output.  To use this global object to log errors and
 *    information, replace the C function output functions [e.g.
 *    xpc_errprint()] with a "log" macro of similar name [e.g.
 *    log_errprint()].
 *
 *//*-------------------------------------------------------------------------*/

#include <string>                      /* class std::string                   */
#include <xpc/errorlogging.h>          /* C::errorlogging functions           */
#include <xpc/syncher.h>               /* xpc_syncher_t structure             */
XPC_REVISION_DECL(errorlog)            /* extern void show_errorlog_info()    */

/******************************************************************************
 * Color-usage macros
 *------------------------------------------------------------------------*//**
 *
 *    These macros determine if color is supported.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
#define XPC_USE_COLORS
#endif

#ifdef WIN32
#undef XPC_USE_COLORS                  /* cannot use, see usecolor() */
#endif

/******************************************************************************
 * log_errprint family of macros
 *------------------------------------------------------------------------*//**
 *
 *    Provides macros to handle obtaining the name of the calling function,
 *    and to provide for a consistent call format for the
 *    "errorlog::errprint" family of functions.
 *
 *    The "_func" macros are necessary so that we can call the errprint
 *    functions of the class instance, yet get the caller's function name.
 *    There is no other way to do that, but to make it a macro, so that the
 *    __func__ token can be expanded to the correct function.
 *
 *    And, so that the caller can have a consistent syntax for all these
 *    calls, all of the other "errprint" functions also include macro
 *    wrappers.
 *
 * \gnu
 *    Note the variadic macros (using "..." and __VA_ARGS__ to cover
 *    variable numbers of parameters).  This is feature of GNU C99.
 *
 *//*-------------------------------------------------------------------------*/

#define log_file()                     xpc::mainlog().logfile()
#define log_parse(c, v)                xpc::mainlog().parse(c, v)
#define log_shownothing()              xpc::mainlog().shownothing()
#define log_showerrors()               xpc::mainlog().showerrors()
#define log_showwarnings()             xpc::mainlog().showwarnings()
#define log_showinfo()                 xpc::mainlog().showinfo()
#define log_showall()                  xpc::mainlog().showall()
#define log_showdebug()                xpc::mainlog().showdebug()

#ifdef XPC_NO_ERRORLOG

#define log_errprint(x)                xpc::mainlog().no_op()
#define log_errprintf(x, ...)          xpc::mainlog().no_op()
#define log_errprintex(x, y)           xpc::mainlog().no_op()
#define log_warnprint(x)               xpc::mainlog().no_op()
#define log_warnprintf(x, ...)         xpc::mainlog().no_op()
#define log_warnprintex(x, y)          xpc::mainlog().no_op()
#define log_infoprint(x)               xpc::mainlog().no_op()
#define log_infoprintf(x, ...)         xpc::mainlog().no_op()
#define log_infoprintex(x, y)          xpc::mainlog().no_op()
#define log_strerrprint(x, err)        xpc::mainlog().no_op()
#define log_strerrprint(x, y, err)     xpc::mainlog().no_op()
#define log_strerrnoprint(x)           xpc::mainlog().no_op()
#define log_strerrnoprintex(x, y)      xpc::mainlog().no_op()

#define log_errprint_func(x)           xpc::mainlog().no_op()
#define log_warnprint_func(x)          xpc::mainlog().no_op()
#define log_infoprint_func(x)          xpc::mainlog().no_op()
#define log_strerrprint_func(x, err)   xpc::mainlog().no_op()
#define log_strerrnoprint_func(x)      xpc::mainlog().no_op()

#else

#define log_errprint(x)                xpc::mainlog().errprint(x)
#define log_errprintf(x, ...)          xpc::mainlog().errprintf(x, __VA_ARGS__)
#define log_errprintex(x, y)           xpc::mainlog().errprintex((x), (y))
#define log_warnprint(x)               xpc::mainlog().warnprint(x)
#define log_warnprintf(x, ...)         xpc::mainlog().warnprintf(x, __VA_ARGS__)
#define log_warnprintex(x, y)          xpc::mainlog().warnprintex((x), (y))
#define log_infoprint(x)               xpc::mainlog().infoprint(x)
#define log_infoprintf(x, ...)         xpc::mainlog().infoprintf(x, __VA_ARGS__)
#define log_infoprintml(x, ...)        xpc::mainlog().infoprintfml(x, __VA_ARGS__)
#define log_infoprintex(x, y)          xpc::mainlog().infoprintex((x), (y))
#define log_strerrprint(x, err)        xpc::mainlog().strerrprint((x), (err))
#define log_strerrprintex(x, y, err)   xpc::mainlog().strerrprintex((x), (y), (err))
#define log_strerrnoprint(x)           xpc::mainlog().strerrnoprint(x)
#define log_strerrnoprintex(x, y)      xpc::mainlog().strerrnoprintex((x), (y))

#define log_errprint_func(x)           xpc::mainlog().errprintex((x), __func__)
#define log_warnprint_func(x)          xpc::mainlog().warnprintex((x), __func__)
#define log_infoprint_func(x)          xpc::mainlog().infoprintex((x), __func__)
#define log_strerrprint_func(x, err)   xpc::mainlog().strerrprintex((x), __func__, err)
#define log_strerrnoprint_func(x)      xpc::mainlog().strerrnoprintex((x), __func__)

#endif   // XPC_NO_ERRORLOG

namespace xpc
{

/******************************************************************************
 * class errorlog
 *------------------------------------------------------------------------*//**
 *
 *    Provides an object that holds error-logging characteristics and
 *    provides functions to write date to error and information logs.
 *
 *    This class is meant to act a lot like the global error-logging C
 *    functions in the XPC C library.
 *
 *    A hidden xpc::errorlog object, accessed by xpc::mainlog(), can be used
 *    in lieu of the global facility.  The caller can also create and
 *    maintain its own xpc::errorlog object to allow diverting log
 *    information to different places, with different options.
 *
 *//*-------------------------------------------------------------------------*/

class errorlog
{

private:                               /* static members    */

   /**
    *    Provides a critical section for synchronizing output for all instances
    *    or xpc::errorlog.
    *
    *    These functions manage the process-global critical sections used for
    *    speeding up the creation of the threads in an application.
    *
    *    The xpc_synchusage_set() function turns the usage of the synchronization
    *    critex on or off.  If it turns it on, the first time this is done, the
    *    synch_critex_destroy() function is registered as an exit routine.
    *    This avoids a compiler warning, and is the only way we can guarantee
    *    the critex is destroyed without making the developer remember to do
    *    it.
    */

   static xpc_syncher_t gm_critex;

   /**
    *    An initialization flag.
    *
    *    This flag helps avoid performing the initialization twice, but also is
    *    used to make sure the initialization does occur the first time the
    *    critex is needed.
    */

   static bool gm_critex_inited;

   /**
    *    An initialization flag for the loading of the critex cleanup routine
    *    that is called by atexit().
    */

   static bool gm_critex_atexit_set;

private:                               /* non-static members   */

   /**
    *    This flag indicates if the text-synchronization critex is to be used.
    *
    *    It is controlled by the --synch and --no-synch options from the
    *    command-line.
    *
    *    If set to 'true', the gm_critex structrue is set up and used to help
    *    keep the text output from different threads from being intermingled.
    *    But please note that it can affect the detectability of threading
    *    problems.
    *
    *    This value is normally false, because it can affect the running of
    *    multi-threaded programs.
    */

   bool m_critex_usage;

   /**
    *    This variable maintains the current error level value.  By
    *    default, the value is XPC_ERROR_LEVEL_ERRORS.
    */

   xpc_errlevel_t m_error_level;

   /**
    *    This variable maintains the pointer to the current log file.
    *
    *    By default, this variable is set to stderr when the object is
    *    constructed.
    */

   FILE * m_log_file;

   /**
    *    Provides a flag to enable the inclusion of microsecond-level time
    *    stamps in the errorlog output.
    */

   bool m_use_timestamps;

   /**
    *    Provides a base time to subtract from the current time in seconds,
    *    to reduce the values, which makes them easier to understand.
    */

   time_t m_timestamps_base;

   /**
    *    Holds the state of the --syslog option.
    *
    *    This item is accessed for private usage, although it can be set from
    *    the command line or by using the xpc_syslogging_set() function.
    */

   bool m_log_to_syslog;

   /**
    *    This color flag is private to the error logging module.  The caller
    *    can access it through the usecolor() accessor functions.
    */

   bool m_use_color;

public:

   errorlog ();
   errorlog (int argc, char * argv []);

   bool synchusage (bool flag);

   /**
    *
    *    Obtains the current value of m_critex_usage.
    *
    *    This function is useful in unit-testing to save the current value for
    *    later restoration.
    *
    * \unittests
    *    -  errorlog_test_02_01() TBD
    *    -  errorlog_test_05_01()
    *
    */

   bool synchusage () const
   {
      return m_critex_usage;
   }

   /*
    * Log-file functions
    */

   bool logfile (FILE * logfilehandle);
   bool open_logfile (const std::string & logfilename);
   bool append_logfile (const std::string & logfilename);
   bool close_logfile ();

   /**
    *    Provides the handle to the error-log file.
    *
    *    This function can be used in fprintf() and similar calls to direct
    *    output to the current log file (\e stderr by default).
    *
    * \return
    *    The private handle to the log file, m_log_file, is returned.
    *
    * \unittests
    *    -  errorlog_test_02_03() TBD
    */

   FILE * logfile () const
   {
      return m_log_file;
   }

   /**
    *    Provides a setter for the internal m_use_timestamps flag.
    *
    * \param flag
    *    The value to which to set the m_use_timestamps flag.
    *
    * \param setbase
    *    If true, the base time in seconds is logged, and then subtracted
    *    from every output of the time-stamp, to make the output timing
    *    easier to understand.  The m_timestamps_base member is set to the
    *    current system time in seconds.
    *
    * \return
    *    Returns the value of m_use_timestamps, which is either 'true' or
    *    'false'.  By default, at program start, this value is false.
    *
    * \unittests
    *    -  errorlog_test_02_05() TBD
    */

   void timestamps (bool flag, bool setbase = false)
   {
      m_use_timestamps = flag;
      if (flag && setbase)
      {
         struct timeval ts;
         xpc_get_microseconds(&ts);
         m_timestamps_base = ts.tv_sec;
      }
   }

   /**
    *    Provides a getter for the internal m_use_timestamps flag.
    *
    * \return
    *    Returns the value of m_use_timestamps, which is either 'true' or
    *    'false'.  By default, at program start, this value is false.
    *
    * \unittests
    *    -  errorlog_test_02_05() TBD
    */

   bool timestamps () const
   {
      return m_use_timestamps;
   }

   bool usecolor (bool flag);
   bool usecolor () const;

   bool errlevel (xpc_errlevel_t errlevel);

   /**
    *
    *    Provides the current error-level code to the caller.
    *
    *    This function makes sure that initialization has taken place, and
    *    then it returns the current error level.
    *
    *    xpc_errlevel() is useful in unit testing, where the test is meant
    *    to generate an erroneous result, but the function shows the error
    *    privately, potentially confusing the user who is viewing the
    *    output.  Call xpc_errlevel(), save its result, do the operations,
    *    then call xpc_errlevel_set() with the saved value, to restore it.
    *
    * \return
    *    The private error-level code, m_error_level, is returned.
    *
    * \unittests
    *    -  errortest_01_01().  This test is a "smoke test".
    *
    */

   xpc_errlevel_t errlevel () const
   {
      return m_error_level;
   }

   bool parse_leveltoken (const std::string & errlevelstring);
   bool parse (int argc, char * argv []);
   void showerrlevel () const;

   /**
    *    Determines if the caller should not show any console messages of
    *    any kind.
    *
    *    This error-level is set from the command line using the "--quiet"
    *    option switch.  The XPCCUT (unit-test) library's --silent option
    *    also sets this setting.
    *
    *    Only output done though the functions in the errorlog module is
    *    amenable to the application of the error-level code.  The caller is
    *    perfectly free to print messages without regard to the error-level.
    *
    *    This function is the first of a family of error-level checking
    *    functions.  This small family of functions allows the caller to see
    *    if showing a message is permitted, based on the current error
    *    level.
    *
    * \return
    *    Returns 'true' if the error level is XPC_ERROR_LEVEL_NONE.
    *
    * \unittests
    *    -  errortest_02_13()
    */

   bool shownothing () const
   {
      return errlevel() == XPC_ERROR_LEVEL_NONE;
   }

   /**
    *    Determines if the caller should show an error message.
    *
    *    An error message is one that causes some kind of inability to
    *    function.  Error messages are tagged by a "?" (question mark)
    *    character.
    *
    *    Error messages can be shown conditionally by using the "errprint"
    *    family of functions [errprint(), errprintex(), errprint_func(),
    *    which is a macro, and errprintf()].
    *
    *    If these functions are not sufficient, or not desired, then
    *    showerrors() can be tested to decide on some other kind of
    *    error-display mechanism.
    *
    *    Error messages are allowed by default.  The user can also supply
    *    the "--error" option switch on the command line.
    *
    * \return
    *    Returns 'true' if the error level is XPC_ERROR_LEVEL_ERRORS, so
    *    that errors are allowed to be shown.
    *
    * \unittests
    *    -  errortest_02_13()
    */

   bool showerrors () const
   {
      return errlevel() >= XPC_ERROR_LEVEL_ERRORS;
   }

   /**
    *    Determines if the caller should show a warning message.
    *
    *    A warning message is one that indicates an unexpected condition
    *    that is not erroneous.  Or it indicates a condition to which
    *    special attention should be paid by the user.  Warning messages are
    *    tagged by a "!" (exclamation point) character.
    *
    *    Warning messages can be shown conditionally by using the
    *    "warnprint" family of functions [warnprint(), warnprintex(),
    *    warnprint_func(), which is a macro, and warnprintf()].
    *
    *    If these functions are not sufficient, or not desired, then
    *    showwarnings () can be tested to decide on some other kind of
    *    warning-display mechanism.
    *
    *    The user can supply the "--warn" option switch on the command line
    *    in order to enable the display of warnings.  The display of
    *    warnings is off by default.
    *
    * \return
    *    'true' if warnings and errors are allowed to be shown.
    *
    * \unittests
    *    -  errortest_02_13()
    *
    */

   bool showwarnings () const
   {
      return errlevel() >= XPC_ERROR_LEVEL_WARNINGS;
   }

   /**
    *    Determines if the caller should show an informational message.
    *
    *    An informational message is one that a user normally does not need
    *    to see.  However, if the user wants to verify the progress of the
    *    application more closely without debugging, the user can provide
    *    the "--info" switch on the command-line, and more messages will be
    *    shown.  Informational messages are tagged with an "*" (asterisk).
    *
    *    Info messages can be shown conditionally by using the "infoprint"
    *    family of functions [infoprint(), infoprintex(), infoprint_func(),
    *    which is a macro, and infoprintf()].
    *
    *    If these functions are not sufficient, or not desired, then
    *    showinfo() can be tested to decide on some other kind of
    *    information-display mechanism.
    *
    *    The user can supply the "--info" option switch on the command line
    *    in order to enable the display of info.  The display of info is off
    *    by default.
    *
    * \return
    *    'true' if information, warnings, and errors can be shown.
    *
    * \unittests
    *    -  errortest_02_13()
    */

   bool showinfo () const
   {
      return errlevel() >= XPC_ERROR_LEVEL_INFO;
   }

   /**
    *    Determines if the caller can show all messages.
    *
    *    The only ways to show such messages are to use print() [no other
    *    forms of this function are provided] or to wrap more conventional
    *    print functions [e.g. fprintf()] in a test of showall().
    *
    *    The option switches "--verbose" or "--all" enable the showing of
    *    all messages.
    *
    * \return
    *    'true' if all messages and tested output can be shown.
    *
    * \unittests
    *    -  errortest_02_13()
    */

   bool showall () const
   {
      return errlevel() >= XPC_ERROR_LEVEL_ALL;
   }

   /**
    *    Determines if the caller can show debug messages.
    *
    *    The option switch "--debug" enables this test, if the application is
    *    compiled for debugging.  Otherwise, the option is equivalent to
    *    "--error".
    *
    * \return
    *    If compiled for debugging, this function acts like showinfo().
    *    Otherwise, it always returns false.
    *
    * \unittests
    *    -  errortest_02_13()
    */

   bool showdebug () const
   {
#if defined DEBUG && ! defined XPC_NO_ERRORLOG
      return showinfo();
#else
      return false;
#endif
   }

   void showerr_version () const;
   bool syslogging (bool flag);
   bool syslogging () const;

#ifdef XPC_NO_ERRORLOG
   void errprint (const std::string & ) const
   {
      /* do nothing */
   }
#else    //  XPC_NO_ERRORLOG
   void errprint (const std::string & errmsg) const;
#endif   //  XPC_NO_ERRORLOG

   void errprintf (const std::string & fmt, ...) const;
   void errprintex
   (
      const std::string & errmsg,
      const std::string & label
   ) const;
   void warnprint (const std::string & warnmsg) const;
   void warnprintf (const std::string & fmt, ...) const;
   void warnprintex
   (
      const std::string & warnmsg,
      const std::string & label
   ) const;

   void infoprint (const std::string & infomsg) const;
   void infoprintf (const std::string & fmt, ...) const;
   void infoprintml (const std::string & fmt, ...) const;
   void infoprintex
   (
      const std::string & infomsg,
      const std::string & label
   ) const;
   std::string infomark_format (const std::string & fmt) const;

#ifdef DEBUG
   void dbginfoprint (const std::string & infomsg) const;
   void dbginfoprintf (const std::string & fmt, ...) const;
   void print_debug (const std::string & infomsg) const;
   void dbginfoprintex
   (
      const std::string & infomsg,
      const std::string & label
   ) const;
#else
   void dbginfoprint (const std::string & ) const
   {
      // do nothing
   }
   void dbginfoprintf (const std::string & , ...) const
   {
      // do nothing
   }
   void print_debug (const std::string & ) const
   {
      // do nothing
   }
   void dbginfoprintex (const std::string & , const std::string &) const
   {
      // do nothing
   }
#endif

   void print (const std::string & infomsg) const;
   int strerrnoprint (const std::string & errmsg) const;
   int strerrnoprintex
   (
      const std::string & errmsg,
      const std::string & label
   ) const;
   void strerrprint (const std::string & errmsg, int errnum) const;
   void strerrprintex
   (
      const std::string & errmsg,
      const std::string & label,
      int errnum
   ) const;

   int getlasterror () const;
   std::string getlasterror_text () const;
   std::string getlasterror_text_ex (int errorcode) const;
   bool buffering (int btype) const;

   void flush_error_log () const;

   /**
    *    Provides a way to disable any kind of logging with no
    *    overhead.
    */

   void no_op () const
   {
      // do nothing
   }

private:

   std::string strerror_wrapper (int errorcode) const;

public:

   static xpc_errlevel_t option_errlevel (const std::string & cmdswitch);
   static std::string errlevel_string (xpc_errlevel_t errlev);
   static int get_priority (xpc_errlevel_t errlev);

private:

   bool parse_nohelp (int argc, char * argv []);
   bool open_logfile_helper
   (
      const std::string & logfilename,
      bool truncateit
   );
   void msgtag
   (
      xpc_errlevel_t errlev,
      const std::string & tag,
      const std::string & errmsg
   ) const;
   void va_tag
   (
      xpc_errlevel_t errlev,
      const std::string & tag,
      const std::string & fmt,
      va_list val
   ) const;

#ifdef DOCUMENT_VA_MSGTAG
   void va_msgtag
   (
      xpc_errlevel_t errlev,
      const std::string & tag,
      const std::string & errmsg,
      va_list val
   ) const;
#endif

   std::string concat_buffer
   (
      const std::string & msg,
      const std::string & label
   ) const;

   void lkprintf (const std::string & fmt, ...) const;

#ifndef XPC_NO_ERRORLOG
   void format_error_string
   (
      const std::string & errmsg,
      const std::string & label,
      const std::string & syserr
   ) const;
#else
   void format_error_string
   (
      const std::string &,
      const std::string &,
      const std::string &
   ) const
   {
      // do nothing
   }
#endif      // XPC_NO_ERRORLOG

private:

   static xpc_syncher_t * synch_critex ();
   static void synch_critex_destroy ();
   static bool synch_lock ();
   static void synch_unlock ();

};             /* class errorlog    */

/******************************************************************************
 * errorlog()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an ever-present xpc::errorlog object that can be used
 *    in lieu of the global C error-logging functions.
 *
 *//*-------------------------------------------------------------------------*/

extern errorlog & mainlog ();

#endif         /* XPC_ERRORLOG_HPP  */

}              /* namespace xpc     */

/******************************************************************************
 * errorlog.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
