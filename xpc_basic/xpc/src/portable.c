/******************************************************************************
 * portable.c
 *------------------------------------------------------------------------*//**
 *
 * \file          portable.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2005-06-26
 * \updates       2012-08-11
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides some miscellaneous portable functions.
 *
 *    This module provides are number of global items to handle portability
 *    between operating systems.  It is an extension to the simpler
 *    'portability.h'.  It provides:
 *
 *       -# A few global functions providing useful operations for normal
 *          applications and for test applications.
 *       -# System header files as specified by the XPC project's config.h
 *          header file (which contains a number of XPC_HAVE_xxxxx_H macros
 *          with values set when the user runs the "configure" application
 *          for UNIXen [the Win32 config.h has to be edited manually,
 *          unfortunately.] Note: These header files are now provided by the
 *          more general header file 'macros.h'.
 *       -# Additional typedefs not covered by other modules, or that are
 *          general enough to be included here.
 *       -# Additional macros not covered by other modules, or that are
 *          general enough to be included here.
 *
 *    Portability is a bit tricky.  This module represents our learning
 *    curve in portability and the usage of automake.  It supports
 *    portability mostly for internet code.  For more general portability,
 *    see 'macros.h'.
 *
 *    At present, the XPC library differentiates only POSIX and
 *    non-POSIX, and it assumes that non-POSIX means "Win32".  As we gain
 *    experience with more operating systems, this differentiation will
 *    become more refined and more conventional.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/portable.h>              /* functions, macros, and headers      */
#include <xpc/errorlogging.h>          /* included only for xpc_errprint()    */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
XPC_REVISION(portable)

#if XPC_HAVE_LIMITS_H
#include <limits.h>                    /* declares ULONG_MAX                  */
#endif

#if XPC_HAVE_POLL_H
#include <poll.h>                      /* declares struct pollfd              */
#endif

#ifdef WIN32
#include <mmsystem.h>                  /* timeBeginPeriod()                   */
#endif

/******************************************************************************
 * xpc_is_empty_string()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a standard C string to see if it is usable.
 *
 *    This function determines if a string is usable by seeing if its
 *    pointer is null (nullptr), and, if not, if the string has zero (0)
 *    length.
 *
 * \note
 *    Compare this function to xpc_string_is_void() in the xstrings.c
 *    module.  That function also requires that at least one character in
 *    the string be a non-space.  (And it is a little faster, too.)
 *
 * \return
 *    Returns 'true' if the string is empty or its pointer is null.
 *    Otherwise, 'false' is returned.  If 'false' is returned, the caller
 *    should not use the string.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_empty_string
(
   const char * s    /**< The null-terminated string to test for emptiness.   */
)
{
   return (is_nullptr(s)) || (strlen(s) == 0);
}

/******************************************************************************
 * bs_false
 *------------------------------------------------------------------------*//**
 *
 *    Yet another damn Boolean value.
 *
 *//*-------------------------------------------------------------------------*/

static const char * bs_false = "false";      /* provisional initialization    */

/******************************************************************************
 * bs_true
 *------------------------------------------------------------------------*//**
 *
 *    Yet another damn Boolean value.
 *
 *//*-------------------------------------------------------------------------*/

static const char * bs_true  = "true";       /* ditto                         */

/******************************************************************************
 * xpc_boolean_string()
 *------------------------------------------------------------------------*//**
 *
 *    Translates a boolean value into a string.
 *
 *    This function 'translates' a cbool_t (int) value to a string, with
 *    potential support for internationalization.
 *
 * \note
 *    There is some concern about multi-threading here.  If two threads
 *    attempt to call this function for the first time in an application at
 *    about the "same" time, they may both end up calling the gettext()
 *    function simultaneously.  This might cause a problem.  If we set the
 *    is_initialized flag first, then the second caller will find it flagged
 *    as initialized, but the bs_false and bs_true static values will be
 *    null.  So we make sure these values are initialized to the English
 *    versions at application setup, by moving their initialization outside
 *    the function.  So the worst thing that should happen is that the
 *    English strings are used the first time.
 *
 *    We should invent a test to see if any problem can still occur.
 *
 * \return
 *    If the value is 'true', returns the translated string for "true",
 *    otherwise it returns the translated string for "false".
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_boolean_string
(
   cbool_t value     /**< The boolean value to be "converted" to a string.    */
)
{
   static cbool_t is_initialized = false;

   /*
    * Moved outside the function for earlier initialization.
    *
    * static const char * bs_false = nullptr;
    * static const char * bs_true  = nullptr;
    */

   if (! is_initialized)
   {
      is_initialized = true;                 /* keep other threads at bay     */
      bs_false = _("false");
      bs_true  = _("true");
   }
   return value ? bs_true : bs_false ;
}

/******************************************************************************
 * xpc_make_ms_timeval()
 *------------------------------------------------------------------------*//**
 *
 *    Fills a timeval structure based on a time value provided in
 *    milliseconds.
 *
 * \return
 *    Returns 'true' if the \a tv parameter was valid.  Otherwise, 'false'
 *    is returned.
 *
 * \unittests
 *    TBD.
 *
 * \todo
 *    Consider range validation for the xpc_make_ms_timeval() function.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_make_ms_timeval
(
   struct timeval * tv, /**< The timeval structure to set                     */
   unsigned long ms     /**< The approximate number of milliseconds to sleep  */
)
{
   cbool_t result;
   if (not_nullptr(tv))
   {
      tv->tv_usec = (ms % 1000) * 1000;
      tv->tv_sec = ms / 1000;
      result = true;
   }
   else
      result = false;

   return result;
}

#ifdef WIN32

/******************************************************************************
 * xpc_us_sleep_win32()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to sleep in Windows with an accuracy of approximately
 *    10 microseconds, using little CPU time.
 *
 *    The algorithm and code comes from
 *
 *       http://www.geisswerks.com/ryan/FAQS/timing.html
 *
 *    Some more discussion can be found at the following links:
 *
 *       -  http://blogs.msdn.com/larryosterman/archive/2005/09/08/462477.aspx
 *       -  http://developer.nvidia.com/object/timer_function_performance.html
 *
 *    and in the "Warning" section below.
 *
 * \usage
 *    -# Specify linking to winmm.lib in the application project.
 *    -# Call timeBeginPeriod(1) at program startup.  This is handled here
 *       using a static initializer procedures.
 *    -# Call timeEndPeriod(1) at program exit.  This call is wrapped in the
 *       helper xpc.....
 *
 * \note
 *    We suppose we could call the timeBeginPeriod() and timeEndPeriod()
 *    functions within every call, if they do not take too long.  For
 *    simplicity, let's do that for now, and see where it becomes a
 *    problem.
 *
 * \warning
 *    The following issues apply:
 *
 *       -  http://support.microsoft.com/default.aspx?scid=KB;EN-US;Q274323&
 *          talks about how "Performance counter value may unexpectedly leap
 *          forward" several seconds!!!
 *       -  On multiprocessor systems you have to be careful that multiple
 *          calls to RDTSC (a per-CPU operation) are actually executing on
 *          the same cpu.
 *
 * \return
 *    Returns 'true' if the timerBeginPeriod() function succeeded, allowing
 *    the function to truly sleep.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    TBD.
 *
 * \todo
 *    THIS ROUTINE IS NOT YET COMPLETED; we need to determine the proper
 *    setting of the static variable \e s_timer_freq; also, this stuff is
 *    too prone to \e gotchas, in our opinion; compare this function to the
 *    Win32 version of xpc_get_microseconds().
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_us_sleep_win32
(
   unsigned long us     /**< The approximate number of microseconds to sleep. */
)
{
#ifdef THIS_CODE_IS_READY
   static LARGE_INTEGER s_timer_freq = { 0 };      /* a union with LONGLONG   */
   static LARGE_INTEGER s_peof = { 0 };            /* previous end-of-frame   */
   cbool_t result = (timeBeginPeriod(1) == TIMERR_NOERROR);
   if (result)
   {
      int max_fps = 60;
      LARGE_INTEGER t;
      QueryPerformanceCounter(&t);
      if (s_peof.QuadPart != 0)
      {
         int ticks_to_wait = (int) s_timer_freq.QuadPart / max_fps;
         cbool_t done = false;
         do
         {
            int ticks, ticks_left;
            QueryPerformanceCounter(&t);
            ticks = (int) ((__int64) t.QuadPart - (__int64) s_peof.QuadPart);
            ticks_left = ticks_to_wait - ticks;
            if (t.QuadPart < s_peof.QuadPart)                  /* time wrap   */
               done = true;

            if (ticks >= ticks_to_wait)
               done = true;

            if (! done)
            {
               /*
                * If greater than 0.002 seconds is left, do Sleep(1), which
                * will sleep some steady amount, about 1 to 2 milliseconds,
                * in a nice way, where the CPU meter drops, and the battery
                * is spared.  Otherwise, do a few Sleep(0) calls, which just
                * give up the timeslice.  This does not really save the CPU
                * or battery, but does pass a tiny amount of time.
                */

               if (ticks_left > (int) s_timer_freq.QuadPart * 2 / 1000)
                  Sleep(1);
               else
               {
                  int i;
                  for (i = 0; i < 10; i++)
                     Sleep(0);                           /* give up timeslice */
               }
            }

         } while (! done);
      }
      s_peof = t;
      (void) timeEndPeriod(1);
   }
   else
      xpc_errprint_func(_("failed to initialize"));

   return result;

#else       /* THIS_CODE_IS_READY   */

   return false;

#endif      /* THIS_CODE_IS_READY   */
}

#endif      /* WIN32                */

/******************************************************************************
 * xpc_us_sleep()
 *------------------------------------------------------------------------*//**
 *
 *    Sleeps for roughly the given number of microseconds.
 *
 *    It is not guaranteed that the amount of sleep will be exactly correct.
 *
 * \win32
 *    Uses the xpc_us_sleep_win32() function, which is not yet fully
 *    implemented.
 *
 * \posix
 *    The select() function is used, with almost all parameters null or
 *    zero.  This setup means that select() is waiting on no file
 *    descriptors, but for the given timeout.
 *
 *    In POSIX, select() can return early if any signal occurs.  We don't
 *    correct for that here at this time.  Actually, it is a convenient
 *    feature, and we wish that Sleep() would provide it.
 *
 *    Please note that this function isn't all that accurate for small
 *    sleep values, due to the time taken to set up the operation.
 *
 *    This algorithm was taken from "Advanced Programming in the UNIX
 *    Environment", section 14.6; see the References for more information.
 *
 * \unittests
 *    TBD.
 *
 * \todo
 *    Make sure the References section is in a dox file for Doxygen.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_us_sleep
(
   unsigned long us     /**< The approximate number of microseconds to sleep. */
)
{
#ifdef POSIX                              /* POSIX                            */

   struct timeval tv;
   struct timeval * tvptr = &tv;
   tv.tv_usec = us;
   tv.tv_sec = us / 1000000;
   (void) select(0, nullptr, nullptr, nullptr, tvptr);

#else                                     /* Win32                            */

   (void) xpc_us_sleep_win32(us);         /* Danger: not ready for prime time */

#endif                                    /* POSIX/Win32                      */
}

/******************************************************************************
 * xpc_us_sleep_poll()
 *------------------------------------------------------------------------*//**
 *
 *    Sleeps for roughly the given number of microseconds.
 *
 *    It is not guaranteed that the amount of sleep will be exactly correct.
 *
 * \win32
 *    Not implemented for Windows.
 *
 * \posix
 *    The poll() function is used instead of the select() function.
 *
 *    This algorithm was taken from "Advanced Programming in the UNIX
 *    Environment", section 14.6; see the References for more information.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

#if XPC_HAVE_POLL_H

void
xpc_us_sleep_poll
(
   unsigned long us     /**< The approximate number of microseconds to sleep. */
)
{
   struct pollfd dummy;
   int timeout;
   if ((timeout = us / 1000) <= 0)
      timeout = 1;

   (void) poll(&dummy, 0, timeout);
}

#endif   // XPC_HACE_POLL_H (or not WIN32)

/******************************************************************************
 * xpc_ms_sleep()
 *------------------------------------------------------------------------*//**
 *
 *    Sleeps for roughly the given number of milliseconds.
 *
 *    It is not guaranteed that the amount of sleep will be exactly correct.
 *    See the following discussion for more details.
 *
 *    A sleep time of 0 means that no sleeping is done.
 *
 * \posix
 *    The select() function is used, as described in the xpc_us_sleep()
 *    function.
 *
 * \win32
 *    The Sleep() function is used.  It has some limitations, as described
 *    in:
 *
 *       http://www.geisswerks.com/ryan/FAQS/timing.html
 *
 *    Also, Sleep(0) immediately causes the thread to yield to other
 *    threads, but the xpc_ms_sleep() function, when called with 0, does not
 *    do that -- it just avoids any sleeping.
 *
 *    Sleep() for values less than 10 ms still sleeps for a minimum of 10
 *    ms.  As the link describes, it is possible to drop the granularity
 *    of the Sleep() call, within a single application, down to 1 or (at
 *    worst) 2 ms using a call to timeBeginPeriod(1).  This call requires
 *    the usage of the winmm.lib library, and it also requires that the
 *    application make the call timeEndPeriod(1) before the application
 *    exits.  See http://msdn2.microsoft.com/en-us/library/ms713413.aspx for
 *    a trivial amount of additional details.
 *
 *    This is a bit much to bother with at this time, so for now we simply
 *    recommend using xpc_ms_sleep() for values of 10 ms or greater, when
 *    the accuracy of the sleep is not important.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_ms_sleep
(
   unsigned long ms     /**< The approximate number of milliseconds to sleep  */
)
{
   if (ms > 0)
   {
#ifdef POSIX                              /* POSIX                            */

      struct timeval tv;
      struct timeval * tvptr = &tv;
      tv.tv_usec = (ms % 1000) * 1000;
      tv.tv_sec = ms / 1000;
      (void) select(0, nullptr, nullptr, nullptr, tvptr);

#else                                     /* Win32                            */

      Sleep((DWORD) ms);

#endif                                    /* POSIX/Win32                      */
   }
}

/******************************************************************************
 * xpc_cpu_time()
 *------------------------------------------------------------------------*//**
 *
 *    Computes the differences between two clock times, in seconds.
 *
 *    This function calculates the CPU time used by a section of a process.
 *    The caller calls clock() before and after the action to be timed, then
 *    passes the two results to this function.
 *
 *    Note that clock() returns the current CPU time (user time + system time)
 *    of the process.  Thus, if a process spends most of its time in a sleep,
 *    this value can be amazingly small (e.g. 0).  Also, on POSIX systems this
 *    value wraps around every 72 hours.  On Windows, maybe a bit longer (a
 *    thousand times).
 *
 *    The value returned is in seconds.
 *
 *    A warning is written to the error log if the CPU time wraps around,
 *    but this function detects the wraparound and corrects the result.
 *
 * \b CLOCKS_PER_SEC:
 *
 *       POSIX:      1000000           <br>
 *       Win32:         1000           <br>
 *
 * \return
 *    The time difference c2 - c1, in seconds, is returned.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_cpu_time
(
   clock_t c1,       /**< The earlier clock time value                        */
   clock_t c2        /**< The later clock time value                          */
)
{
   double result = 0.0;
   if (c2 > c1)
   {
      double difference = (double) c2 - (double) c1;
      if (difference < 0.0)
      {
         unsigned long u1 = (unsigned long) c1;          /* highest value     */
         u1 = ULONG_MAX - u1;                            /* get top section   */
         difference = (double) c2 + (double) u1;         /* wrapped diff      */
         xpc_warnprint_func(_("wrap-around"));
      }
      result = difference / (double) CLOCKS_PER_SEC;     /* cast it           */
   }
   return result;
}

/******************************************************************************
 * xpc_get_microseconds()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the current "time-of-day" in microseconds.
 *
 *    This function gets the current value of microseconds from the system.  It
 *    is assumed that the value is used solely for interval timing, so that the
 *    tick value returned does not necessarily correspond to any true time.
 *
 * \posix
 *    This function is a simpler wrapper for gettimeofday().  It's main
 *    purpose, though, is for obtaining elapsed time in performance tests.
 *
 * \todo
 *    HOWEVER, the man page gettimeofday(2) notes that this function is
 *    <i>obsolete</i>, and should be replaced by clock_gettime(2).
 *    We will ultimately create a new function, xpc_clock_microseconds(),
 *    that use <tt>struct timespec</tt> and clock_gettime() instead.
 *
 * \win32
 *    The function wraps GetSystemTimeAsFileTime(), and converts it to the
 *    BSD timeval structure.  Win32 "file time" is defined as the number of
 *    100-nanosecond intervals since January 1, 1601.  It's format is:
 *
 *         struct FILETIME { DWORD dwLowDateTime; DWORD dwHighDateTime; }
 *
 *    where dwLowDateTime hold the low-order 32-bits of the time, and
 *    dwHighDateTime holds the high-order 32-bits of the time.
 *
 *    The granularity of GetSystemTimeAsFileTime() is apparently about 10
 *    ms, as opposed to the 10 us granularity of most UNIXen.  This definitely
 *    affects the results of the --suite tests in atomictest.c.  Therefore,
 *    we provide a better implementation using the Win32 performance counter,
 *    which is based on the counter built into latter-day Pentium chips.
 *
 *    We want to convert each tick value into microseconds.  Let t be the tick
 *    value returned by QueryPerformanceCounter(), and let t0 be the
 *    counts-per-second value returned by QueryPerformanceFrequency().  Then
 *
 *          t sec = t / t0
 *
 *          t us  = 1000000t / t0
 *
 *    The second calculation is easy, but could overflow.  We have 2^63 ticks
 *    capacity, and (on one computer) a frequency value of 3,579,545, so that
 *    the counter can handle around 29,822,782 days (81706 years).  If we
 *    first convert to microseconds, then, we can see that we'll overflow the
 *    counter if the computer has been up for 30 days or more.
 *
 *    To get around this problem, we log the initial tick value, and subtract
 *    it from all subsequent measurements.  In this way, the ticks will be
 *    valid as long as the test application stops before the month is up.
 *
 *    (We could provide a function to clear it every 24 hours.)
 *
 * \note
 *    On a dual Xeon running Win 2003 Server, t0 comes back as
 *    2793070000, or about 2.8 giga-ticks-per-second.  If we convert this
 *    to ticks-per-microsecond, we get 2793, which is an error of about
 *    0.003 percent
 *
 *    In one run, we've seen the tick count at the first call to be T0 =
 *    233413178840009 (15 digits).  The next call was T1 = 255695241880239.
 *    The difference is 22282063040230 (14 digits).  If we multiply by
 *    1000000, it comes out as 3835318966520448384 (19 digits), and this is
 *    grossly wrong.
 *
 *    So now we have T1 - T0 = 22282063040230 = D
 *
 *    ln D = 30.734803123, ln 2 = 0.693147181, ln D / ln 2 = 44.34, so this
 *    is a 45-bit number.
 *
 *    22282063040230 ticks / 2793 ticks/microsecond yields 7977824217,
 *    which is 33 bits.  We cannot get microseconds directly.  We have to
 *    cull out the seconds first.
 *
 * \param c
 *      The structure in which to insert the time.
 *
 * \return
 *    Returns true if the function was able to obtain the time.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX                                             /* POSIX             */

cbool_t
xpc_get_microseconds (struct timeval * c)
{
   cbool_t result = false;
   if (not_nullptr(c))
   {
      /*
       * Note that we should eventually replace this call with a call to
       *  clock_gettime(2).
       */

      int errcode = gettimeofday(c, nullptr);            /* fill in c         */
      if (is_posix_error(errcode))
      {
         c->tv_sec = 0;
         c->tv_usec = 0;
      }
      else
         result = true;
   }
   return result;
}

#else                                                    /* Win32             */

#ifdef USE_WIN32_FILETIME                                /* too coarse        */

cbool_t
xpc_get_microseconds (struct timeval * c)
{
   cbool_t result = false;
   if (not_nullptr(c))
   {
      FILETIME filetime;
      ULARGE_INTEGER t;
      unsigned __int64 seconds;
      unsigned __int64 sec_us;
      unsigned __int64 us;
      GetSystemTimeAsFileTime(&filetime);
      t.u.LowPart = filetime.dwLowDateTime;
      t.u.HighPart = filetime.dwHighDateTime;
      t.QuadPart /= 10;                          /* convert to u-sec  */
      seconds = t.QuadPart / 1000000;
      sec_us  = seconds * 1000000;
      us = t.QuadPart - sec_us;
      c->tv_sec = (long) seconds;
      c->tv_usec = (long) us;
      result = true;
   }
   return result;
}

#else                                                    /* much better       */

cbool_t
xpc_get_microseconds (struct timeval * c)
{
   static cbool_t gm_is_ready = false;
   static LARGE_INTEGER gm_frequency   = { 0 };          /* tick frequency    */
   static LARGE_INTEGER gm_t0  = { 0 };                  /* starting ticks    */
   static unsigned __int64 gm_ticks_per_us = 0;          /* extracts usecs    */
   cbool_t result = false;
   if (not_nullptr(c))
   {
      if (! gm_is_ready)
      {
         BOOL okay = QueryPerformanceFrequency(&gm_frequency);
         if (okay)
         {
            gm_ticks_per_us = gm_frequency.QuadPart;        /* supplemental   */
            okay = QueryPerformanceCounter(&gm_t0);         /* ticks now      */
            if (okay)
               gm_is_ready = true;
         }
         else
            gm_frequency.QuadPart = 0;
      }
      if (gm_is_ready)
      {
         LARGE_INTEGER t;
         BOOL okay = QueryPerformanceCounter(&t);           /* time in ticks  */
         if (okay)
         {
            unsigned __int64 ticks = t.QuadPart - gm_t0.QuadPart;
            unsigned __int64 seconds = ticks / gm_frequency.QuadPart;
            unsigned __int64 us = seconds * gm_frequency.QuadPart;
            us = (ticks - us) / gm_ticks_per_us;
            c->tv_sec = (long) seconds;
            c->tv_usec = (long) us;
         }
         else
         {
            c->tv_sec = (long) 0;
            c->tv_usec = (long) 0;
         }
      }
   }
   return result;
}

#endif                                                   /* USE_WIN32_FILETIME*/
#endif                                                   /* POSIX vs Win32    */

/******************************************************************************
 * xpc_time_add()
 *------------------------------------------------------------------------*//**
 *
 *    Similar to xpc_time_subtract(), but adds the two times.
 *
 * \param c1
 *    A pointer to the timeval structure representing the original time.
 *    This structure is modified in order to return the result.
 *
 * \param c2
 *    The timeval structure representing the addend.
 *
 * \return
 *    Returns the summed time, in microseconds.  Zero is returned either in
 *    case of error, or if the times are identical.  If a zero is returned,
 *    c1 has not been modified.
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
xpc_time_add (struct timeval * c1, struct timeval c2)
{
   unsigned long result = 0UL;
   if (xpc_not_nullptr(c1, "xpc_time_add: null timeval destination"))
   {
      struct timeval temp;
      temp.tv_sec = c1->tv_sec;
      temp.tv_usec = c1->tv_usec;
      temp.tv_usec += c2.tv_usec;
      if (temp.tv_usec >= 1000000)
      {
         temp.tv_usec -= 1000000;
         temp.tv_sec++;
      }
      temp.tv_sec += c2.tv_sec;
      if ((unsigned int) temp.tv_sec > UINT_MAX / 1000000)
         result = 0UL;
      else
      {
         result = (unsigned long) temp.tv_sec * 1000000 +
            (unsigned long) temp.tv_usec;

         c1->tv_sec = temp.tv_sec;
         c1->tv_usec = temp.tv_usec;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_time_subtract()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the time difference between two time stamps as another time
 *    stamp.
 *
 *    This function calculates the time difference between two timevals
 *    obtained from gettimeofday().
 *
 *    There may be some issues in how this is done, so make very sure this
 *    function works for your particular application.
 *
 * \param c1
 *    A pointer to the timeval structure representing the earlier time.
 *    This structure is modified in order to return the result.
 *
 * \param c2
 *    The timeval structure representing the later time.
 *
 * \return
 *    Returns the time difference, in microseconds, like
 *    xpc_time_difference_us().  Zero is returned either in case of error,
 *    or if the times are identical.  If a zero is returned, c1 has not been
 *    modified.
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
xpc_time_subtract (struct timeval * c1, struct timeval c2)
{
   unsigned long result = 0UL;
   if (xpc_not_nullptr(c1, "xpc_time_subtract: null timeval destination"))
   {
      struct timeval earlier;
      earlier.tv_sec = c1->tv_sec;
      earlier.tv_usec = c1->tv_usec;
      if (c2.tv_usec < earlier.tv_usec)      /* need to borrow from seconds?  */
      {
         c2.tv_sec--;
         c2.tv_usec += 1000000;
      }
      earlier.tv_sec = c2.tv_sec - earlier.tv_sec;
      earlier.tv_usec = c2.tv_usec - earlier.tv_usec;
      if ((unsigned int) earlier.tv_sec > UINT_MAX / 1000000)
         result = 0UL;
      else
      {
         result = (unsigned long) earlier.tv_sec * 1000000 +
            (unsigned long) earlier.tv_usec;

         c1->tv_sec = earlier.tv_sec;
         c1->tv_usec = earlier.tv_usec;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_double_time_ptr()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the given time in double-precision format.
 *
 * \note
 *    Double precision values with double type have 8 bytes. The format is
 *    similar to the float format except that it has an 11-bit excess-1023
 *    exponent and a 52-bit mantissa, plus the implied high-order 1 bit.
 *    This format gives a range of approximately 1.7E-308 to 1.7E+308
 *    for type double.
 *
 * Microsoft Specific:
 *
 *    The double type contains 64 bits: 1 for sign, 11 for the exponent, and
 *    52 for the mantissa. Its range is +/-1.7E308 with at least 15 digits
 *    of precision.
 *
 * \param c
 *    The time value structure to convert to a double value, provided as a
 *    pointer.
 *
 * \return
 *    Returns the sum of the seconds and microseconds fields, in seconds.
 *    If the pointer is bad, 0.0 is returned.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_double_time_ptr (struct timeval * c)
{
   double result = 0.0;
   if (not_nullptr(c))
   {
      result = (double) c->tv_sec;
      result += (double) c->tv_usec * 0.000001;
   }
   return result;
}

/******************************************************************************
 * xpc_double_time()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the given time in double-precision format.
 *
 *    Also see the notes at xpc_double_time_ptr().
 *
 * \param c
 *    The time value structure to convert to a double value.
 *
 * \return
 *    Returns the sum of the seconds and microseconds fields, in seconds.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_double_time (struct timeval c)
{
   double result = (double) c.tv_sec;
   result += (double) c.tv_usec * 0.000001;
   return result;
}

/******************************************************************************
 * xpc_time_fix()
 *------------------------------------------------------------------------*//**
 *
 *    Normalizes a timeval structure, if necessary.
 *
 *    The \a tv_usec value must range only from 0 to 999999.  As noted
 *    below, it is a signed value.  It can then be less than zero, or
 *    greater than 999999.
 *
 *    If it is less than 0, then it should be increased until it is not, and
 *    the corresponding number of seconds should be subtracted from the \a
 *    tv_sec field.
 *
 *    If it is greater than 999999, then it should be reduced until it is,
 *    and the corresponding number of seconds be added to the \a tv_sec
 *    field.
 *
 * \return
 *    Returns 'true' if an adjustment had to be made.  Otherwise, 'false' is
 *    returned.  This function also returns 'false' if the pointer was
 *    invalid (null).
 *
 * \note
 *    In /usr/include/bits/time.h, the tv_usec field is typed as
 *    __suseconds_t.
 *
 *    In /usr/include/bits/types.h, we see these declarations:
 *
 *       __STD_TYPE __USECONDS_T_TYPE __useconds_t;   // Count of us
 *       __STD_TYPE __SUSECONDS_T_TYPE __suseconds_t; // Signed count of us
 *
 *    Looking for __USECONDS_T_TYPE, we find it in bits/typesizes.h as
 *    __U32_type (unsigned int).
 *
 *    Looking for __SUSECONDS_T_TYPE, we find it in bits/typesizes.h as a
 *    macro for __SLONGWORD_TYPE.  In /usr/include/bits/types.h, this macro
 *    is defined as "long int".  So there is a potential inconsistency in
 *    the sizes of the signed and unsigned "seconds" type between 32-bit and
 *    64-bit systems!
 *
 *    In /usr/include/sys/time.h, select.h and types.h, __suseconds_t is
 *    re-typed as suseconds_t.
 *
 *    In any case, \a tv_usec is a signed integer.
 *
 * \param c1
 *    Provides a pointer to the structure containing the time-stamp to be
 *    adjusted.
 *
 * \return
 *    Returns 'true' if ... TBD
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_time_fix (struct timeval * c1)
{
   cbool_t result = false;
   if (not_nullptr(c1))
   {
      if (c1->tv_usec > 999999)
      {
         c1->tv_sec += c1->tv_usec / 1000000;
         c1->tv_usec %= 1000000;
         result = true;
      }
      else if (c1->tv_usec < 0)
      {
         int delta_sec = c1->tv_usec / 1000000 + 1;
         c1->tv_sec -= delta_sec;
         c1->tv_usec += delta_sec * 1000000;
         result = true;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_time_difference_us()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the time difference between two timeval structures in
 *    microseconds.
 *
 *    This function is similar to xpc_time_difference(), but it has two
 *    differences:
 *
 *       -  It returns the value in microseconds, rather than in seconds.
 *       -  It returns the value as a long value, rather than as a double
 *          value.
 *
 * \warning
 *    An <i>unsigned long</i> value is 32 bits in Lin32, Win32, and in Win64
 *    systems.  An <i>unsigned long</i> value is 64-bits in Lin64.
 *
 *    A 32-bit unsigned integer can hold a value up to 4294967295 (us), which
 *    translates to about 4294967 ms, or 4294 seconds.
 *
 *    Therefore, do not use this function to measure intervals that are much
 *    over one hour (3600 seconds) in duration.  Such a failure can be
 *    detected by the return value.
 *
 * \note
 *    Fixes to this algorithm have to be made in all of the following
 *    functions:
 *
 *       - xpc_time_difference_us()
 *       - xpc_time_difference_ms()
 *       - xpc_time_difference()
 *
 *    The algorithm is duplicated in each function in order to obtain
 *    marginally more efficiency.
 *
 * \param c1
 *    Provides the structure for the earlier time-stamp.
 *
 * \param c2
 *    Provides the structure for the later time-stamp.
 *
 * \return
 *    Returns the difference between the two times ("c2 - c1") in
 *    microseconds.  If the time difference was detected to be too long to
 *    fit in the return value, then 0 is returned.  The caller should avoid
 *    using the result of this function if it is 0.
 *
 * \todo
 *    Make sure this function works with negative differences, and unit test
 *    it.
 *
 * \unittests
 *    TBD, but very high priority!
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
xpc_time_difference_us (struct timeval c1, struct timeval c2)
{
   unsigned long result;
   int difference = c2.tv_usec - c1.tv_usec;
   if (difference < 0)
   {
      int delta_sec = -difference / 1000000 + 1;
      c1.tv_usec -= 1000000 * delta_sec;
      c1.tv_sec += delta_sec;
   }
   else if (difference > 1000000)
   {
      int delta_sec = difference / 1000000;
      c1.tv_usec += 1000000 * delta_sec;
      c1.tv_sec -= delta_sec;
   }
   c1.tv_sec  = c2.tv_sec  - c1.tv_sec;
   c1.tv_usec = c2.tv_usec - c1.tv_usec;
   if (c1.tv_sec >= INT_MAX / 1000000)                /* UINT_MAX in win32?   */
      result = 0;
   else
      result = (unsigned long) c1.tv_sec * 1000000 + (unsigned long) c1.tv_usec;

   return result;
}

/******************************************************************************
 * xpc_time_difference_ms()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the time difference between two timeval structures in
 *    milliseconds.
 *
 *    This function is similar to xpc_time_difference_us(), but returns the
 *    result in milliseconds instead of microseconds.  Please see that
 *    function for more information.
 *
 * \warning
 *    An <i>unsigned long</i> value is 32 bits in Lin32, Win32, and in Win64
 *    systems.  An <i>unsigned long</i> value is 64-bits in Lin64.
 *
 *    A 32-bit unsigned integer can hold a value up to 4294967295 (ms), which
 *    translates to about or 4294496 seconds.
 *
 *    Therefore, do not use this function to measure intervals that are
 *    over 49.7 days in duration.  Such a failure can be detected by the
 *    return value.
 *
 * \note
 *    Fixes to this algorithm have to be made in all of the following
 *    functions:
 *
 *       - xpc_time_difference_us()
 *       - xpc_time_difference_ms()
 *       - xpc_time_difference()
 *
 *    The algorithm is duplicated in each function in order to obtain
 *    marginally more efficiency.
 *
 * \param c1
 *    Provides the structure for the earlier time-stamp.
 *
 * \param c2
 *    Provides the structure for the later time-stamp.
 *
 * \return
 *    Returns the difference between the two times ("c2 - c1") in
 *    milliseconds.  If the time difference was detectd to be too long to
 *    fit in the return value, then 0 is returned.  The caller should avoid
 *    using the result of this function if it is 0.
 *
 * \unittests
 *    TBD, but very high priority!
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
xpc_time_difference_ms (struct timeval c1, struct timeval c2)
{
   unsigned long result;
   int difference = c2.tv_usec - c1.tv_usec;
   if (difference < 0)
   {
      int delta_sec = -difference / 1000000 + 1;
      c1.tv_usec -= 1000000 * delta_sec;
      c1.tv_sec += delta_sec;
   }
   else if (difference > 1000000)
   {
      int delta_sec = difference / 1000000;
      c1.tv_usec += 1000000 * delta_sec;
      c1.tv_sec -= delta_sec;
   }
   c1.tv_sec  = c2.tv_sec  - c1.tv_sec;
   c1.tv_usec = c2.tv_usec - c1.tv_usec;
   if (c1.tv_sec >= INT_MAX / 1000)                /* UINT_MAX in win32?   */
      result = 0;
   else
      result = (unsigned long) c1.tv_sec * 1000 +
         (unsigned long) c1.tv_usec / 1000;

   return result;
}

/******************************************************************************
 * xpc_time_difference()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the time difference (in seconds) between two timeval
 *    structures, and returns the result as a <i> floating-point </i> value.
 *
 *    This function calculates the time difference between two timevals
 *    obtained from gettimeofday() or xpc_get_microseconds().
 *
 *    This function is similar to the one discussed in Loosemore, et al. (2004)
 *    "GNU C Library:  Application Fundamentals", GNU Press, section 10.2
 *    "Elapsed Time".  It handles overflow of the time counter.  It also
 *    seems to assume that the
 *
 *    If the later (c2) microsecond value is less than the earlier (c1)
 *    value, then we have to deduct from the c2 microsecond value to bring
 *    it below the c1 microsecond value, and increment the c1 second value a
 *    corresponding amount
 *
 *    If the difference between the two microsecond values is greater than 1
 *    million, then we have to bring the c1 microseconds within 1 million or
 *    less of the c2 microsecond value (so that the subtraction will end in
 *    the range of 0 to 999999), and then subtract the corresponding
 *    amount from the c1 seconds field.
 *
 *    Another option is to simply subtract each field's value, and then call
 *    the xpc_time_fix() function.  It is at least instructive to compare
 *    the two functions.
 *
 *    Also compare this function to the xpc_cpu_time() function.
 *
 * \linux
 *    A 32-bit unsigned integer can hold a value up to 4294967295 (us), which
 *    translates to about 4294967 ms, or 4294 seconds.
 *
 *    Can we improve on it?  Do we need to? Are there other issues?
 *
 *    A comment in /usr/include/pcap.h says:
 *
 *       The time stamp can and should be a "struct timeval", regardless of
 *       whether your system supports 32-bit tv_sec in "struct timeval",
 *       64-bit tv_sec in "struct timeval", or both if it supports both
 *       32-bit and 64-bit applications.
 *
 * \note
 *    Fixes to this algorithm have to be made in all of the following
 *    functions:
 *
 *       - xpc_time_difference_us()
 *       - xpc_time_difference_ms()
 *       - xpc_time_difference()
 *
 *    The algorithm is duplicated in each function in order to obtain
 *    marginally more efficiency.
 *
 * \param c1
 *    Provides the structure for the earlier time-stamp.
 *
 * \param c2
 *    Provides the structure for the later time-stamp.
 *
 * \return
 *    The value returned is "c2 - c1" in seconds.
 *
 * \unittests
 *    TBD, but very high priority!
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_time_difference (struct timeval c1, struct timeval c2)
{
   double result;
   int difference = c2.tv_usec - c1.tv_usec;
   if (difference < 0)
   {
      int delta_sec = -difference / 1000000 + 1;
      c1.tv_usec -= 1000000 * delta_sec;
      c1.tv_sec += delta_sec;
   }
   else if (difference > 1000000)
   {
      int delta_sec = difference / 1000000;
      c1.tv_usec += 1000000 * delta_sec;
      c1.tv_sec -= delta_sec;
   }

   /* Compute the time remaining; tv_usec is certainly positive */

   c1.tv_sec  = c2.tv_sec  - c1.tv_sec;
   c1.tv_usec = c2.tv_usec - c1.tv_usec;
   result = (double) c1.tv_sec + (double) c1.tv_usec * 0.000001;
   return result;
}

/******************************************************************************
 * xpc_time_difference_ptr()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a pointer version of xpc_time_difference().
 *
 *    What happened was that the original version of xpc_time_difference(),
 *    which used pointers, was incorrect.  It inadvertently altered its
 *    parameters.  Also, we realized that copying the parameters wasn't a
 *    big deal (it's only two integers).
 *
 * \deprecated
 *    However, this function really isn't necessary.
 *
 * \param c1
 *    Provides the structure for the earlier time-stamp.
 *
 * \param c2
 *    Provides the structure for the later time-stamp.
 *
 * \return
 *    The value returned is "c2 - c1" in seconds.
 *
 * \unittests
 *    None.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_time_difference_ptr (struct timeval * c1, struct timeval * c2)
{
   return xpc_time_difference(*c1, *c2);
}

/******************************************************************************
 * xpc_stopwatch_start()
 *------------------------------------------------------------------------*//**
 *
 *    Starts a single-threaded stopwatch with microsecond resolution.
 *
 *    This function first gets that start time using xpc_get_microseconds().
 *    Then it sets the lap time to this time.  Finally, a "stopwatch
 *    started" flag is set to true.
 *
 *    The xpc_stopwatch_start(), xpc_stopwatch_duration(), and xpc_stopwatch_lap()
 *    functions provide a global timer so that the call doesn't even have
 *    to know about struct timeval.  The "duration" function gets the
 *    current time and returns the difference between it and the start
 *    time, in seconds.  The "lap" function returns the time between the
 *    previous lap time and now.
 *
 * \threadunsafe
 *    Please note that these functions are not thread-safe, since the start
 *    and lap times are stored in a static variable.  Access them only with
 *    a single thread.  They are meant mostly for testing and for the
 *    evaluation of algorithms.
 *
 *    We could provide an optional structure parameter to make this function
 *    thread-safe, if the need becomes evident.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t g_xpc_stopwatch_started = false;
static struct timeval g_xpc_stopwatch_start_time;
static struct timeval g_xpc_stopwatch_lap_time;

void
xpc_stopwatch_start (void)
{
   xpc_get_microseconds(&g_xpc_stopwatch_start_time);
   g_xpc_stopwatch_lap_time = g_xpc_stopwatch_start_time;
   g_xpc_stopwatch_started = true;
}

/******************************************************************************
 * xpc_stopwatch_duration()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the total time elapsed since the start time.
 *
 *    This function gets the current time with xpc_get_microseconds(), and then
 *    it returns the difference between the end time (current time) and the
 *    start time using the xpc_time_difference() function.
 *
 * \return
 *    The duration since the start time of the stopwatch is returned, in
 *    units of microseconds.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_stopwatch_duration (void)
{
   double result = 0.0;
   if (g_xpc_stopwatch_started)
   {
      struct timeval end_time;
      xpc_get_microseconds(&end_time);
      result = xpc_time_difference(g_xpc_stopwatch_start_time, end_time);
   }
   return result;
}

/******************************************************************************
 * xpc_stopwatch_lap()
 *------------------------------------------------------------------------*//**
 *
 *    Returns the time difference between the current call to
 *    xpc_stopwatch_lap() and the previous call to it.
 *
 *    This function gets the current time with xpc_get_microseconds(), and
 *    then it returns the difference between the end time (current time) and
 *    the last lap time using the xpc_time_difference() function.
 *
 *    Then the old lap time is updated with the current time in anticipation
 *    of the next call to xpc_stopwatch_lap().  To increase the accuracy a
 *    little bit, the new lap time is obtained by another call to
 *    xpc_get_microseconds().
 *
 * \return
 *    Returns the difference in microseconds between the current call to
 *    xpc_stopwatch_lap(), and the last call [or to xpc_stopwatch_start() if
 *    there was no previous call to xpc_stopwatch_lap().
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_stopwatch_lap (void)
{
   double result = 0.0;
   if (g_xpc_stopwatch_started)
   {
      struct timeval end_time;
      xpc_get_microseconds(&end_time);
      result = xpc_time_difference(g_xpc_stopwatch_lap_time, end_time);

      /* This time is now out-of-date: g_xpc_stopwatch_lap_time = end_time    */

      xpc_get_microseconds(&g_xpc_stopwatch_lap_time);
   }
   return result;
}

/******************************************************************************
 * xpc_current_date()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the current date in a canonical format.
 *
 *    The canonical format is YYYY-mm-dd, as in 2010-07-19.
 *
 * \threadunsafe
 *    A static buffer for the date is used.
 *
 * \return
 *    Returns a pointer to the interally-stored date string, which should be
 *    used or copied immediately
 *
 * \return
 *    Returns a pointer to the interally-stored date string, which should be
 *    used or copied immediately
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_current_date ()
{
   time_t t;
   struct tm * tm;
   static char temp[64];
   static const char * const format = "%Y-%m-%d %H:%M:%S";
   memset(temp, 0, 64);
   time(&t);
   tm = localtime(&t);
   strftime(temp, 63, format, tm);
   return temp;
}

/******************************************************************************
 * portable.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
