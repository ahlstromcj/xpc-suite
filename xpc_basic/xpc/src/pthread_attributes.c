/******************************************************************************
 * pthread_attributes.c
 *------------------------------------------------------------------------*//**
 *
 * \file          pthread_attributes.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       05/01/2008-08/24/2009
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This utility provides global convenience functions for accessing the
 *    POSIX pthread_attr_t structure.
 *
 * \posix
 * \win32
 *    For the XPC library, we're going to employ the pthreads_w32 LGPL library
 *    to map the POSIX threads API onto Windows.
 *
 *    Also see pthread_attributes.h for more documentation.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/pthread_attributes.h>    /* pthread attribute functions         */
XPC_REVISION(pthread_attributes)

/******************************************************************************
 * pthread_attributes_init()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the pthread_attr_t structure to XPC defaults.
 *
 *    These defaults are currently the same as the normal attribute
 *    defaults described in pthread_attr_init(3), though some are set
 *    explicitly.
 *
 * \todo
 *    The priority setting is not yet supported; we have more learning to
 *    do.
 *
 * \return
 *    Returns 'true' if the parameter was valid and the setting succeeded.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthread_attributes_init
(
   pthread_attr_t * attributes   /**< The thread attributes structure.        */
)
{
   return pthread_attributes_initialize
   (
      attributes,
      PTHREAD_CREATE_JOINABLE,      /* detachstate       */
      SCHED_OTHER,                  /* policy            */
      -1,                           /* priority          */
      PTHREAD_EXPLICIT_SCHED,       /* inherit           */
      PTHREAD_SCOPE_SYSTEM,         /* scope             */
      0,                            /* stack size        */
      0                             /* stack guard size  */
   );
}

/******************************************************************************
 * pthread_attributes_initialize()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the pthread_attr_t structure to the values provided.
 *
 *    This function provides a convenient way to completely tailor the
 *    attribute structure prior to creating a POSIX thread.
 *
 *    If you don't want an integer parameter to be modified, then pass a
 *    value of -1 for it.  If you don't want a size_t parameter to be
 *    modified, then pass 0 for it.  If you don't want a pointer parameter
 *    to be modified, then pass 'nullptr' for it.
 *
 *    Here are all of the parameters and brief descriptions of them.  Not
 *    all of them are explained in pthread_attr_init(3), but they all appear
 *    in /usr/include/pthread.h
 *
 *    -  Joinable state versus detached state.
 *       -  PTHREAD_CREATE_JOINABLE [default].  Allows another thread to
 *          synchronize on the threads termination using pthread_join().
 *          Some thread resources persist after termination, until
 *          pthread_join() is called.
 *       -  PTHREAD_CREATE_DETACHED.  Resources are freed upon thread
 *          termination, and no synchronization on termination is possible.
 *    -  Scheduling policy.
 *       -  SCHED_OTHER [default].  Regular non-realtime scheduling.
 *       -  SCHED_BATCH.  Batch-style (CPU intensive) execution.
 *       -  SCHED_RR.  Realtime round-robin scheduling.  Requires superuser
 *          privilege.
 *       -  SCHED_FIFO.  Realtime first-in/first-out.  Requires superuser
 *          privilege.
 *    -  Scheduling parameter (priority).  This item applies only to
 *       SCHED_RR and SCHED_FIFO.  It is a structure that include a number
 *       of items as described in sched_setparam(2) and
 *       sched_setscheduler(2).  The priority item must be 0 for SCHED_OTHER
 *       and SCHED_BATCH.  The priority range for the realtime scheduling
 *       items is 1 to 99.
 *       <b> For now, the scheduling parameter is not supported.</b>
 *    -  Scheduling policy and parameters inheritance.
 *       -  PTHREAD_EXPLICIT_SCHED [default].
 *       -  PTHREAD_INHERIT_SCHED.
 *    -  Scheduling contention scope.
 *       -  PTHREAD_SCOPE_SYSTEM [default].  The thread contends with all
 *          processes for CPU time.
 *       -  PTHREAD_SCOPE_PROCESS.  The threads in a process contend only
 *          with each other.  Not supported in LinuxThreads.
 *    -  Stack size.  The size of the stack to be used for the thread.
 *    -  Stack guard size.  This is the size of the guard area created for
 *       stack overflow protection.
 *    -  Stack address.  Sets the starting address of the stack; valid
 *       addresses are system-dependent, and right now this setting is not
 *       supported.  The pthread_attr_setstackaddr() and
 *       pthread_attr_getstackaddr() functions are deprecated, anyway.
 *    -  Processor affinity.  Currently not supported, even in the parameter
 *       list.  It is a non-portable GNU extension.  The process should use
 *       sched_affinity() instead.
 *
 * \return
 *    Returns 'true' if the setting succeeded.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthread_attributes_initialize
(
   pthread_attr_t * attributes,  /**< The thread attributes structure.        */
   int detachstate,              /**< The join/detach state to use.           */
   int policy,                   /**< The scheduling policy value to use.     */
   int priority,                 /**< The thread priority value to use.       */
   int inherit,                  /**< The inheritance policy value to use.    */
   int scope,                    /**< The contention scope value to use.      */
   size_t stacksize,             /**< The thread stack size value to use.     */
   size_t stackguardsize         /**< The thread stack guard size to use.     */
)
{
   cbool_t result = false;
   if (is_thisptr(attributes))
   {
      int rc = pthread_attr_init(attributes);
      result = true;
      if (not_posix_success(rc))
      {
         result = false;
         xpc_strerrnoprint_func(_("pthread_attr_init() failed"));
      }
      if (result && (detachstate != -1))
      {
         rc = pthread_attr_setdetachstate(attributes, detachstate);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setdetachstate() failed"));
         }
      }
      if (result && (policy != -1))
      {
         rc = pthread_attr_setschedpolicy(attributes, policy);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setschedpolicy() failed"));
         }
      }
      if (result && (priority != -1))
      {
         /**
          * \todo
          *    rc = pthread_attr_setschedparam(attributes, ...);
          */
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setschedparam() failed"));
         }
      }
      if (result && (inherit != -1))
      {
         rc = pthread_attr_setinheritsched(attributes, inherit);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setinheritsched() failed"));
         }
      }
      if (result && (scope != -1))
      {
         rc = pthread_attr_setscope(attributes, scope);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setscope() failed"));
         }
      }
      if (result && (stacksize > 0))
      {
         rc = pthread_attr_setstacksize(attributes, stacksize);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setstacksize() failed"));
         }
      }
      if (result && (stackguardsize > 0))
      {
#ifdef WIN32
         xpc_warnprint_func(_("guard size not supported in pthreads-w32"));
#else
         rc = pthread_attr_setguardsize(attributes, stackguardsize);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_setguardsize() failed"));
         }
#endif
      }
   }
   return result;
}

/******************************************************************************
 * pthread_attributes_get()
 *------------------------------------------------------------------------*//**
 *
 *    Retrieves most pthread_attr_t values into a list of arguments.
 *
 * \return
 *    Return's true if all of the parameter pointers are valid, and all of
 *    the "pthread_attr_get" calls succeeded.  Otherwise, 'false' is
 *    returned, and none of the results should be relied on.
 *
 * \sideeffect
 *    The int and size_t parameters are modified to reflect the values
 *    stored in the pthread_attr_t structure.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthread_attributes_get
(
   pthread_attr_t * attributes,  /**< Thread attributes structure to read.    */
   int * detachstate,            /**< Place to store the join/detach state.   */
   int * policy,                 /**< Place to store the scheduling policy.   */
   int * priority,               /**< Place to store the thread priority.     */
   int * inherit,                /**< Place to store the inheritance policy.  */
   int * scope,                  /**< Place to store the contention scope.    */
   size_t * stacksize,           /**< Place to store the thread stack size.   */
   size_t * stackguardsize       /**< Place to store the stack guard size.    */
)
{
   cbool_t result = false;
   if (is_thisptr(attributes))
   {
      int rc = POSIX_ERROR;
      result = true;
      if (result && (not_nullptr(detachstate)))
      {
         rc = pthread_attr_getdetachstate(attributes, detachstate);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getdetachstate() failed"));
         }
      }
      if (result && (not_nullptr(policy)))
      {
         rc = pthread_attr_getschedpolicy(attributes, policy);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getschedpolicy() failed"));
         }
      }
      if (result && (not_nullptr(priority)))
      {
         /**
          * \todo
          *    rc = pthread_attr_getschedparam(attributes, ...);
          */
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getschedparam() failed"));
         }
      }
      if (result && (not_nullptr(inherit)))
      {
         rc = pthread_attr_getinheritsched(attributes, inherit);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getinheritsched() failed"));
         }
      }
      if (result && (not_nullptr(scope)))
      {
         rc = pthread_attr_getscope(attributes, scope);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getscope() failed"));
         }
      }
      if (result && (not_nullptr(stacksize)))
      {
         rc = pthread_attr_getstacksize(attributes, stacksize);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getstacksize() failed"));
         }
      }
      if (result && (not_nullptr(stackguardsize)))
      {
#ifdef WIN32
         xpc_warnprint_func(_("guard size not supported in pthreads-w32"));
         *stackguardsize = 0;
#else
         rc = pthread_attr_getguardsize(attributes, stackguardsize);
         if (not_posix_success(rc))
         {
            result = false;
            xpc_strerrnoprint_func(_("pthread_attr_getguardsize() failed"));
         }
#endif
      }
   }
   return result;
}

/******************************************************************************
 * pthread_attributes_show()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains all of the thread attributes and shows them in a nice format.
 *
 *    This showing is done only if xpc_showinfo() is 'true'.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

void
pthread_attributes_show
(
   pthread_attr_t * attributes   /**< The thread attributes structure.        */
)
{
   if (is_thisptr(attributes) && xpc_showinfo())
   {
      int detachstate;
      int policy;
      int priority;
      int inherit;
      int scope;
      size_t stacksize;
      size_t stackguardsize;
      cbool_t ok = pthread_attributes_get
      (
         attributes, &detachstate, &policy, &priority,
         &inherit, &scope, &stacksize, &stackguardsize
      );
      if (ok)
      {
         xpc_infoprintf
         (
            "pthread_attr_t:\n"
            "   detachstate:     %d\n"
            "   policy:          %d\n"
            "   priority:        %d\n"
            "   inherit:         %d\n"
            "   scope:           %d\n"
            "   stacksize:       %d\n"
            "   stackguardsize:  %d\n"
            ,
            detachstate, policy, priority,
            inherit, scope, stacksize, stackguardsize
         );
      }
   }
}

/******************************************************************************
 * pthread_attributes_set_stack_size()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the stack size of the given pthread attribute structure.
 *
 *    In POSIX, pthread_attr_setstacksize() returns EINVAL if the stack size
 *    is less than PTHREAD_STACK_MIN, or is larger than the limit imposed by
 *    the operating system.  This function logs the error if it occurs.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the setting succeeded.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthread_attributes_set_stack_size
(
   pthread_attr_t * pattr,    /**< The thread-attribute structure to change.  */
   size_t stacksize           /**< The stack size to be applied.              */
)
{
   cbool_t result = is_thisptr(pattr);
   if (result)
   {
      result = is_posix_success(pthread_attr_setstacksize(pattr, stacksize));
      if (! result)
         xpc_strerrnoprint_func(_("failed"));
   }
   return result;
}

/******************************************************************************
 * pthread_attributes_get_stack_size()
 *------------------------------------------------------------------------*//**
 *
 *    Gets the stack size of the given pthread attribute structure, and
 *    shows it if error-logging at "info" level is turned on.
 *
 *    The pthread_attr_t is an opaque structure (read /usr/include/pthread.h
 *    and see for yourself), so there is no other way to obtain this value.
 *
 * \return
 *    Returns the size of the stack specified in the pthread_attr_t
 *    structure if the pointer is valid.  Otherwise, 0 is returned.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

size_t
pthread_attributes_get_stack_size
(
   pthread_attr_t * pattr     /**< The thread-attribute structure to change.  */
)
{
   size_t result = 0;
   if (is_thisptr(pattr))
   {
      size_t size = 0;
      if (is_posix_success(pthread_attr_getstacksize(pattr, &result)))
         xpc_infoprintf("%s %d", _("stack size is"), size);
      else
      {
         result = 0;
         xpc_strerrnoprint_func(_("failed"));
      }
   }
   return result;
}

/******************************************************************************
 * pthread_attributes_set_detach_state()
 *------------------------------------------------------------------------*//**
 *
 *    Sets the stack size of the current thread.
 *
 *    I'm not sure exactly how this function works, I think it has to be
 *    studied and potentially corrected.
 *
 * \note
 *    Normally, this function needs to be called only to enable the
 *    'detached' state, as the normal state is 'joinable'.
 *
 * \return
 *    Returns true if the settings succeeded.
 *
 * \unittests
 *    TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
pthread_attributes_set_detach_state
(
   pthread_attr_t * pattr,    /**< The thread-attribute structure to change   */
   int s                      /**< The detach-state to be applied             */
)
{
   cbool_t result = not_nullptr(pattr);
   if (result)
   {
      if ((s == PTHREAD_CREATE_JOINABLE) || (s == PTHREAD_CREATE_DETACHED))
      {
         result = is_posix_success(pthread_attr_setdetachstate(pattr, s));
         if (! result)
            xpc_strerrnoprint_func(_("failed"));
      }

#ifdef DEBUG                                          /* DEBUG                */

      if (xpc_showinfo())
      {
         int st = 0;
         if (is_posix_success(pthread_attr_getdetachstate(pattr, &st)))
            xpc_infoprintf("%s %d", _("detach state is"), st);
         else
            xpc_strerrnoprint(_("pthread_getdetachstate() failed"));
      }

#endif                                                /* end of DEBUG         */

   }
   return result;
}

/******************************************************************************
 * pthread_attributes.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
