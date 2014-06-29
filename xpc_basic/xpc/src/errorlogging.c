/******************************************************************************
 * errorlogging.c
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlogging.c
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2010-05-06
 * \updates       2013-07-28
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides error and strings logging useful in C code.
 *
 *    It provides simple methods for showing console error messages.  It
 *    provides a way to set the application-global level of runtime
 *    information, warning, and error output.
 *
 *    It is annoying to set compile-time flags to get console output for
 *    debugging in the field.  The errorlogging.c module allows a run-time
 *    command-line switch to set the verbosity of the output.
 *
 *    Some of the error functions also accept a POSIX error code, or use the
 *    \e errno variable, and display a system error message for the caller.
 *
 *    The errorlogging.c module also provides some error strings for the XPC
 *    library.  It is also intended to simplify internationalization.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlog_macros.h>       /* macros                              */
#include <xpc/errorlogging.h>          /* external functions                  */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/portable.h>              /* xpc_get_microseconds()              */
#include <xpc/xstrings.h>              /* xpc_string_n_cat()                  */
#include <xpc/syncher.h>               /* xpc_syncher_t structure             */
XPC_REVISION(errorlogging)

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

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>                    /* malloc() and free()                 */
#endif

#if XPC_HAVE_MALLOC_H
#include <malloc.h>                    /* malloc() and free()                 */
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
#define USE_XPC_COLORS
#define XPC_NO_ERRORLOG
#define DOCUMENT_VA_MSGTAG
#endif

/******************************************************************************
 * Critical section for synchronizing output [static]
 *------------------------------------------------------------------------*//**
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
 *
 *//*-------------------------------------------------------------------------*/

static xpc_syncher_t gcritex;

/******************************************************************************
 * gcritex_inited
 *------------------------------------------------------------------------*//**
 *
 *    An initialization flag.
 *
 *    This flag helps avoid performing the initialization twice, but also is
 *    used to make sure the initialization does occur the first time the
 *    critex is needed.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gcritex_inited = false;

/******************************************************************************
 * gs_Critex_Usage
 *------------------------------------------------------------------------*//**
 *
 *    This flag indicates if the text-synchronization critex is to be used.
 *
 *    It is controlled by the --synch and --no-synch options from the
 *    command-line.
 *
 *    If set to 'true', the gcritex structrue is set up and used to help
 *    keep the text output from different threads from being intermingled.
 *    But please note that it can affect the detectability of threading
 *    problems.
 *
 *    This value is normally false, because it can affect the running of
 *    multi-threaded programs.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gs_Critex_Usage = false;

/******************************************************************************
 * gcritex_atexit_set
 *------------------------------------------------------------------------*//**
 *
 *    An initialization flag for the loading of the critex cleanup routine
 *    that is called by atexit().
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gcritex_atexit_set = false;

/******************************************************************************
 * synch_critex() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Provides a critical section for optional synchronization of error-log
 *    messages.
 *
 *    This function initializes the private critical section (critex) and
 *    then returns a pointer to it.
 *
 *    This critex is used if the application specifies the "--synch"
 *    command-line option.
 *
 * \private
 *    This function is a static C function.
 *
 * \return
 *    A pointer to the private errorlogging critex.  If it could not be
 *    initialized, then a null pointer is returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static xpc_syncher_t *
synch_critex (void)
{
   xpc_syncher_t * result = &gcritex;
   if (! gcritex_inited)
   {
      if (xpc_syncher_create(&gcritex, false))              /* not recursive  */
         gcritex_inited = true;
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

static void
synch_critex_destroy (void)
{
   if (gcritex_inited)
   {
      (void) xpc_syncher_destroy(&gcritex);
      gcritex_inited = false;
      xpc_infoprint(_("error-logging critex destroyed"));
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
 *    This function is a static C function.
 *
 * \return
 *    Returns 'true' if the x_syncher_t object is available, and the
 *    xpc_syncher_enter() call succeeded.  Also returns 'true' if the usage
 *    of locking is not set, so that the caller can go ahead and execute the
 *    locked code.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
synch_lock (void)
{
   cbool_t result = true;
   if (gs_Critex_Usage)
      result = xpc_syncher_enter(synch_critex());

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
 *    This function is a static C function.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static void
synch_unlock (void)
{
   if (gs_Critex_Usage)
   {
      fflush(xpc_logfile());
      (void) xpc_syncher_leave(synch_critex());
   }
}

/******************************************************************************
 * xpc_synchusage_set()
 *------------------------------------------------------------------------*//**
 *
 *    Turns the output log synchronization option on or off.
 *
 *    This function is activated by the "--synch" command-line option.  Its
 *    effect is to make the output much less garbled when multithreading is
 *    in force.
 *
 *    The private variable controlling this usage is gs_Critex_Usage.  The
 *    critex value used is gcritex.
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
 *    -  errorlogging_test_02_01()
 *    -  errorlogging_test_05_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_synchusage_set (cbool_t flag)
{
   cbool_t result = true;
   gs_Critex_Usage = flag;
   if (flag)
      xpc_warnprint(_("output synchronization enabled"));
   else
      xpc_warnprint(_("output synchronization disabled"));

   if (! gcritex_atexit_set)
   {
      int rcode = atexit(synch_critex_destroy);
      if (is_posix_success(rcode))
      {
         gcritex_atexit_set = true;
         xpc_infoprintex("atexit(synch_critex_destroy)", _("succeeded"));
      }
      else
      {
         xpc_errprintex("atexit(synch_critex_destroy)", _("failed"));
         result = false;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_synchusage()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the current value of gs_Critex_Usage.
 *
 *    This function is useful in unit-testing to save the current value for
 *    later restoration.
 *
 * \unittests
 *    -  errorlogging_test_02_01()
 *    -  errorlogging_test_05_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_synchusage (void)
{
   return gs_Critex_Usage;
}

/******************************************************************************
 * gs_Use_Color
 *------------------------------------------------------------------------*//**
 *
 *    The following section handles adding some color to the messages.
 *
 * \private
 *    This color flag is private to the error logging module.  The caller
 *    can access it through the xpc_usecolor_set() and xpc_usecolor()
 *    functions.
 *
 *//*-------------------------------------------------------------------------*/

#undef USE_XPC_COLORS

#ifdef POSIX
#define USE_XPC_COLORS
static cbool_t gs_Use_Color = true;
#endif

#ifdef WIN32
#undef USE_XPC_COLORS      /* cannot use, see comments for xpc_usecolor_set() */
static cbool_t gs_Use_Color = false;
#endif

/******************************************************************************
 * xpc_usecolor_set() [POSIX]
 *------------------------------------------------------------------------*//**
 *
 *    Turns the usage of console colors on or off.
 *
 *    The command-line option to turn colors on is "--color", but colors are
 *    on by default.   The command-line option to turn colors off is
 *    "--mono".  If the error-log is a file, then color is turned off no
 *    matter what, to avoid cluttering the file with ANSI escape sequences.
 *
 *    The colors are applied only to the characters that flag the type of
 *    message (warning, error, etc.) that is being logged.
 *
 *    This function does nothing (except print an error message) under
 *    Win32, because the console output does not seem to support color, even
 *    with the ANSI.SYS file loaded (on Windows XP anyway).  Here, it looks
 *    like it cannot work in a Win32 console application, since the MS-DOS
 *    subsystem is required:
 *
 *       http://support.microsoft.com/kb/101875
 *
 *    We will not bother to try to get the XPC to build and operate under
 *    MS-DOS !
 *
\verbatim
   Attribute codes       Text color codes      Background color codes

      00 = none             30 = black           40 = black
      01 = bold             31 = red             41 = red
      04 = underscore       32 = green           42 = green
      05 = blink            33 = yellow          43 = yellow
      07 = reverse          34 = blue            44 = blue
      08 = concealed        35 = magenta         45 = magenta
                            36 = cyan            46 = cyan
                            37 = white           47 = white
\endverbatim
 *
 * Escape-sequence formats:
 *
\verbatim
         static const char * const gs_Color_Fmt = "\x1B[0%1d;%2dm";
         static const char * const gs_Color_Off = "\x1B[00m";
\endverbatim
 *
 * \win32
 *    Even with %systemroot%/system32/config.nt edited to contain the line
 *    "device=%systemroot%/system32/ansi.sys", neither cmd.exe nor
 *    command.com seem to support ANSI escape sequences on Windows XP.
 *    Yet, the Cygwin bash shell does color in a Windows "cmd.exe" window.
 *    The reason seems to be in the difference between color escape
 *    sequences.  Compare these:
 *
\verbatim
      DOS (cmd.exe)

            ^[[0mfilename^[[0m               No color
            ^[[01;34mdirectory^[[0m          Blue

      Linux (rxvt)

            ^[[00mfilename&[[00m             No color
            ^[[01;34mdirectory^[[00m         Blue
\endverbatim
 *
 * \param flag
 *    Determines the status of color usage.  True == colors on, false ==
 *    colors off.
 *
 * \return
 *    Returns 'true' if the compiler settings allow the setting of color to
 *    be performed.
 *
 * \unittests
 *    -  errorlogging_test_02_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_usecolor_set (cbool_t flag)
{
#ifdef USE_XPC_COLORS

   gs_Use_Color = flag;
   return true;

#else

   if (flag)
   {
      xpc_errprint_func(_("console color support not supported"));
      return false;
   }
   else
   {
      gs_Use_Color = false;
      return true;
   }

#endif                                                   /* USE_XPC_COLOR     */
}

/******************************************************************************
 * xpc_usecolor()
 *------------------------------------------------------------------------*//**
 *
 *    Returns the status of color usage in the output.
 *
 *    This status is normally on by default for UNIXen, and off for Windows.
 *    It is also disabled if the log file is an actual file, rather than
 *    stdout or stderr.
 *
 * \posix
 *    If isatty() is false, then gs_Use_Color is turned off, since it's
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
 *    -  errorlogging_test_02_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_usecolor (void)
{

#if XPC_HAVE_UNISTD_H

   if (gs_Use_Color)
   {
      FILE * logfile = xpc_logfile();        /* get the current file pointer  */
      if (! isatty(fileno(logfile)))         /* is it a descriptor for a TTY? */
         gs_Use_Color = false;               /* no, so don't allow color      */
   }

#endif

   return gs_Use_Color;
}

/******************************************************************************
 * xpc_dbgmark()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the proper marker for the 'debug' error-level.
 *
 *    The marker is either uncolored, or it is colored.  In the latter case,
 *    it includes ANSI escape sequences that color the character in
 *    terminals or consoles that support ANSI escape sequences.
 *
 *    This function is exposed as a convenience for callers.
 *
 * \return
 *    Either a colored or uncolored ">" character as a null-terminated string.
 *
 * \unittests
 *    -  errorlogging_test_02_04() [placeholder]
 *    -  No unit-test for this trivial function that exposes internal macros.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_dbgmark (void)
{
   return xpc_usecolor() ? COLOR_STR_DEBUG : ERRL_STR_DEBUG ;
}

/******************************************************************************
 * xpc_infomark()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the proper marker for the 'info' error-level.
 *
 *    The marker is either uncolored, or it is colored.  In the latter case,
 *    it includes ANSI escape sequences that color the character in
 *    terminals or consoles that support ANSI escape sequences.
 *
 *    This function is exposed as a convenience for callers, but the
 *    xpc_infoprintml() and xpc_infoprintf() functions remove much of the
 *    need for this class of functions.
 *
 * \return
 *    Either a colored or uncolored "*" character as a null-terminated string.
 *
 * \unittests
 *    -  errorlogging_test_02_04()
 *    -  No unit-test for this trivial function that exposes internal macros.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_infomark (void)
{
   return xpc_usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO ;
}

/******************************************************************************
 * xpc_warnmark()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the proper marker for the 'warn' error-level.
 *
 *    The marker is either uncolored, or it is colored.  In the latter case,
 *    it includes ANSI escape sequences that color the character in
 *    terminals or consoles that support ANSI escape sequences.
 *
 *    This function is exposed as a convenience for callers.
 *
 * \return
 *    Either a colored or uncolored "!" character as a null-terminated string.
 *
 * \unittests
 *    -  errorlogging_test_02_04()
 *    -  No unit-test for this trivial function that exposes internal macros.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_warnmark (void)
{
   return xpc_usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN ;
}

/******************************************************************************
 * xpc_errmark()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the proper marker for the 'error' error-level.
 *
 *    The marker is either uncolored, or it is colored.  In the latter case,
 *    it includes ANSI escape sequences that color the character in
 *    terminals or consoles that support ANSI escape sequences.
 *
 *    This function is exposed as a convenience for callers.
 *
 * \return
 *    Either a colored or uncolored "?" character as a null-terminated string.
 *
 * \unittests
 *    -  errorlogging_test_02_04()
 *    -  No unit-test for this trivial function that exposes internal macros.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_errmark (void)
{
   return xpc_usecolor() ? COLOR_STR_ERROR : ERRL_STR_ERROR ;
}

/******************************************************************************
 * xpc_usermark()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the proper marker for the 'user' error-level.
 *
 *    The marker is either uncolored, or it is colored.  In the latter case,
 *    it includes ANSI escape sequences that color the character in
 *    terminals or consoles that support ANSI escape sequences.
 *
 *    This function is exposed as a convenience for callers.
 *
 * \return
 *    Either a colored or uncolored "-" character as a null-terminated string.
 *
 * \unittests
 *    -  errorlogging_test_02_04()
 *    -  No unit-test for this trivial function that exposes internal macros.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_usermark (void)
{
   return xpc_usecolor() ? COLOR_STR_USER : ERRL_STR_USER ;
}

/******************************************************************************
 * gs_Error_Level
 *------------------------------------------------------------------------*//**
 *
 *    This static variable maintains the current error level value.  By
 *    default, the value is XPC_ERROR_LEVEL_ERRORS.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NO_ERRORLOG
static int gs_Error_Level = XPC_ERROR_LEVEL_NONE;
#else
static int gs_Error_Level = XPC_ERROR_LEVEL_ERRORS;
#endif

/******************************************************************************
 * gs_Error_Inited
 *------------------------------------------------------------------------*//**
 *
 *    This static variable maintains the status of the errorlogging module.
 *    It is provided to keep from needlessly initializing the subsystem, and
 *    yet make sure that it is initialized at least once before the first
 *    real usage of the subsystem.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gs_Error_Inited = false;

/******************************************************************************
 * gs_Log_File
 *------------------------------------------------------------------------*//**
 *
 *    This static variable maintains the pointer to the current log file.
 *
 *    By default, this variable is set to stderr before its first usage.
 *    This is done by the s_init_logfile() function, which is called by the
 *    s_logfile_set(), xpc_logfile(), and xpc_buffering_set() functions.
 *
 *//*-------------------------------------------------------------------------*/

static FILE * gs_Log_File = NULLptr;

/******************************************************************************
 * init_option_errlevel() [static]
 *------------------------------------------------------------------------*//**
 *
 *    A private one-shot function to initialize the error-level to the
 *    default.
 *
 *    The default error-level is 'error', where any output using the
 *    xpc_errprint family of functions, or the print() function, is written to
 *    the error-log.
 *
 *    We found that, in some applications, the debug level global variable was
 *    not getting set to XPC_ERROR_LEVEL_ERRORS.  So we need a way to force it
 *    to be initialized.
 *
 * \todo
 *    We have a problem where we would segfault in the command "./threadtest
 *    --clock --info".  This problem occurs only on the faster machines, in
 *    Linux.  A backtrace shows that it is occurring in the libc function
 *    cuserid(), which might not be re-entrant in the hidden vfprintf()
 *    call.  However, using the synch-lock doesn't prevent the error, so
 *    we're not sure what to do yet.
 *
 *    There is a workaround:  add the "--log filename.txt" option.  This
 *    output is much faster than console output, and so is not nearly so
 *    susceptible to the segfault.
 *
 *    The other workaround, of course, is to not use the "--info" option.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static void
init_option_errlevel (void)
{
   if (! gs_Error_Inited)
   {
      gs_Error_Level = XPC_ERROR_LEVEL_ERRORS;
      gs_Error_Inited = true;
   }
}

/******************************************************************************
 * s_init_logfile() [static]
 *------------------------------------------------------------------------*//**
 *
 *    A private function to guarantee that the log-file pointer is
 *    initialized properly before being used.
 *
 *    The default error-log destination is stderr.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static void
s_init_logfile (void)
{
   if (is_NULL(gs_Log_File))
      gs_Log_File = stderr;
}

/******************************************************************************
 * s_logfile_set()
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
 * \param logfile
 *    The handle to be used as the error-log file.
 *
 * \return
 *    Returns 'true' if the \e logfile parameter is valid (that is, not
 *    null).
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
s_logfile_set (FILE * logfile)
{
   cbool_t result = false;
   s_init_logfile();
   if (not_NULL(logfile))
   {
      if (gs_Use_Color && ((logfile != stdout) && (logfile != stderr)))
         (void) xpc_usecolor_set(false);     /* (no need to log this action)  */

      gs_Log_File = logfile;
      result = true;
   }
   return result;
}

/******************************************************************************
 * xpc_open_logfile_helper()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for usage as the error-log, for either truncating it or
 *    appending to it.
 *
 *    This function is a private (C static) helper function for
 *    xpc_open_logfile() and xpc_append_logfile().
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
 *    xpc_open_logfile().
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_open_logfile_helper (const char * logfilename, cbool_t truncateit)
{
   cbool_t result = false;
   if (not_nullptr(logfilename) && (strlen(logfilename) > 0))
   {
      FILE * lf;
      (void) xpc_close_logfile();
      lf = fopen(logfilename, truncateit ? "w+" : "a");
      if (not_NULL(lf))
      {
         if (truncateit)
            xpc_infoprint(_("log-file truncated"));

         result = s_logfile_set(lf);
      }
      else
      {
         xpc_strerrnoprint_func(_("failed"));
         (void) s_logfile_set(stderr);
      }
   }
   else
      xpc_errprint_func(_("invalid filename"));

   return result;
}

/******************************************************************************
 * xpc_open_logfile()
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
 *    -  errorlogging_test_02_03()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_open_logfile (const char * logfilename)
{
   cbool_t result = true;
   FILE * stdio_file = NULLptr;
   cbool_t match = (strcmp(logfilename, "stderr")) == 0 ||
      (strcmp(logfilename, "STDERR") == 0);

   if (match)
      stdio_file = stderr;
   else
   {
      match = (strcmp(logfilename, "stdout")) == 0 ||
         (strcmp(logfilename, "STDOUT") == 0);

      if (match)
         stdio_file = stdout;
      else
      {
         match = (strcmp(logfilename, "stdin")) == 0 ||
            (strcmp(logfilename, "STDIN") == 0);

         if (match)
         {
            result = false;
            if (! xpc_shownothing())
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
         (void) xpc_close_logfile();           /* todo:  use return? */
         result = s_logfile_set(stdio_file);
      }
      else
         result =  xpc_open_logfile_helper(logfilename, true);   /* truncate */
   }
   return result;
}

/******************************************************************************
 * xpc_append_logfile()
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
 *    -  errorlogging_test_02_03()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_append_logfile (const char * logfilename)
{
   return xpc_open_logfile_helper(logfilename, false); /* append to log file */
}

/******************************************************************************
 * xpc_close_logfile()
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
 *    -  errorlogging_test_02_03()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_close_logfile (void)
{
   cbool_t result = false;
   FILE * lf = xpc_logfile();
   cbool_t ok = (lf != stderr) && (lf != stdout) && (lf != stdin);
   if (ok && not_NULL(lf))
   {
      int rcode = fclose(lf);
      result = (rcode == 0);
      (void) xpc_usecolor_set(true);      /* do not use the return value here */
   }
   (void) s_logfile_set(stderr);
   return result;
}

/******************************************************************************
 * xpc_logfile()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the handle to the error-log file.
 *
 *    This function can be used in fprintf() and similar calls to direct
 *    output to the current log file (\e stderr by default).
 *
 *    This function always calls the private helper function s_init_logfile()
 *    in order to guarantee that the handle is properly initialized.  The
 *    s_init_logfile() function has a boolean the makes it do the setup just
 *    once during the lifetime of an application.  Otherwise, there is no
 *    way to guarantee that the handle is initialized by the time the caller
 *    uses the log-file handle.
 *
 * \return
 *    The private handle to the log file, gs_Log_File, is returned.
 *
 * \unittests
 *    -  errorlogging_test_02_03()
 *
 *//*-------------------------------------------------------------------------*/

FILE *
xpc_logfile (void)
{
   s_init_logfile();                         /* *guarantee* it is not null    */
   return gs_Log_File;
}

/******************************************************************************
 * gs_Use_TimeStamps
 *------------------------------------------------------------------------*//**
 *
 *    Provides a flag to enable the inclusion of microsecond-level time stamps
 *    in the errorlogging output.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gs_Use_TimeStamps = false;

/******************************************************************************
 * gs_TimeStamps_Base
 *------------------------------------------------------------------------*//**
 *
 *    Given that time-stamps are being logged, a non-zero value for this
 *    items means that it is deducted from the current seconds value of the
 *    time-stamp.  This deduction is useful to make the time-stamps easier
 *    to read (shorter).
 *
 *//*-------------------------------------------------------------------------*/

static time_t gs_TimeStamps_Base = 0;

/******************************************************************************
 * xpc_timestamps_set()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a setter for the internal gs_Use_TimeStamps flag.
 *
 * \param flag
 *    The value to set the gs_Use_TimeStamps flag.
 *
 * \param setbase
 *    If true, the current time in seconds is logged, so that it can be
 *    deducted from subsequent time-stamp measurements.  Only used if \a
 *    flag is true.
 *
 * \return
 *    This function always returns 'true'.  A result is returned in order to
 *    maintain consistency with the other setter functions.
 *
 * \unittests
 *    -  errorlogging_test_02_05()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_timestamps_set (cbool_t flag, cbool_t setbase)
{
   gs_Use_TimeStamps = flag;
   if (flag && setbase)
   {
      struct timeval ts;
      xpc_get_microseconds(&ts);
      gs_TimeStamps_Base = ts.tv_sec;
   }
   return true;
}

/******************************************************************************
 * time_t xpc_timestamps()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a getter for the internal gs_Use_TimeStamps flag.
 *
 * \return
 *    Returns the value of gs_Use_TimeStamps, which is either 'true' or
 *    'false'.  By default, at program start, this value is false.
 *
 * \unittests
 *    -  errorlogging_test_02_05()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_timestamps (void)
{
   return gs_Use_TimeStamps;
}

/******************************************************************************
 * xpc_errlevel_set()
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
 *    The values that are allowed are defined in the errorlogging.h header
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
 *    -  errorlogging_test_02_06()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_errlevel_set (int errlevel)
{

#ifdef XPC_NO_ERRORLOG

   fprintf
   (
      xpc_logfile(), "%s %s",
      xpc_usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN,
      _("this application is configured to disable error-logging")
   );
   gs_Error_Level = XPC_ERROR_LEVEL_NONE;
   return true;

#else

   cbool_t result = (errlevel < XPC_ERROR_LEVEL_NOT_APPLICABLE);
   if (result)
      result = (errlevel >= XPC_ERROR_LEVEL_NONE);

   init_option_errlevel();                   /* must guarantee initialization */
   if (result)
      gs_Error_Level = errlevel;

   return result;

#endif

}

/******************************************************************************
 * xpc_parse_leveltoken()
 *------------------------------------------------------------------------*//**
 *
 *    Converts the string-name to an error-level and sets that error-level.
 *
 *    This function converts a string value ("error", "warnings", etc.) into
 *    the proper error-level, and then sets the error-level appropriately.
 *
 * \param errlevelstring
 *    Provides the error-level string -- "error", "info", etc. -- to check
 *    for.
 *
 * \return
 *    It returns 'true' if the setting was valid.  A null error-level string
 *    is not valid.
 *
 * \unittests
 *    -  errorlogging_test_02_07()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_leveltoken (const char * errlevelstring)
{
   cbool_t result = false;
   if (not_nullptr(errlevelstring))
   {
      xpc_errlevel_t level;
      char temp[64];
      if (errlevelstring[0] != '-')
      {
         strcpy(temp, "--");
         strncat(temp, errlevelstring, 64-strlen("--"));
      }
      else
         strncpy(temp, errlevelstring, 64);

      temp[63] = 0;
      level = xpc_option_errlevel(temp);
      if (level != XPC_ERROR_LEVEL_NOT_APPLICABLE)
      {
         result = xpc_errlevel_set(level);
         if (! result)
            xpc_errprintex("xpc_errlevel_set()", _("failed"));
      }
      else
         xpc_errprintex(temp, _("failed"));
   }
   return result;
}

/******************************************************************************
 * xpc_filename_check()
 *------------------------------------------------------------------------*//**
 *
 *    Checks the viability of a file-name.
 *
 *    A file-name is invalid if the pointer is null, the file-name is
 *    zero-length, is "." or ".." or other bogus combinations.  We also
 *    disallow filenames beginning with a hyphen.
 *
 *    Note that, at this time, permissions are not part of the check.
 *
 * \param filename
 *    The filename pointer to be checked.
 *
 * \return
 *    Returns 'true' if the filename is valid.  Otherwise, 'false' is
 *    returned.
 *
 * \unittests
 *    -  errorlogging_test_02_09().  Tested implicitly.
 *    -  unit_test_s_filename_check().  This static function should be a
 *       one-to-one match for xpc_filename_check(), and should be updated in
 *       synchrony with it..
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_filename_check (const char * filename)
{
   cbool_t result = false;
   if (not_nullptr(filename))
   {
      if (strlen(filename) > 0)
      {
         result = !
         (
            (strcmp(filename, ".") == 0)     ||
            (strcmp(filename, "..") == 0)    ||
            (strcmp(filename, "./") == 0)    ||
            (strcmp(filename, "../") == 0)   ||
            (strcmp(filename, ".\\") == 0)   ||
            (strcmp(filename, "..\\") == 0)  ||
            (strncmp(filename, "-", 1) == 0)
         );
      }

      /*
       * No need to report any bad file-name in error-logging, because the
       * problem may simply be that there is no file-name, just the next
       * option.
       *
       *    if (! result)
       *       xpc_errprint_func(_("the filename is not acceptable"));
       */
   }
   return result;
}

/******************************************************************************
 * xpc_parse_errlevel_nohelp()
 *------------------------------------------------------------------------*//**
 *
 *    Parses a command-line argument array for error-level arguments, but
 *    ignores "--help".
 *
 *    This function is the same as xpc_parse_errlevel(), but it ignores
 *    "--help".  It should be used in lieu of xpc_parse_errlevel() by
 *    callers that want to create their own help.
 *
 *    Note that this version does check for the --help switching, returning
 *    false if it is encountered.  This function also returns false upon an
 *    error.  But, otherwise, it has to scan every argument, in case
 *    multiple error-logging options were specified.
 *
 *    This function is called by xpc_parse_errlevel() to promote code reuse.
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
 *    errorlogging_test_02_09().  This test does not attempt to handle all
 *    of the command-line options, due to the large-number of side-effects
 *    that would have to be guarded against during the test.  Nonetheless, a
 *    lot of other internal properties do get tested.
 *
 *    For tests of the functions called inside this function, see the
 *    unit-tests for:
 *
 *       -  xpc_errlevel_set()
 *       -  xpc_open_logfile()
 *       -  xpc_syslogging_set()
 *       -  xpc_usecolor_set()
 *       -  xpc_timestamps_set()
 *       -  xpc_buffering_set()
 *       -  xpc_synchusage_set()
 *       -  xpc_showerr_version()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_errlevel_nohelp (int argc, char * argv [])
{
   cbool_t result = false;
   if (not_nullptr(argv))
   {
      if (argc > 1)
      {
         int argi = 0;
         while (++argi < argc)
         {
            xpc_errlevel_t level = xpc_option_errlevel(argv[argi]);
            if (level != XPC_ERROR_LEVEL_NOT_APPLICABLE)
            {
               result = xpc_errlevel_set(level);
            }
            else if (strcmp(argv[argi], CMD(_STDOUT)) == 0)
            {
               result = s_logfile_set(stdout);
            }
            else if (strcmp(argv[argi], CMD(_STDERR)) == 0)
            {
               result = s_logfile_set(stderr);
            }
            else if (strcmp(argv[argi], CMD(_LOG)) == 0)
            {
               if ((argi+1) < argc)
               {
                  result = xpc_filename_check(argv[argi+1]);
                  if (result)
                     result = xpc_open_logfile(argv[argi+1]);
                  else
                     break;            /* don't bother checking any further   */
               }
               else
               {
                  xpc_errprint_func(_("--log requires a filename"));
                  result = false;
                  break;               /* don't bother checking any further   */
               }
            }
            else if (strcmp(argv[argi], CMD(_APPEND)) == 0)
            {
               if ((argi+1) < argc)
               {
                  result = xpc_filename_check(argv[argi+1]);
                  if (result)
                     result = xpc_append_logfile(argv[argi+1]);
                  else
                     break;            /* don't bother checking any further   */
               }
               else
               {
                  xpc_errprint_func(_("--append requires a filename"));
                  result = false;
                  break;
               }
            }
            else if (strcmp(argv[argi], CMD(_SYSLOG)) == 0)
            {
               xpc_infoprint(_("setting system logging"));
               result = xpc_syslogging_set(true);
            }
            else if (strcmp(argv[argi], CMD(_NO_SYSLOG)) == 0)
            {
               result = xpc_syslogging_set(false);
            }
            else if (strcmp(argv[argi], CMD(_COLOR)) == 0)
            {
               /*
                * We used to do this to check that the log file is a TTY.
                * However, this logic is better off in xpc_usecolor().
                *
                */

               FILE * temp = xpc_logfile();
               if ((temp == stderr) || (temp == stdout))
               {
                  result = xpc_usecolor_set(true);
               }
               else
                  result = true;
            }
            else if (strcmp(argv[argi], CMD(_MONO)) == 0)
            {
               result = xpc_usecolor_set(false);
            }
            else if (strcmp(argv[argi], _NC) == 0)    /* a special case */
            {
               result = xpc_usecolor_set(false);
            }
            else if (strcmp(argv[argi], CMD(_NOCOLOR)) == 0)
            {
               result = xpc_usecolor_set(false);
            }
            else if (strcmp(argv[argi], CMD(_NO_COLOR)) == 0)
            {
               result = xpc_usecolor_set(false);
            }
            else if
            (
               (strcmp(argv[argi], CMD(_TIMESTAMPS)) == 0)
                        ||
               (strcmp(argv[argi], CMD(_TIME_STAMPS)) == 0)
            )
            {
               cbool_t rebase = false;
               cbool_t ok = xpc_filename_check(argv[argi+1]);
               if (ok)
               {
                  if (strcmp(argv[argi+1], "rebase") == 0)
                     rebase = true;
               }
               result = xpc_timestamps_set(true, rebase);
            }
            else if
            (
               (strcmp(argv[argi], CMD(_NO_TIMESTAMPS)) == 0)
                        ||
               (strcmp(argv[argi], CMD(_NO_TIME_STAMPS)) == 0)
            )
            {
               result = xpc_timestamps_set(false, false);
            }
            else if (strcmp(argv[argi], CMD(_UNBUFFER)) == 0)
            {
               result = xpc_buffering_set(XPC_ERROR_NOT_BUFFERED);
            }
            else if (strcmp(argv[argi], CMD(_BUFFER)) == 0)
            {
               result = xpc_buffering_set(XPC_ERROR_LINE_BUFFERED);
            }
            else if (strcmp(argv[argi], CMD(_UNBUFFERED)) == 0)
            {
               result = xpc_buffering_set(XPC_ERROR_NOT_BUFFERED);
            }
            else if (strcmp(argv[argi], CMD(_BUFFERED)) == 0)
            {
               result = xpc_buffering_set(XPC_ERROR_LINE_BUFFERED);
            }
            else if (strcmp(argv[argi], CMD(_SYNCH)) == 0)
            {
               result = xpc_synchusage_set(true);
            }
            else if (strcmp(argv[argi], CMD(_NO_SYNCH)) == 0)
            {
               result = xpc_synchusage_set(false);
            }
            else if (strcmp(argv[argi], CMD(_VERSION)) == 0)
            {
               xpc_showerr_version();
               result = false;
            }
            else if (strcmp(argv[argi], CMD(_HELP)) == 0)
            {
               result = false;
               break;
            }
            else
            {
               /*
                * Here, we found no match for the given option.
                *
                * This could be an error, or it could be an application
                * option that is unknown to the error-logging module.  If it
                * is the latter, then we do not want to ignore (skip) the
                * functionality of the application.  So we have to return a
                * value that lets the application proceed.
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
 * xpc_parse_errlevel()
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
 *    'true' is returned if xpc_parse_errlevel_nohelp() detected an
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
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  errorlogging_test_02_10()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_errlevel (int argc, char * argv [])
{
   cbool_t result = false;
   if (not_nullptr(argv))
   {
      result = xpc_parse_errlevel_nohelp(argc, argv);
      while (argc-- > 1)
      {
         if (strcmp(argv[argc], CMD(_HELP)) == 0)
         {
            xpc_error_help();
            result = false;
            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_errlevel()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the current error-level code to the caller.
 *
 *    This function makes sure that initialization has taken place, and then
 *    it returns the current error level.
 *
 *    xpc_errlevel() is useful in unit testing, where the test is meant to
 *    generate an erroneous result, but the function shows the error
 *    privately, potentially confusing the user who is viewing the output.
 *    Call xpc_errlevel(), save its result, do the operations, then call
 *    xpc_errlevel_set() with the saved value, to restore it.
 *
 * \return
 *    The private error-level code is returned.  However, if the
 *    XPC_NO_ERRORLOG is defined, then XPC_ERROR_LEVEL_NONE is always
 *    returned, for extra speed.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *
 *//*-------------------------------------------------------------------------*/

xpc_errlevel_t
xpc_errlevel (void)
{
   init_option_errlevel();             /* only has an effect once, at maximum */
   return gs_Error_Level;
}

/******************************************************************************
 * xpc_option_errlevel()
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
 *    -  errorlogging_test_02_08()
 *
 *//*-------------------------------------------------------------------------*/

xpc_errlevel_t
xpc_option_errlevel (const char * cmdswitch)
{
   xpc_errlevel_t result = XPC_ERROR_LEVEL_NOT_APPLICABLE;
   if (not_nullptr(cmdswitch))
   {
      if (strcmp(cmdswitch, CMD(_QUIET)) == 0)
         result = XPC_ERROR_LEVEL_NONE;
      if (strcmp(cmdswitch, CMD(_SILENT)) == 0)          /* XPC CUT option    */
         result = XPC_ERROR_LEVEL_NONE;
      else if (strcmp(cmdswitch, CMD(_DAEMON)) == 0)
         result = XPC_ERROR_LEVEL_NONE;
      else if (strcmp(cmdswitch, CMD(_ERROR)) == 0)
         result = XPC_ERROR_LEVEL_ERRORS;
      else if (strcmp(cmdswitch, CMD(_ERRORS)) == 0)
         result = XPC_ERROR_LEVEL_ERRORS;
      else if (strcmp(cmdswitch, CMD(_WARN)) == 0)
         result = XPC_ERROR_LEVEL_WARNINGS;
      else if (strcmp(cmdswitch, CMD(_WARNINGS)) == 0)
         result = XPC_ERROR_LEVEL_WARNINGS;
      else if (strcmp(cmdswitch, CMD(_INFO)) == 0)
         result = XPC_ERROR_LEVEL_INFO;
      else if (strcmp(cmdswitch, CMD(_INFORMATION)) == 0)
         result = XPC_ERROR_LEVEL_INFO;
      else if (strcmp(cmdswitch, CMD(_ALL)) == 0)
         result = XPC_ERROR_LEVEL_ALL;
      else if (strcmp(cmdswitch, CMD(_VERBOSE)) == 0)
         result = XPC_ERROR_LEVEL_ALL;
      else if (strcmp(cmdswitch, CMD(_SDEBUG)) == 0)
#ifdef DEBUG
         result = XPC_ERROR_LEVEL_INFO;
#else
         result = XPC_ERROR_LEVEL_ERRORS;
#endif
   }
   return result;
}

/******************************************************************************
 * xpc_errlevel_string()
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
 *    -  errorlogging_test_02_11()
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_errlevel_string (xpc_errlevel_t errlev)
{
   const char * elstring = _("unknown");
   switch (errlev)
   {
      case XPC_ERROR_LEVEL_ERRORS:

         elstring = _(_ERRORS);
         break;

      case XPC_ERROR_LEVEL_WARNINGS:

         elstring = _(_WARNINGS);
         break;

      case XPC_ERROR_LEVEL_INFO:

         elstring = _(_INFO);
         break;

      case XPC_ERROR_LEVEL_ALL:

         elstring = _(_VERBOSE);
         break;

      case XPC_ERROR_LEVEL_NONE:

         elstring = _(_QUIET);
         break;

      case XPC_ERROR_LEVEL_NOT_APPLICABLE:

         elstring = _(_NOT_APPLICABLE);
         break;
   }
   return elstring;
}

/******************************************************************************
 * xpc_showerrlevel()
 *------------------------------------------------------------------------*//**
 *
 *    Displays the current debug level as an option string, with hyphens.
 *
 *    The string is obtained by using xpc_errlevel() to get the current
 *    error-level, and then converting this level to a string using
 *    xpc_errlevel_string().
 *
 *    This function shows the level regardless of what it is, except for the
 *    "none" level.
 *
 * \unittests
 *    -  errorlogging_test_02_12()
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_showerrlevel (void)
{
   xpc_errlevel_t level = xpc_errlevel();
   if (level != XPC_ERROR_LEVEL_NONE)
   {
      const char * dbgstring = xpc_errlevel_string(level);
      fprintf
      (
         xpc_logfile(), ERRL_FMT_EXT_MESSAGE,
         xpc_usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO,
         _("error level"), dbgstring
      );
   }
}

/******************************************************************************
 * xpc_shownothing()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller should not show any console messages of any
 *    kind.
 *
 *    This error-level is set from the command line using the "--quiet"
 *    option switch.  The XPCCUT (unit-test) library's --silent option also
 *    causes this setting.
 *
 *    Only output done though the functions in the errorlogging module is
 *    amenable to the application of the error-level code.  The caller is
 *    perfectly free to print messages without regard to the error-level.
 *
 *    This function is the first of a family of error-level checking
 *    functions.  This small family of functions allows the caller to see if
 *    showing a message is permitted, based on the current error level.
 *
 * \return
 *    Returns 'true' if the error level is XPC_ERROR_LEVEL_NONE.
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *---------------------------------------------------------------------------*/

cbool_t
xpc_shownothing (void)
{
   return xpc_errlevel() == XPC_ERROR_LEVEL_NONE;
}

/******************************************************************************
 * xpc_showerrors()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller should show an error message.
 *
 *    An error message is one that causes some kind of inability to
 *    function.  Error messages are tagged by a "?" (question mark)
 *    character.
 *
 *    Error messages can be shown conditionally by using the
 *    "xpc_errprint" family of functions [xpc_errprint(),
 *    xpc_errprintex(), xpc_errprint_func(), which is a macro, and
 *    xpc_errprintf()].
 *
 *    If these functions are not sufficient, or not desired, then
 *    xpc_showerrors() can be tested to decide on some other kind of
 *    error-display mechanism.
 *
 *    Error messages are allowed by default.  The user can also supply the
 *    "--error" option switch on the command line.
 *
 * \return
 *    'true' if errors are allowed to be shown.  If not, then the caller
 *    means to show absolutely no output text (as in a daemon).
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_showerrors (void)
{
#ifdef XPC_NO_ERRORLOG
   return false;
#else
   return xpc_errlevel() >= XPC_ERROR_LEVEL_ERRORS;
#endif
}

/******************************************************************************
 * xpc_showwarnings ()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller should show a warning message.
 *
 *    A warning message is one that indicates an unexpected condition that
 *    is not erroneous.  Or it indicates a condition to which special
 *    attention should be paid by the user.  Warning messages are tagged by
 *    a "!" (exclamation point) character.
 *
 *    Warning messages can be shown conditionally by using the
 *    "xpc_warnprint" family of functions [xpc_warnprint(),
 *    xpc_warnprintex(), xpc_warnprint_func(), which is a macro, and
 *    xpc_warnprintf()].
 *
 *    If these functions are not sufficient, or not desired, then
 *    xpc_showwarnings () can be tested to decide on some other kind of
 *    warning-display mechanism.
 *
 *    The user can supply the "--warn" option switch on the command line in
 *    order to enable the display of warnings.  The display of warnings is
 *    off by default.
 *
 * \return
 *    'true' if warnings and errors are allowed to be shown.
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_showwarnings  (void)
{
   return xpc_errlevel() >= XPC_ERROR_LEVEL_WARNINGS;
}

/******************************************************************************
 * xpc_showinfo()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller should show an informational message.
 *
 *    An informational message is one that a user normally does not need to
 *    see.  However, if the user wants to verify the progress of the
 *    application more closely without debugging, the user can provide the
 *    "--info" switch on the command-line, and more messages will be shown.
 *    Informational messages are tagged with an "*" (asterisk).
 *
 *    Info messages can be shown conditionally by using the "xpc_infoprint"
 *    family of functions [xpc_infoprint(), xpc_infoprintex(),
 *    xpc_infoprint_func(), which is a macro, and xpc_infoprintf()].
 *
 *    If these functions are not sufficient, or not desired, then
 *    xpc_showinfo() can be tested to decide on some other kind of
 *    information-display mechanism.
 *
 *    The user can supply the "--info" option switch on the command line in
 *    order to enable the display of info.  The display of info is
 *    off by default.
 *
 * \return
 *    'true' if information, warnings, and errors can be shown.
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_showinfo (void)
{
   return xpc_errlevel() >= XPC_ERROR_LEVEL_INFO;
}

/******************************************************************************
 * xpc_showall()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller can show all messages.
 *
 *    The only ways to show such messages are to use print() [no other forms
 *    of this function are provided] or to wrap more conventional print
 *    functions [e.g. fprintf()] in a test of xpc_showall().
 *
 *    The option switches "--verbose" or "--all" enable the showing of all
 *    messages.
 *
 * \return
 *    'true' if all messages and tested output can be shown.
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_showall (void)
{
   return xpc_errlevel() >= XPC_ERROR_LEVEL_ALL;
}

/******************************************************************************
 * xpc_showdebug()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the caller can show debug messages.
 *
 *    The option switch "--debug" enables this test, if the application is
 *    compiled for debugging.  Otherwise, the option is equivalent to
 *    "--error".
 *
 * \return
 *    If compiled for debugging, this function acts like xpc_showinfo().
 *    Otherwise, it always returns false.
 *
 * \unittests
 *    -  errorlogging_test_02_13()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_showdebug (void)
{
#if defined DEBUG && ! defined XPC_NO_ERRORLOG
   return xpc_showinfo();
#else
   return false;
#endif
}

/******************************************************************************
 * xpc_showerr_version()
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
 *    -  errorlogging_test_02_14()
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_showerr_version (void)
{
#ifdef DEBUG
   xpc_infoprintf
   (
      "%s: %s %s \n%s.\n",
      _("Approximate build time"), __DATE__,  __TIME__,
      _("Compiled with debugging information")
   );
#else
   xpc_infoprint(_("Release version"));
#endif

#ifdef XPC_NO_ERRORLOG
   xpc_infoprint(_("Compiled with disabled error-logging functions."));
#endif
}

/******************************************************************************
 * xpc_error_help()
 *------------------------------------------------------------------------*//**
 *
 *    This function emits a brief summary of the error options, as help
 *    text.  It is not static, just in case the caller wants to handle the
 *    --help option on its own.
 *
 *    Note that, although translation is allowed, the names of the options
 *    must currently always be in the English form.
 *
 * \unittests
 *    N/A.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_error_help (void)
{
   static cbool_t g_xpc_error_help_done = false;
   const char * const helptext =
   M_(
"Error-logging options:\n"
"\n"
"--quiet             Show no messages, not even errors.\n"
"--silent            The same as --quiet; this option matches the --silent.\n"
"                    option in the XPC unit-test library (libxpccut).\n"
"--errors            Show error messages.\n"
"--warnings          Show errors and warnings.\n"
"--warn              Same as --warnings.\n"
"--info              Show errors, warnings, and information.\n"
"--verbose           Show all messages.\n"
"--all               Same as --verbose.\n"
"--stdout            Change the log file from stderr to stdout.\n"
"--stderr            Use stderr as the log file [this is the default].\n"
"--log file          Change the log file to the given file.  The file\n"
"                    is truncated if it exists, erasing the previous\n"
"                    contents.\n"
"--append file       Same as --log, except that any existing file is\n"
"                    appended, not replaced.\n"
"                    ('stdout' and 'stderr' are valid filenames here.)\n"
"--syslog            Redirect output to the system log.  Normally, this\n"
"                    is needed only by daemons, which set it themselves\n"
"                    indirectly in the xpc_daemonize() function.  [The\n"
"                    default is --no-syslog.\n"
"--no-syslog         Output the log to the normal place, stderr.\n"
"--buffered          Buffer the output log.  Normally, stdout and files\n"
"                    are already buffered, while, stderr is not.  Use\n"
"                    this option if the log-printing functions cause a\n"
"                    segmentation fault.\n"
"--unbuffered        Make the output log unbuffered.\n"
"--color             Write the tag characters (?!*>) in color [default].\n"
"                    [Not supported in Windows].\n"
"--mono              Write tag characters without color.  This option\n"
"                    will avoid ANSI escape sequences in log files.\n"
"--no-color          Synonym for --mono.  Also --nocolor and -nc.\n"
"--timestamps        Enable adding time-stamps to the errorlogging output.\n"
"--time-stamps       The time stamp goes between the tag character and the\n"
"                    message, and has the format '[seconds.microseconds]'.\n"
"--timestamps rebase The same as without 'rebase', but subtract the initial\n"
"--time-stamps rebase time in seconds, to get an easier-to-grok number.\n"
"--synch             Lock the output (to avoid intermixing of messages\n"
"                    from different threads.) [The default is --no-synch].\n"
"--no-synch          Do not synchronize the stderr output stream used for\n"
"                    logging.\n"
"--daemon            Same as quiet.  This option (if provided) is\n"
"                    usually coded to cause operation as a daemon or\n"
"                    a service.  Put other options before it to keep\n"
"                    logging active, but be aware that stdout and stderr\n"
"                    are closed in daemon mode.\n"
"--help              Show this help banner.\n"
"--version           Show basic version information.\n"
"\n"
   );
   if (! g_xpc_error_help_done)
   {
      fprintf(stdout, "%s", helptext);
      g_xpc_error_help_done = true;
   }
}

/******************************************************************************
 * xpc_application_help()
 *------------------------------------------------------------------------*//**
 *
 *    This function emits a brief summary of the error options, as help
 *    text.  It is not static, just in case the caller wants to handle the
 *    --help option on its own.
 *
 *    Note that, although translation is allowed, the names of the options
 *    must currently always be in the English form.
 *
 * \param helptext
 *    Provides the help text to be printed.
 *
 * \unittests
 *    N/A.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_application_help (const char * helptext)
{
   fprintf(stdout, "%s", helptext);
   fprintf(stdout, "\n");
}

/******************************************************************************
 * gs_Log_to_Syslog [static]
 *------------------------------------------------------------------------*//**
 *
 *    Holds the state of the --syslog option.
 *
 * \private
 *    This item is accessed for private usage, although it can be set from
 *    the command line or by using the xpc_syslogging_set() function.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t gs_Log_to_Syslog = false;

/******************************************************************************
 * xpc_syslogging_set()
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
 *          performed by calling the xpc_open_logfile() function.  The only
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
 *    This function is called by xpc_daemonize() in the xpc_daemonize.c module.
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
 *    -  errorlogging_test_02_15()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syslogging_set (cbool_t flag)
{
   gs_Log_to_Syslog = flag;               /* enable this option               */
   return xpc_usecolor_set(false);        /* don't want colors in system log  */
}

/******************************************************************************
 * xpc_syslogging()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the current value of gs_Log_to_Syslog.
 *
 *    This function is useful in unit-testing to save the current value for
 *    later restoration.
 *
 * \unittests
 *    -  errorlogging_test_02_15()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syslogging (void)
{
   return gs_Log_to_Syslog;
}

/******************************************************************************
 * xpc_get_priority()
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
 *    This function is a helper for the msgtag() and va_tag() functions, but
 *    we've exposed it to support a C++ semi-wrapper class.
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
xpc_get_priority (xpc_errlevel_t errlev)
{

#ifdef POSIX

   int priority = POSIX_ERROR;
   switch (errlev)
   {
   case XPC_ERROR_LEVEL_NONE:           priority = LOG_EMERG;   break;
   case XPC_ERROR_LEVEL_ERRORS:         priority = LOG_ERR;     break;
   case XPC_ERROR_LEVEL_WARNINGS:       priority = LOG_WARNING; break;
   case XPC_ERROR_LEVEL_INFO:           priority = LOG_INFO;    break;
   case XPC_ERROR_LEVEL_ALL:            priority = LOG_INFO;    break;
   case XPC_ERROR_LEVEL_NOT_APPLICABLE: priority = LOG_EMERG;   break;
   }

#else

   int priority = (int) errlev;

#endif

   return priority;
}

/******************************************************************************
 * msgtag() [static]
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

static void
msgtag
(
   xpc_errlevel_t errlev,
   const char * tag,
   const char * errmsg
)
{
   if (xpc_errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      if (is_nullptr(tag))                            /* programmer goofed    */
         tag = _("programmer");

      if (is_nullptr(errmsg))                         /* programmer goofed    */
         errmsg = _("missing error message");

      if (gs_Log_to_Syslog)
      {
         int priority = xpc_get_priority(errlev);

#ifdef POSIX
         syslog(priority, _(ERRL_FMT_BASIC_MESSAGE), tag, errmsg);
#else
         fprintf
         (
            xpc_logfile(),
            "%s %s (%s %s)\n",                  /* ERRL_FMT_PRIORITY_MESSAGE  */
            tag, errmsg, _("priority"),
            xpc_errlevel_string((xpc_errlevel_t) priority)
         );
#endif

      }
      else
      {
         if (synch_lock())                            /* --synch              */
         {
            if (xpc_timestamps())
            {
               time_t seconds;
               suseconds_t microseconds;
               struct timeval ts;
               xpc_get_microseconds(&ts);
               seconds = ts.tv_sec;
               microseconds = ts.tv_usec;
               if (gs_TimeStamps_Base > 0)
                  seconds -= gs_TimeStamps_Base;

               fprintf
               (
                  xpc_logfile(), ERRL_FMT_TIMESTAMP_MESSAGE, tag,
                  (int) seconds, (int) microseconds, errmsg
               );
            }
            else
               fprintf(xpc_logfile(), ERRL_FMT_BASIC_MESSAGE, tag, errmsg);

            synch_unlock();
         }
      }
   }
}

/******************************************************************************
 * va_tag() [static]
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
 *    A single-character tag string (e.g. "?")
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

static void
va_tag
(
   xpc_errlevel_t errlev,
   const char * tag,
   const char * fmt,
   va_list val
)
{
   if (xpc_errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      if (is_nullptr(tag) || is_nullptr(fmt))         /* programmer goofed    */
         xpc_errprint_func(_("null tag or format"));
      else
      {
         if (gs_Log_to_Syslog)
         {
#ifdef POSIX
            int priority = xpc_get_priority(errlev);
            vsyslog(priority, fmt, val);
#else
            vfprintf(xpc_logfile(), fmt, val);
#endif
         }
         else
         {
            FILE * fp = xpc_logfile();
            fprintf(fp, "%s ", tag);
            vfprintf(fp, fmt, val);
            fprintf(fp, "\n");                  /* consistent w/other calls   */
            fflush(fp);
         }
      }
   }
}

#ifdef DOCUMENT_VA_MSGTAG

/******************************************************************************
 * va_msgtag() [static]
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

static void
va_msgtag
(
   xpc_errlevel_t errlev,
   const char * tag,
   const char * errmsg,
   va_list val
)
{
   if (xpc_errlevel() > XPC_ERROR_LEVEL_NONE)
   {
      if (is_nullptr(tag) || is_nullptr(fmt))         /* programmer goofed    */
         xpc_errprint(_("tag or format is null in error message"));
      else
      {
         int size = 256;                              /* starting size        */
         char * p = malloc(size);
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
            xpc_errprint(_("failed to allocate in va_msgtag()"));
      }
   }
}

#endif      // DOCUMENT_VA_MSGTAG

/******************************************************************************
 * These disabling macros replace their respective functions, and must not
 * be defined internally in errorlogging.c, so we must force these macros to
 * be undefined at this point.
 *----------------------------------------------------------------------------*/

#ifdef XPC_NO_ERRORLOG__DISABLED_AS_A_TEST

/******************************************************************************
 * xpc_no_op()
 *------------------------------------------------------------------------*//**
 *
 *    A inline function to provide a replacement for the errprint functions
 *    when XPC_NO_ERRORLOG (--disable-errorlog) is specified.
 *
 *    This function is implemented as an inline function in the
 *    errorlogging.h module.
 *
 * \todo
 *    Not sure that we really need this no-op function now.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_no_op ()
{
   static int i = 0;
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_errprint()
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
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_errprint (const char * errmsg)
{
   if (xpc_showerrors())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_ERRORS,
         xpc_usecolor() ? COLOR_STR_ERROR : ERRL_STR_ERROR, errmsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_errprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatted error message string.
 *
 *    Similar to xpc_errprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_errprintf (const char * fmt, ...)
{
   if (xpc_showerrors())
   {
      if (not_nullptr(fmt))
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_ERRORS,
            xpc_usecolor() ? COLOR_STR_ERROR : ERRL_STR_ERROR,
            fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * concat_buffer() [static]
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
 * \param msg
 *    The main message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \return
 *    The address of the heap-allocated buffer is returned.
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static const char *
concat_buffer (const char * msg, const char * label)
{
   const char * result = nullptr;
   size_t length;                                     /* weird                */
   if (is_nullptr(msg))
      msg = _("missing error message");               /* programmer goofed    */

   if (is_nullptr(label))
      label = _("missing label");                     /* programmer goofed    */

   length = strlen(msg) + strlen(label) + strlen(ERRL_STR_EXTRA);
   if (length > 0)
   {
      char * msgbuffer = malloc(length+32);           /* add slop             */
      if (not_nullptr(msgbuffer))
      {
         if (xpc_usecolor())
         {
            strcpy(msgbuffer, COLOR_STR_LABEL_START); /* turn on the color    */
            strcat(msgbuffer, label);                 /* object portion       */
            strcat(msgbuffer, ERRL_STR_EXTRA);        /* ': '                 */
            strcat(msgbuffer, COLOR_STR_END);         /* turn off color       */
         }
         else
         {
            strcpy(msgbuffer, label);                 /* object portion       */
            strcat(msgbuffer, ERRL_STR_EXTRA);        /* ': '                 */
         }
         strcat(msgbuffer, msg);                      /* message portion      */
         result = msgbuffer;                          /* object:  message     */
      }
      else                                            /* malloc() goofed      */
         xpc_errprintex("malloc()", _("failed"));     /* NOT ..._printex()!!! */
   }
   return result;
}

/******************************************************************************
 * free_concat_buffer() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Frees the buffer allocated by concat_buffer().
 *
 *    This function merely checks the pointer, and then deletes the memory
 *    it points to, if not null.
 *
 * \param buffer
 *    A buffer allocated by concat_buffer().
 *
 * \private
 *    This function frees a buffer created by xpc_errprintex(),
 *    xpc_warnprintex(), or xpc_infoprintex().
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static void
free_concat_buffer (const char * buffer)
{
   if (not_nullptr(buffer))
      free((void *) (char *) buffer);
}

/******************************************************************************
 * xpc_errprintex()
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
 *    testing xpc_showerrors() here instead.  [We now have a macro,
 *    xpc_errprint_func(), that fills in the 'label' parameter using the
 *    '__func__' macro.  See how it is used in the rest of this module.]
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_errprintex (const char * errmsg, const char * label)
{
   if (xpc_showerrors())                     // ca 2010-09-12 !!!
   {
      if (not_nullptr(label))
      {
         const char * msgbuffer = concat_buffer(errmsg, label);
         if (not_nullptr(msgbuffer))
         {
            xpc_errprint(msgbuffer);
            free_concat_buffer(msgbuffer);
         }
         else
            xpc_errprint(errmsg);
      }
      else
         xpc_errprint(errmsg);
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_warnprint()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a warning message.
 *
 *    Similar to xpc_errprint(), but presents a warning message instead.
 *
 *    This function is used in reporting an unexpected event or parameter
 *    value that does not affect the application in any dire manner.
 *
 * \param warnmsg
 *    The null-terminated warning message string.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_warnprint (const char * warnmsg)
{
   if (xpc_showwarnings())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_WARNINGS,
         xpc_usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN, warnmsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_warnprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting warning message.
 *
 *    Similar to xpc_warnprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    List of parameters that match the format.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_warnprintf (const char * fmt, ...)
{
   if (xpc_showwarnings())
   {
      if (not_nullptr(fmt))
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_WARNINGS,
            xpc_usecolor() ? COLOR_STR_WARN : ERRL_STR_WARN,
            fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_warnprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a warning message consisting of a main message and a secondary
 *    string.
 *
 *    Similar to xpc_errprintex(), but presents a warning message
 *    instead.  It assembles a message composed of two parts.  It is used in
 *    reporting both a warning and some parameter related to the warning.
 *
 * \param warnmsg
 *    The main warning message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_warnprintex (const char * warnmsg, const char * label)
{
   if (xpc_showwarnings ())
   {
      const char * msgbuffer = concat_buffer(warnmsg, label);
      if (not_nullptr(msgbuffer))
      {
         (void) xpc_warnprint(msgbuffer);
         free_concat_buffer(msgbuffer);
      }
      else
         xpc_warnprint(warnmsg);
   }
   else
      xpc_warnprint(warnmsg);
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_infoprint()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message.
 *
 *    Similar to xpc_errprint(), but presents an informational message
 *    instead.
 *
 * \param infomsg
 *    Provides the null-terminated info message string.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_infoprint (const char * infomsg)
{
   if (xpc_showinfo())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_INFO,
         xpc_usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO, infomsg
      );
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_infoprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message.
 *
 *    Similar to xpc_infoprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \usage
 *    -# Each line of text needs to be terminated with a newline sequence
 *       ("\n"), in order to make the output look good for multiline output.
 *    -# Each line of text after the first needs to start with two spaces,
 *       in order to look good for multiline output.
 *
 *    This function is similar to xpc_infoprintml().  The only difference
 *    between xpc_infoprintf() and xpc_infoprintml() is that the latter
 *    inserts a tag character and space automatically at the beginning of
 *    each line.
 *
 * \param fmt
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    Provides the list of parameters that match the format.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_infoprintf (const char * fmt, ...)
{
   if (xpc_showinfo())
   {
      if (not_nullptr(fmt))
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_INFO,
            xpc_usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO, fmt, val
         );
         va_end(val);
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_infoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message consisting of a main message and a
 *    secondary string.
 *
 *    Similar to xpc_errprintex(), but presents an informational message
 *    instead.
 *
 * \param infomsg
 *    Provides the main info message string.
 *
 * \param label
 *    A qualifier string, often a file or function name.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_infoprintex (const char * infomsg, const char * label)
{
   if (xpc_showinfo())
   {
      const char * msgbuffer = concat_buffer(infomsg, label);
      if (not_nullptr(msgbuffer))
      {
         xpc_infoprint(msgbuffer);
         free_concat_buffer(msgbuffer);
      }
      else
         xpc_infoprint(infomsg);
   }
   else
      xpc_infoprint(infomsg);
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_infomark_format()
 *------------------------------------------------------------------------*//**
 *
 *    A private function to massage a format string to add info-marker
 *    characters.
 *
 *    See xpc_infoprintml() for more information.  A string is allocated that
 *    that function must free.
 *
 * \private
 *    A helper function in the errorlogging.c module.
 *
 * \param fmt
 *    Provides the printf-style formatting string to use.
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
 * \todo
 *    How about a multi-line output function that automatically inserts
 *    newlines and indentation?
 *
 * \unittests
 *    No direct unit-test possible in a static C function.
 *
 *//*-------------------------------------------------------------------------*/

static const char *
xpc_infomark_format (const char * fmt)
{
   char * result = nullptr;
   size_t length = 0;
   if (not_nullptr(fmt))
      length = strlen(fmt);

   if (length > 0)                           /* we're good to go!             */
   {
      size_t added = strlen(xpc_infomark())+1;  /* added space per line       */
      int nlcount = 0;                       /* prepare to count newlines     */
      char * p = (char *) fmt;
      while (1)
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
      result = malloc(length + added);       /* freed in xpc_infoprintml()    */
      if (not_nullptr(result))
      {
         ptrdiff_t length;                   /* used in the while loop        */
         int i = 0;
         const char * p2 = fmt;              /* init the start pointer        */
         char * q;                           /* this is the 'next' pointer    */
         *result = 0;                        /* start with empty string       */
         while (nlcount > 0)
         {
            if (i++ > 0)                     /* don't do it for first line    */
            {
               strcat(result, xpc_infomark());  /* can be 1 or more chars     */
               strcat(result, " ");             /* add a spacer               */
            }
            q = strchr(p2, '\n');             /* find next newline             */
            if (is_nullptr(q))               /* not found                     */
               q = strchr(p2, 0);             /* find next null character      */

            length = q - p2;
            if (length > 0)                  /* no text before newline        */
               (void) strncat(result, p2, length);

            if (nlcount > 1)                 /* don't terminate last line     */
               strcat(result, "\n");

            nlcount--;
            p2 = q + 1;
         }
      }
   }
   else
      xpc_errprint_func(_("invalid printf() format"));

   return result;
}

/******************************************************************************
 * xpc_infoprintml()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message that spans multiple
 *    lines.
 *
 *    It inserts the info-marker(), saving the caller a lot of trouble.
 *
 *    The name is short for "xpc_infoprint multi-line".
 *
 *    Similar to xpc_infoprintf(), it accepts variable formatting and
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
            xpc_infomark(),
            xpc_infomark(), sizeof(xpc_app_arguments_t),
            xpc_infomark(), xpc_boolean_string(runasdaemon)
         );
\endverbatim
 *
 *    with the call:
 *
\verbatim
         xpc_infoprintml
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
 *    This function is similar to xpc_infoprintf().  The only difference
 *    between xpc_infoprintf() and xpc_infoprintml() is that the latter
 *    inserts a tag character and space automatically at the beginning of
 *    each line.
 *
 * \note
 *    A trick is used below to avoid this warning caused by replacing fmt
 *    with newfmt:
 *
 *       warning: second parameter of 'va_start' not last named argument
 *
 *    In this trick, we take the original function parameter \a fmt and
 *    assign \a newfmt to it.  This removes the warning.
 *
 *    Are we being too tricky here (i.e. is something going wrong that we
 *    cannot see?)  Don't know.  The code seems to work.
 *
 * \param fmt
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    Provides the list of parameters that match the format.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_NO_ERRORLOG

void
xpc_infoprintml (const char * fmt, ...)
{
   if (xpc_showinfo())
   {
      if (not_nullptr(fmt))
      {
         const char * newfmt = xpc_infomark_format(fmt); /* rip a new one     */
         if (not_nullptr(newfmt))
         {
            va_list val;
            fmt = newfmt;                                /* see Note above    */
            va_start(val, fmt);
            va_tag
            (
               XPC_ERROR_LEVEL_INFO,
               xpc_usecolor() ? COLOR_STR_INFO : ERRL_STR_INFO,
               fmt, val
            );
            va_end(val);
            free((char *) fmt);                          /* free() new one    */
         }
      }
   }
}

#endif   /* XPC_NO_ERRORLOG   */

/******************************************************************************
 * xpc_dbginfoprint() and xpc_dbginfoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    One of many functions active only when compiled for debugging.
 *
 *    The functions are xpc_dbginfoprint(), xpc_dbginfoprintex(),
 *    xpc_dbginfoprintf(), and xpc_print_debug().
 *
 *    xpc_dbginfoprint() is similar to xpc_infoprint(), but performs only if
 *    DEBUG is defined.  Use it when you want to see the message only if
 *    debugging and when --info is specified.
 *
 *    xpc_print_debug() is similar to print(), but performs only if DEBUG is
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
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DEBUG                                             /* DEBUG             */

void
xpc_dbginfoprint (const char * infomsg)
{
   if (xpc_showinfo())
   {
      msgtag
      (
         XPC_ERROR_LEVEL_INFO,
         xpc_usecolor() ? COLOR_STR_DEBUG : ERRL_STR_DEBUG, infomsg
      );
   }
}

/******************************************************************************
 * xpc_dbginfoprintf()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a printf()-formatting informational message, if debugging is
 *    enabled.
 *
 *    Similar to xpc_infoprint(), but accepts variable formatting and
 *    arguments.  It uses va_tag() to display or log the message.
 *
 * \param fmt
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    Provides the list of parameters that match the format.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_dbginfoprintf (const char * fmt, ...)
{
   if (xpc_showinfo())
   {
      if (not_nullptr(fmt))
      {
         va_list val;
         va_start(val, fmt);
         va_tag
         (
            XPC_ERROR_LEVEL_INFO,
            xpc_usecolor() ? COLOR_STR_DEBUG : ERRL_STR_DEBUG, fmt, val
         );
         va_end(val);
      }
   }
}

/******************************************************************************
 * xpc_print_debug()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an unconditional informational message, if debugging is enabled.
 *
 *    Basically, it is simply a debug-enabled variant of print().
 *
 * \param infomsg
 *    Provides the main info message string.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_print_debug (const char * infomsg)
{
   xpc_print(infomsg);
}

/******************************************************************************
 * xpc_dbginfoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Logs an informational message consisting of a main message and a
 *    secondary string.
 *
 *    Similar to xpc_errprintex(), but presents an informational message
 *    instead, and only when the application is built with debugging
 *    enabled.
 *
 * \param infomsg
 *    The main info message string.
 *
 * \param label
 *    A qualifier, often a file or function name.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_dbginfoprintex (const char * infomsg, const char * label)
{
   if (xpc_showinfo())
   {
      const char * msgbuffer = concat_buffer(infomsg, label);
      if (not_nullptr(msgbuffer))
      {
         xpc_dbginfoprint(msgbuffer);
         free_concat_buffer(msgbuffer);
      }
      else
         xpc_dbginfoprint(infomsg);
   }
   else
      xpc_dbginfoprint(infomsg);
}

#else                                                    /* ! DEBUG           */

void
xpc_dbginfoprintf (const char * fmt, ...)
{
   if (not_nullptr(fmt))                                 /* avoid warning msg */
   {
      /* nothing to do */
   }
}

#endif                                                   /* DEBUG             */

/******************************************************************************
 * xpc_print()
 *------------------------------------------------------------------------*//**
 *
 *    Logs a message as information, no matter what the error-level.
 *
 *    Similar to xpc_infoprint(), but prints as long as the debug level is
 *    not XPC_ERROR_LEVEL_NONE.
 *
 *    Note that we don't need to define a printf() function (grin).
 *
 * \param infomsg
 *    The main info message string.
 *
 * \unittests
 *    -  errorlogging_test_01_01().  This test is a "smoke test".
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_print (const char * infomsg)
{
   msgtag
   (
      XPC_ERROR_LEVEL_INFO,
      xpc_usecolor() ? COLOR_STR_PRINT : ERRL_STR_PRINT, infomsg
   );
}

/******************************************************************************
 * xpc_lkprintf()
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
 *    Provides the printf()-style format string.
 *
 * \param ...
 *    Provides the list of parameters that match the format.
 *
 * \unittests
 *    -  See errorlogging_test_05_01() for a potential test.
 *    -  Tests that check output to a log file could be written, but have
 *       not yet.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_lkprintf (const char * fmt, ...)
{
   if (not_nullptr(fmt))
   {
      va_list val;
      va_start(val, fmt);
      if (synch_lock())
      {
         vfprintf(xpc_logfile(), fmt, val);
         synch_unlock();
      }
      va_end(val);
   }
}

/******************************************************************************
 * xpc_format_error_string()
 *------------------------------------------------------------------------*//**
 *
 *    Formats an error-log message for logging.
 *
 *    This function is a helper function used in xpc_strerrnoprintex() and
 *    xpc_strerrprintex().
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
 *    from error-generating functions other than xpc_strerrnoprintex() and
 *    xpc_strerrprintex().
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
xpc_format_error_string
(
   const char * errmsg,
   const char * label,
   const char * syserr
)
{
   size_t length;
   size_t seplen = strlen(ERRL_STR_SEPARATOR) + strlen(ERRL_STR_EXTRA);
   cbool_t havesyserr = not_nullptr(syserr);
   if (is_nullptr(errmsg))
      errmsg = _("missing error message");

   if (is_nullptr(label))
      label = " ";

   if (! havesyserr)
      syserr = " ";

   length = strlen(errmsg) + seplen + strlen(label) + strlen(syserr) + 2;
   if (length > 0)
   {
      char * msgbuffer = malloc(length + 32);                  /* add slop    */
      if (not_nullptr(msgbuffer))
      {
         strcpy(msgbuffer, label);
         strcat(msgbuffer, ERRL_STR_EXTRA);
         strcat(msgbuffer, errmsg);
         if (havesyserr)
         {
            strcat(msgbuffer, ERRL_STR_SEPARATOR);
            strcat(msgbuffer, "(");
            strcat(msgbuffer, syserr);
            strcat(msgbuffer, ")");
         }
#ifndef XPC_NO_ERRORLOG
         xpc_errprint(msgbuffer);
#endif
         free(msgbuffer);
      }
#ifndef XPC_NO_ERRORLOG
      else
         xpc_errprintex("malloc()", _("failed"));
#endif
   }
}

/******************************************************************************
 * xpc_strerrnoprint()
 *------------------------------------------------------------------------*//**
 *
 *    Combines a string and a system error message, logs them, and returns a
 *    POSIX error code.
 *
 *    This function simply calls xpc_strerrnoprintex() with the "label"
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
 *    The value returned by xpc_strerrnoprintex() is returned.
 *
 * \unittests
 *    -  Tests that check output to a log file perhaps could be written, but
 *       have not yet.
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_strerrnoprint (const char * errmsg)
{
   return xpc_strerrnoprintex(errmsg, nullptr);
}

/******************************************************************************
 * xpc_strerrnoprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Combines two strings and a system error message, logs them, and
 *    returns a POSIX error code.
 *
 * \threadsafe
 *    This function allocates a stack buffer for the message, and passes it
 *    to the threadsafe function xpc_getlasterror_text_r().
 *
 * \param errmsg
 *    Provides The main error message string.
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
xpc_strerrnoprintex (const char * errmsg, const char * label)
{
   int result = xpc_getlasterror();
   const char * errormessage = _("success");
   char buffer[XPC_STRERROR_BUFLEN];         /* for aiding thread-safety   */
   if (not_posix_success(result))
   {
      errormessage = xpc_getlasterror_text_r(result, buffer, sizeof(buffer));
   }
   else
      errormessage = _("errno is 0, error in user code");

   xpc_format_error_string(errmsg, label, errormessage);
   return result;
}

/******************************************************************************
 * xpc_strerrprint()
 *------------------------------------------------------------------------*//**
 *
 *    Assembles a message with a POSIX error number, and logs it.
 *
 *    See the xpc_strerrprintex() function, which is used to implement the
 *    xpc_strerrprint() function.
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
xpc_strerrprint (const char * errmsg, int errnum)
{
   xpc_strerrprintex(errmsg, nullptr, errnum);
}

/******************************************************************************
 * xpc_strerrprintex()
 *------------------------------------------------------------------------*//**
 *
 *    Assembles a message with a POSIX error number, and logs it.
 *
 *    This function and xpc_strerrprintex() are similar to
 *    xpc_strerrnoprint() and xpc_strerrnoprintex(), but they are provided
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
 *       A qualifier, often a file or function name.
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
xpc_strerrprintex
(
   const char * errmsg,
   const char * label,
   int errnum
)
{
   const char * codeerror = _("success");
   if (not_posix_success(errnum))
   {
      if (not_posix_error(errnum))
      {
         char buffer[XPC_STRERROR_BUFLEN];
         char * errmsg2 = strerror(errnum);              /* not thread-safe   */
         (void) strncpy(buffer, errmsg2, XPC_STRERROR_BUFLEN);
         buffer[XPC_STRERROR_BUFLEN-1] = 0;
         codeerror = &buffer[0];
      }
      else
         codeerror = _("N/A");

      xpc_format_error_string(errmsg, label, codeerror);
   }
   else
      xpc_format_error_string(errmsg, label, codeerror);
}

/******************************************************************************
 * xpc_getlasterror() [POSIX]
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
 *    -  errorlogging_test_02_16()
 *
 *//*-------------------------------------------------------------------------*/

int
xpc_getlasterror (void)
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
 * xpc_getlasterror_text() [POSIX and Win32]
 *------------------------------------------------------------------------*//**
 *
 *    This function retrieves a human-readable error message.
 *
 *    This function is implemented by retrieving the system error number,
 *    and then passing it to xpc_getlasterror_text_ex().
 *
 * \posix
 *    The current value of \a errno is grabbed immediately.
 *
 * \win32
 *    The Windows implementation of this function is in the
 *    errorlogging_w32.c module.
 *
 * \warning
 *    Intervening system calls may change the system error code.  Therefore,
 *    call this function before making other system calls.
 *
 * \linux
 *    In some systems, the errno value is unique to each thread.  We are not
 *    sure if this is the case in Linux.
 *
 * \return
 *    If an error is in force, then this function returns a pointer to the
 *    string corresponding to the error that is in force.  If no error is in
 *    force, then this function returns a null pointer.  In this case, the
 *    programmer ought to look at the code to see why this "error" function
 *    is being called when there is no error.
 *
 * \unittests
 *    -  errorlogging_test_02_16()
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_getlasterror_text (void)
{
   const char * result = nullptr;
   int last_error = xpc_getlasterror();
   if (last_error != 0)
      result = xpc_getlasterror_text_ex(last_error);

   return result;
}

/******************************************************************************
 * xpc_getlasterror_text_ex() [POSIX]
 *------------------------------------------------------------------------*//**
 *
 *    Retrieves a human-readable error message from the operating system.
 *
 *    When an error occurs that sets the global errno, the system usually
 *    provides a function to return an error message based on the error.
 *    The xpc_getlasterror_text_ex() uses that function to obtain the
 *    message.
 *
 *    The message will be stored in a native (byte-based) string.
 *
 *    If there is no system error in force, a null is returned, and the
 *    caller should not show the message (or at least should show a success
 *    message.)
 *
 * \posix
 *    The error string is obtained by the strerr() function, and the message
 *    is immediately constructed from its parts.  If __GNUC__ is defined,
 *    then strerror_r() is used.  Note that The XSI-compliant version of
 *    strerror_r() is provided if:
 *
 *    (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
 *
 * \linux
 *    We think that, based on strerror(3), the XSI version is used by
 *    default in Linux.  (No, testing shows it is not.)
 *
 * \win32
 *    The message is obtained from the operating system based on the message
 *    ID returned by the Win32 GetLastError() function.  It is formatted
 *    into a self-allocated array by the Win32 function FormatMessage().
 *
 * \threadunsafe
 *    The result is stored in a static text buffer.  This is reasonable if
 *    errors are rare.
 *
 * \param errorcode
 *    The errno-style (POSIX) or Win32 system error number.
 *
 * \return
 *    A pointer to the private text buffer is returned.  This buffer is
 *    static, so this function is not thread safe.  The only thing to do to
 *    mitigate this problem is to copy the text immediately to a caller's
 *    buffer, or print it right away.
 *
 * \unittests
 *    -  errorlogging_test_02_16() [indirect test]
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_getlasterror_text_ex (int errorcode)
{
   const size_t textsize = XPC_STRERROR_BUFLEN;
   static char text[XPC_STRERROR_BUFLEN];
   return xpc_getlasterror_text_r(errorcode, text, textsize);
}

/**
 *    The XSI-compliant strerror_r() function returns 0 on success; on
 *    error, -1 is returned and errno is set to indicate the error.
 *
 * \linux
 *    However, under Linux the XSI-compliant version of strerror_r() is \e
 *    not supplied.
 */

#ifdef __GNUC__

#if XSI_STRERROR_R

static const char *
s_strerror_wrapper
(
   int errorcode,    /**< errno-style (POSIX) or Win32 system error number.   */
   char * dest,      /**< The caller's destination for the system message.    */
   size_t destsize   /**< The size of the caller's destination buffer.        */
)
{
   const char * result = nullptr;
   char msg[128];
   int rc = strerror_r(errorcode, msg, sizeof(msg));        /* system message */
   if (not_posix_success(rc))
   {
      snprintf(msg, msgsize, "%s: strerror_r() %s", __func__, _("failed"));
   }
   else
   {
      strncpy(dest, msg, destsize);
      dest[destsize-1] = 0;
      result = dest;
   }
   return result;
}

#else                   /* ! XSI_STRERROR_R  */

/**
 *    The GNU version of strerror_r() doesn't necessarily fill the caller's
 *    buffer -- it may instead just return the pointer to a
 *    statically-allocated message.
 *
 *    This strerror_r() function cannot fail.  At most, it will return
 *    "Unknown error nnn".
 */

static const char *
s_strerror_wrapper
(
   int errorcode,    /**< errno-style (POSIX) or Win32 system error number.   */
   char * dest,      /**< The caller's destination for the system message.    */
   size_t destsize   /**< The size of the caller's destination buffer.        */
)
{
#ifdef __MINGW32__
   const char * tmp = strerror(errorcode);
#else
   char msg[128];
   const char * tmp = strerror_r(errorcode, msg, sizeof(msg));
#endif
   strncpy(dest, tmp, destsize);
   dest[destsize-1] = 0;
   return dest;
}

#endif                  /* XSI_STRERROR_R    */

#else                   /* ! __GNUC__        */

/**
 *    The GNU version of strerror_r() doesn't necessarily fill the caller's
 *    buffer -- it may instead just return the pointer to a
 *    statically-allocated message.
 *
 *    This strerror_r() function cannot fail.  At most, it will return
 *    "Unknown error nnn".
 */

static const char *
s_strerror_wrapper
(
   int errorcode,    /**< errno-style (POSIX) or Win32 system error number.   */
   char * dest,      /**< The caller's destination for the system message.    */
   size_t destsize   /**< The size of the caller's destination buffer.        */
)
{
   char * systemerror = strerror(errorcode);                /* system message */
   (void) xpc_string_n_copy(dest, systemerror, destsize);   /* copy it        */
   return dest;
}

#endif                  /* __GNUC__          */

/******************************************************************************
 * xpc_getlasterror_text_r()
 *------------------------------------------------------------------------*//**
 *
 *    Creates an error message based on a system error code.
 *
 *    This function does not rely on a static buffer, so it is reentrant.
 *    The caller supplies the buffer and guarantees it is accessed by only
 *    one thread.
 *
 *    Use this function where you would otherwise use strerror()
 *    [POSIX/Win32] or FormatMessage() [Win32].
 *
 * \posix
 *    This version of the function uses strerror() to obtain the error message.
 *
 * \gnu
 *    This version of the function uses the GNU extension strerror_r() to
 *    obtain the error message, and may be a little safer.  See "man 3
 *    strerror_r" for a discussion of the XSI-compliant version.
 *
 * \win32
 *    This function uses FormatMessage() to create an error message.  This
 *    implementation is a lot longer than the POSIX or GNU implementations,
 *    but can potentially handle Unicode characters.  See the
 *    errorlogging_w32.c module for the Win32 implementation.
 *
 * \threadsafe
 *    This function is threadsafe in GNU implementations.  It may not be
 *    threadsafe in other UNIX operating system environments.
 *
 * \param errorcode
 *    System error-code for which to get system message.
 *
 * \param text
 *    Destination for null-terminated error message.
 *
 * \param textsize
 *    Provides the size of the destination text buffer.
 *
 * \return
 *    A pointer to the message buffer is returned.  This pointer is always
 *    valid, even if the message cannot be retrieved.  If the parameters are
 *    bad, it points to an internal (static) and constant message; this
 *    message is initialized as per the locale, using GNU gettext, the first
 *    time bad parameters are used.
 *
 * \unittests
 *    -  errorlogging_test_02_16()
 *
 *----------------------------------------------------------------------------*/

const char *
xpc_getlasterror_text_r
(
   int errorcode,
   char * text,
   size_t textsize
)
{
   const char * result;
   if (not_nullptr(text) && (textsize > 0))
   {
      const char * msg = s_strerror_wrapper(errorcode, text, textsize);
      if (strlen(msg) == 0)
      {
         static const char * bad_errormsg = nullptr;
         if (is_nullptr(bad_errormsg))
            bad_errormsg = _("strerror() returned an empty message!");

         result = bad_errormsg;
      }
      result = text;
   }
   else
   {
      static const char * bad_parameters = nullptr;
      if (is_nullptr(bad_parameters))
         bad_parameters = _("bad xpc_getlasterror_text_r() parameters");

      result = bad_parameters;
   }
   return result;
}

/******************************************************************************
 * xpc_buffering_set()
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
 *    -  errorlogging_test_02_17()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_buffering_set (int btype)
{
   cbool_t result = false;
   s_init_logfile();                                     /* guarantee it      */
   if ((btype >= XPC_ERROR_NOT_BUFFERED) && (btype <= XPC_ERROR_LINE_BUFFERED))
   {
      int rcode;
      FILE * fp = xpc_logfile();
      if (not_NULL(fp))
      {
         switch (btype)
         {
         case XPC_ERROR_NOT_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IONBF, 0);
            if (not_posix_success(rcode))
               xpc_errprint_func(_("unbuffered failed"));
            else
            {
               xpc_infoprint_func(_("unbuffered"));
               result = true;
            }
            break;

         case XPC_ERROR_BLOCK_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IOFBF, 0);
            if (not_posix_success(rcode))
               xpc_errprint_func(_("block-buffered failed"));
            else
            {
               xpc_infoprint_func(_("block-buffered"));
               result = true;
            }
            break;

         case XPC_ERROR_LINE_BUFFERED:

            rcode = setvbuf(fp, (char *) nullptr, _IOLBF, 0);
            if (not_posix_success(rcode))
               xpc_errprint_func(_("line-buffered failed"));
            else
            {
               xpc_infoprint_func(_("line buffered specified explicitly"));
               result = true;
            }
            break;
         }
      }
   }
   else
      xpc_errprint_func(_("invalid buffering type"));

   return result;
}

/******************************************************************************
 * xpc_flush_error_log()
 *------------------------------------------------------------------------*//**
 *
 *    Flushes the error-log file.
 *
 *    This function simply passes the result of xpc_logfile() to the
 *    system call fflush().
 *
 * \unittests
 *    Not sure right now how one could test this reliably.  The stderr
 *    stream seems to flush after a relatively small amount of output (on
 *    Linux). TBD.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_flush_error_log (void)
{
   fflush(xpc_logfile());
}

/******************************************************************************
 * xpc_is_thisptr()
 *------------------------------------------------------------------------*//**
 *
 *    Combines the is_thisptr() macro and the xpc_errprint() function.
 *
 *    This function is provided for convenience.  It is used in the
 *    is_thisptr() macro, which makes it easy to check a "this" pointer
 *    and generate an error message containing the name of the offending
 *    function (as provided by the __func__ macro).
 *
 * \usage
 *    Do not call this function directly (although it is not an error to do
 *    so, and might be convenient in some contexts).  Instead, call the
 *    is_thisptr() macro.  This macro defines to a call to this function if
 *    this-pointer checking is enabled; otherwise it reduces to 'true'.
 *
\verbatim
         if (is_thisptr(my_structure_pointer))
         {
            ... // Operate on the pointer called 'my_structure_pointer'
         }
\endverbatim
 *
 *
 * \param pointer
 *    The pointer to be checked for validity.  If "thisptr" support is
 *    disabled (as should be the case if you want to dispense with pointer
 *    checking in C++, or if you really care more about efficiency than
 *    checking bugs), then the pointer parameter is unused.
 *
 * \param funcname
 *    The tag name for the message to be printed.  This is usally the value
 *    of the __func__ macro, and names the function in which the check was
 *    made.
 *
 * \return
 *    Returns 'true' if the \a pointer parameter is valid.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_thisptr (const void * pointer, const char * funcname)
{
   if (not_thisptr(pointer))
   {
      if (is_nullptr(funcname))
         funcname = _("null function name");

      xpc_errprintex(_("null 'this' pointer"), funcname);
      return false;
   }
   else
      return not_nullptr(pointer);           // instead of true;
}

/******************************************************************************
 * xpc_not_nullptr()
 *------------------------------------------------------------------------*//**
 *
 *    Combines the not_nullptr() macro and the xpc_errprint() function.
 *
 *    This function is provided for convenience.  It is used in the
 *    not_nullptr() macro, which makes it easy to check a pointer for null,
 *    and generate an error message containing the name of the offending
 *    function (as provided by the __func__ macro).
 *
 * \usage
 *    Normally, you only need to call the not_nullptr() macro.  This macro
 *    defines to a call to this function if null-pointer checking is
 *    enabled; otherwise it reduces to 'true'.  However, you can call
 *    xpc_not_nullptr() directly if you want to tailor the tag parameter.
 *
\verbatim
         if (not_nullptr(my_structure_pointer))
         {
            ... // Operate on the pointer called 'my_structure_pointer'
         }
\endverbatim
 *
 *    The not_nullptr_2(), not_nullptr_3(), and not_nullptr_4() macros are
 *    implemented in terms of the not_nullptr() macro, so they can be used
 *    in the very same manner.
 *
 * \param pointer
 *    The pointer to be checked for validity.
 *
 * \param funcname
 *    The tag for the message to be printed, usually just the function name.
 *
 * \return
 *    Returns 'true' if the \a pointer parameter is valid.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_not_nullptr (const void * pointer, const char * funcname)
{
   if (is_nullptr(pointer))
   {
      if (is_nullptr(funcname))
         funcname = _("null function name");

      xpc_errprintex(_("null pointer"), funcname);
      return false;
   }
   else
      return true;
}

/******************************************************************************
 * errorlogging.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
