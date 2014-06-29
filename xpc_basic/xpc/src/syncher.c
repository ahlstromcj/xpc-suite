/******************************************************************************
 * syncher.c
 *------------------------------------------------------------------------*//**
 *
 * \file          syncher.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       04/29/2008-05/01/2008
 * \updates       2008-04-29 to 2010-09-11
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides an object for simple, testable synchronization between the
 *    threads of an application.
 *
 *    Part of the reason for including this item in the XPC library is to
 *    give this particular implementation a lot of testing.  Also, it is
 *    used in the errorlogging.c module as an option to emit log messages
 *    without jumbling them together.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/syncher.h>               /* xpc_syncher_t and its functions     */
XPC_REVISION(syncher)

/*******************************************************************************
 * xpc_syncher_delete()
 *------------------------------------------------------------------------*//**
 *
 *    Destroys or uninitializes a syncher-object.
 *
 *    If the object is still owned (that is, entered or locked), there's
 *    really no choice but to spin and wait for it to become free.  This
 *    is handled differently in each operating system.
 *
 * \posix
 *    This function checks if the m_Syncher_Owned flag is set.  If so,
 *    then the syncher-object object is forcibly unlocked by a call to
 *    pthread_mutex_unlock().
 *
 *    [This will only error out for an "error-checking" mutex, which is
 *    supported here only for debugging.]
 *
 *    After the forced unlocking, then pthread_mutex_destroy() is called.
 *
 *    This is \e different from what is done in Windows.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 * \return
 *    Returns 'true' if the parameter is valid and the function
 *    succeeded.  Otherwise, 'false' is returned.
 *
 * \note
 *    -  There may be potential issues with multiple threads acting on the
 *       "ok" and "owned" flags, but the code should work in the x86
 *       environment.
 *    -  Since this is an internal helper function, the parameter is not
 *       checked.
 *
 * \return
 *    Returns 'true' if the function succeeded.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_syncher_delete
(
   xpc_syncher_t * xs      /**< The "This" for this function, assumed valid.  */
)
{
   cbool_t result;
   int rc;
   if (xs->m_Syncher_Owned)
   {
      rc = pthread_mutex_unlock(&xs->m_Syncher);
      result = is_posix_success(rc);
      if (! result)
         xpc_strerrprint_func(_("can't unlock mutex"), rc);
   }
   rc = pthread_mutex_destroy(&xs->m_Syncher);
   xs->m_Syncher_Owned = false;
   result = is_posix_success(rc);
   if (! result)
      xpc_strerrprint_func(_("failed"), rc);

   return result;
}

/******************************************************************************
 * xpc_syncher_create()
 *------------------------------------------------------------------------*//**
 *
 *    Creates and initializes a platform-dependent object to implement a
 *    syncher-object.
 *
 *    This function allocates the locking member (syncher-object),
 *    m_Syncher, and then calls xpc_syncher_create().
 *
 *    In addition to the syncher-object, there is a boolean that
 *    indicates if it is useable, and a boolean that indicates if it is
 *    owned.  These members are also initialized.
 *
 * \warning
 *    The "ok" and "owned" flags can potentially not yet be set before
 *    another thread tries to use the critical section.  In fact, the
 *    "ok" flag cannot be guaranteed to be off, so it cannot be used to
 *    check that the critical section is already initialized at this
 *    point.
 *
 * \param recursive
 *    If 'true', the syncher-object is recursive.  This value applies
 *    only to POSIX code, and is ignored under Windows.  See the "Win32"
 *    note below.
 *
 * \posix
 *    Avoid using the recursive type of mutex, as it requires that the
 *    owning thread call the unlock function the same number of times as the
 *    lock function was called.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *    The syncher-object is always recursive.  However, it is wise to
 *    avoid using the syncher-object in a recursive manner, as it is
 *    difficult to be sure that locks and unlocks match up in number.
 *
 * \debug
 *    Set the kind of mutex.  By default, this is a 'fast' mutex
 *    (PTHREAD_MUTEX_FAST_NP).
 *
 *    For debugging, we use an error-checking mutex
 *    (PTHREAD_MUTEX_ERRORCHECK_NP).  This allows one the chance to find
 *    threading errors under Linux, and can be used to find obvious
 *    threading errors that would occur if the code were compiled for Win32.
 *
 *    If recursive is true (not recommended), we want
 *    PTHREAD_MUTEX_RECURSIVE_NP.  See "man pthread_mutexattr_settype" for
 *    more information.
 *
 *    Debugging is turned on by defining DEBUG_MUTEX_LOCKS.  We could use
 *    error-checking mutexes all the time, but they are not available in any
 *    form under Windows.
 *
 * \posix
 *    This function is implemented using pthread_mutex_init().  There is
 *    no practical way this function can fail under POSIX.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 * \param recursive
 *    If recursive is 'true', the critical section is recursive.  This value
 *    applies only to POSIX code, and is ignored under Windows.  Under
 *    Windows, the critical section is always recursive.  However, in both
 *    environments, avoid relying on recursive usage of syncher-objects.
 *
 * \return
 *    Returns 'true' if the initialization succeed.  Otherwise, 'false'
 *    is returned.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syncher_create
(
   xpc_syncher_t * xs,     /**< The "this-pointer" for this function.         */
   cbool_t recursive       /**< Flag for turning on the recursive feature.    */
)
{
   cbool_t result = false;
   if (is_thisptr(xs))
   {
      int rc;
      pthread_mutexattr_t attr;
      rc = pthread_mutexattr_init(&attr);                /* always "succeeds" */
      xs->m_Syncher_Owned = false;
      xs->m_Syncher_Ready = false;
      if (recursive)
      {
         pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
         xpc_warnprint_func(_("recursion specified"));
      }
      else
      {
#if defined DEBUG_MUTEX_LOCKS                      /* default is fast mutex   */
         rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
         if (not_posix_success(rc))
            xpc_errprint_func(_("settin up error-checking mutex failed"));
#endif
      }
      if (is_posix_success(rc))
      {
         rc = pthread_mutex_init(&xs->m_Syncher, &attr);
         (void) pthread_mutexattr_destroy(&attr);
         result = is_posix_success(rc);
         if (result)
            xs->m_Syncher_Ready = true;
         else
            xpc_strerrprint_func(_("failed"), rc);
      }
   }
   else
      result = false;

   return result;
}

/******************************************************************************
 * xpc_syncher_destroy()
 *------------------------------------------------------------------------*//**
 *
 *    Uninitializes and deallocates the syncher-object object.
 *
 *    The platform-dependent uninitialization is performed by the
 *    xpc_syncher_delete() function.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *    Should we try to leave() this syncher-object?  Not sure; don't want to
 *    do that without some heavy testing.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syncher_destroy
(
   xpc_syncher_t * xs      /**< The "this-pointer" for this function.         */
)
{
   cbool_t result = false;
   if (is_thisptr(xs))
   {
      cbool_t ok;
      if (xs->m_Syncher_Owned)
      {
         xpc_errprint_func(_("deleting an owned syncher-object"));
         result = false;
      }
      else
         result = true;

      xs->m_Syncher_Ready = false;                 /* disable the item now    */
      ok = xpc_syncher_delete(xs);
      if (result)
         result = ok;
   }
   return result;
}

/******************************************************************************
 * xpc_syncher_enter()
 *------------------------------------------------------------------------*//**
 *
 *    This function locks (enters) the critical-section.
 *
 *    The m_Syncher_Owned flag is used to prevent destruction of the
 *    critex while it is still owned.  Under Win32, this act has
 *    undefined effects.
 *
 *    The m_Syncher_Owned flag is a potential issue.  However, once we
 *    acquire the lock, we've got exclusive access to that flag anyway,
 *    so we're safe.
 *
 *    Also, because of the ability to call EnterCriticalSection multiple
 *    times, as noted below for Win32, it is best to explicitly avoid
 *    doing so, and the m_Syncher_Owned flag prevents this.  There is no
 *    way for one thread to tell if it is the thread currently having
 *    ownership.
 *
 * \posix
 *    The pthread_mutex_lock() function returns an error if the mutex has
 *    not been initialized, or if the mutex is an error-checking mutex
 *    and the mutex is already locked.  This utility does not support
 *    error-checking mutexes except for debugging.
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *    This function can raise EXCEPTION_POSSIBLE_DEADLOCK if the critical
 *    section is corrupt or deadlock detection is enabled. Do not handle
 *    this exception; continue execution, or debug the application.
 *
 *    After a thread has ownership of a critical-section, it can make more
 *    calls to EnterCriticalSection() or TryEnterCriticalSection() without
 *    blocking. This prevents a thread from self-deadlock on a
 *    critical-section it already owns.  A thread must call
 *    LeaveCriticalSection() once for each time that it entered the
 *    critical-section.
 *
 *    If a thread terminates while it has ownership of a critical section,
 *    the state of the critical-section is undefined.
 *
 *    If a critical-section is deleted while it is still owned, the state of
 *    the threads waiting for ownership of the deleted critical section is
 *    undefined.
 *
 *    There is no way to make this function foolproof under Windows:
 *
 *       EnterCriticalSection() \n
 *       http://msdn2.microsoft.com/en-us/library/ms682608.aspx
 *
 *    However, note that, on Windows XP or later, EnterCriticalSection() is
 *    guaranteed not to fail due to lack of resources.
 *
 * \return
 *    Returns 'true' if the parameters are valid and the locking operation
 *    succeeds.  Otherwise 'false' is returned.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syncher_enter
(
   xpc_syncher_t * xs      /**< The "this-pointer" for this function.         */
)
{
   cbool_t result = false;
   if (is_thisptr(xs))
   {
      int rc = pthread_mutex_lock(&xs->m_Syncher);
      result = is_posix_success(rc);
      if (result)
         xs->m_Syncher_Owned = true;
      else
         xpc_strerrprint_func(_("failed"), rc);
   }
   return result;
}

/******************************************************************************
 * xpc_syncher_try_enter()
 *------------------------------------------------------------------------*//**
 *
 *    Performs a try-lock on a critical section.
 *
 *    A try-lock function tries to gain ownership of the
 *    critical-section.  Rather than blocking on the function call when
 *    it cannot attain ownership of the critical-section, It returns
 *    'false' if it did not gain ownership.
 *
 * \posix
 *    This function calls pthread_mutex_trylock().
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *    This function calls TryEnterCriticalSection(), and hence it
 *    requires Win NT 4.0 and above.
 *
 *       TryEnterCriticalSection() \n
 *       http://msdn2.microsoft.com/en-us/library/ms686857.aspx
 *
 * \return
 *    Returns 'true' if the parameter is valid and the function
 *    succeeded.  Otherwise, 'false' is returned, including if the
 *    Windows version for which the code was build is pre-NT-4.0.
 *    There's no easy way to distinguish between failure and merely being
 *    unable to own the lock.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syncher_tryenter
(
   xpc_syncher_t * xs      /**< The "this-pointer" for this function.         */
)
{
   cbool_t result = false;
   if (is_thisptr(xs))
   {
      int rc = pthread_mutex_trylock(&xs->m_Syncher);
      result = is_posix_success(rc);
      if (result)
         xs->m_Syncher_Owned = true;
   }
   return result;
}

/******************************************************************************
 * xpc_syncher_leave()
 *------------------------------------------------------------------------*//**
 *
 *    Unlocks a critical-section object.
 *
 *    Unlocking a critical-section means giving up ownership of it, so the
 *    m_Syncher_Owned flag gets set to false.  It gets set just before
 *    the call that unlocks the critical section.  It is erroneous to unlock
 *    it after the unlocking, because another thread may have gotten
 *    ownership and set it to true, only to have the current thread falsify
 *    it.
 *
 * \posix
 *    This function calls pthread_mutex_unlock().
 *
 * \win32
 *    Under Win32, the pthread_w32 port of the pthread API is used.
 *
 *    This function calls LeaveCriticalSection().
 *
 * \return
 *    Returns 'true' if the parameter is valid and the function succeeded.
 *    Otherwise, 'false' is returned, including if the Windows version for
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_syncher_leave
(
   xpc_syncher_t * xs      /**< The "this-pointer" for this function.         */
)
{
   cbool_t result = false;
   if (is_thisptr(xs))
   {
      int rc;
      xs->m_Syncher_Owned = false;
      rc = pthread_mutex_unlock(&xs->m_Syncher);
      result = is_posix_success(rc);
      if (! result)
         xpc_strerrprint_func(_("failed"), rc);
   }
   return result;
}

/******************************************************************************
 * syncher.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
