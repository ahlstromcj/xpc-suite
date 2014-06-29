/******************************************************************************
 * pthreader.c
 *------------------------------------------------------------------------*//**
 *
 * \file          pthreader.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       04/29/2008-05/17/2008
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a light-weight wrapper around POSIX threads.
 *
 *    The approach is this:  implement threads using the pthreads API, and
 *    then use the open-source "pthreads-w32" code to run the same functions
 *    under Windows.  (Our own implementation of Win32 code exists in the
 *    w32 sub-directory of this project, however, so we can compare
 *    performance and error-checking features.)
 *
 *    This module provides only the most basic cross-platform thread
 *    support.  It is meant to provide a simplified base upon which to build
 *    small threaded applications and more powerful threading libraries.  It
 *    does not try to impose any kind of threading mode. Consider it to be a
 *    set of convenience functions that provide error-logging as an aid to
 *    debugging.
 *
 *    The features this module provides are:
 *
 *       -  Easy modification of thread attributes before the thread is
 *          created.
 *       -  Logging of errors, warnings, or information that might make it
 *          easier to debug one's threaded code.
 *
 * \references
 *    -  http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/integers.h>
#include <xpc/pthreader.h>             /* pthread typedefs and macros         */
#include <xpc/pthread_attributes.h>    /* pthread_attr_t, etc.  functions     */
XPC_REVISION(pthreader)

#if XPC_HAVE_STDDEF_H
#include <stddef.h>                    /* for intptr_t in Win32               */
#endif

/******************************************************************************
 * pthreader_null_thread()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a function to return a "null" thread ID.
 *
 *    This function provides a pthread_t value that is considered to be
 *    "null".  This function initializes it to null bytes the first time it
 *    is called.
 *
 * \warning
 *    We are not sure if this is portable.  It requires that no platform
 *    ever returns a valid thread ID that is composed only of "0" bytes.
 *
 *    Also, this function really isn't thread safe.  However, that probably
 *    doesn't matter.  If we find it does matter, we'll have to mandate
 *    a call to it before any thread-creation occurs.
 *
 * \return
 *    Returns a copy of the null thread ID.
 *
 *//*-------------------------------------------------------------------------*/

pthread_t
pthreader_null_thread (void)
{
   static cbool_t gs_pthreader_null_thread_id_not_inited = true;
   static pthread_t gs_pthreader_null_thread_id;
   if (gs_pthreader_null_thread_id_not_inited)
   {
      (void) memset(&gs_pthreader_null_thread_id, 0, sizeof(pthread_t));
      gs_pthreader_null_thread_id_not_inited = false;
   }
   return gs_pthreader_null_thread_id;
}

/******************************************************************************
 * pthreader_is_null_thread()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to check for null thread IDs.
 *
 *    This function provides a pthread_t value that is considered to be
 *    "null".  It initializes it to null bytes the first time it is called.
 *
 * \warning
 *    See the warnings for the pthreader_null_thread() function.
 *
 *    Also, we could make this function more efficient by accessing the
 *    globals directly, and avoid making a copy of the "null" thread ID.
 *    We will see later if that matters.
 *
 *    Finally, we wonder which implemention is better -- one using memcmp(),
 *    or one using pthread_equal().  Is it faster to compare objects that
 *    are potentially structures by fields or by bytes?
 *
 *       -  memcmp(pthreader_null_thread(), th, sizeof(pthread_t)) == 0
 *       -  pthread_equal(pthreader_null_thread(), th)
 *
 * \return
 *    Returns 'true' if the given thread ID matches the "null" thread ID.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthreader_is_null_thread
(
   pthread_t th            /**< The thread ID of the thread to be checked.    */
)
{
   return pthread_equal(pthreader_null_thread(), th);
}

/******************************************************************************
 * pthreader_is_valid_thread()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to check for null thread IDs.
 *
 *    This function provides a pthread_t value that is considered to be
 *    "null".  It initializes it to null bytes the first time it is called.
 *
 * \warning
 *    See the warnings for the pthreader_is_null_thread() function.
 *
 * \return
 *    Returns 'true' if the given thread ID matches the "null" thread ID.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthreader_is_valid_thread
(
   pthread_t th            /**< The thread ID of the thread to be checked.    */
)
{
   if (pthread_equal(pthreader_null_thread(), th))
   {
      xpc_errprint_func(_("bad thread ID"));
      return false;
   }
   else
      return true;
}

/******************************************************************************
 * pthreader_is_cancelled()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to check for cancelled thread IDs.
 *
 *    The pthreader_join() function calls pthread_join().  The latter
 *    function is part of the pthreads library, and it obtains the return
 *    value of the thread that was terminated.  If that thread was
 *    cancelled, then PTHREAD_CANCELED is the return value of the
 *    terminated thread.  This value is returned by pthreader_join().
 *
 *    The pthreader_is_cancelled() function checks the return value of a
 *    pthreader_join() call.
 *
 * \return
 *    Returns 'true' if the given thread ID matches the value of
 *    PTHREAD_CANCELED (which is a -1 on Linux).
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthreader_is_cancelled
(
   void * rvalue           /**< The return-value of pthreader_join().         */
)
{
   return rvalue == PTHREAD_CANCELED;
}

/******************************************************************************
 * pthreader_create()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a thread, passing it a thread function and a generic data
 *    area.
 *
 *    pthreader_create() implements and augments the pthread_create()
 *    function.
 *
 *    The thread is started so it runs right away; suspended threads are not
 *    supported by the XPC library.
 *
 * \usage
 *    -# Define a thread callback function that will do all the processing
 *       for the thread.  This function should have the following
 *       properties:
 *       -# It can be exited in three ways:
 *          -# Via return, with the "exit-code" being returned.
 *          -# Via pthread_exit(), with the "exit-code" being passed to
 *             this function.  The XPC library provides no wrapper for that
 *             function.
 *          -# Being cancelled by another thread in the same process.
 *       -# The "exit-code" can be one of the following types.  All callers
 *          must have knowledge of the format of the exit-code.
 *          -# A pointer to a global variable or structure.
 *          -# An integer value casted to a void pointer.
 *       -  The thread function can insert pthread_testcancel() calls in
 *          order to introduce cancellation points into long stretches of
 *          code.  The XPC provides no wrapper for that function.
 *       -# A thread function can call optional thread-cleanup handler
 *          functions using the pthread_cleanup_push() or
 *          pthread_cleanup_pop() functions.  Right now, the XPC library
 *          doesn't provide any wrapper for these optional functions.
 *    -# Define a persistent data area, if the thread needs to operate
 *       on data not local to the thread function.
 *    -# Call pthread_attr_init() or pthread_attr_initialize() to set up the
 *       desired attributes.  One of these functions must be called first,
 *       unless a null pointer is passed as the \a thread_attr parameter.
 *    -# Call pthreader_create(), passing in the items defined above.
 *
 * \win32
 *    The thread is created with no security token.
 *
 * \return
 *    The thread ID is returned if the thread was successfully created.
 *    Otherwise, a "null" thread ID is returned.  The caller can check the
 *    return value with pthreader_is_valid_thread().
 *
 *//*-------------------------------------------------------------------------*/

pthread_t
pthreader_create
(
   const pthread_attr_t * thread_attr, /**< An initialized attributes struct. */
   pthreader_func_t thread_callback,   /**< The thread callback function.     */
   void * thread_data                  /**< Optional data for the thread.     */
)
{
   pthread_t result = pthreader_null_thread();

   /*
    * The casts avoid warnings in gcc and Visual C.
    */

   if (not_nullptr((void *) (intptr_t) thread_callback))
   {
      pthread_t thread_tid;                  /* a place to put the thread ID  */
      int rcode = pthread_create             /* create and start the thread   */
      (
         &thread_tid, thread_attr, thread_callback, thread_data
      );
      if (not_posix_success(rcode))
         xpc_strerrnoprint_func(_("failed"));
      else
      {
         result = thread_tid;                /* provide thread ID for caller  */
         if (pthreader_is_null_thread(result))
            xpc_warnprint_func(_("null thread ID"));
      }
   }
   return result;
}

/******************************************************************************
 * pthreader_yield()
 *------------------------------------------------------------------------*//**
 *
 *    Causes the thread to yield control to the thread scheduler.
 *
 *    pthreader_yield() implements the sched_yield() function.
 *    Win32 Sleep() function called with a sleep-time of zero.
 *
 *    Note the usage of xpc_strerrnoprint(), and be sure to remember that each
 *    thread has its own errno variable.
 *
 * \posix
 *    POSIX systems on which sched_yield() is available will define
 *    _POSIX_PRIORITY_SCHEDULING in <unistd.h>
 *
 *    See also sched_setscheduler(2) for a description of Linux scheduling.
 *
 *    The sched_yield(2) man page indicates that the process is what is doing
 *    the yielding, not a thread.  Does this make any difference in
 *    functionality?  Is it affected by the contention scope of the thread?
 *
 * \linux
 *    The sched_yield() function always succeeds in Linux.  See
 *    sched_yield(2) on such systems.
 *
 * \win32
 *    In the pthreads_w32 library, all this function does is call Sleep(0)
 *    and return 0.
 *
 *//*-------------------------------------------------------------------------*/

void
pthreader_yield (void)
{
   if (not_posix_success(sched_yield()))           /* side-effect:  errno     */
      xpc_strerrnoprint_func(_("failed"));
}

/******************************************************************************
 * pthreader_join()
 *------------------------------------------------------------------------*//**
 *
 *    pthreader_join() implements the pthread_join() function.
 *
 *    Furthermore, it allows the passing back of a "thread-return" value.
 *
 * \posix
 *    pthread_join() suspends the execution of the calling thread until the
 *    specified thread terminates [either by calling pthread_exit() or by
 *    being cancelled].  The \a data_return parameter is filled either with
 *    the value the thread function passed to pthread_exit(), or
 *    PTHREAD_CANCELED (defined as "(void *) -1" in /usr/include/pthread.h).
 *
 *    When a joinable thread terminates, its memory resources (thread
 *    descriptor and stack) are not deallocated until another thread
 *    performs pthread_join on it. Therefore, pthread_join must be called
 *    once for each joinable thread created to avoid memory leaks.
 *
 *    At most one thread can wait for the termination of a given thread.
 *    Calling pthread_join on a thread th on which another thread is already
 *    waiting for termination returns an error.
 *
 * \return
 *    If the join succeeds, then the data for the joined thread is returned.
 *    If the thread was cancelled, the PTHREAD_CANCELED is returned.
 *    Otherwise, nullptr is returned.  (This could be the value of the
 *    original data, however.)
 *
 *//*-------------------------------------------------------------------------*/

void *
pthreader_join
(
   pthread_t th            /**< The thread ID of the thread to be joined.     */
)
{
   void * result = nullptr;
   if (pthreader_is_valid_thread(th))
   {
      void * data_return;                       /* is this the original data? */
      int rc = pthread_join(th, &data_return);
      if (is_posix_success(rc))
      {
         result = data_return;
         if (data_return == PTHREAD_CANCELED)
            xpc_infoprint_func(_("thread cancelled"));
      }
      else
      {
         /*
          * The error code is returned.  It is zero (0 or POSIX_SUCCESS) if
          * no error occurred, and non-zero if an error occurred.
          */

         xpc_strerrnoprint_func(_("failed"));
      }
   }
   return result;
}

/******************************************************************************
 * pthreader_cancel()
 *------------------------------------------------------------------------*//**
 *
 *    pthreader_cancel() implements the pthread_cancel() function.
 *
 * \posix
 *    Cancellation is the mechanism by which a thread can terminate the
 *    execution of another thread. More precisely, a thread can send a
 *    cancellation request to another thread. Depending on its settings, the
 *    target thread can then either ignore the request, honor it immediately,
 *    or defer it till it reaches a cancellation point.
 *
 *    When a thread eventually honors a cancellation request, it performs as
 *    if pthread_exit(PTHREAD_CANCELED) has been called at that point: all
 *    cleanup handlers are executed in reverse order, finalization functions
 *    for thread-specific data are called, and finally the thread stops
 *    executing with the return value PTHREAD_CANCELED.
 *
 *    The pthread_cancel() function returns an error code.  It is zero (0 or
 *    POSIX_SUCCESS) if no error occurred, and non-zero if an error
 *    occurred.
 *
 * \return
 *    Returns 'true' if the cancellation succeeded.  Otherwise, 'false' is
 *    returned.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthreader_cancel
(
   pthread_t th            /**< The thread ID of the thread to be cancelled.  */
)
{
   cbool_t result = false;
   if (pthreader_is_valid_thread(th))
   {
      int rcode = pthread_cancel(th);
      if (not_posix_success(rcode))
         xpc_strerrnoprint_func(_("failed"));
      else
         result = true;
   }
   return result;
}

/******************************************************************************
 * pthreader_detach()
 *------------------------------------------------------------------------*//**
 *
 *    pthreader_detach() implements the detach() function.
 *
 *    pthread_detach() puts the thread in the detached state. This
 *    guarantees that the memory resources consumed will be freed
 *    immediately when th terminates. However, this prevents other threads
 *    from synchronizing on the termination using pthread_join().  Please
 *    read the pthread_detach() man page for more information.
 *
 * \return
 *    The error code is returned.  It is zero (0 or POSIX_SUCCESS) if no
 *    error occurred, and non-zero if an error occurred.
 *
 *//*-------------------------------------------------------------------------*/

int
pthreader_detach
(
   pthread_t th            /**< The thread ID of the thread to be cancelled.  */
)
{
   int result = POSIX_ERROR;
   if (pthreader_is_valid_thread(th))
   {
      result = pthread_detach(th);
      if (not_posix_success(result))
         xpc_strerrnoprint_func(_("failed"));
   }
   return result;
}

/******************************************************************************
 * pthreader_set_cancel()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the cancellation state of the current thread.
 *
 *    This function sets the cancel states of a POSIX thread (Win32 supports
 *    this concept using the open-source pthreads-w32 library).
 *
 *    Threads  are always created by pthread_create() with cancellation
 *    enabled and deferred. That is, the initial cancellation  state is
 *    PTHREAD_CANCEL_ENABLE   and   the  initial  type  is PTHREAD_CAN-
 *    CEL_DEFERRED.
 *
 * \return
 *    Returns true if the settings all succeeded.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthreader_set_cancel
(
   cbool_t cancelenable,   /**< True if cancellation is to be enabled.        */
   cbool_t cancelasync     /**< True if cancellation is to be asynchronous.   */
)
{
   int rcode;
   if (cancelenable)
   {
      rcode = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
      if (is_posix_success(rcode))
      {
         if (cancelasync)
            rcode = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
         else
            rcode = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);

         if (not_posix_success(rcode))
            xpc_strerrnoprint(_("pthread_setcanceltype() failed"));
      }
      else
         xpc_strerrnoprint(_("pthread_setcanceltype() failed"));
   }
   else
   {
      rcode = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
      if (not_posix_success(rcode))
         xpc_strerrnoprint(_("pthread_setcanceltype() failed"));
   }
   return is_posix_success(rcode);
}

/******************************************************************************
 * pthreader.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
