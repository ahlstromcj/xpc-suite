/******************************************************************************
 * syncher_thread_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          syncher_thread_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       05/03/2008-08/23/2009
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    thread-related modules.  It provides a demonstration of how to use
 *    these utilities.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the pthread_attributes.c
 *                   functions.
 *       -  Group 2. Basic unit-tests of the pthreader.c functions.
 *       -  Group 3. Simple tests of threading.
 *       -  Group 4. Basic unit-tests of the syncher.c functions.
 *       -  Group 5. Hard-core performance tests for the syncher utility.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/unit_test.h>             /* unit_test_t structure               */
#include <xpc/pthread_attributes.h>    /* pthread attributes functions        */
#include <xpc/pthreader.h>             /* pthreader functions                 */
#include <xpc/syncher.h>               /* xpc_syncher_t synchronizer          */

#if XPC_HAVE_STDIO_H
#include <stdio.h>
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
 * syncher_thread_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test group provides basic unit-tests of the
 *    pthread_attributes.c module.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \tests
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  pthread_attributes_xxx()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
syncher_thread_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("pthread_xxx"), _("Smoke Test")
   );
   if (ok)
   {
      /**
       * \todo
       *    Make sure we understand this; it seems like this functionality
       *    could be built into both unit_test_status_initialize() and
       *    unit_test_status_next_subtest() instead, or in addition to this
       *    one.
       */

      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "next test"))
            unit_test_status_pass(&status, true);
      }
   }
   return status;
}

/******************************************************************************
 * syncher_thread_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    This first test group provides basic unit-tests of the
 *    pthread.c module.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \tests
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  pthreader_xxx()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
syncher_thread_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 1, _("pthreader_xxx"), _("Smoke Test")
   );
   if (ok)
   {
      if (! unit_test_status_can_proceed(&status)) /* is test allowed to run? */
      {
         unit_test_status_pass(&status, true);     /* no, force it to pass    */
      }
      else
      {
         if (unit_test_options_show_values(options))
            fprintf(stdout, "  %s\n", _("No values to show in this test"));

         /*  1 */

         if (unit_test_status_next_subtest(&status, "next test"))
            unit_test_status_pass(&status, true);
      }
   }
   return status;
}

/******************************************************************************
 * simple_thread_function()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an example of how to write a simple thread function.
 *
 *//*-------------------------------------------------------------------------*/

void *
simple_thread_function
(
   void * thread_id     /**< An integer parameter identifying the thread.     */
)
{
   int tid = (int) (intptr_t) thread_id;
   if (! xpccut_is_silent())
      printf("Simple thread #%d!\n", tid);

   return nullptr;            /* Alternate return:  pthread_exit(nullptr);    */
}

/******************************************************************************
 * sleeper_thread_parameters_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides some parameters to control the xpc_sleeper_thread_function().
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   int thread_identifier;     /**< An ordinal number to identify the thread.  */
   int inner_loop_count;      /**< Number of times the inner loop runs.       */
   int inner_loop_sleep;      /**< Time the thread is suspended each pass.    */
   int outer_loop_count;      /**< Number of time the outer loop runs.        */
   int outer_loop_sleep;      /**< Time the thread is suspended each pass.    */
   int final_inner_count;     /**< Return value of the inner loop count.      */
   int final_outer_count;     /**< Return value of the inner loop count.      */

} sleeper_thread_parameters_t;

/******************************************************************************
 * sleeper_thread_parameters_show()
 *------------------------------------------------------------------------*//**
 *
 *    Shows the parameters used to control the xpc_sleeper_thread_function().
 *
 * \param stp
 *    Provides the parameters structure to reveal.
 *
 *//*-------------------------------------------------------------------------*/

void
sleeper_thread_parameters_show
(
   const sleeper_thread_parameters_t * stp
)
{
   if (is_thisptr(stp) && xpc_showinfo())
   {
      int total_sleep_time_ms = stp->outer_loop_count *
      (
         (stp->inner_loop_count * stp->inner_loop_sleep) + stp->outer_loop_sleep
      );
      xpc_infoprintf
      (
         "sleeper_thread_parameters_t:\n"
         "   thread_identifier:  %d\n"
         "   inner_loop_count:   %d\n"
         "   inner_loop_sleep:   %d\n"
         "   outer_loop_count:   %d\n"
         "   outer_loop_sleep:   %d\n"
         "   final_inner_count:  %d\n"
         "   final_outer_count:  %d\n"
         "   --------------------------\n"
         "   Total time (ms):    %d\n"
         ,
         stp->thread_identifier,
         stp->inner_loop_count,
         stp->inner_loop_sleep,
         stp->outer_loop_count,
         stp->outer_loop_sleep,
         stp->final_inner_count,
         stp->final_outer_count,
         total_sleep_time_ms
      );
   }
}

/******************************************************************************
 * xpc_sleeper_thread_function()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an example of how to write a thread function that can be
 *    cancelled.
 *
 *    The function runs in two nested loops.
 *
 * \return
 *    Returns the original structure, modified to show the final inner and
 *    outer loop counts.
 *
 *//*-------------------------------------------------------------------------*/

void *
xpc_sleeper_thread_function
(
   void * thread_sleeper   /**< Structure controlling action of the thread.   */
)
{
   void * result = nullptr;
   if (not_nullptr(thread_sleeper))
   {
      int tid;
      int inner;
      int outer;
      sleeper_thread_parameters_t * stp = (sleeper_thread_parameters_t *)
         thread_sleeper;

      sleeper_thread_parameters_t ctrller = *stp;
      tid = ctrller.thread_identifier;
      if (! xpccut_is_silent())
         printf("Sleeper thread #%d.\n", tid);

      for (outer = 0; outer < ctrller.outer_loop_count; outer++)
      {
         stp->final_outer_count = outer;
         for (inner = 0; inner < ctrller.inner_loop_count; inner++)
         {
            stp->final_inner_count = inner;
            xpc_ms_sleep(ctrller.inner_loop_sleep);
         }
         xpc_ms_sleep(ctrller.outer_loop_sleep);
      }
      result = thread_sleeper;
   }
   else
   {
      if (! xpccut_is_silent())
         xpc_errprint_func(_("null pointer"));
   }
   return result;                   /* alternative:  pthread_exit(result);    */
}

/******************************************************************************
 * thread_cancel_smoke_test()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a smoke test used in 03.01.3 and above.
 *
 *    It implements a loop with different delays before cancellation, to
 *    make sure the timing is consistent.
 *
 *    The sleeper parameters shown mean that each loop through the
 *    outer loop will take
 *
 *       inner_loop_count * inner_loop_sleep + outer_loop_sleep
 *
 *    milliseconds, or 110 ms in this case.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
thread_cancel_smoke_test
(
   int sleep_time,            /**< TBD */
   int expected_inner_count,  /**< TBD */
   int expected_outer_count   /**< TBD */
)
{
   cbool_t ok;
   pthread_t tid;
   sleeper_thread_parameters_t * stp;
   sleeper_thread_parameters_t loop_stp =
   {
       1,                  /* thread_identifier */
      10,                  /* inner_loop_count  */
      10,                  /* inner_loop_sleep  */
      10,                  /* outer_loop_count  */
      10,                  /* outer_loop_sleep  */
       0,                  /* final_inner_count */
       0                   /* final_outer_count */
   };
   pthread_attr_t x_attributes;
   ok = pthread_attributes_init(&x_attributes);
   tid = pthreader_create
   (
      &x_attributes,
      xpc_sleeper_thread_function,
      (void *) &loop_stp
   );
   ok = pthreader_is_valid_thread(tid);
   if (ok)
   {
      /*
       * Sleep, cancel the thread, and then show and verify the counts.  We
       * cannot join the thread once it has been cancelled, and
       * pthreader_join() returns stp == -1 or 0xffffffff.  This is checked
       * in a more cross-platform manner using pthreader_is_cancelled().
       */

      xpc_ms_sleep(sleep_time);
      ok = pthreader_cancel(tid);
      if (ok)
      {
         stp = (sleeper_thread_parameters_t *) pthreader_join(tid);
         if (is_nullptr(stp))
         {
            sleeper_thread_parameters_show(&loop_stp);
            xpc_warnprint(_("The join returned null!  Impossible!"));
            ok = false;
         }
         else if (pthreader_is_cancelled(stp))
         {
            sleeper_thread_parameters_show(&loop_stp);
         }
         else
         {
            sleeper_thread_parameters_show(stp);
            xpc_warnprint(_("The join succeeded!  Impossible!"));
            ok = false;
         }
         if (ok)
         {
            ok = loop_stp.final_inner_count == expected_inner_count;
            if (ok)
               ok = loop_stp.final_outer_count == expected_outer_count;
         }
      }
   }
   return ok;
}

// #define NUM_THREADS  1 /////////////////////////////////

#define NUM_THREADS  10   /////////////////////////////////

/******************************************************************************
 * syncher_thread_test_03_01()
 *------------------------------------------------------------------------*//**
 *
 *    This test group provides threaded smoke-test unit-tests of the
 *    pthreader.c module.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \tests
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  pthreader_xxx()
 *    -  pthread_attributes_xxx()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
syncher_thread_test_03_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 1, _("pthreader"), _("Smoke Test")
   );
   if (ok)
   {
      pthread_attr_t x_attributes;
      ok = pthread_attributes_init(&x_attributes);

      /*  1 */

      if (unit_test_status_next_subtest(&status, "pthreader_create()"))
      {
         pthread_t threads[NUM_THREADS];
         int t;
         for (t = 0; t < NUM_THREADS; t++)
         {
            if (unit_test_options_is_verbose(options))
               printf("creating thread %d\n", t);

            threads[t] = pthreader_create
            (
               &x_attributes,
               simple_thread_function,
               (void *) (intptr_t) t
            );
            if (pthreader_is_null_thread(threads[t]))
            {
               ok = false;
               break;
            }
         }

#ifdef WIN32
         /**
          * \win32
          *    For the Windows version, we have to add a few milliseconds
          *    of delay, otherwise the thread output will not appear,
          *    since the application can exit too quickly (especially if
          *    03.01 is the only test that is run.  Without this delay,
          *    the test lasts 6 ms (compared to about 1 ms under Linux).
          *
          *    In a virtual machine running Win 2000, 10 ms is not
          *    enough, but 50 ms is enough.  Ironically, delaying for
          *    the show-progress output below requires more sleep time --
          *    30 ms versus 15 ms!
          */

         if (ok)
         {
            xpc_ms_sleep(30);
            if (unit_test_options_show_progress(options))
               fprintf(stdout, _("Should see output from all 10 threads.\n"));
         }
#endif

         /*
          * Since the threads above are in a joinable state, they must
          * be joined in order to avoid a memory leak.
          */

         for (t = 0; t < NUM_THREADS; t++)
         {
            void * jp = pthreader_join(threads[t]);
            if (pthreader_is_cancelled(jp))
            {
               xpc_warnprint(_("thread is cancelled"));
            }
            else if (is_nullptr(jp))
            {
               xpc_warnprint(_("null pointer after joining"));
            }
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "pthreader_join()"))
      {
         pthread_t tid_2;
         sleeper_thread_parameters_t * stp_2;
         sleeper_thread_parameters_t loop_stp_2 =
         {
             0,                  /* thread_identifier */
            10,                  /* inner_loop_count  */
            10,                  /* inner_loop_sleep  */
            10,                  /* outer_loop_count  */
            10,                  /* outer_loop_sleep  */
             0,                  /* final_inner_count */
             0                   /* final_outer_count */
         };

         /*
          * Unnecessary to show the before snapshot:
          *
          * sleeper_thread_parameters_show(&loop_stp_2);
          */

         tid_2 = pthreader_create
         (
            &x_attributes,
            xpc_sleeper_thread_function,
            (void *) &loop_stp_2
         );
         ok = pthreader_is_valid_thread(tid_2);
         if (ok)
         {
            /* Join the thread, and then show and verify the counts. */

            stp_2 = (sleeper_thread_parameters_t *) pthreader_join(tid_2);
            if (! pthreader_is_cancelled(stp_2))
               sleeper_thread_parameters_show(stp_2);

            ok = (stp_2->final_inner_count + 1) ==
               stp_2->inner_loop_count;
            if (ok)
               ok = (stp_2->final_outer_count + 1) ==
                  stp_2->outer_loop_count;
            if (ok)
               ok = (stp_2->final_inner_count + 1) ==
                  loop_stp_2.inner_loop_count;
            if (ok)
               ok = (stp_2->final_outer_count + 1) ==
                  loop_stp_2.outer_loop_count;
         }
         unit_test_status_pass(&status, ok);
      }

      //// xpc_ms_sleep(1000);
      //// return status;    ///////////////////////////////////

      /*  3 */

      if (unit_test_status_next_subtest(&status, "pthreader_cancel(110 ms)"))
      {
         ok = thread_cancel_smoke_test(110, 9, 0);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "pthreader_join(220 ms)"))
      {
         ok = thread_cancel_smoke_test(220, 7, 1);
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "pthreader_join(500 ms)"))
      {
         ok = thread_cancel_smoke_test(500, 8, 3);
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * simple_syncher_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides some parameters to control the simple_syncher_thread_function().
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   int thread_identifier;     /**< An ordinal number to identify the thread.  */
   int loop_count;            /**< Number of times the main loop runs.        */
   int loop_sleep;            /**< Time the thread is suspended each pass.    */
   char emitted_character;    /**< The character to emit as thread runs.      */
   xpc_syncher_t * critex;    /**< To synchronize output with other thread.   */

} simple_syncher_t;

/******************************************************************************
 * simple_syncher_thread_function()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a thread function to test the functionality of the
 *    xpc_syncher_t object.
 *
 * \note
 *    -  The xpc_ms_sleep() function does not sleep if 0 is passed as a
 *       parameter.
 *    -  The critex pointer is not checked, to save time.
 *
 * \return
 *    Returns ...
 *
 *//*-------------------------------------------------------------------------*/

void *
simple_syncher_thread_function
(
   void * thread_syncher   /**< Structure controlling action of the thread.   */
)
{
   void * result = nullptr;
   if (not_nullptr(thread_syncher))
   {
      int tid;
      int index;
      simple_syncher_t * stp = (simple_syncher_t *) thread_syncher;
      simple_syncher_t ctrller = *stp;
      tid = ctrller.thread_identifier;
      xpc_infoprintf("Syncher thread #%d.\n", tid);

      for (index = 0; index < ctrller.loop_count; index++)
      {
         cbool_t ok = xpc_syncher_enter(ctrller.critex);
         if (ok)
         {
            if (! xpccut_is_silent())
               putc(ctrller.emitted_character, stdout);

            (void) xpc_syncher_leave(ctrller.critex);
         }
         xpc_ms_sleep(ctrller.loop_sleep);
      }
      result = thread_syncher;
   }
   else
   {
      if (! xpccut_is_silent())
         xpc_errprint_func(_("null pointer"));
   }
   return result;                   /* alternative:  pthread_exit(result);    */
}

/******************************************************************************
 * LOOP_SLEEP_05_01
 *------------------------------------------------------------------------*//**
 *
 *    LOOP_SLEEP_05_01 provides a way to probe the effects of delays in the
 *    thread processing.
 *
 * \posix
 *    On our dual-core x86 laptop, running Linux, setting this loop-sleep
 *    define to anything greater than 0 causes the expected output:
 *
 *       12121212121212121212
 *
 *    However, setting it to 0 yields:
 *
 *       11111111112222222222
 *
 *    It looks like the first thread finishes before the second one gets to
 *    start.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
#define LOOP_SLEEP_05_01      1
#else
#define LOOP_SLEEP_05_01      1
#endif

/******************************************************************************
 * POST_SLEEP_05_01
 *------------------------------------------------------------------------*//**
 *
 *    POST_SLEEP_05_01 provides another way to probe the effects of delays
 *    in the thread processing.
 *
 *    Instead of coming inside the thread-function's loop, this delay occurs
 *    after the creation of the first thread.
 *
 *    If LOOP_SLEEP_05_01 is 1, and POST_SLEEP_05_01 > 0, then the output
 *    has extra 1's at the beginning.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
#define POST_SLEEP_05_01      0  /* or 1 or 5 */
#else
#define POST_SLEEP_05_01      0
#endif

/******************************************************************************
 * syncher_thread_test_05_01()
 *------------------------------------------------------------------------*//**
 *
 *    This test group provides threaded smoke-test unit-tests of the
 *    pthreader.c module.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \tests
 *    This unit-test function perforce tests more than one unit from the
 *    unit-test library (libxpccut):
 *
 *    -  pthreader_xxx()
 *    -  pthread_attributes_xxx()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
syncher_thread_test_05_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 5, 1, _("syncher"), _("Smoke Test")
   );
   if (ok)
   {
      /*
       * \steps
       *    -# Provide the "XPC default" thread attributes.
       *    -# Create a non-recursive (in POSIX, anyway) syncher object,
       *       called a "critex" (our own term for it).
       *    -# Create two data structures that share the same "critex".
       *    -# Wire the critex into the data structure for the first thread.
       *    -# Wire the critex into the data structure for the second thread.
       */

      pthread_attr_t x_attributes;                    /* Step 1   */
      xpc_syncher_t dual_thread_critex;               /* Step 2   */
      simple_syncher_t sst_1;                         /* Step 3   */
      simple_syncher_t sst_2;                         /* ditto    */
      simple_syncher_t * stp;
      pthread_t thread_1;
      pthread_t thread_2;
      ok = pthread_attributes_init(&x_attributes);
      if (ok)
         ok = xpc_syncher_create(&dual_thread_critex, false);

      if (ok)
      {
         sst_1.thread_identifier =  1;                /* Step 4   */
         sst_1.loop_count        = 10;
         sst_1.loop_sleep        = LOOP_SLEEP_05_01;
         sst_1.emitted_character = '1';
         sst_1.critex            = &dual_thread_critex;

         sst_2.thread_identifier =  2;                /* Step 5   */
         sst_2.loop_count        = 10;
         sst_2.loop_sleep        = LOOP_SLEEP_05_01;
         sst_2.emitted_character = '2';
         sst_2.critex            = &dual_thread_critex;
      }

      /*  1 */

      if (unit_test_status_next_subtest(&status, "Verify first thread"))
      {
         if (ok)
         {
            thread_1 = pthreader_create
            (
               &x_attributes,
               simple_syncher_thread_function,
               (void *) &sst_1
            );
            stp = (simple_syncher_t *) pthreader_join(thread_1);
            if (is_nullptr(stp))
            {
               xpc_warnprint(_("The first join returned null!  Impossible!"));
               ok = false;
            }
            else if (pthreader_is_cancelled(stp))
            {
               xpc_warnprint(_("The first thread was cancelled."));
            }
            else
            {
               xpc_infoprint(_("The first join succeeded"));
            }
            ok = xpc_syncher_destroy(&dual_thread_critex);
            if (! xpccut_is_silent())
               fprintf(stdout, "\n");
         }
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "Verify second thread"))
      {
         /*
          * Re-initialize the syncher object, since it was destroyed at the
          * end of the previous sub-test.
          */

         if (ok)
            ok = xpc_syncher_create(&dual_thread_critex, false);

         if (ok)
         {
            thread_2 = pthreader_create
            (
               &x_attributes,
               simple_syncher_thread_function,
               (void *) &sst_2
            );
            stp = (simple_syncher_t *) pthreader_join(thread_2);
            if (is_nullptr(stp))
            {
               xpc_warnprint(_("The second join returned null!  Impossible!"));
               ok = false;
            }
            else if (pthreader_is_cancelled(stp))
            {
               xpc_warnprint(_("The second thread was cancelled."));
            }
            else
            {
               xpc_infoprint(_("The second join succeeded."));
            }
            ok = xpc_syncher_destroy(&dual_thread_critex);
            if (! xpccut_is_silent())
               fprintf(stdout, "\n");
         }
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "Run both threads"))
      {
         /*
          * Re-initialize the syncher object, since it was destroyed at the
          * end of the previous sub-test.
          */

         if (ok)
            ok = xpc_syncher_create(&dual_thread_critex, false);

         if (ok)
         {
            thread_1 = pthreader_create
            (
               &x_attributes,
               simple_syncher_thread_function,
               (void *) &sst_1
            );
            xpc_ms_sleep(POST_SLEEP_05_01);
            thread_2 = pthreader_create
            (
               &x_attributes,
               simple_syncher_thread_function,
               (void *) &sst_2
            );
            stp = (simple_syncher_t *) pthreader_join(thread_1);
            if (is_nullptr(stp))
            {
               xpc_warnprint(_("The first join returned null!  Impossible!"));
               ok = false;
            }
            else if (pthreader_is_cancelled(stp))
            {
               xpc_warnprint(_("The first thread was cancelled."));
            }
            else
            {
               xpc_infoprint(_("The first join succeeded"));
            }
            stp = (simple_syncher_t *) pthreader_join(thread_2);
            if (is_nullptr(stp))
            {
               xpc_warnprint(_("The second join returned null!  Impossible!"));
               ok = false;
            }
            else if (pthreader_is_cancelled(stp))
            {
               xpc_warnprint(_("The second thread was cancelled."));
            }
            else
            {
               xpc_warnprint(_("The second join succeeded."));
            }
            ok = xpc_syncher_destroy(&dual_thread_critex);
            if (! xpccut_is_silent())
               fprintf(stdout, "\n");
         }
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

#define XPC_APP_NAME          "XPC library syncher/pthreader test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The executable name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "syncher_thread_ut"

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
 *    This is the main routine for the syncher_thread_ut application.
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
"--leak-check        Disable some of the 'destructor' calls.  This item can\n"
"                    be used when running valgrind (as an example).\n"

   ;

   unit_test_t testbattery;                           /* uses default values  */
   cbool_t ok = xpc_parse_errlevel(argc, argv);       /* cool feature         */
   ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPC_TEST_NAME,
      "Synch Test 0.1",             /* XPC_VERSION_STRING(XPC_TEST_VERSION),  */
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

            ok = unit_test_load(&testbattery, syncher_thread_test_01_01);
            if (ok)
            {
               (void) unit_test_load(&testbattery, syncher_thread_test_02_01);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, syncher_thread_test_03_01);
            }
            if (ok)
            {
               (void) unit_test_load(&testbattery, syncher_thread_test_05_01);
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
 * syncher_thread_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
