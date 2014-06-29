/******************************************************************************
 * numerics_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          numerics_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2008-06-25 to 2013-07-21
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a few unit tests of the XPC library
 *    numerics_support.c/h module.
 *
 *    The unit-test groups planned are:
 *
 *       -  Group 1. Basic unit-tests of the numerics-related functions.
 *          Also referred to as the "smoke tests".
 *       -  Group 2. Basic unit-tests of the numerics_support.c functions.
 *
 *    Some other odds-and-ends are included in this application.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/build_versions.h>        /* informative show-build functions    */
#include <xpc/cpu.h>                   /* macros for CPU support              */
#include <xpc/errorlogging.h>          /* macros and external functions       */
#include <xpc/os.h>                    /* macros for OS support               */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/numerics.h>              /* numeric (float/integer) functions   */
#include <xpc/unit_test.h>             /* unit_test_t structure               */

/******************************************************************************
 * numerics_test_01_01()
 *------------------------------------------------------------------------*//**
 *
 * \group
 *    1. Smoke tests
 *
 * \case
 *    1. Most functions
 *
 *    This first test case of the first test group merely provides some
 *    basic smoke-tests of the numerics.c functions.
 *
 *    Simply examine the console output and see if it matches your
 *    expectations for the current system.
 *
 *    One reason for this test group is that we have a lot of combinations
 *    to test, which will take a long time.  We at least want a quick check
 *    that the code compiles and provides decent results.
 *
 *    More detailed tests will be added in separate groups and cases as time
 *    allows.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 * \test
 *    Rudimentary ("smoke") tests of the following functions are done:
 *    -  next_power_of_2()
 *    -  long_round()
 *    -  int_round()
 *    -  dtoul()
 *    -  ultod()
 *    -  strreverse()
 *    -  itoa10()
 *    -  atox()
 *    -  itoa()
 *    -  xtoa()
 *    -  combinations()
 *    -  pairing_to_index()
 *    -  index_to_pairing()
 *    -  random_number()
 *    -  randomize()  [cannot test]
 *    -  seedrandom() [cannot test]
 *    -  random_integers()
 *    -  xxxx()
 *    -  xxxx()
 *    -  xxxx()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
numerics_test_01_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 1, _("Smoke tests"), _("Most functions")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "bogus()"))
      {
         // reserved for numerical neatness
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "next_power_of_2()"))
      {
         size_t powerof2 = next_power_of_2(0);
         ok = powerof2 == 1;
         if (ok)
         {
            /*
             * Note that 1 is a power of 2.  It is the 0th power of any
             * number.
             */

            powerof2 = next_power_of_2(1);
            ok = powerof2 == 1;
         }
         if (ok)
         {
            powerof2 = next_power_of_2(15);
            ok = powerof2 == 16;
         }
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "long_round()"))
      {
         long rounded = long_round(3.555);
         ok = rounded == 4;
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "int_round()"))
      {
         int rounded = int_round(3.555);
         ok = rounded == 4;
         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "dtoul()"))
      {
         unsigned long ul = dtoul(1e8);
         ok = ul == 100000000;
         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "ultod()"))
      {
         double dub = ultod(100000000);
         ok = dub == 1e8;
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "strrevers()"))
      {
         char value[32];
         strcpy(value, "123456");
         strreverse(value);
         ok = strcmp(value, "654321") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "itoa10()"))
      {
         char value[32];
         char * result = itoa10(123456, value, 32);
         ok = strcmp(result, "123456") == 0;
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "atox()"))
      {
         unsigned result = atox("0x100");
         ok = result == 256;
         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "itoa()"))
      {
         char outbuffer[32];
         char * result = itoa(32, outbuffer, 10);
         ok = strcmp(result, "32") == 0;
         if (ok)
         {
            result = itoa(32, outbuffer, 8);
            ok = strcmp(result, "40") == 0;
         }
         if (ok)
         {
            result = itoa(32, outbuffer, 16);
            ok = strcmp(result, "20") == 0;
         }
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "xtoa()"))
      {
         char outbuffer[32];
         char * result = xtoa(35, outbuffer, sizeof(outbuffer));
         ok = strcmp(result, "23") == 0;
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "combinations()"))
      {
         /*
          *                 n!
          *  C(n,r) = -----------
          *            r! (n-r)!
          */

         int combos = combinations(3, 2);
         int target = (1 * 2 * 3) / ((1 * 2) * (1 * (3 - 2)));
         ok = combos == target;
         if (unit_test_options_show_values(options))
            fprintf(stdout, "- C(3, 2):             %d\n", combos);

         if (ok)
         {
            combos = combinations(4, 2);
            target = (1 * 2 * 3 * 4) / ((1 * 2) * (1 * (4 - 2)));
            ok = combos == target;
            if (unit_test_options_show_values(options))
               fprintf(stdout, "- C(4, 2):             %d\n", combos);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "pairing_to_index()"))
      {
         /*
          * This is the inverse of subtest 14.
          */

         int index = pairing_to_index(4, 1, 2);
         ok = index == 3;
         if (unit_test_options_show_values(options))
            fprintf(stdout, "- pairing_to_index():  %d\n", index);

         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "index_to_pairing()"))
      {
         /*
          * This is the inverse of subtest 13.
          */

         int j = 0;
         int k = 0;
         index_to_pairing(3, 4, &j, &k);
         ok = (j == 1) && (k == 2);
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "random_number()"))
      {
         int r = random_number(10, true, 999);
         ok = (r >= 0) && (r < 10);
         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "random_integers()"))
      {
         int * rarray = random_integers(3);
         ok = not_nullptr(rarray);
         if (ok)
         {
            int i;
            for (i = 0; i < 3; i++)
            {
               ok = (rarray[i] >= 0) && (rarray[i] < 3);
               if (! ok)
                  break;
            }
            free(rarray);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "ran2()"))
      {
         double r = ran2(0);
         ok = (r >= 0.0);
         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "ran2list()"))
      {
         int * rarray = malloc(3 * sizeof(int));
         ok = not_nullptr(rarray);
         if (ok)
            ok = ran2list(rarray, 3, XPC_SEEDING_DONE, false);

         if (ok)
         {
            int i;
            for (i = 0; i < 3; i++)
            {
               ok = (rarray[i] >= 0) && (rarray[i] < 3);
               if (! ok)
                  break;
            }
            free(rarray);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "ran2shuffle()"))
      {
         int * rarray = malloc(3 * sizeof(int));
         ok = not_nullptr(rarray);
         if (ok)
            ok = ran2list(rarray, 3, XPC_SEEDING_DONE, false);

         if (ok)
            ok = ran2shuffle(rarray, 3, XPC_SEEDING_DONE);

         if (ok)
         {
            int i;
            for (i = 0; i < 3; i++)
            {
               ok = (rarray[i] >= 0) && (rarray[i] < 3);
               if (! ok)
                  break;
            }
            free(rarray);
         }
         unit_test_status_pass(&status, ok);
      }

      /* 20 */

      if (unit_test_status_next_subtest(&status, "cumulative_average()"))
      {
         /*
          * For this smoke test, we will pretend we've already got 10
          * data-points that average out to 100.  We add one more value,
          * 210, and should see that the new average for the 11 points is
          * 110.
          */

         double average = 100.0;
         ok = cumulative_average(&average, 210.0, 11);
         if (ok)
            ok = average = 110.0;

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * numerics_test_01_02()
 *------------------------------------------------------------------------*//**
 *
 * \group
 *    1. Smoke tests
 *
 * \case
 *    2. Simple floating-point functions.
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 * \test
 *    Rudimentary ("smoke") tests of the following functions are done:
 *    -  xpc_nan()
 *    -  xpc_infinite()
 *    -  xpc_is_zero()
 *    -  xpc_is_infinite()
 *    -  ieee_is_infinite()
 *    -  xpc_is_minus_infinite()
 *    -  xpc_is_huge()
 *    -  xpc_is_minus_huge()
 *    -  xpc_is_normal()
 *    -  xpc_is_nan()
 *    -  ieee_is_nan()
 *    -  ieee_sign()
 *    -  ieee_almost_equal()
 *    -  ieee_minus_zero()
 *    -  ieee_nan()
 *    -  ieee_nan_2()
 *    -  ieee_nan_3()
 *    -  ieee_infinite()
 *    -  xpc_is_neq()
 *    -  xpc_is_neq_epsilon()
 *    -  xpc_is_eq()
 *    -  xpc_is_eq_epsilon()
 *    -  xpc_is_lt_or_eq()
 *    -  xpc_is_lt_or_eq_epsilon()
 *    -  xpc_is_gt_or_eq()
 *    -  xpc_is_gt_or_eq_epsilon()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
numerics_test_01_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 1, 2, _("Smoke tests"), _("Simple float functions")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "xpc_nan()"))
      {
         double nan = xpc_nan();

         /*
          * We cannot test directly against NAN because it is a union of
          * values.
          *
          * #ifdef NAN
          *    ok = nan == NAN;
          * #endif
          *
          * The isnan() function is provided by GNU.  In Windows, we provide
          * a function of the same name and purpose.
          */

         ok = isnan(nan);
         unit_test_status_pass(&status, ok);
      }

      /*  2 */

      if (unit_test_status_next_subtest(&status, "xpc_infinite()"))
      {
         double infinite = xpc_infinite();

         /*
          * We cannot test directly against INFINITY because it is a union of
          * values.
          *
          * #ifdef INFINITY
          *    ok = infinite == INFINITY;
          * #endif
          *
          * The isinf() function is provided by GNU.  In Windows, we provide
          * a function of the same name and purpose.
          */

         ok = isinf(infinite);
         unit_test_status_pass(&status, ok);
      }

      /*  3 */

      if (unit_test_status_next_subtest(&status, "xpc_is_zero()"))
      {
         ok = xpc_is_zero(0.0f);
         unit_test_status_pass(&status, ok);
      }

      /*  4 */

      if (unit_test_status_next_subtest(&status, "xpc_is_infinite()"))
      {
         ok = xpc_is_infinite(xpc_infinite());
         if (ok)
            ok = xpc_is_infinite(xpc_infinite() * -1.0);

         unit_test_status_pass(&status, ok);
      }

      /*  5 */

      if (unit_test_status_next_subtest(&status, "ieee_is_infinite()"))
      {
         ok = ieee_is_infinite(xpc_infinite());
         if (ok)
            ok = ieee_is_infinite(xpc_infinite() * -1.0);

         unit_test_status_pass(&status, ok);
      }

      /*  6 */

      if (unit_test_status_next_subtest(&status, "xpc_is_minus_infinite()"))
      {
         ok = xpc_is_minus_infinite(xpc_infinite() * -1.0);
         unit_test_status_pass(&status, ok);
      }

      /*  7 */

      if (unit_test_status_next_subtest(&status, "xpc_is_huge()"))
      {
         ok = xpc_is_huge(xpc_infinite());
         unit_test_status_pass(&status, ok);
      }

      /*  8 */

      if (unit_test_status_next_subtest(&status, "xpc_is_minus_huge()"))
      {
         ok = xpc_is_minus_huge(xpc_infinite() * -1.0);
         unit_test_status_pass(&status, ok);
      }

      /*  9 */

      if (unit_test_status_next_subtest(&status, "xpc_is_normal()"))
      {
         ok = xpc_is_normal(-1.0);
         if (ok)
            ok = xpc_is_normal(1.0);

         unit_test_status_pass(&status, ok);
      }

      /* 10 */

      if (unit_test_status_next_subtest(&status, "xpc_is_nan()"))
      {
         ok = xpc_is_nan(xpc_nan());
         unit_test_status_pass(&status, ok);
      }

      /* 11 */

      if (unit_test_status_next_subtest(&status, "ieee_is_nan()"))
      {
         ok = ieee_is_nan(xpc_nan());
         unit_test_status_pass(&status, ok);
      }

      /* 12 */

      if (unit_test_status_next_subtest(&status, "ieee_sign()"))
      {
         int sign = ieee_sign(1.0);
         ok = sign == 0;
         if (ok)
         {
            sign = ieee_sign(0.0);
            ok = sign == 0;
         }
         if (ok)
         {
            sign = ieee_sign(-1.0);
            ok = sign < 0;
         }
         if (ok)
         {
            sign = ieee_sign(-0.0);
            ok = sign < 0;
         }
         unit_test_status_pass(&status, ok);
      }

      /* 13 */

      if (unit_test_status_next_subtest(&status, "ieee_almost_equal()"))
      {
         ok = ieee_almost_equal(0.0, 0.0, 1);
         unit_test_status_pass(&status, ok);
      }

      /* 14 */

      if (unit_test_status_next_subtest(&status, "ieee_minus_zero()"))
      {
         ok = ieee_minus_zero(-0.0);
         unit_test_status_pass(&status, ok);
      }

      /* 15 */

      if (unit_test_status_next_subtest(&status, "ieee_nan()"))
      {
         ok = xpc_is_nan(ieee_nan());
         if (ok)
            ok = ieee_is_nan(ieee_nan());

         unit_test_status_pass(&status, ok);
      }

      /* 16 */

      if (unit_test_status_next_subtest(&status, "ieee_nan_2()"))
      {
         ok = xpc_is_nan(ieee_nan_2());
         if (ok)
            ok = ieee_is_nan(ieee_nan_2());

         unit_test_status_pass(&status, ok);
      }

      /* 17 */

      if (unit_test_status_next_subtest(&status, "ieee_nan_3()"))
      {
         ok = xpc_is_nan(ieee_nan_3());
         if (ok)
            ok = ieee_is_nan(ieee_nan_3());

         unit_test_status_pass(&status, ok);
      }

      /* 18 */

      if (unit_test_status_next_subtest(&status, "ieee_infinite()"))
      {
         double infinite = ieee_infinite();
         ok = isinf(infinite);
         unit_test_status_pass(&status, ok);
      }

      /* 19 */

      if (unit_test_status_next_subtest(&status, "xpc_is_neq()"))
      {
         ok = xpc_is_neq(0.0000, 0.0001);
         unit_test_status_pass(&status, ok);
      }

      /* 20 */

      if (unit_test_status_next_subtest(&status, "xpc_is_neq_epsilon()"))
      {
         ok = xpc_is_neq_epsilon(0.0000, 0.0001, 0.000099);
         unit_test_status_pass(&status, ok);
      }

      /* 21 */

      if (unit_test_status_next_subtest(&status, "xpc_is_eq()"))
      {
         ok = xpc_is_eq(0.0000, 0.0000001);
         unit_test_status_pass(&status, ok);
      }

      /* 22 */

      if (unit_test_status_next_subtest(&status, "xpc_is_eq_epsilon()"))
      {
         ok = xpc_is_eq_epsilon(0.0000, 0.0001, 0.00099);
         unit_test_status_pass(&status, ok);
      }

      /* 23 */

      if (unit_test_status_next_subtest(&status, "xpc_is_lt_or_eq()"))
      {
         ok = xpc_is_lt_or_eq(0.0000, 0.0000001);
         if (ok)
            ok = xpc_is_lt_or_eq(0.0000, 0.0000);

         unit_test_status_pass(&status, ok);
      }

      /* 24 */

      if (unit_test_status_next_subtest(&status, "xpc_is_lt_or_eq_epsilon()"))
      {
         ok = xpc_is_lt_or_eq_epsilon(0.0000, 0.0000001, 0.00001);
         if (ok)
            ok = xpc_is_lt_or_eq_epsilon(0.0000, 0.0000, 0.00001);

         unit_test_status_pass(&status, ok);
      }

      /* 25 */

      if (unit_test_status_next_subtest(&status, "xpc_is_gt_or_eq()"))
      {
         ok = xpc_is_gt_or_eq(0.0000001, 0.0000);
         if (ok)
            ok = xpc_is_gt_or_eq(0.0000, 0.0000);

         unit_test_status_pass(&status, ok);
      }

      /* 26 */

      if (unit_test_status_next_subtest(&status, "xpc_is_gt_or_eq_epsilon()"))
      {
         ok = xpc_is_gt_or_eq_epsilon(0.0000001, 0.0000, 0.00001);
         if (ok)
            ok = xpc_is_gt_or_eq_epsilon(0.0000, 0.0000, 0.00001);

         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * long_test_pair_t
 *------------------------------------------------------------------------*//**
 *
 *    A simple structure to use in testing long-integer parameters and
 *    long-integer results.
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   unsigned long  ltp_parameter; /**< Value of the long-integer parameter.    */
   unsigned long  ltp_result;    /**< The expected value of the result.       */

} long_test_pair_t;

/******************************************************************************
 * gs_power_of_2_results[]
 *------------------------------------------------------------------------*//**
 *
 *    The test-set and expected results for unit-test 02.01.
 *
 *    The list ends with a parameter value of ULONG_MAX.
 *
 *//*-------------------------------------------------------------------------*/

#define ULPOW_MAX    ((long) INT_MAX + 1)

long_test_pair_t gs_power_of_2_results [] =
{
   {          0L,        1L  },
   {          1L,        1L  },
   {          2L,        2L  },
   {          3L,        4L  },
   {          4L,        4L  },
   {          5L,        8L  },
   {          6L,        8L  },
   {          7L,        8L  },
   {          8L,        8L  },
   {          9L,       16L  },
   {         10L,       16L  },
   {         11L,       16L  },
   {         12L,       16L  },
   {         13L,       16L  },
   {         14L,       16L  },
   {         15L,       16L  },
   {         16L,       16L  },
   {        127L,      128L  },
   {        128L,      128L  },
   {        129L,      256L  },
   {        255L,      256L  },
   {        256L,      256L  },
   {        257L,      512L  },
   {        511L,      512L  },
   {        512L,      512L  },
   {        513L,     1024L  },
   {       1023L,     1024L  },
   {       1024L,     1024L  },
   {       1025L,     2048L  },
   {       2047L,     2048L  },
   {       2048L,     2048L  },
   {       2049L,     4096L  },
   {       4095L,     4096L  },
   {       4096L,     4096L  },
   {       4097L,     8192L  },
   {       8191L,     8192L  },
   {       8192L,     8192L  },
   {       8193L,    16384L  },
   {      16383L,    16384L  },
   {      16384L,    16384L  },
   {      16385L,    32768L  },
   {      32767L,    32768L  },
   {      32768L,    32768L  },
   {      32769L,    65536L  },
   {      65535L,    65536L  },
   {      65536L,    65536L  },
   {      65537L,   131072L  },
#if XPC_64_BITS
   {    INT_MAX,  ULPOW_MAX  },
   {   LONG_MAX,         0   },
#elif XPC_32_BITS
   {    INT_MAX,         0   },
#endif

   /*
    * Terminator
    */

   {  ULONG_MAX,         0   }
};

/******************************************************************************
 * numerics_test_02_01()
 *------------------------------------------------------------------------*//**
 *
 *    Provides more involved tests for the functions smoke-tested in
 *    numerics_test_01_01().
 *
 * \group
 *    2. Numerics
 *
 * \case
 *    1. next_power_of_2()
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 * \test
 *    -  next_power_of_2()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
numerics_test_02_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 1, _("Numerics"), _("next_power_of_2()")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Value list"))
      {
         int i = 0;
         for (i = 0; ; i++)
         {
            unsigned long value = gs_power_of_2_results[i].ltp_parameter;
            if (value != ULONG_MAX)
            {
               unsigned long result = gs_power_of_2_results[i].ltp_result;
               unsigned long power = next_power_of_2(value);
               ok = result == power;
               if (ok)
               {
                  if (unit_test_options_show_values(options))
                     fprintf(stdout, "- %lu --> %lu\n", value, power);
               }
               else
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout,
                        "? Unexpect result %lu for value %lu\n",
                        power, value
                     );
                  }
                  break;
               }
            }
            else
               break;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * dl_test_pair_t
 *------------------------------------------------------------------------*//**
 *
 *    A simple structure to use in testing double parameters and integer and
 *    long-integer results.
 *
 *//*-------------------------------------------------------------------------*/

typedef struct
{
   double dtp_parameter;      /**< Value of the double-float parameter.       */
   unsigned long dtp_result;  /**< The expected value of the result.          */

} dl_test_pair_t;

/******************************************************************************
 * gs_rounding_results[]
 *------------------------------------------------------------------------*//**
 *
 *    The test-set and expected results for unit-test 02.01.
 *
 *    The list ends with a parameter value of 0.0 (double).
 *
 *//*-------------------------------------------------------------------------*/

dl_test_pair_t gs_rounding_results [] =
{
   {     -5.5,       -6L   },
   {     -5.0,       -5L   },
   {     -4.5,       -5L   },
   {     -0.5,       -1L   },
   {     -0.4,        0L   },
   {      0.1,        0L   },
   {      0.49,       0L   },
   {      0.499,      0L   },
   {      0.4999,     0L   },
   {      0.49999,    0L   },
   {      0.499999,   0L   },
   {      0.4999999,  0L   },
   {      0.49999999, 0L   },
   {      0.5,        1L   },
   {      0.99999999, 1L   },
   {      1.0,        1L   },
   {      1.49999999, 1L   },
   {      1.5,        2L   },

   /*
    * Terminator
    */

   {      0.0,        0L   }
};

/******************************************************************************
 * numerics_test_02_02()
 *------------------------------------------------------------------------*//**
 *
 *    Provides more involved tests for the functions smoke-tested in
 *    numerics_test_01_01().
 *
 * \group
 *    2. Numerics
 *
 * \case
 *    2. Rounding functions
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 * \test
 *    -  long_round()
 *    -  int_round()
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
numerics_test_02_02 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 2, 2, _("Numerics"), _("Rounding functions")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Value list"))
      {
         int i = 0;
         for (i = 0; ; i++)
         {
            double value = gs_rounding_results[i].dtp_parameter;
            if (value != 0.0)
            {
               unsigned long result = gs_rounding_results[i].dtp_result;
               unsigned long rounded = long_round(value);
               ok = result == rounded;
               if (ok)
               {
                  if (unit_test_options_show_values(options))
                     fprintf(stdout, "- %g --> %lu\n", value, rounded);
               }
               else
               {
                  if (! xpccut_is_silent())
                  {
                     fprintf
                     (
                        stdout,
                        "? Unexpect result %lu for value %g\n",
                        rounded, value
                     );
                  }
                  break;
               }
            }
            else
               break;
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * numerics_test_03_01()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the cumulative_average() function against the brute force
 *    calculation.
 *
 * \group
 *    3. Calculations
 *
 * \case
 *    1. cumulative_average()
 *
 * \test
 *    -  cumulative_average()
 *
 * \param options
 *    Provides the options given to the application on the command-line.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static unit_test_status_t
numerics_test_03_01 (const unit_test_options_t * options)
{
   unit_test_status_t status;
   cbool_t ok = unit_test_status_initialize
   (
      &status, options, 3, 1, _("Calculations"), _("cumulative average")
   );
   if (ok)
   {
      /*  1 */

      if (unit_test_status_next_subtest(&status, "Smoke test"))
      {
         double average;
         ok = cumulative_average(&average, 1.0, 1);

         /*
          * I forgot to increment 1 the first time I ran this test!
          */

         if (ok)
            ok = cumulative_average(&average, 3.0, 2);

         if (ok)
         {
            ok = average == 2.0;
            if (unit_test_options_show_values(options))
               fprintf(stdout, "- Average:             %f\n", average);
         }
         unit_test_status_pass(&status, ok);
      }
   }
   return status;
}

/******************************************************************************
 * ieee_print_number()
 *------------------------------------------------------------------------*//**
 *
 *    Function to print a number and its representation, in hex and decimal
 *
 * @gnu
 *    The gcc compiler yields this warning:
 *
 *       warning: dereferencing type-punned pointer will break strict
 *       -aliasing rules [-Wstrict-aliasing]
 *
 * \param f
 *    The number to print.
 *
 * \param offset
 *    To tell you the truth, I can't remember the purpose of this parameter.
 *
 *//*-------------------------------------------------------------------------*/

void ieee_print_number (float f, int offset)
{
    (*((int*)&f)) += offset;
    printf("%+1.8g,0x%08lX,%d\n", f, (unsigned long)(*(int*)&f), *(int*)&f);
}

/******************************************************************************
 * ieee_print_numbers()
 *------------------------------------------------------------------------*//**
 *
 *    This function just prints some numbers, as floats, hex int, and
 *    decimal int for incorporation into the author's article on comparing
 *    floats as integers.
 *
 *    Typical print results, adjusted to have tabs instead of commas for
 *    easy pasting into a spreadsheet:
 *
\verbatim
 *       +1.99999976	0x3FFFFFFE	1073741822
 *       +1.99999988	0x3FFFFFFF	1073741823
 *       +2.00000000	0x40000000	1073741824
 *       +2.00000024	0x40000001	1073741825
 *       +2.00000048	0x40000002	1073741826
 *       +0.00000000	0x00000000	0
 *       +0.00000000	0x80000000	-2147483648
 *
 *       +4.2038954e-045	0x00000003	3
 *       +2.8025969e-045	0x00000002	2
 *       +1.4012985e-045	0x00000001	1
 *       +0	0x00000000	0
 *       -0	0x80000000	-2147483648
 *       -1.4012985e-045	0x80000001	-2147483647
 *       -2.8025969e-045	0x80000002	-2147483646
 *       -4.2038954e-045	0x80000003	-2147483645
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

void ieee_print_numbers ()
{
    float f = 2.0;
    for (int i = -2; i <= 2; ++i)
    {
        ieee_print_number(f, i);
    }
    ieee_print_number(0.0, 0);
    ieee_print_number(0.0, 0x80000000);

    for (int i = 3; i >= 0; --i)
        ieee_print_number( 0.0, i );

    for (int i = 0; i <= 3; ++i)
        ieee_print_number( 0.0, i + 0x80000000 );
}

/******************************************************************************
 * test_ieee_almost_equal()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a test of the ieee_almost_equal() function.
 *
 * \param x
 *    The floating-point value to be compared against.
 *
 * \param y
 *    The floating-point value to be compared.
 *
 * \param expected_result
 *    The expected boolean result of the comparison.
 *
 * \param maxdiff
 *    The maximum number of values by which \a x and \a y can differ.  This
 *    value determines differences based on converting the values to their
 *    equivalent integer format.
 *
 * \return
 *    Returns the value of the sign (0 or 1).  0 means the number is
 *    positive, we believe.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
test_ieee_almost_equal
(
   float x,
   float y,
   cbool_t expected_result,
   int maxdiff
)
{
   cbool_t result = ieee_almost_equal(x, y, maxdiff);
   if (result != expected_result)
   {
      if (! xpccut_is_silent())
      {
         fprintf
         (
            stdout, "Unexpected result final - %1.9f, %1.9f, %d, expected %s\n",
            x, y, maxdiff, expected_result ? "true" : "false"
         );
      }
   }
   return result;
}

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_APP_NAME          "XPC library numerics test"

/******************************************************************************
 * Macro
 *------------------------------------------------------------------------*//**
 *
 *    The executable name of the application.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_TEST_NAME         "numerics_ut"

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
 *    The base directory of the application.
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
 *    This is the main routine for the numerics_ut application.
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
   unit_test_t testbattery;                           /* uses default values  */
   cbool_t ok = xpc_parse_errlevel(argc, argv);       /* cool feature         */
   ok = unit_test_initialize
   (
      &testbattery, argc, argv,
      XPC_TEST_NAME,                                  /* nullptr for testin   */
      "Numerics Test 0.1",                            /* nullptr for testin   */
      nullptr                                         /* no added help        */
   );
   if (ok)
   {
      /*
       * int argcount = argc;
       * char ** arglist = argv;
       */

      char gsBasename[80];
      char gsAuthor[80];
      cbool_t load_the_tests = true;
      strcpy(gsBasename, DEFAULT_BASE);
      strcpy(gsAuthor, DEFAULT_AUTHOR);

      /*
       * if (argcount > 1)
       * {
       *    int currentarg = 1;
       *    while (currentarg < argcount)
       *    {
       *       const char * arg = arglist[currentarg];
       *       if (strcmp(arg, "--leak-check") == 0)
       *       {
       *          g_do_leak_check = true;
       *          xpccut_infoprint(_("memory leakage enabled"));
       *       }
       *       currentarg++;
       *    }
       * }
       */

      if (ok)                           /* \note fails if --help specified */
      {
         if (load_the_tests)
         {
            (void) unit_test_load(&testbattery, numerics_test_01_01);
            ok = unit_test_load(&testbattery, numerics_test_01_02);
            if (ok)
            {
               (void) unit_test_load(&testbattery, numerics_test_02_01);
               ok = unit_test_load(&testbattery, numerics_test_02_02);
            }
            if (ok)
               ok = unit_test_load(&testbattery, numerics_test_03_01);
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
 * numerics_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
