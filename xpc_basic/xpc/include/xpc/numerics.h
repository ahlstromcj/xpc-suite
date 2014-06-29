#ifndef XPC_NUMERICS_H
#define XPC_NUMERICS_H

/*******************************************************************************
 * numerics.h
 *------------------------------------------------------------------------*//**
 *
 * \file          numerics.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2005-06-26 to 2011-09-24
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides some miscellaneous portable functions.
 *
 *    This file should include ../config.h for configurations built using
 *    automake (for Linux/UNIX) or ../win32/config.h for configuration built
 *    for Windows.
 *
 *    In other words, "xpc-config.h" below should be the one needed for
 *    your project and for the current operating system.
 *
 *    For example, for Windows compilation, the proper config.h is located
 *    in the win32 sub-directory for the project.  Like the normal autoconf
 *    config.h, it contains a number of "HAVE" macros.  However, these
 *    macros have to be edited by hand, as there seems to be no easy
 *    automake facility for Windows.
 *
 *    This macro needed to define NAN in C code (as opposed to C++ code).
 *    However, defining it here doesn't work.  So we're going to try
 *    -std=c99 or -std=gnu99.
 *
\verbatim
            #define __USE_ISOC99   1
\endverbatim
 *
 *    This module also provides more fool-proof usage of floating-point
 *    values.  It supports the concepts of huge values, not-a-number,
 *    infinity, and some others.
 *
 *    See numerics.c for full details.
 *
 * \todo
 *    -# At present, the xpc library differentiates only POSIX and
 *       non-POSIX, and it assumes that non-POSIX means "Win32".  As we gain
 *       experience with more operating systems, this differentiation will
 *       become more refined.
 *    -# if !defined __BORLANDC__
 *       include (cerrno)                   // errno
 *       endif
 *    -# We find that the C++ <b> string </b> header file must precede
 *       numerics.h (at least when USE_GETTEXT is not defined), otherwise
 *       we get an error in /usr/include/libintl.h.  We now co-erce this
 *       order below when __cplusplus is defined.
 *    -# We find that leakdebug.h must follow numerics.h, because it
 *       needs the POSIX macro to be defined.  This macro is defined if
 *       xpc-config.h defines XPC_HAVE_UNISTD.  Again, should we co-erce
 *       it here?
 *    -# This module is a bit of a grab-bag of functionality, unfortunately.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* support for special XPC features    */

#if XPC_HAVE_LIMITS_H
#include <limits.h>                    /* ULONG_MAX, etc.                     */
#endif

#if XPC_HAVE_MATH_H
#include <math.h>                      /* HUGE, HUGE_VAL, NAN, etc.           */

#ifndef INFINITY
#ifdef WIN32
#define INFINITY  HUGE_VAL
#endif
#endif

#endif

#ifdef __MINGW32__
#define HUGE   HUGE_VAL
#endif

XPC_REVISION_DECL(numerics)            /* void show_xpc_numerics_info()       */

/******************************************************************************
 * xpc_seedings_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides codes for a couple of seeding methods.
 *
 *    These codes determine how seeding will be performed.
 *
 * \tip
 *    "Random numbers are too important to be left to chance."
 *
 * \var XPC_SEEDING_NORMAL
 *    Initialize once, use random() to get the seed in the normal way.  If
 *    the list hasn't been initialized, use randomize() to get a truly
 *    random seed value, then seed ran2list().
 *
 * \var XPC_SEEDING_AGAIN
 *    Use this or greater values to seed again.
 *
 * \var XPC_SEEDING_DONE
 *    Don't initialize the generator at all.  Don't re-seed; we don't want
 *    to start a new random sequence
 *
 *//*-------------------------------------------------------------------------*/

typedef enum
{
   XPC_SEEDING_NORMAL   = -1,
   XPC_SEEDING_AGAIN    = -2,
   XPC_SEEDING_DONE     = -3

} xpc_seedings_t;

/******************************************************************************
 * XPC_DEFINE_RANDOMIZE
 *------------------------------------------------------------------------*//**
 *
 *    Borland provides the randomize() seeding function; Microsoft does not.
 *    Neither does GNU gcc.  This macro enables the definition of this
 *    function for these environments, and for Doxygen documentation.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define XPC_DEFINE_RANDOMIZE
#endif

#ifdef _MSC_VER
#define XPC_DEFINE_RANDOMIZE
#endif

#ifdef __GNUC__
#define XPC_DEFINE_RANDOMIZE
#endif

/******************************************************************************
 * Portable C integer functions
 *-----------------------------------------------------------------------------
 *
 *    Global and portable functions for some common, basic, numeric tasks.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern unsigned long next_power_of_2 (unsigned long value);
extern long long_round (double x);
extern int int_round (double x);
extern unsigned long dtoul (double x);
extern double ultod (unsigned long x);
extern void strreverse (char * source);
extern char * itoa10
(
   int number,
   char * destination,
   size_t n
);
extern unsigned atox (const char * s);

#if defined __GNUC__
extern char * itoa (int value, char * outbuffer, int radix);
#else
extern char * alt_itoa (int value, char * outbuffer, int radix);
#endif

extern char * xtoa (unsigned value, char * destination, size_t n);
extern int combinations (int n, int r);
extern int pairing_to_index (int n, int j, int k);
extern void index_to_pairing (int comboindex, int n, int * j, int * k);
extern int random_number (int rangemax, cbool_t seedfirst, cbool_t seedsame);

#ifdef XPC_DEFINE_RANDOMIZE
void randomize (void);
#endif                              /* XPC_DEFINE_RANDOMIZE                   */

extern void seedrandom (void);
extern int * random_integers (int siz);
extern double ran2 (long initvalue);
extern cbool_t ran2list
(
   int * userbuffer,
   int siz,
   int seedvalue,
   cbool_t inplaceshuffle
);
extern cbool_t ran2shuffle
(
   int * userbuffer,
   int siz,
   int seedvalue
);

/******************************************************************************
 * Floating-Point Section
 *----------------------------------------------------------------------------*/

extern cbool_t cumulative_average
(
   double * cumavg,
   double xn,
   unsigned n
);
extern double xpc_float_nan (void);
extern double xpc_float_infinite (void);
extern cbool_t xpc_is_zero (double x);
extern cbool_t xpc_is_infinite (double x);
extern cbool_t ieee_is_infinite (float x);
extern cbool_t xpc_is_minus_infinite (double x);
extern cbool_t xpc_is_huge (double x);
extern cbool_t xpc_is_minus_huge (double x);
extern cbool_t xpc_is_normal (double x);
extern cbool_t xpc_is_nan (double x);
extern cbool_t ieee_is_nan (float x);
extern int ieee_sign (float x);
extern cbool_t ieee_almost_equal (float x, float y, int maxdiff);
extern float ieee_minus_zero ();
extern float ieee_nan ();
extern float ieee_nan_2 ();
extern float ieee_nan_3 ();
extern float ieee_infinite ();
extern cbool_t xpc_is_neq (double a, double b);
extern cbool_t xpc_is_neq_epsilon (double a, double b, double e);
extern cbool_t xpc_is_eq (double a, double b);
extern cbool_t xpc_is_eq_epsilon (double a, double b, double e);
extern cbool_t xpc_is_lt_or_eq (double a, double b);
extern cbool_t xpc_is_lt_or_eq_epsilon (double a, double b, double e);
extern cbool_t xpc_is_gt_or_eq (double a, double b);
extern cbool_t xpc_is_gt_or_eq_epsilon (double a, double b, double e);

#ifdef __GNUC__

EXTERN_C_DEC

extern char * itoa
(
   int value,
   char * buffer,
   int radix
);

EXTERN_C_END

#else

EXTERN_C_DEC

extern char * alt_itoa
(
   int value,
   char * buffer,
   int radix
);

EXTERN_C_END

#endif                              /* __GNUC__                               */

/******************************************************************************
 * NAN_INF_EPSILON
 *------------------------------------------------------------------------*//**
 *
 *    Provides a default "epsilon" value.  We need to provide a source that
 *    shows this is a good value to use!
 *
 *    And, in fact, use of this variable is too simplistic.
 *
 *//*-------------------------------------------------------------------------*/

#define NAN_INF_EPSILON   0.000001

/******************************************************************************
 * Portable C floating-point functions
 *-----------------------------------------------------------------------------
 *
 *    Global and portable functions for some common, basic, numeric tasks.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern double xpc_nan (void);
extern double xpc_infinite (void);
extern cbool_t xpc_is_zero (double x);
extern cbool_t xpc_is_infinite (double x);
extern cbool_t ieee_is_infinite (float x);
extern cbool_t xpc_is_minus_infinite (double x);
extern cbool_t xpc_is_huge (double x);
extern cbool_t xpc_is_minus_huge (double x);
extern cbool_t xpc_is_normal (double x);
extern cbool_t xpc_is_nan (double x);
extern cbool_t ieee_is_nan (float x);
extern int ieee_sign (float x);
extern cbool_t ieee_almost_equal (float x, float y, int maxdiff);
extern cbool_t xpc_is_neq (double x, double y);
extern cbool_t xpc_is_eq (double x, double y);
extern cbool_t xpc_is_lt_or_eq (double x, double y);
extern cbool_t xpc_is_gt_or_eq (double x, double y);
extern cbool_t xpc_is_neq_epsilon (double x, double y, double epsilon);
extern cbool_t xpc_is_eq_epsilon (double x, double y, double epsilon);
extern cbool_t xpc_is_lt_or_eq_epsilon (double x, double y, double epsilon);
extern cbool_t xpc_is_gt_or_eq_epsilon (double x, double y, double epsilon);

EXTERN_C_END

#endif                              /* XPC_NUMERICS_H                         */

/******************************************************************************
 * numerics.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
