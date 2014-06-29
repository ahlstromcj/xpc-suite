/******************************************************************************
 * errorlog.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlog.cpp
 * \library       xpc++
 * \author        Chris Ahlstrom
 * \updates       2010-09-11 to 2011-11-26
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Also see the errorlogging.c module in the XPC (C) library.  This
 *    class is essential a duplicate of that module, with as much of the
 *    extra protection and simplicity of C++ added as possible.
 *
 *    There are some settings that don't make much sense to make per-object
 *    settings, or for other reasons:
 *
 *       -  Buffering method of standard output.
 *       -  Direct access to the "marker" functions.  (Obviated by the
 *          "printml" and "printf" functions.)
 *
 *    For these settings, the caller should use the C functions (the ones
 *    whose names start with "xpc_".)
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlog_macros.h>       /* macros from the XPC library         */
#include <xpc/errorlogging.h>          /* XPC C versions of error functions   */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/portable.h>              /* xpc_get_microseconds()              */
#include <xpc/syncher.h>               /* xpc_syncher_t structure             */
#include <xpc/stringmap.hpp>           /* xpc::iequal() function              */
#include <xpc/errorlog.hpp>            /* xpc::errorlog class                 */
XPC_REVISION(errorlog)

#if XPC_HAVE_STDARG_H
#include <stdarg.h>                    /* Win32 too                           */
#else
#include <varargs.h>                   /* old, obsolete, deprecated           */
#endif

#if XPC_HAVE_STDDEF_H
#include <stddef.h>                    /* size_t                              */
#endif

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* snprintf(), _snprintr()             */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* strlen(), strcpy(), strcat()        */
#endif

#if XPC_HAVE_SYSLOG_H
#include <syslog.h>                    /* syslog() and related constants      */
#endif

#if XPC_HAVE_ERRNO_H
#include <errno.h>                     /* thread-safe global C error variable */
#endif

#if XPC_HAVE_UNISTD_H
#include <unistd.h>                    /* here, it defines isatty()           */
#endif

#define CMDSTRING(x)                   std::string("--" x)

/******************************************************************************
 * Internal __func__ macros
 *------------------------------------------------------------------------*//**
 *
 *    These macros are necessary so that we can call the errprint functions
 *    of the class instance, yet get the caller's function name.
 *
 *    There is no other way to do it, but to make it a macro.  Note that
 *    these versions are for internal usage only.  A different style, needed
 *    for usage by callers of xpc::errorlog callers, is shown in the
 *    errorlog.hpp header file.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NO_ERRORLOG

#define ERRPRINT_FUNC(x)               this->no_op()
#define WARNPRINT_FUNC(x)              this->no_op()
#define INFOPRINT_FUNC(x)              this->no_op()
#define STRERRPRINT_FUNC(x, err)       this->no_op()
#define STRERRNOPRINT_FUNC(x)          this->no_op()

#else

#define ERRPRINT_FUNC(x)               this->errprintex((x), __func__)
#define WARNPRINT_FUNC(x)              this->warnprintex((x), __func__)
#define INFOPRINT_FUNC(x)              this->infoprintex((x), __func__)
#define STRERRPRINT_FUNC(x, err)       this->strerrprintex((x), __func__, err)
#define STRERRNOPRINT_FUNC(x)          this->strerrnoprintex((x), __func__)

#endif   // XPC_NO_ERRORLOG

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    If Doxygen is running, define WIN32, to read in the documentation for
 *    the Windows-specific code.  See doc/doxygen.cfg for more
 *    information.
 *
 *    Some other code needs to be enabled, too.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define WIN32
#define DEBUG
#define XPC_USE_COLORS
#define XPC_NO_ERRORLOG
#define DOCUMENT_VA_MSGTAG
#endif

namespace xpc
{

/******************************************************************************
 * errorlog()
 *-----------------------------------------------------------------------------
 *
 *    Provides an ever-present xpc::errorlog object that can be used
 *    in lieu of the global C error-logging functions.
 *
 *----------------------------------------------------------------------------*/

errorlog & mainlog ()
{
   static errorlog ErrorLog;
   return ErrorLog;
}

/******************************************************************************
 * Static errorlog members
 *----------------------------------------------------------------------------*/

xpc_syncher_t errorlog::gm_critex;
bool errorlog::gm_critex_inited = false;
bool errorlog::gm_critex_atexit_set = false;

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 * \defaultctor
 *
 *//*-------------------------------------------------------------------------*/

errorlog::errorlog ()
 :
   m_critex_usage    (false),
#ifdef XPC_NO_ERRORLOG
   m_error_level     (XPC_ERROR_LEVEL_NONE),
#else
   m_error_level     (XPC_ERROR_LEVEL_ERRORS),
#endif
   m_log_file        (stderr),
   m_use_timestamps  (false),
   m_timestamps_base (0),
   m_log_to_syslog   (false),
#ifdef XPC_USE_COLORS
   m_use_color       (true)
#else
   m_use_color       (false)
#endif
{
   // Nothing else to do.
}

/******************************************************************************
 * Principle constructor
 *------------------------------------------------------------------------*//**
 *
 *    Creates an error-logging object and sets it up according to
 *    command-line arguments.
 *
 * \param argc
 *    The main()-style number of command-line arguments.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 *//*-------------------------------------------------------------------------*/

errorlog::errorlog (int argc, char * argv [])
 :
   m_critex_usage    (false),
#ifdef XPC_NO_ERRORLOG
   m_error_level     (XPC_ERROR_LEVEL_NONE),
#else
   m_error_level     (XPC_ERROR_LEVEL_ERRORS),
#endif
   m_log_file        (stderr),
   m_use_timestamps  (false),
   m_timestamps_base (0),
   m_log_to_syslog   (false),
#ifdef XPC_USE_COLORS
   m_use_color       (true)
#else
   m_use_color       (false)
#endif
{
   (void) parse(argc, argv);
}

/******************************************************************************
 * synch_critex() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Provides a critical section for optional synchronization of error-log
 *    messages.
 *
 *    This function initializes the private critical section (critex) and
 *    then returns a pointer to it.  This critex is used if the application
 *    specifies the "--synch" command-line option.
 *
 * \private
 *    A static member function.
 *
 * \return
 *    A pointer to the private errorlog critex.  If it could not be
 *    initialized, then a null pointer is returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

xpc_syncher_t *
errorlog::synch_critex ()
{
   xpc_syncher_t * result = &gm_critex;
   if (! gm_critex_inited)
   {
      if (xpc_syncher_create(&gm_critex, false))            /* not recursive  */
         gm_critex_inited = true;
      else
         result = nullptr;
   }
   return result;
}

/******************************************************************************
 * synch_critex_destroy() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Clears the resources allocated to the error-log's synchronization
 *    critex.
 *
 * \private
 *    This function is a static C function.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::synch_critex_destroy ()
{
   if (gm_critex_inited)
   {
      (void) xpc_syncher_destroy(&gm_critex);               // C function
      gm_critex_inited = false;
      xpc_infoprint(_("error-logging critex destroyed"));   // C function
   }
}

/******************************************************************************
 * synch_lock() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Locks the error log, using the private critex.
 *
 *    This function is used if activated by the "--synch" command-line
 *    option.  Its effect is to make the output much less garbled when
 *    multithreading is in force.
 *
 * \private
 *    A static member function.
 *
 * \return
 *    Returns 'true' if the x_syncher_t object is available, and the
 *    xpc_syncher_enter() call succeeded.  Also returns 'true' if the usage
 *    of locking is not set, so that the caller can go ahead and execute the
 *    locked code.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::synch_lock ()
{
   bool result = false;
   if (gm_critex_inited)
      result = xpc_syncher_enter(synch_critex()) != 0;           // C function

   return result;
}

/******************************************************************************
 * synch_unlock() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Flushes, and then Unlocks the output log.
 *
 *    Used only if the output log synchonization option is in force.
 *
 * \private
 *    A static member function.
 *
 * \unittests
 *    -  TBD
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::synch_unlock ()
{
   if (gm_critex_inited)
      (void) xpc_syncher_leave(synch_critex());
}

/******************************************************************************
 * synchusage()
 *------------------------------------------------------------------------*//**
 *
 *    Turns the output log synchronization option on or off.
 *
 *    This function is activated by the "--synch" command-line option.  Its
 *    effect is to make the output much less garbled when multithreading is
 *    in force.
 *
 *    The private variable controlling this usage is m_critex_usage.  The
 *    critex value used is gm_critex.
 *
 *    This function also makes sure that synch_critex_destroy() is
 *    registered as a C atexit() handler.
 *
 * \warning
 *    -  Be aware that this synchronization of output might alter the
 *       behavior of your multithreaded programs.
 *    -  Do not call this function while actually using the synch_lock() and
 *       synch_unlock() functions.  There is currently no protection against
 *       misusing this function.
 *
 * \param flag
 *    The value to which to set the synch-usage flag.
 *
 * \return
 *    Returns 'true' if the atexit() handler was set up correctly.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  errortest_02_01()  TBD
 *    -  errortest_05_01()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::synchusage (bool flag)
{
   bool result = true;
   m_critex_usage = flag;
   if (flag)
      warnprint(_("output synchronization enabled"));
   else
      warnprint(_("output synchronization disabled"));

   if (! gm_critex_atexit_set)
   {
      int rcode = atexit(synch_critex_destroy);
      if (is_posix_success(rcode))
      {
         gm_critex_atexit_set = true;
         infoprintex("atexit(synch_critex_destroy)", _("succeeded"));
      }
      else
      {
         errprintex("atexit(synch_critex_destroy)", _("failed"));
         result = false;
      }
   }
   return result;
}

/******************************************************************************
 * usecolor() [POSIX]
 *------------------------------------------------------------------------*//**
 *
 *    Turns the usage of console colors on or off.
 *
 *    See the xpc_usecolor_set() function in the XPC C library for more
 *    explanation.
 *
 * \return
 *    Returns 'true' if the compiler settings allow the setting of color to
 *    be performed.
 *
 * \param flag
 *    True == colors on, false == colors off.
 *
 * \unittests
 *    -  errortest_02_02() TBD
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::usecolor (bool flag)
{
#ifdef XPC_USE_COLORS
   m_use_color = flag;
   return true;
#else
   if (flag)
   {
      ERRPRINT_FUNC(_("console color support not supported"));
      return false;
   }
   else
   {
      m_use_color = false;
      return true;
   }
#endif                                                   /* USE_XPC_COLOR     */
}

/******************************************************************************
 * usecolor() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Returns the status of color usage in the output.
 *
 *    This status is normally on by default for UNIXen, and off for Windows.
 *    It is also disabled if the log file is an actual file, rather than
 *    stdout or stderr.
 *
 * \posix
 *    If isatty() is false, then m_use_color is turned off, since it's
 *    nice to not have escape sequences in output that is redirected to a
 *    file.
 *
 *    Would someone ever want to capture the control characters?  Yes, but
 *    too bad for now.
 *
 * \return
 *    The private boolean value for the usage of color.
 *
 * \unittests
 *    -  errortest_02_02()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::usecolor () const
{

#if XPC_HAVE_UNISTD_H
   if (m_use_color)
   {
      FILE * logfile = xpc_logfile();        /* get the current file pointer  */
      if (! isatty(fileno(logfile)))         /* is it a descriptor for a TTY? */
      {
         errorlog * ncthis = const_cast<errorlog *>(this);
         ncthis->m_use_color = false;        /* no, so don't allow color      */
      }
   }
#endif

   return m_use_color;
}

/******************************************************************************
 * logfile()
 *------------------------------------------------------------------------*//**
 *
 *    This variable/function group supports the changing of the log file.
 *    The default log file is stderr.  A different log-file can be opened,
 *    which automatically closes the previous one (except for stderr and
 *    stdout [stdin is also protected, what the heck.])
 *
 *    When the log file is closed, it automatically reverts to the default,
 *    stderr.  Right now, we don't deal with a failure to close the previous
 *    file.
 *
 *    Note that enabling a log file will turn off the usage of color (to
 *    avoid ugly ESC-sequences in the file), but then the usage of color
 *    cannot be turned back on. [Note:  now we turn it back on.  Too
 *    confusing otherwise.]
 *
 * \posix
 *    According to a comment in stdio.h, C89/99 considers stderr etc to be
 *    macros (GNU stdio say's their pointers to _IO_FILE structures), so we
 *    cannot make an assignment here.
 *
 * \param logfilehandle
 *    The handle to be used as the error-log file.
 *
 * \return
 *    Returns 'true' if the \e logfile parameter is valid (that is, not
 *    null).
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 * \todo
 *    Replace the old-style C FILE pointer with C++ I/O streams.
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::logfile (FILE * logfilehandle)
{
   bool result = false;
   if (not_NULL(logfilehandle))
   {
      if
      (
         m_use_color && ((logfilehandle != stdout) && (logfilehandle != stderr))
      )
      {
         (void) usecolor(false);
      }
      m_log_file = logfilehandle;
      result = true;
   }
   return result;
}

/******************************************************************************
 * open_logfile_helper()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for usage as the error-log, for either truncating it or
 *    appending to it.
 *
 *    This function is a private (C static) helper function for
 *    open_logfile() and xpc_append_logfile().
 *
 * \param logfilename
 *    Full path name to log file to be opened.
 *
 * \param truncateit
 *    If true, truncate it, else append to it.
 *
 * \return
 *    If the parameters were appropriate, and the file was able to be opened
 *    for appending, 'true' is returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function; see
 *    open_logfile().
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::open_logfile_helper
(
   const std::string & logfilename,
   bool truncateit
)
{
   cbool_t result = false;
   if (logfilename.size() > 0)
   {
      FILE * lf;
      (void) close_logfile();
      lf = fopen(logfilename.c_str(), truncateit ? "w+" : "a");
      if (not_NULL(lf))
      {
         if (truncateit)
            infoprint(_("log-file truncated"));

         result = logfile(lf);
      }
      else
      {
         STRERRNOPRINT_FUNC(_("failed"));
         (void) logfile(stderr);
      }
   }
   else
      ERRPRINT_FUNC(_("invalid filename"));

   return result != 0;
}

/******************************************************************************
 * open_logfile()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for usage as the error-log destination, truncating it if
 *    it exists.
 *
 *    The default destination is stderr.  But, if the user specifies the
 *    "--log filename" option on the command line, then the specified file
 *    is used for logging the error and information strings.
 *
 *    If the file-name is 'stderr' or 'stdout', then that device is simply
 *    set as the current log-file.
 *
 * \param logfilename
 *    Full path name to log file to be opened.
 *
 * \return
 *    If the parameters were appropriate, and the file was able to be opened
 *    for appending, 'true' is returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  errortest_02_03() TBD
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::open_logfile (const std::string & logfilename)
{
   bool result = true;
   FILE * stdio_file = NULLptr;
   bool match = xpc::iequal(logfilename, "stderr");
   if (match)
      stdio_file = stderr;
   else
   {
      match = xpc::iequal(logfilename, "stdout");
      if (match)
         stdio_file = stdout;
      else
      {
         match = xpc::iequal(logfilename, "stdin");
         if (match)
         {
            result = false;
            if (! shownothing())
            {
               fprintf
               (
                  stderr, "??? %s!!!\n", _("Cannot use stdin as a log file")
               );
            }
         }
      }
   }
   if (result)
   {
      if (not_NULL(stdio_file))
      {
         (void) close_logfile();                      /* todo:  use return?   */
         result = logfile(stdio_file);
      }
      else
         result = open_logfile_helper(logfilename, true);  /* truncate */
   }
   return result;
}

/******************************************************************************
 * append_logfile()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for usage as the error-log destination, for appending.
 *
 *    The default destination is stderr.  But, if the user specifies the
 *    "--log filename" option on the command line, then the specified file
 *    is used for logging the error and information strings.
 *
 * \param logfilename
 *    Full path name to log file to be opened.
 *
 * \return
 *    If the parameters were appropriate, and the file was able to be opened
 *    for appending, 'true' is returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  errortest_02_03() TBD
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::append_logfile (const std::string & logfilename)
{
   return open_logfile_helper(logfilename, false);    /* append to log file   */
}

/******************************************************************************
 * close_logfile()
 *------------------------------------------------------------------------*//**
 *
 *    Closes the current log file for the error-log.
 *
 *    If the log file is not one of the standard ones (<i>stderr</i>,
 *    <i>stdout</i>, or <i>stdin</i>), it is closed.
 *
 * \sideeffect
 *    Whether or not the closing succeeds, the log-file is returned to the
 *    handle it had at the start of the application -- \e stderr.
 *
 * \return
 *    If the file was not one of the standard ones, and it was successfully
 *    closed, 'true' is returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  errortest_02_03()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::close_logfile ()
{
   bool result = false;
   FILE * lf = logfile();
   bool ok = (lf != stderr) && (lf != stdout) && (lf != stdin);
   if (ok && not_NULL(lf))
   {
      int rcode = fclose(lf);
      result = (rcode == 0);
      (void) usecolor(true);        /* do not use the return value here */
   }
   (void) logfile(stderr);
   return result;
}

/******************************************************************************
 * errlevel()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the current error-level for the application.
 *
 *    The error-level is selectable via the following command-line options.
 *
 *       -  --quiet or --silent (the latter matches a unit-test option)
 *       -  --errors [the default]
 *       -  --warnings
 *       -  --info
 *       -  --verbose
 *
 *    This function checks the parameter, and, if valid, it copies it into
 *    the global error-level variable.
 *
 *    The values that are allowed are defined in the errorlog.h header
 *    file.
 *
 *    If the code is configured with the --disable-errorlog option, then
 *    XPC_NO_ERRORLOG is true, and this function will merely show a warning
 *    and hardwire the error-level to XPC_ERROR_LEVEL_NONE.
 *
 * \param errlevel
 *    The desired error level for the whole application.
 *
 * \return
 *    'false' is returned for bad error-level values.  Otherwise, 'true' is
 *    returned.
 *
 * \todo
 *    Change it to return the original error level, or, if the input is bad,
 *    do not change the level, but return a bad level code.
 *
 * \unittests
 *    -  errortest_02_06()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::errlevel (xpc_errlevel_t errlevel)
{

#ifdef XPC_NO_ERRORLOG

   fprintf
   (
      logfile(), "%s %s",
      usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN,
      _("this application is configured to disable error-logging")
   );
   errlevel = int(XPC_ERROR_LEVEL_NONE);

#endif

   bool result = (errlevel < XPC_ERROR_LEVEL_NOT_APPLICABLE);
   if (result)
      result = (errlevel >= XPC_ERROR_LEVEL_NONE);

   if (result)
      m_error_level = xpc_errlevel_t(errlevel);    /* conversion from int     */

   return result;
}

/******************************************************************************
 * parse_leveltoken()
 *------------------------------------------------------------------------*//**
 *
 *    Converts the string-name to an error-level and sets that error-level.
 *
 *    This function converts a string value ("error", "warnings", etc.) into
 *    the proper error-level, and then sets the error-level appropriately.
 *
 * \param errlevelstring
 *    "error", "info", etc. to check for.
 *
 * \return
 *    It returns 'true' if the setting was valid.  A null error-level string
 *    is not valid.
 *
 * \unittests
 *    -  errortest_02_07() TBD
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::parse_leveltoken (const std::string & errlevelstring)
{
   bool result = false;
   xpc_errlevel_t level;
   std::string temp;
   if (errlevelstring[0] != '-')
   {
      temp = "--";
      temp += errlevelstring;
   }
   else
      temp = errlevelstring;

   level = option_errlevel(temp);
   if (level != XPC_ERROR_LEVEL_NOT_APPLICABLE)
   {
      result = errlevel(level);
      if (! result)
         errprintex("errlevel(setter)", _("failed"));
   }
   else
      errprintex(temp, _("failed"));

   return result;
}

/******************************************************************************
 * parse_nohelp()
 *------------------------------------------------------------------------*//**
 *
 *    Parses a command-line argument array for error-level arguments, but
 *    ignores "--help".
 *
 *    This function is the same as parse(), but it ignores "--help".  It
 *    should be used in lieu of parse() by callers that want to create their
 *    own help.
 *
 *    Note that this version does check for the --help switching, returning
 *    false if it is encountered.  This function also returns false upon an
 *    error.  But, otherwise, it has to scan every argument, in case
 *    multiple error-logging options were specified.
 *
 *    This function is called by parse() to promote code reuse.
 *
 * \param argc
 *    The main()-style number of command-line arguments.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 * \return
 *    Returns 'true' if the argument array was valid, and the function
 *    succeeded in finding a proper error-logging command-line option.
 *    'false' is returned if the parameters were incorrect, or if the
 *    "--help" or "--version" tokens were encountered, if argc is bogus (0),
 *    or if some other error occurred.
 *
 *    Please note that an option (e.g. "--my-app-option") that is unknown to
 *    the error-logging is simply ignored, and 'true' is returned, so that
 *    the rest of the application processing can proceed.
 *
 * \unittests
 *    errortest_02_09().  This test does not attempt to handle all
 *    of the command-line options, due to the large-number of side-effects
 *    that would have to be guarded against during the test.  Nonetheless, a
 *    lot of other internal properties do get tested.
 *
 *    For tests of the functions called inside this function, see the
 *    unit-tests for:
 *
 *       -  errlevel()
 *       -  open_logfile()
 *       -  syslogging()
 *       -  usecolor()
 *       -  timestamps()
 *       -  buffering()
 *       -  synchusage()
 *       -  showerr_version()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::parse_nohelp (int argc, char * argv [])
{
   bool result = false;
   if (not_nullptr(argv))
   {
      if (argc > 1)
      {
         int argi = 0;
         while (++argi < argc)
         {
            std::string arg = argv[argi];
            xpc_errlevel_t level = option_errlevel(argv[argi]);
            if (level != XPC_ERROR_LEVEL_NOT_APPLICABLE)
            {
               result = errlevel(level);
            }
            else if (arg == CMDSTRING(_STDOUT))
            {
               result = logfile(stdout);
            }
            else if (arg == CMDSTRING(_STDERR))
            {
               result = logfile(stderr);
            }
            else if (arg == CMDSTRING(_LOG))
            {
               if ((argi+1) < argc)
               {
                  result = xpc_filename_check(argv[argi+1]) != 0;      // C
                  if (result)
                     result = open_logfile(argv[argi+1]);
                  else
                     break;            /* don't bother checking any further   */
               }
               else
               {
                  ERRPRINT_FUNC(_("--log requires a filename"));
                  result = false;
                  break;               /* don't bother checking any further   */
               }
            }
            else if (arg == CMDSTRING(_APPEND))
            {
               if ((argi+1) < argc)
               {
                  result = xpc_filename_check(argv[argi+1]) != 0;      // C
                  if (result)
                     result = append_logfile(argv[argi+1]);
                  else
                     break;            /* don't bother checking any further   */
               }
               else
               {
                  ERRPRINT_FUNC(_("--append requires a filename"));
                  result = false;
                  break;
               }
            }
            else if (arg == CMDSTRING(_SYSLOG))
            {
               infoprint(_("setting system logging"));
               result = syslogging(true);
            }
            else if (arg == CMDSTRING(_NO_SYSLOG))
            {
               result = syslogging(false);
            }
            else if (arg == CMDSTRING(_COLOR))
            {
               /*
                * We used to do this to check that the log file is a TTY.
                * However, this logic is better off in usecolor().
                */

               FILE * temp = logfile();
               if ((temp == stderr) || (temp == stdout))
               {
                  result = usecolor(true);
               }
               else
                  result = true;
            }
            else if (arg == CMDSTRING(_MONO))
            {
               result = usecolor(false);
            }
            else if (arg == CMDSTRING(_NC))
            {
               result = usecolor(false);
            }
            else if (arg == CMDSTRING(_NOCOLOR))
            {
               result = usecolor(false);
            }
            else if (arg == CMDSTRING(_NO_COLOR))
            {
               result = usecolor(false);
            }
            else if
            (
               arg == CMDSTRING(_TIMESTAMPS) || arg == CMDSTRING(_TIME_STAMPS)
            )
            {
               bool rebase = false;
               bool ok = xpc_filename_check(argv[argi+1]) != 0;      // C
               if (ok)
               {
                  if (std::string(argv[argi+1]) == "rebase")
                     rebase = true;
               }
               timestamps(true, rebase);
            }
            else if
            (
               arg == CMDSTRING(_NO_TIMESTAMPS)
                  ||
               arg == CMDSTRING(_NO_TIME_STAMPS)
            )
            {
               timestamps(false);
            }
            else if (arg == CMDSTRING(_UNBUFFER))
            {
               result = buffering(XPC_ERROR_NOT_BUFFERED);
            }
            else if (arg == CMDSTRING(_BUFFER))
            {
               result = buffering(XPC_ERROR_LINE_BUFFERED);
            }
            else if (arg == CMDSTRING(_UNBUFFERED))
            {
               result = buffering(XPC_ERROR_NOT_BUFFERED);
            }
            else if (arg == CMDSTRING(_BUFFERED))
            {
               result = buffering(XPC_ERROR_LINE_BUFFERED);
            }
            else if (arg == CMDSTRING(_SYNCH))
            {
               result = synchusage(true);
            }
            else if (arg == CMDSTRING(_NO_SYNCH))
            {
               result = synchusage(false);
            }
            else if (arg == CMDSTRING(_VERSION))
            {
               showerr_version();
               result = false;
            }
            else if (arg == CMDSTRING(_HELP))
            {
               result = false;
               break;
            }
            else
            {
               /*
                * Here, we found no match for the given option.  This could
                * be an error, or it could be an application option that is
                * unknown to the error-logging module.  If it is the latter,
                * then we do not want to ignore (skip) the functionality of
                * the application.  So we have to return a value that lets
                * the application proceed.
                */

               result = true;
            }
         }
      }
      else
         result = argc > 0;
   }
   return result;
}

/******************************************************************************
 * parse()
 *------------------------------------------------------------------------*//**
 *
 *    Parses a command-line argument array for error-level arguments.
 *
 *    This function scans through a list of standard C command-line
 *    arguments.  It checks each option string, starting from the last
 *    argument.  If it finds one that is in the legal list of error-level
 *    options, it gets the corresponding error level and sets it.  It does
 *    not exit immediately -- this results in the behavior that the first
 *    error-level option in the list, not the last, is the one that takes
 *    effect.
 *
 * \param argc
 *    The main()-style number of command-line arguments.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 * \return
 *    'true' is returned if parse_nohelp() detected an
 *    error-logging option.  'false' is returned if a bad parameter is
 *    passed into the function, but not if a bad command-line option is
 *    passed in.  The reason is that other subsystems may have their own
 *    options on the command line.  Therefore, we really can't have any
 *    command-line error detection.  'false' is also returned if the
 *    "--help" option is encountered.  This allows the caller to abort.
 *    However, if the caller does not want to abort just because help was
 *    called, he'll have to check for that himself, ignoring the return
 *    value.
 *
 * \todo
 *    Should we also close the logfile here, just in case this function
 *    is called later on in the application beyond startup?
 *
 * \todo
 *    Is there any way to detect if stdout or stderr are being redirected
 *    to a file?
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  errortest_02_10()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::parse (int argc, char * argv [])
{
   bool result = false;
   if (not_nullptr(argv))
   {
      result = parse_nohelp(argc, argv);
      while (argc-- > 1)
      {
         std::string arg = CMDSTRING(_HELP);
         if (argv[argc] == arg)
         {
            xpc_error_help();                   // call the C function
            result = false;
            break;
         }
      }
   }
   return result;
}


/******************************************************************************
 * option_errlevel() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Converts the name of an error-logging option into an error-level
 *    value.
 *
 *    This function converts a valid English option switch into the
 *    appropriate error-level value.  It is assume that the hyphens are part
 *    of the string, as in "--quiet".
 *
 * \param cmdswitch
 *    Error-level option string with hyphens.
 *
 * \return
 *    If the parameter is not null, and the string is a legal name for an
 *    error-level option, the proper XPC_ERROR_LEVEL enumeration value is
 *    returned.  Otherwise, XPC_ERROR_LEVEL_NOT_APPLICABLE is returned.
 *
 *    If the build was a debug build, and the option name is "--debug", the
 *    effect is as if "--info" were the parameter.  If the build was not
 *    debug-enabled, "--debug" maps to "--error".
 *
 * \unittests
 *    -  errortest_02_08()
 *
 *//*-------------------------------------------------------------------------*/

xpc_errlevel_t
errorlog::option_errlevel (const std::string & cmdswitch)
{
   return xpc_option_errlevel(cmdswitch.c_str());
}

/******************************************************************************
 * errlevel_string() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Converts an error-level code into an error-level string, without
 *    hyphens.
 *
 * \param errlev
 *    The error-level code to convert to a string.
 *
 * \return
 *    A string that is the option switch representing the current
 *    error-level, minus the hyphens.
 *
 *    The "debug" string is impossible to return, since that string
 *    represents an "info" error level for a debug-enabled build.
 *
 * \unittests
 *    -  errortest_02_11()
 *
 *//*-------------------------------------------------------------------------*/

std::string
errorlog::errlevel_string (xpc_errlevel_t errlev)
{
   const char * elstring = xpc_errlevel_string(errlev);     // the C function
   return std::string(elstring);
}

/******************************************************************************
 * showerrlevel()
 *------------------------------------------------------------------------*//**
 *
 *    Displays the current debug level as an option string, with hyphens.
 *
 *    The string is obtained by using xpc_errlevel() to get the current
 *    error-level, and then converting this level to a string using
 *    xpc_errlevel_string().
 *
 *    This function shows the level regardless of what it is, except for the
 *    "none" level.  We don't want see anything at "none" level.
 *
 * \unittests
 *    -  errortest_02_12()
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::showerrlevel () const
{
   xpc_errlevel_t level = errlevel();
   if (level != XPC_ERROR_LEVEL_NONE)
   {
      std::string dbgstring = errlevel_string(level);
      fprintf
      (
         logfile(), ERRL_FMT_EXT_MESSAGE,
         usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO,
         _("error level"), dbgstring.c_str()
      );
   }
}

/******************************************************************************
 * showerr_version()
 *------------------------------------------------------------------------*//**
 *
 *    Shows the approximate build time of this module.
 *
 *    In addition to showing the build time, this function will indicate
 *    when the module is compiled for debugging (the DEBUG macro is
 *    defined), or when the XPC_NO_ERRORLOG macro is defined.
 *
 *    This function is not all that useful, but leave it alone, it might
 *    come in handy once in a while.
 *
 * \unittests
 *    -  errortest_02_14()
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::showerr_version () const
{
#ifdef DEBUG
   infoprintf
   (
      "%s: %s %s \n%s, C++.\n",
      _("Approximate build time"), __DATE__,  __TIME__,
      _("Compiled with debugging information.")
   );
#else
   infoprint(_("Release version, C++"));
#endif

#ifdef XPC_NO_ERRORLOG
   infoprint(_("Compiled with disabled error-logging functions."));
#endif
}

/******************************************************************************
 * syslogging_set()
 *------------------------------------------------------------------------*//**
 *
 *    Causes the error-log output to be directed to the system log.
 *
 *    In a daemon, the standard output files are closed.  Therefore, we have
 *    to do one of the following:
 *
 *       -# Disable error-logging.  This option can be performed by calling
 *          the xpc_errlevel_set() function.
 *       -# Redirect error-logging to a text file.  This option can be
 *          performed by calling the open_logfile() function.  The only
 *          issue is what name to give the file.
 *       -# Redirect error-logging to the syslog() facility.  This is
 *          probably the best option.  The error-levels are mapped onto the
 *          following system-log levels.
 *
\verbatim
         Error-logging level           System-logging level
         ===================           ====================

         XPC_ERROR_LEVEL_NONE          not applicable
         not applicable                LOG_EMERG
         not applicable                LOG_ALERT
         not applicable                LOG_CRIT
         XPC_ERROR_LEVEL_ERRORS        LOG_ERR
         XPC_ERROR_LEVEL_WARNINGS      LOG_WARNING
         not applicable                LOG_NOTICE
         XPC_ERROR_LEVEL_INFO          LOG_INFO
         XPC_ERROR_LEVEL_ALL           LOG_INFO
         not applicable                LOG_DEBUG
         XPC_ERROR_LEVEL_NOT_APPL...   not applicable
\endverbatim
 *
 *    The "--syslog" option can also be used, if you should so desire.
 *
 * \param flag
 *    The desired boolean setting for sys-logging.
 *
 * \return
 *    Returns 'true' if the setting succeeded.
 *
 * \unittests
 *    -  errortest_02_15()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::syslogging (bool flag)
{
   m_log_to_syslog = flag;               /* enable this option               */
   return usecolor(false);        /* don't want colors in system log  */
}

/******************************************************************************
 * syslogging()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the current value of m_log_to_syslog.
 *
 *    This function is useful in unit-testing to save the current value for
 *    later restoration.
 *
 * \unittests
 *    -  errortest_02_15()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::syslogging () const
{
   return m_log_to_syslog;
}

/******************************************************************************
 * get_priority() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Converts an error-level code into a UNIX syslog() priority value.
 *
\verbatim
          XPC_ERROR_LEVEL_NONE:           LOG_EMERG
          XPC_ERROR_LEVEL_ERRORS:         LOG_ERR
          XPC_ERROR_LEVEL_WARNINGS:       LOG_WARNING
          XPC_ERROR_LEVEL_INFO:           LOG_INFO
          XPC_ERROR_LEVEL_ALL:            LOG_INFO
          XPC_ERROR_LEVEL_NOT_APPLICABLE: LOG_EMERG
\endverbatim
 *
 * \private
 *    This function is a private helper for the msgtag() and va_tag()
 *    functions.
 *
 * \todo
 *    Support for Windows error priorities.  At present, the error-level
 *    value parameter is simply returned.
 *
 * \param errlev
 *    Error-level code to convert to a string.
 *
 * \return
 *    Returns the POSIX priority level the corresponds to the XPC
 *    error-level provided.  If there is no match, then -1 (POSIX_ERROR) is
 *    returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

int
errorlog::get_priority (xpc_errlevel_t errlev)
{
   return xpc_get_priority(errlev);            // the C function
}

/******************************************************************************
 * msgtag()
 *------------------------------------------------------------------------*//**
 *
 *    Writes one or two strings to the error-log based on the current
 *    error-level setting.
 *
 *    This function provides a formulaic error or warning message.  It can
 *    also support other types of messages that fit the formula given by the
 *    ERRL_FMT_BASIC_MESSAGE macro.
 *
 *    This function is ultimately responsible for all logging output.
 *
 *    At some point, this function can be updated to support gettext() or
 *    some other method of internationalization.
 *
 *    Right now, we use the short tags, which are single characters, and
 *    look much better in a stream of output than do the long tags, which
 *    are words.
 *
 *    The message isn't written if the error level is set to
 *    XPC_ERROR_LEVEL_NONE.
 *
 * \param errlev
 *    The error level of the message.
 *
 * \param tag
 *    A single-character tag string (e.g. "?").
 *
 * \param errmsg
 *    The error or info message to be logged.
 *
 * \todo
 *    Support for Windows Event Log.  For now, only the normal error-log is
 *    used for Win32 code.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::msgtag
(
   xpc_errlevel_t errlev,
   const std::string & tag,
   const std::string & errmsg
) const
{
   if (xpc_errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      if (m_log_to_syslog)
      {
         int priority = get_priority(errlev);

#ifdef POSIX
         syslog
         (
            priority, _(ERRL_FMT_BASIC_MESSAGE), tag.c_str(), errmsg.c_str()
         );
#else
         fprintf
         (
            xpc_logfile(),
            "%s %s (%s %s)\n",                  /* ERRL_FMT_PRIORITY_MESSAGE  */
            tag, errmsg, _("priority"),
            errlevel_string((xpc_errlevel_t) priority)
         );
#endif

      }
      else
      {
         if (! m_critex_usage || synch_lock())  /* --synch              */
         {
            if (timestamps())
            {
               time_t seconds;
               suseconds_t microseconds;
               struct timeval ts;
               xpc_get_microseconds(&ts);
               seconds = ts.tv_sec;
               microseconds = ts.tv_usec;
               if (m_timestamps_base > 0)
                  seconds -= m_timestamps_base;

               fprintf
               (
                  logfile(), ERRL_FMT_TIMESTAMP_MESSAGE, tag.c_str(),
                  (int) seconds, (int) microseconds, errmsg.c_str()
               );
            }
            else
            {
               fprintf
               (
                  logfile(), ERRL_FMT_BASIC_MESSAGE, tag.c_str(), errmsg.c_str()
               );
            }
            fflush(logfile());
            synch_unlock();
         }
      }
   }
}

/******************************************************************************
 * va_tag()
 *------------------------------------------------------------------------*//**
 *
 *    Provides message support for variable numbers of arguments.
 *
 *    This function is relatively new.  It is like msgtag(), but it supports
 *    passing a format and a variable number of arguments.
 *
 * \param errlev
 *    Error level of message, not used in Win32.
 *
 * \param tag
 *    A single-character tag string (e.g. "?").
 *
 * \param fmt
 *    The error/info format message/string.
 *
 * \param val
 *    Variable argument-list construct.
 *
 * \todo
 *    Support for Windows Event Log.  For now, only the normal error-log is
 *    used for Win32 code.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning ( disable : 4100 )     /* Unreferenced formal parameter.      */
#endif

void
errorlog::va_tag
(
   xpc_errlevel_t errlev,
   const std::string & tag,
   const std::string & fmt,
   va_list val
) const
{
   if (errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      if (m_log_to_syslog)
      {
#ifdef POSIX
         int priority = get_priority(errlev);
         vsyslog(priority, fmt.c_str(), val);
#else
         vfprintf(logfile(), fmt.c_str(), val);
#endif
      }
      else
      {
         FILE * fp = logfile();
         fprintf(fp, "%s ", tag.c_str());
         vfprintf(fp, fmt.c_str(), val);
         fprintf(fp, "\n");                  /* consistent w/other calls   */
         fflush(fp);
      }
   }
}

#ifdef DOCUMENT_VA_MSGTAG

/******************************************************************************
 * va_msgtag()
 *------------------------------------------------------------------------*//**
 *
 *    Provides message support for variable numbers of arguments.
 *
 *    This function is even newer than va_tag(), and is meant to ultimately
 *    replace it.  It allocates a buffer, formats the string data into this
 *    buffer, and then shows it with msg_tag(), to keep the real work
 *    centralized in that function.
 *
 * \param errlev
 *    Error level of message, not used in Win32.
 *
 * \param tag
 *    A single-character tag string (e.g. "?").
 *
 * \param errmsg
 *    The error or info message to be logged.
 *
 * \param val
 *    Variable argument-list construct.
 *
 * \todo
 *    Actually use this function some day.  It compiles right now.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::va_msgtag
(
   xpc_errlevel_t errlev,
   const std::string & tag,
   const std::string & errmsg,
   va_list val
) const
{
   if (errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      int size = 256;                              /* starting size        */
      char * p = malloc(size);                     // TODO use string
      if (not_nullptr(p))
      {
         int n;
         char * np;
         while (true)
         {
            n = vsnprintf(p, size, fmt, val);
            if ((n > -1) && (n < size))            /* it worked            */
            {
               msgtag(errlev, tag, p);
               free(p);
               break;
            }
            if (n > -1)                            /* glibc 2.1            */
               size = n + 1;                       /* need this much       */
            else                                   /* glibc 2.0            */
               size *= 2;                          /* double it            */

            np = realloc(p, size);
            if (not_nullptr(np))
               p = np;
            else
            {
               free(p);                            /* it failed bad!       */
               break;
            }
         }
      }
      else
         errprint(_("failed to allocate in va_msgtag()"));
   }
}

#endif      // DOCUMENT_VA_MSGTAG

/******************************************************************************
 * errprint()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a basic error message string.
 *
 *    Prints a basic error message using msgtag() to do all the checks and
 *    dirty work.  It checks to see if errors are allowed to be shown, and
 *    then sends the error tag (a '?') and passes along the message.
 *
 *    If the message is null, then it is assumed that the error is null,
 *    and it is not shown.
 *
 * \param errmsg
 *    Standard C null-terminated message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::errprint (const std::string & errmsg) const
{
   if (showerrors())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_ERRORS,
         usecolor() ? COLOR_STR_ERROR : ERRL_STR_ERROR, errmsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * errprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatted error message string.
 *
 *    Similar to xpc_errprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::errprintf (const std::string & fmt, ...) const
{
   if (showerrors())
   {
      if (fmt.size() > 0)              // NEED TO CHECKI FOR %s
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_ERRORS,
            usecolor() ? COLOR_STR_ERROR : ERRL_STR_ERROR,
            fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * concat_buffer()
 *------------------------------------------------------------------------*//**
 *
 *    Allocates a buffer for storage and concatenates two strings into it.
 *
 *    The first string parameter is the "message".  This will usually be an
 *    error, warning, or informational message.
 *
 *    The second string is the "label" part.
 *
 *    The output is in the form
 *
 *          <i> label:  msg </i>
 *
 *    for consistency with UNIX syslog() messages.  However, in some cases
 *    that ordering won't look right, and the caller may have to reverse the
 *    arguments.
 *
 * \param msg
 *    The main message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \bug
 *    The parameters are not in the natural order, but there are too many
 *    places in which to correct it!
 *
 * \private
 *    This function allocates a buffer for xpc_errprintex(),
 *    xpc_warnprintex(), and xpc_infoprintex().
 *
 * \warning
 *    Don't modify this routine to use error functions that call this
 *    routine!
 *
 * \return
 *    The address of the heap-allocated buffer is returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

std::string
errorlog::concat_buffer
(
   const std::string & msg,
   const std::string & label
) const
{
   std::string result;
   size_t length = msg.size() + label.size();         // strlen(ERRL_STR_EXTRA);
   if (length > 0)
   {
      if (xpc_usecolor())
      {
         result += COLOR_STR_LABEL_START;             /* turn on the color    */
         result += label;                             /* object portion       */
         result += ERRL_STR_EXTRA;                    /* ': '                 */
         result += COLOR_STR_END;                     /* turn off color       */
      }
      else
      {
         result  = label;                             /* object portion       */
         result += ERRL_STR_EXTRA;                    /* ': '                 */
      }
      result +=msg;                                   /* message portion      */
   }
   return result;
}

/******************************************************************************
 * errprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a message consisting of a main string and secondary string.
 *
 *    This function assembles a message composed of two parts.  It
 *    allocates a generous buffer for the text, assembles the parts, and
 *    passes it to xpc_errprint().  To make it easier on the caller, if
 *    the second parameter is null, then xpc_errprint() is called.
 *
 *    This function is used in reporting both an error and some parameter
 *    related to the error.
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \todo
 *    Develop a way to make this more efficient.  Consider a macro and
 *    testing showerrors() here instead.  [We now have a macro,
 *    errprint_func(), that fills in the 'label' parameter using the
 *    '__func__' macro.  See how it is used in the rest of this module.]
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::errprintex
(
   const std::string & errmsg,
   const std::string & label
) const
{
   if (showerrors())
      errprint(concat_buffer(errmsg, label));
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * warnprint()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a warning message.
 *
 *    Similar to errprint(), but presents a warning message instead.
 *
 *    This function is used in reporting an unexpected event or parameter
 *    value that does not affect the application in any dire manner.
 *
 * \param warnmsg
 *    The null-terminated warning message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::warnprint (const std::string & warnmsg) const
{
   if (showwarnings())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_WARNINGS,
         usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN,
         warnmsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * warnprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting warning message.
 *
 *    Similar to warnprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::warnprintf (const std::string & fmt, ...) const
{
   if (showwarnings())
   {
      if (fmt.size() > 0)              // NEED TO CHECK FOR % character
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_WARNINGS,
            usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN,
            fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * warnprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a warning message consisting of a main message and a secondary
 *    string.
 *
 *    Similar to errprintex(), but presents a warning message
 *    instead.  It assembles a message composed of two parts.  It is used in
 *    reporting both a warning and some parameter related to the warning.
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::warnprintex
(
   const std::string & warnmsg,
   const std::string & label
) const
{
   if (showwarnings())
      (void) warnprint(concat_buffer(warnmsg, label));
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * infoprint()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message.
 *
 *    Similar to errprint(), but presents an informational message
 *    instead.
 *
 * \param infomsg
 *    The informational message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::infoprint (const std::string & infomsg) const
{
   if (showinfo())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_INFO,
         usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO, infomsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * infoprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message.
 *
 *    Similar to infoprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \usage
 *    -# Each line of text needs to be terminated with a newline sequence
 *       ("\n"), in order to make the output look good for multiline output.
 *    -# Each line of text after the first needs to start with two spaces,
 *       in order to look good for multiline output.
 *
 *    This function is similar to infoprintml().  The only difference
 *    between infoprintf() and infoprintml() is that the latter
 *    inserts a tag character and space automatically at the beginning of
 *    each line.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::infoprintf (const std::string & fmt, ...) const
{
   if (showinfo())
   {
      if (fmt.size() > 0)
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_INFO,
            usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO, fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * infoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message consisting of a main message and a
 *    secondary string.
 *
 *    Similar to errprintex(), but presents an informational message
 *    instead.
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::infoprintex
(
   const std::string & infomsg,
   const std::string & label
) const
{
   if (showinfo())
      infoprint(concat_buffer(infomsg, label));
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * infomark_format()
 *------------------------------------------------------------------------*//**
 *
 *    A private function to massage a format string to add info-marker
 *    characters.
 *
 *    See infoprintml() for more information.  A string is allocated that
 *    that function must free.
 *
 * \return
 *    If the parameter is valid and the function succeeds, a pointer to the
 *    new format string is returned.  Otherwise, nullptr is returned.
 *
 * \todo
 *    On potentially nifty upgrade to this function would be to scan for an
 *    unsupported format specifier (e.g. "%C") that would convert to a "%s"
 *    format specifier with color-sequences around it, so that the caller
 *    could specify colored strings in the format.
 *
 * \param fmt
 *    The printf-style formatting string to use.
 *
 * \todo
 *    How about a multi-line output function that automatically inserts
 *    newlines and indentation?
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

std::string
errorlog::infomark_format (const std::string & fmt) const
{
   std::string result;
   size_t length = fmt.size();
   if (length > 0)                           /* we're good to go!             */
   {
      size_t added = strlen(xpc_infomark())+1; /* added space per line        */
      int nlcount = 0;                       /* prepare to count newlines     */
      const char * p = (const char *) fmt.c_str();
      for (;;)
      {
         p = strchr(p, '\n');                /* find the first newline        */
         if (is_nullptr(p))                  /* no more newlines              */
            break;

         nlcount++;                          /* count the newline             */
         p++;                                /* get past the newline          */
      }
      if (nlcount == 0)                      /* always have at least one line */
         nlcount++;

      added *= nlcount;                      /* space for "* "                */
      added += 8;                            /* MXC safety fluid              */

      ptrdiff_t length;                      /* used in the while loop        */
      int i = 0;
      const char * ps = fmt.c_str();         /* init the start pointer        */
      while (nlcount > 0)
      {
         char * q;                           /* this is the 'next' pointer    */
         if (i++ > 0)                        /* don't do it for first line    */
         {
            result += xpc_infomark();        /* can be 1 or more chars        */
            result += " ";                   /* add a spacer                  */
         }
         q = strchr(const_cast<char *>(ps), '\n'); /* find next newline       */
         if (is_nullptr(q))                  /* not found                     */
            q = strchr(const_cast<char *>(ps), 0); /* find next null          */

         length = q - ps;
         if (length > 0)                     /* no text before newline        */
            result += ps;

         if (nlcount > 1)                    /* don't terminate last line     */
            result += "\n";

         nlcount--;
         ps = q + 1;
      }
   }
   else
      ERRPRINT_FUNC(_("invalid printf() format"));

   return result;
}

/******************************************************************************
 * infoprintml()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message that spans multiple
 *    lines.
 *
 *    It inserts the info-marker(), saving the caller a lot of trouble.
 *
 *    The name is short for "infoprint multi-line".
 *
 *    Similar to infoprintf(), it accepts variable formatting and
 *    arguments, but also inserts makers.  It replaces a calling sequence
 *    such as:
 *
\verbatim
         fprintf
         (
            xpc_logfile(),
            "%s xpc_app_arguments_t:\n"
            "%s   sizeof()             = %d\n"
            "%s   Run as daemon        = %s\n"
            ,
            infomark(),
            infomark(), sizeof(xpc_app_arguments_t),
            infomark(), xpc_boolean_string(runasdaemon)
         );
\endverbatim
 *
 *    with the call:
 *
\verbatim
         infoprintml
         (
            "xpc_app_arguments_t:\n"
            "  sizeof()             = %d\n"
            "  Run as daemon        = %s\n"
            ,
            sizeof(xpc_app_arguments_t),
            xpc_boolean_string(runasdaemon)
         );
\endverbatim
 *
 *    generating output such as:
 *
\verbatim
         * xpc_app_arguments_t:
         *   sizeof()             = 18
         *   Run as daemon        = true
\endverbatim
 *
 * \usage
 *    -# Each line of text needs to be terminated with a newline sequence
 *       ("\n"), in order to make the output look good for multiline output.
 *
 *    This function is similar to infoprintf().  The only difference
 *    between infoprintf() and infoprintml() is that the latter
 *    inserts a tag character and space automatically at the beginning of
 *    each line.
 *
 * \tricky
 *    A trick is used below to avoid this warning caused by replacing fmt
 *    with newfmt:
 *
\verbatim
         warning: second parameter of 'va_start' not last named argument
\endverbatim
 *
 *    In this trick, we take the original function parameter \a fmt and
 *    assign \a newfmt to it.  This removes the warning.
 *    Are we being too tricky here (i.e. is something going wrong that we
 *    cannot see?)  Don't know.  The code seems to work.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
errorlog::infoprintml (const std::string & fmt, ...) const
{
   if (showinfo())
   {
      if (fmt.size() > 0)
      {
         std::string newfmt = infomark_format(fmt);      /* rip a new one     */
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_INFO,
            usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO,
            newfmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * dbginfoprint() and dbginfoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    One of many functions active only when compiled for debugging.
 *
 *    The functions are dbginfoprint(), dbginfoprintex(),
 *    dbginfoprintf(), and print_debug().
 *
 *    dbginfoprint() is similar to infoprint(), but performs only if
 *    DEBUG is defined.  Use it when you want to see the message only if
 *    debugging and when --info is specified.
 *
 *    print_debug() is similar to print(), but performs only if DEBUG is
 *    defined.  Use it when you want to see the message any time debugging
 *    is compiled in.  It can still be disabled by setting debug to
 *    XPC_ERROR_LEVEL_NONE.
 *
 *    See the similarly name functions for warnings, errors, and info.
 *
 * \param infomsg
 *    The main info message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DEBUG                                             /* DEBUG             */

void
errorlog::dbginfoprint (const std::string & infomsg) const
{
   if (showinfo())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_INFO,
         usecolor() ? COLOR_STR_DEBUG : ERRL_STR_DEBUG,
         infomsg
      );
   }
}

/******************************************************************************
 * dbginfoprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message, if debugging is
 *    enabled.
 *
 *    Similar to infoprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::dbginfoprintf (const std::string & fmt, ...) const
{
   if (showinfo())
   {
      if (fmt.size() > 0)
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_INFO,
            usecolor() ? COLOR_STR_DEBUG : ERRL_STR_DEBUG,
            fmt, val
         );
         va_end(val);
      }
   }
}

/******************************************************************************
 * print_debug()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an unconditional informational message, if debugging is enabled.
 *
 *    Basically, it is simply a debug-enabled variant of print().
 *
 * \param infomsg
 *    The main info message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::print_debug (const std::string & infomsg) const
{
   print(infomsg);
}

/******************************************************************************
 * dbginfoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message consisting of a main message and a
 *    secondary string.
 *
 *    Similar errprintex(), but presents an informational message
 *    instead, and only when the application is built with debugging
 *    enabled.
 *
 * \param infomsg
 *    The main informational message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::dbginfoprintex
(
   const std::string & infomsg,
   const std::string & label
) const
{
   if (showinfo())
      dbginfoprint(concat_buffer(infomsg, label));
}

#endif                                                   /* DEBUG             */

/******************************************************************************
 * print()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a message as information, no matter what the error-level.
 *
 *    Similar to infoprint(), but prints as long as the debug level is
 *    not XPC_ERROR_LEVEL_NONE.
 *
 *    Note that we don't need to define a printf() function (grin).
 *
 * \param infomsg
 *    The main info message string.
 *
 * \unittests
 *    -  errortest_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::print (const std::string & infomsg) const
{
   msgtag
   (
      XPC_ERROR_LEVEL_INFO,
      usecolor() ? COLOR_STR_PRINT : ERRL_STR_PRINT,
      infomsg
   );
}

/******************************************************************************
 * lkprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a message with optional usage of the synchronization critex.
 *
 *    Similar to fprintf(), but will lock the fprintf() call if the --synch
 *    option was used.
 *
 *    It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    A printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  See errortest_05_01() for a potential test.
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::lkprintf (const std::string & fmt, ...) const
{
   if (fmt.size() > 0)
   {
      va_list val;
      va_start(val, fmt);
      if (! m_critex_usage || synch_lock())
      {
         vfprintf(logfile(), fmt.c_str(), val);
         fflush(logfile());
         synch_unlock();
      }
      va_end(val);
   }
}

/******************************************************************************
 * format_error_string()
 *------------------------------------------------------------------------*//**
 *
 *    Formats an error-log message for logging.
 *
 *    This function is a helper function used in strerrnoprintex() and
 *    strerrprintex().
 *
 *    It can also be used in external functions, though we do not yet have
 *    an example handy.
 *
 *    It assembles a message composed of three parts.  This function is used
 *    in reporting an error, some parameter related to the error, and a
 *    C-library error string.
 *
 *    This function has to be made public in case we need to be able to
 *    "derive" code that uses this helper function, to format error messages
 *    from error-generating functions other than strerrnoprintex() and
 *    strerrprintex().
 *
 * \param errmsg
 *    The main error message from the developer.
 *
 * \param label
 *    Supplemental information (e.g. a label).
 *
 * \param syserr
 *    A system-formatted system error message.
 *
 * \unittests
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.  This stuff is very time-consuming to write!
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::format_error_string
(
   const std::string & errmsg,
   const std::string & label,
   const std::string & syserr
) const
{
   bool havesyserr = syserr.size() > 0;
   std::string msgbuffer;
   msgbuffer = label;
   msgbuffer += ERRL_STR_EXTRA;
   msgbuffer += errmsg;
   if (havesyserr)
   {
      msgbuffer += ERRL_STR_SEPARATOR;
      msgbuffer += "(";
      msgbuffer += syserr;
      msgbuffer += ")";
   }
   errprint(msgbuffer);
}

/******************************************************************************
 * strerrnoprint()
 *------------------------------------------------------------------------*//**
 *
 *    Combines a string and a system error message, logs them, and returns a
 *    POSIX error code.
 *
 *    This function simply calls strerrnoprintex() with the "label"
 *    parameter set to nullptr.  See that function for more details.
 *
 * \note
 *    Although the system error code is returned, most commonly it is
 *    ignored, without even a "(void)" cast to indicate the ignoring of the
 *    return value.
 *
 * \param errmsg
 *    The caller's supplemental error message string.
 *
 * \return
 *    The value returned by strerrnoprintex() is returned.
 *
 * \unittests
 *    -  Tests that check output to a log file perhaps could be written, but
 *       have not yet.
 *
 *//*-------------------------------------------------------------------------*/

int
errorlog::strerrnoprint (const std::string & errmsg) const
{
   return strerrnoprintex(errmsg, "");
}

/******************************************************************************
 * strerrnoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Combines two strings and a system error message, logs them, and
 *    returns a POSIX error code.
 *
 * \threadsafe
 *    This function allocates a stack buffer for the message, and passes it
 *    to the threadsafe function getlasterror_text_r().
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \return
 *    Returns the value of \e errno (or whatever the operating system
 *    provides as the system error code).
 *
 * \unittests
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

int
errorlog::strerrnoprintex
(
   const std::string & errmsg,
   const std::string & label
) const
{
   int result = getlasterror();
   std::string errormessage = _("success");
   if (not_posix_success(result))
      errormessage = strerror_wrapper(result);
   else
      errormessage = _("errno is 0, why is strerrnoprintex() being called?");

   format_error_string(errmsg, label, errormessage);
   return result;
}

/******************************************************************************
 * strerrprint()
 *------------------------------------------------------------------------*//**
 *
 *    Assembles a message with a POSIX error number, and logs it.
 *
 *    See the strerrprintex() function, which is used to implement the
 *    strerrprint() function.
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param errnum
 *    The POSIX \e errno for the error.
 *
 * \unittests
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::strerrprint (const std::string & errmsg, int errnum) const
{
   strerrprintex(errmsg, "", errnum);
}

/******************************************************************************
 * strerrprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Assembles a message with a POSIX error number, and logs it.
 *
 *    This function and strerrprintex() are similar to
 *    strerrnoprint() and strerrnoprintex(), but they are provided
 *    the error variable as a parameter, instead of getting it from errno.
 *
 *    Each assembles a message composed of three parts.  These functions are
 *    used in reporting an error, some parameter related to the error, and a
 *    system error string.
 *
 * \param errmsg
 *    The main error message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \param errnum
 *    The POSIX \e errno for the error.
 *
 * \unittests
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::strerrprintex
(
   const std::string & errmsg,
   const std::string & label,
   int errnum
) const
{
   std::string codeerror = _("success");
   if (not_posix_success(errnum))
   {
      if (not_posix_error(errnum))
      {
         std::string msgbuffer;
         char * errmsg2 = strerror(errnum);              /* not thread-safe   */
         msgbuffer = errmsg2;
         codeerror = msgbuffer;
      }
      else
         codeerror = _("N/A");

      format_error_string(errmsg, label, codeerror);
   }
   else
      format_error_string(errmsg, label, codeerror);
}

/******************************************************************************
 * getlasterror() [POSIX]
 *------------------------------------------------------------------------*//**
 *
 *    This function retrieves the latest system error.
 *
 * \posix
 *    The value stored in the system-variable \e errno is obtained.  set by
 *    system calls and some library functions in the event of an  error to
 *    indicate what went wrong.  Its value is significant only when the call
 *    returned an error (usually -1), and a function that does succeed is
 *    allowed to change \e errno.  Sometimes, when -1 is also a valid
 *    successful return value one has to zero \e errno before the call in
 *    order to detect possible errors.
 *
 *    \e errno is defined by the ISO C standard to be a modifiable lvalue of
 *    type int, and must not be explicitly declared; \e errno may be a
 *    macro.  \e errno is thread-local; setting it in one thread does not
 *    affect its value in any other thread. \e errno is never set to zero by
 *    any library function.
 *
 * \win32
 *    Under Windows, socket functions do not place error codes in \e errno.
 *    Instead, WSAGetLastError() is supposed to return the error code, and
 *    experience shows that \e errno == 0.
 *
 *    However, we've found that GetLastError() also works for socket errors,
 *    returning the same value as WSAGetLastError().
 *
 *    Hence, if \e errno comes up 0, we fall back to GetLastError().
 *
 * \return
 *    Returns the value of \e errno or the value of GetLastError(),
 *    depending on the operating system and the current value of \e errno.
 *
 * \unittests
 *    -  errortest_02_16()
 *
 *//*-------------------------------------------------------------------------*/

int
errorlog::getlasterror () const
{
#ifdef WIN32
   int result = errno;
   if (errno == 0)
   {
      DWORD errorcode = GetLastError();
      result = (int) errorcode;
   }
   return result;
#else
   return errno;
#endif
}

/******************************************************************************
 * getlasterror_text() [POSIX and Win32]
 *------------------------------------------------------------------------*//**
 *
 *    This function retrieves a human-readable error message.
 *
 *    This function is implemented by retrieving the system error number,
 *    and then passing it to strerror_wrapper().
 *    sure if this is the case in Linux.
 *
 * \return
 *    If an error is in force, then this function returns the string
 *    corresponding to the error that is in force.  If no error is in force,
 *    then this function returns an empty string.  In this case, the
 *    programmer ought to look at the code to see why this "error" function
 *    is being called when there is no error.
 *
 * \unittests
 *    -  errortest_02_16()
 *
 *//*-------------------------------------------------------------------------*/

std::string
errorlog::getlasterror_text () const
{
   int last_error = getlasterror();
   return strerror_wrapper(last_error);
}

/******************************************************************************
 * strerror_wrapper
 *------------------------------------------------------------------------*//**
 *
 *    Provides a system-independent interface for retrieving system error
 *    messages based on system error codes.
 *
 *    When an error occurs that sets the global \e errno, the system usually
 *    provides a function to return an error message based on the error.
 *    The strerror_wrapper() uses that function to obtain the
 *    message.
 *
 *    The message will be stored in a native (byte-based) string.
 *
 * \gnu
 *    The GNU version of strerror_r() doesn't necessarily fill the caller's
 *    buffer -- it may instead just return the pointer to a
 *    statically-allocated message.  The strerror_r() function cannot fail.
 *    At most, it will return "Unknown error nnn".
 *
 * \linux
 *    Under Linux the XSI-compliant version of strerror_r() is \e not
 *    supplied.  Instead, the GNU version is used.
 *
 * \posix
 *    The XSI-compliant strerror_r() fills a buffer of the given size, and
 *    then returns 0 on success; on error, -1 is returned and errno is set
 *    to indicate the error.  The error string is obtained by the strerr()
 *    function, and the message is immediately constructed from its parts.
 *    If __GNUC__ is defined, then strerror_r() is used.  Note that The
 *    XSI-compliant version of strerror_r() is provided if:
 *
 *    (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
 *
 * \win32
 *    This function uses strerror().  FormatMessage() could also be used
 *    to create an error message.  See the errorw32.c module in the XPC
 *    C library for a Win32 implementation.
 *
 * \param errorcode
 *    The errno-style (POSIX) or Win32 system error number for which the
 *    system message is desired.
 *
 * \return
 *    The system error message for the given error-code is returned.  If the
 *    function fails, the result is a string stating the str_error_r() error.
 *    If there is no system error in force, an empty string is returned, and
 *    the caller need not show the message.
 *
 * \todo
 *    Apply any fixes made here to the C version of the strerror_wrapper()
 *    function.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __GNUC__

#if XSI_STRERROR_R

std::string
errorlog::strerror_wrapper (int errorcode) const
{
   const std::string result;
   if (errorcode != 0)
   {
      char msg[256];
      int rc = strerror_r(errorcode, msg, sizeof(msg));  /* XSI-compliant     */
      if (not_posix_success(rc))
      {
         if (rc == EINVAL)
         {
            char temp[32];
            sprintf(temp, "%d: ", errcode);
            result = temp;
            result += _("invalid error code");
         }
         else if (rc == ERANGE)
         {
            result = _("insufficient storage for the system error message");
         }
         else
         {
            char temp[32];
            sprintf(temp, "%d: ", rc);
            result = temp;
            result += _("unexpected code from XSI-compliant strerror_r()");
         }
      }
      else
         result = msg;
   }
   return result;
}

#else                                                    /* ! XSI_STRERROR_R  */

std::string
errorlog::strerror_wrapper (int errorcode) const
{
   std::string result;
   if (errorcode != 0)
   {
#ifdef __MINGW32__
      const char * tmp = strerror(errorcode);            /* normal version    */
      if (is_nullptr(tmp))
         tmp = _("strerror() returned a null system error message");

      result = tmp;
#else
      char msg[256];
      (void) strerror_r(errorcode, msg, sizeof(msg));    /* GNU version       */
      result = msg;
#endif
   }
   return result;
}

#endif                                                   /* XSI_STRERROR_R    */

#else                                                    /* ! __GNUC__        */

std::string
errorlog::strerror_wrapper (int errorcode) const
{
   std::string result;
   if (errorcode != 0)
   {
      char * systemerror = strerror(errorcode);          /* system message    */
      if (is_nullptr(systemerror))
         systemerror = _("strerror() returned a null system error message");

      result = systemerror;
   }
   return result;
}

#endif                  /* __GNUC__          */

/******************************************************************************
 * buffering()
 *------------------------------------------------------------------------*//**
 *
 *    Changes the buffering on the current error-log file.
 *
 *    stdout is line-buffered, by default, if it refers to a terminal (as it
 *    normally does).
 *
 *    stderr is always unbuffered by default.
 *
 *    The buffering types available are:
 *
 *       -  XPC_ERROR_NOT_BUFFERED
 *       -  XPC_ERROR_BLOCK_BUFFERED
 *       -  XPC_ERROR_LINE_BUFFERED
 *
 * \note
 *    For now, we let the system choose the buffer-size, if applicable.
 *
\verbatim
         #define XPC_ERROR_BUFFER_SIZE    1024
         static char iobuffer[XPC_ERROR_BUFFER_SIZE];
         static size_t iobuffersize = XPC_ERROR_BUFFER_SIZE;
\endverbatim
 *
 * \win32
 * \posix
 *    We could add configuration checks for POSIX systems and Win32.
 *    However, it looks like both systems support this functionality.
 *
 * \param btype
 *    The type of buffering, one of: not, block, and line.
 *
 * \return
 *    Returns 'true' if the setting of buffering succeeded.
 *
 * \unittests
 *    -  errortest_02_17()
 *
 *//*-------------------------------------------------------------------------*/

bool
errorlog::buffering (int btype) const
{
   bool result = false;
   if ((btype >= XPC_ERROR_NOT_BUFFERED) && (btype <= XPC_ERROR_LINE_BUFFERED))
   {
      int rcode;
      FILE * fp = logfile();
      if (not_NULL(fp))
      {
         switch (btype)
         {
         case XPC_ERROR_NOT_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IONBF, 0);
            if (not_posix_success(rcode))
               ERRPRINT_FUNC(_("unbuffered failed"));
            else
            {
               INFOPRINT_FUNC(_("unbuffered"));
               result = true;
            }
            break;

         case XPC_ERROR_BLOCK_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IOFBF, 0);
            if (not_posix_success(rcode))
               ERRPRINT_FUNC(_("block-buffered failed"));
            else
            {
               INFOPRINT_FUNC(_("block-buffered"));
               result = true;
            }
            break;

         case XPC_ERROR_LINE_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IOLBF, 0);
            if (not_posix_success(rcode))
               ERRPRINT_FUNC(_("line-buffered failed"));
            else
            {
               INFOPRINT_FUNC(_("line buffered specified explicitly"));
               result = true;
            }
            break;
         }
      }
   }
   else
      ERRPRINT_FUNC(_("invalid buffering type"));

   return result;
}

/******************************************************************************
 * flush_error_log()
 *------------------------------------------------------------------------*//**
 *
 *    Flushes the error-log file.
 *
 *    This function simply passes the result of logfile() to the
 *    system call fflush().
 *
 * \unittests
 *    Not sure right now how one could test this reliably.  The stderr
 *    stream seems to flush after a relatively small amount of output (on
 *    Linux). TBD.
 *
 *//*-------------------------------------------------------------------------*/

void
errorlog::flush_error_log () const
{
   fflush(logfile());
}

}              // namespace xpc

/******************************************************************************
 * errorlog.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
