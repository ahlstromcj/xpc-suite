/******************************************************************************
 * numerics.c
 *------------------------------------------------------------------------*//**
 *
 * \file          numerics.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2005-06-26
 * \updates       2013-03-03
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides are number of global items to handle number-related work.
 *
 *    This module provides some miscellaneous portable functions.  These
 *    functions are divided into two sections:
 *
 *       -  Integer functions
 *       -  Floating-point functions
 *
 *    The integer functions handle conversions between characters and
 *    integers, generating combinations, random numbers, indices, number
 *    pairs, and other interesting stuff.
 *
 *    The \a nan_inf module provides some basic functions to make the
 *    usage of floating-point values more fool-proof.
 *
 *    This module supports the concepts of huge values, not-a-number,
 *    infinity, and some others.  It also supports using the same concepts
 *    in Win32 code, in a portable manner.
 *
 *    For interesting discussions of floating-point numbers, see the
 *    following references:
 *
 *       -  http://docs.sun.com/source/806-3568/ncg_goldberg.html
 *       -  http://www.boost.org/doc/libs/1_34_0/libs/test/doc/components/
 *             test_tools/floating_point_comparison.html
 *       -  http://www.cygnus-software.com/papers/comparingfloats/
 *             comparingfloats.htm
 *
 *    For a 4-byte float, being off by only the least-significant bit for a
 *    value of 10000 yields 10000.000977, which is a very tiny percentage of
 *    the value.  An error of 0.00001 is appropriate for numbers around one,
 *    too big for numbers around 0.00001, and too small for numbers of large
 *    magnitude.
 *
 * \todo
 *    Can we add the compiler option -fno-strict-aliasing to the compilation
 *    of just the numerics module?  -fstrict-aliasing and -Wstrict-aliasing
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/numerics.h>              /* functions, macros, and headers      */
#include <xpc/errorlogging.h>          /* included only for xpc_errprint()    */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */

#if XPC_HAVE_TIME_H
#include <time.h>                      /* time() function                     */
#endif

#if XPC_HAVE_ALLOCA_H
#include <alloca.h>                    /* POSIX/GNU version of alloca()       */
#else
#include <malloc.h>                    /* Win32 version of alloca()           */
#endif

XPC_REVISION(numerics)

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    If Doxygen is running, define WIN32 and other values, to read in the
 *    documentation for the Windows-specific code.
 *
 *    Same for __GNUCC__.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define WIN32
#define _MSC_VER
#define __GNUC__
#endif

#ifdef _MSC_VER                        /* MS WIN32 floating point stuff       */
#include <float.h>

/******************************************************************************
 * __nan_bytes
 *------------------------------------------------------------------------*//**
 *
 * \win32
 *    Defines the bytes in the NAN constant for Windows, in Windows
 *    little-endian format.
 *
 * \gnu
 * \linux
 *    In GNU/Linux, the NAN value seems to be a union, where the first
 *    member is a pair of integers with the values 0 and 2146959360, and the
 *    second member is a double value with bits set to 0x8000000000000.
 *
 *//*-------------------------------------------------------------------------*/

#define __nan_bytes  0, 0, 0xc0, 0x7f  /* Windows little-endian format        */

/******************************************************************************
 * __nan_union_t
 *------------------------------------------------------------------------*//**
 *
 * \typedef __nan_union_t
 *
 *    NaN and INF values for Windows are provided by a union and a macro,
 *    respectively.
 *
 *    Windows doesn't really provide a good way to obtain the actual value of
 *    the IEEE NaN value, though it does provide HUGE_VAL for use as an
 *    infinite number.  See the _nan_value union and the NAN macros defined
 *    below.
 *
 *//*-------------------------------------------------------------------------*/

typedef union
{
   unsigned char __c[4];
   float __d;

} __nan_union_t;

/******************************************************************************
 * __nan_value
 *------------------------------------------------------------------------*//**
 *
 *    This value is used as a way to provide a NaN value for usage with
 *    Windows compilation.
 *
 *//*-------------------------------------------------------------------------*/

static __nan_union_t  __nan_value = { __nan_bytes };

/******************************************************************************
 * NAN macro
 *------------------------------------------------------------------------*//**
 *
 *    NaN for Windows is defined as the float member of the __nan_union_t
 *    union called __nan_value.
 *
 *    The NaN value determined here is the same as that defined in the Linux
 *    header file /usr/include/bits/nan.h.  It turns out to be the same as
 *    Microsoft's "quiet NaN" value.  It appears in printf() output as
 *    "#QNAN".
 *
 *//*-------------------------------------------------------------------------*/

#define NAN      (__nan_value.__d)

/******************************************************************************
 * isnan macro
 *------------------------------------------------------------------------*//**
 *
 *    Windows provides HUGE_VAL for use as an infinite number.
 *
 *    Infinity appears in Microsoft's printf() as "#INF".
 *
 *    Microsoft provides an _isnan() function, so the 'isnan' macro is
 *    provided to fake the appearance of the BSD 4.3 isnan() function.
 *
 *//*-------------------------------------------------------------------------*/

#define isnan    _isnan

/******************************************************************************
 * isinf()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a value against "infinity".
 *
 *    This version is provided for Windows code (actually Visual C code) to
 *    use.  It is a Win32 implementation of the BSD 4.3 function of the same
 *    name.
 *
 *    This function is used privately.
 *
 * \todo
 *    We need to define NAN appropriately in Cygwin builds.  Right now NAN
 *    is set to zero (0).
 *
 * \return
 *    This function returns 1 if the parameter is equal to HUGE_VAL, -1 if
 *    equal to -HUGE_VAL, and 0 otherwise.  Hence, if non-zero, the number
 *    is infinite.
 *
 *//*-------------------------------------------------------------------------*/

static int
isinf
(
   double x    /**< The floating-point value to be tested against huge-value. */
)
{
   int result = 0;
   if (x == HUGE_VAL)
      result = 1;
   else if (x == (-HUGE_VAL))
      result = -1;

   return result;
}

#endif   /* _MSC_VER */                      /* end of MS WIN32 stuff         */

/******************************************************************************
 * Integer Section
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * next_power_of_2()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the next highest power of 2 equal to or above a given number.
 *
 *    This function takes the given value and increases it (if necessary)
 *    to the next power of 2.  If the value is 0, the result is 1.
 *
\verbatim
          0 --> 1
          1 --> 1 (2 to the 0th power)
          2 --> 2
          3 --> 4
          4 --> 4
          5 --> 8
            ...
\endverbatim
 *
 * \param value
 *    Provides the integer value to raise to the next power of 2.
 *
 * \return
 *    The next highest power of 2 is returned.  If the number is already a
 *    power of 2, then the number itself is returned.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *    -	numeric_test_02_01()
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
next_power_of_2 (unsigned long value)
{
   unsigned long result = value;
   int count = 0;                         /* a bit-shift counter              */
   while (result != 0)                    /* while we have bits               */
   {
      result >>= 1;                       /* drop the current LSB             */
      count++;                            /* count the shifts                 */
   }
   if (count == 0)                        /* no bit-shift was needed          */
      result = 1;                         /* just return this value           */
   else if (count == 1)                   /* don't bother shifting by 0 bits  */
      result = 1;                         /* just return this value           */
   else                                   /* value was 2 or greater           */
   {
      result = 1;                         /* make a bit                       */
      result <<= count - 1;               /* undo the shifting                */
      if (result != value)                /* value not already a power of 2   */
         result <<= 1;                    /* shift one more time              */
   }
   return result;
}

/******************************************************************************
 * long_round()
 *------------------------------------------------------------------------*//**
 *
 *    Rounds a real number (double), and returns the result as a long
 *    integer.
 *
 *    The round operation is achieve by increasing the value by 0.5 and
 *    truncating it.
 *
 *    In environments that provide lround() [see the declaration commented
 *    on below], use long_round() for portability.
 *
 * \note
 *    Compare the rounding functions in this library to the normal math
 *    functions for rounding in various ways.  These functions come from the
 *    GNU library, glibc.  We don't know which ones are implemented by
 *    Microsoft compilers, so we make sure to provide our own versions of some
 *    of these functions.
 *
 *       -  float roundf (float x)
 *       -  double round (double x)
 *       -  long double roundl (long double x)
 *       -  double ceil (double x)
 *       -  float ceilf (float x)
 *       -  long double ceill (long double x)
 *       -  double floor (double x)
 *       -  float floorf (float x)
 *       -  long double floorl (long double x)
 *       -  long int lrint (double x)
 *       -  long int lrintf (float x)
 *       -  long int lrintl (long double x)
 *       -  long long int llrint (double x)
 *       -  long long int llrintf (float x)
 *       -  long long int llrintl (long double x)
 *       -  long int lround (double x)
 *       -  long int lroundf (float x)
 *       -  long int lroundl (long double x)
 *       -  long long int llround (double x)
 *       -  long long int llroundf (float x)
 *       -  long long int llroundl (long double x)
 *       -  double nearbyint (double x)
 *       -  float nearbyintf (float x)
 *       -  long double nearbyintl (long double x)
 *       -  double rint (double x)
 *       -  float rintf (float x)
 *       -  long double rintl (long double x)
 *       -  double trunc (double x)
 *       -  float truncf (float x)
 *       -  long double truncl (long double x)
 *
 * \warning
 *    There is no checking that the floating-point number fit in the range
 *    of the return-value type.  Apply this function only to values that
 *    are within the range of a long integer.
 *
 * \param x
 *    The real number to round up to the next long value.
 *
 * \return
 *    Returns a long integer version of the rounded number.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *    -	numeric_test_02_02()
 *
 *//*-------------------------------------------------------------------------*/

long
long_round (double x)
{
   if (x > 0.0)
      x += 0.5;
   else if (x < 0.0)
      x -= 0.5;

   return (long) x;
}

/******************************************************************************
 * int_round()
 *------------------------------------------------------------------------*//**
 *
 *    Rounds a real number (double), and returns the result as an integer.
 *
 *    The round operation is achieve by increasing the value by 0.5 and
 *    truncating it.
 *
 *    In environments that provide trunc() [see the declaration commented
 *    on below], use int_round() for portability and the convenience of not
 *    having to add in 0.5.
 *
 * \warning
 *    There is no checking that the floating-point number fit in the range
 *    of the return-value type.  Apply this function only to values that
 *    are within the range of an integer.
 *
 * \param x
 *    The real number to round up to the next int value.
 *
 * \return
 *    Returns an integer (int) version of the rounded number.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *    -	numeric_test_02_02()
 *
 *//*-------------------------------------------------------------------------*/

int
int_round (double x)
{
   if (x > 0.0)
      x += 0.5;
   else if (x < 0.0)
      x -= 0.5;

   return (int) x;
}

/******************************************************************************
 * UL_PACK_SIZE
 *------------------------------------------------------------------------*//**
 *
 *    The UL_PACK_SIZE macro is specific to Borland C++ right now; that
 *    compiler allows sizeof() in the preprocessor. Anyway, the routines
 *    depend upon float and long being the same number of bytes.
 *
 * \warning
 *    AVOID USING THIS MACRO.  It won't work in 64-bit Linux.
 *
 *//*-------------------------------------------------------------------------*/

#define UL_PACK_SIZE   sizeof(unsigned long)       // WIN32 only

/******************************************************************************
 * dtoul()
 *------------------------------------------------------------------------*//**
 *
 *    Converts a double value to an unsigned long value.
 *
 *    The dtoul() and ultod() routines convert between double variables and
 *    unsigned long variables, with a key feature -- the following calls
 *    yield the original number, without truncation, within the constraints
 *    of the implicit conversion from double to float:
 *
\verbatim
        double y, x = 10.3;         // x is the original
        y = ultod(dtoul(x));        // y == x [no truncation]
\endverbatim
 *
 *    Basically, the calls above reduce the precision of x to float, then stuff
 *    the bytes into a long. Then the bytes are stuffed into a float... order
 *    should be preserved under any architecture.  Then the float is expanded
 *    to a double.
 *
 * \win32
 *    These routines are useful for packing data into the \a lparam of a
 *    Windows message function, and extracting it later. They might have
 *    other uses, so the XPC library includes them here.  From windows.h:
 *    "typedef unsigned long DWORD".
 *
 * \warning
 *    This routine currently sucks.
 *
 * \param x
 *    The real number to convert to unsigned long.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

unsigned long
dtoul (double x)
{
#ifdef WIN32
   float xf;                                 /* float version of the double   */
#endif

   unsigned long xul;                        /* long version to return        */
   if (x > HUGE)                             /* flag that it is > 3.4e38      */
      x = -1.0;

   /*
    * This won't work in 64-bit Linux.
    */

#ifdef WIN32
    xf = (float) x;                          /* convert to shorter version    */
    memcpy((void *) &xul, (void *) &xf, UL_PACK_SIZE);
#else
   xul = (unsigned long) x;
#endif

   return xul;
}

/******************************************************************************
 * ultod()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an unsigned long value to a double value.
 *
 * \win32
 *    See the dtoul() function for more details.
 *
 * \param x
 *    The long value to convert to a double value.
 *
 * \return
 *    Returns a double version of the unsigned long value.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

double
ultod (unsigned long x)
{
   /*
    * This, in general, cannot work, since it relies on UL_PACK_SIZE
    * applying to both data types.
    */

#ifdef WIN32
    float xf;                                         /* float version of x   */
    memcpy((void *) &xf, (void *) &x, UL_PACK_SIZE);
    return (double) xf;
#else
   return (double) x;
#endif
}

/******************************************************************************
 * s_get_digits()  [only a local routine]
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to a decimal string, but backwards.
 *
 *    It is a helper function, so no pointer checks are done.
 *
 * \private
 *    This function is a private (C static) function to convert an
 *    integer to a decimal string.  The \a digitslot pointer should point to
 *    the \e end of the string to convert.
 *
 * \recursive
 *    This function lops off the rightmost digit, saves the result, and then
 *    recovers the last digit.  This is converted to an ASCII character.  If
 *    the saved result is non-zero, this function calls itself again with
 *    the saved result and with the output pointer moved one character
 *    backward.
 *
 * \param number
 *    The integer to be converted to a character string.
 *
 * \param digitslot
 *    The destination pointer for the next character.  This point is \e
 *    incremented with each successive recursive call.  This pointer never
 *    checked -- the caller is responsible for providing a good pointer to
 *    the end of an array large enough to hold the result.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

static void
s_get_digits (int number, char * digitslot)
{
   int nnext = number / 10;                     /* lop off rightmost digit    */
   int trunk = nnext * 10;                      /* shift back leftward        */
   int digit = number - trunk;                  /* recover rightmost digit    */
   *digitslot++ = (char) (digit + '0');         /* convert and store it       */
   *digitslot = '\0';                           /* null-terminate it          */
   if (nnext > 0)
      s_get_digits(nnext, digitslot);
}

/******************************************************************************
 * strreverse()
 *------------------------------------------------------------------------*//**
 *
 *    Reverses a character string in place.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

void
strreverse (char * source)
{
   int length = strlen(source);
   if (length > 1)
   {
#if XPC_HAVE_ALLOCA
      void * allocation = alloca(length + 1);
#else
      void * allocation = malloc(length + 1);
      if (not_nullptr(allocation))
      {
#endif

         char * src = source;
         char * dst = (char *) allocation;
         strcpy(dst, src);
         src = dst + length - 1;
         for (;;)
         {
            *source++ = *src;
            if (src == dst)
               break;

            src--;
         }

#if XPC_HAVE_ALLOCA
#else
      free(allocation);
      }
#endif
   }
}

/******************************************************************************
 * itoa10()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to an ASCII string, base 10.
 *
 *    This function is similar to a call to the Microsoft stdlib.h routine
 *    itoa(x, s, 10), except that the present function also specifies the
 *    minimum number of digits to use, including the negative sign.  The
 *    result is padded with 0's, if necessary.
 *
 * \warning
 *    The destination must be long enough.  It must be at least 'digits' in
 *    length, and at least long enough to hold the complete conversion.
 *
 * \return
 *    A pointer to the converted string is returned.  If the parameters
 *    were invalid, then a null pointer is returned.  Sometimes, a partial
 *    string may still be found, but there's no way to determine that
 *    status.
 *
 * \param number
 *    The integer to convert to a string.
 *
 * \param destination
 *    The destination string buffer.
 *
 * \param dsize
 *    The size of the destination buffer.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

char *
itoa10 (int number, char * destination, size_t dsize)
{
   char * result = destination;
   if (not_nullptr(destination))
   {
      if (number < 0)                              /* negative number?     */
      {
         *destination++ = '-';                     /* add minus sign       */
         dsize--;                                  /* count it             */
         number = -number;                         /* make it positive     */
      }
      if (dsize > 1)                               /* still have more room */
      {
         s_get_digits(number, destination);        /* convert backwards    */
         strreverse(destination);
      }
      else
         xpc_errprint_func(_("buffer too small"));
   }
   return result;
}

/******************************************************************************
 * atox()
 *------------------------------------------------------------------------*//**
 *
 *    Converts a hexadecimal string to an integer.
 *
 *    atox() is like the Standard C routine atoi(), except that it assumes the
 *    characters represent hexadecimal format.
 *
 * \param s
 *    The hex string to convert to an unsigned number.
 *
 * \return
 *    Returns the value represented by the string.  If the \a s parameter is
 *    null, then 0 is returned.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

unsigned
atox (const char * s)
{
   unsigned result = 0;
   if (not_nullptr(s))
      result = (unsigned) strtol(s, 0, 16);

   return result;
}

/******************************************************************************
 * ntoa()
 *------------------------------------------------------------------------*//**
 *
 *    ntoa() converts an integer to a string.
 *
 * \private
 *    It is a static function that provides support for itoa().  It is used
 *    to make all of the calculations for radix values that are \e not 10 or
 *    16.  Radix 10 requires handling of signed values, while we can make
 *    radix 16 calculations slightly more efficient than this function does.
 *
 * \param ui
 *    The unsigned integer to be converted to a string.  It is typed as a
 *    plain integer in order to allow the function to accept normal integers
 *    without warning.o
 *
 * \param outbuffer
 *    Provides the outputbuffer that will receive the string.  This buffer
 *    must be as least 32 characters in size, including space for the null
 *    pointer.
 *
 * \param radix
 *    Provides the radix of the conversion.  It can be any value except 1 or
 *    0, but itoa() handles radix 10 normally, and passes radix 16 calls to
 *    xtoa() and to this function.
 *
 * \return
 *    Returns a pointer to the original string buffer.  If a null pointer is
 *    returned, the buffer should not be used.  Otherwise, the string will
 *    be a valid, non-empty, null-terminated string.
 *
 * \todo
 *    Make the ntoa() function air-tight in regard to 64-bit versus 32-bit,
 *    signed versus unsigned, and long versus int; this will not be easy.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

static char *
ntoa (int ui, char * outbuffer, int radix)
{
   char * result = nullptr;
   if (not_nullptr(outbuffer))
   {
      static const int maxsize = 33;
      char temp[36];
      char * bptr = &temp[0];
      int currentsize = 0;
      unsigned value = xpc_statcast(unsigned, ui);    /* cast to unsigned     */
      char * outp;                                    /* used further below   */
      while (++currentsize < maxsize)
      {
         unsigned digivalue = value % radix;          /* get rightmost digit  */
         int digit = xpc_int_cast(digivalue) + '0';   /* convert to ascii     */
         if (digit > '9')
            digit = 'a' + (xpc_int_cast(digivalue) - 10);

         *bptr++ = (char) (digit);                    /* put in temp buffer   */
         value /= radix;                              /* "shift" rightward    */
         if (value == 0)                              /* nothing left         */
            break;
      }

      /* 'bptr' points after the last digit. Copy it backwards to outbuffer.  */

      outp = outbuffer;
      while (currentsize-- > 0)
         *outp++ = *(--bptr);

      *outp = 0;                                      /* terminate with null  */
      result = outbuffer;                             /* point to the output  */
   }
   return result;
}

/******************************************************************************
 * itoa()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to a string.
 *
 * \gcc
 *    itoa() is not an ANSI-C function, and gcc does not define it in the
 *    stdlib.h header file.  Here are it's properties:
 *
 *       Converts an integer value to a null-terminated string using the
 *       specified radix and stores the result in the given buffer.  If
 *       radix is 10 and value is negative the string is preceded by the
 *       minus sign (-). With any other radix, value is always considered
 *       unsigned.  buffer should be large enough to contain any possible
 *       value: (sizeof(int)*8+1) for radix=2, i.e.  17 bytes in 16-bits
 *       platforms and 33 in 32-bits platforms.
 *
 *    This function provides three variants of functionality in order to
 *    provide a slight edge in efficiency:
 *
 *       -  <b>Radix 10</b>.  Decimal conversions are signed, provide a
 *          minus sign, and don't require the handling of higher-base digits
 *          such as 'a' through 'f'.
 *       -  <b>Radix 16</b>.  Hexadecimal conversions are pass to xtoa(),
 *          which uses bit-shifting instead of division.
 *       -  <b>Radix 2 through 36</b>.  Exclusive of the 10 and 16 radices,
 *          the rest are handled by ntoa(), which treats the number as
 *          unsigned.
 *
 * \win32
 *    This function is made available under the name alt_itoa() in case one
 *    wants to use it instead of the itoa() provided by Visual Studio.
 *
 * \bit32
 *    Right now, the value 0x8000000 fails in the radix 10 conversion.  It
 *    fails because 0x80000000 is -2147483648, and negating this value
 *    returns the same value.
 *
 * \bit64
 *    Probably the same issue with 0x8000000000000000 (which is
 *    -9,223,372,036,854,775,808).
 *
 * \warning
 *    Since it emulates a legacy function, we can't add any real safety to
 *    this function.  It is a vulnerability!
 *
 * \param value
 *    The signed or unsigned integer to be converted to a string.  If the
 *    radix is 10, the integer is treated as signed.  Otherwise, it is
 *    treated as unsigned.
 *
 * \param outbuffer
 *    Provides the outputbuffer that will receive the string.  This buffer
 *    must be as least 32 characters in size, including space for the null
 *    pointer.  Unlike in xtoa(), the size of the buffer is not a parameter,
 *    to keep this function similar to other implementations of itoa().
 *
 * \param radix
 *    Provides the radix of the conversion.  It can be any value except 1 or
 *    0, but itoa() handles radix 10 normally, and passes radix 16 calls to
 *    xtoa() and radix 10 calls to ntoa().  Since this is a C++ function, it
 *    has a default value, 10.
 *
 * \return
 *    Returns a pointer to the original string buffer.  If a null pointer is
 *    returned, the buffer should not be used.  Otherwise, the string will
 *    be a valid, non-empty, null-terminated string.  If the number is the
 *    lowest negative number, then the string "INT_MIN" is returned.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

#if defined __GNUC__
char *
itoa (int value, char * outbuffer, int radix)
#else
char *
alt_itoa (int value, char * outbuffer, int radix)
#endif
{
   char * result = nullptr;
   if (not_nullptr(outbuffer))
   {
      static const int maxsize = 33;
      if (radix == 10)                                /* decimal version      */
      {
         char temp[36];
         char * bptr = &temp[0];
         cbool_t is_minus = value < 0;
         int currentsize = 0;
         char * outp;                                 /* used further below   */
         if (is_minus)
            value = -value;

         if (value == -value)                         /* 0x80000000... fails  */
         {
            strcpy(outbuffer, "INT_MIN");             /* alternative          */
            return outbuffer;
         }
         while (++currentsize < maxsize)
         {
            int digivalue = value % 10;               /* get rightmost digit  */
            int digit = digivalue + '0';              /* convert to ascii     */
            *bptr++ = xpc_statcast(char, digit);      /* put in temp buffer   */
            value /= 10;                              /* "shift" rightward    */
            if (value == 0)                           /* nothing left         */
               break;
         }

         /* 'bptr' points past the last digit; copy it backwards to outbuffer */

         outp = outbuffer;
         if (is_minus)
            *outp++ = '-';

         while (currentsize-- > 0)
            *outp++ = *(--bptr);

         *outp = 0;                                   /* terminate with null  */
         result = outbuffer;                          /* point to the output  */
      }
      else if (radix == 16)                           /* hex-optimized        */
         result = xtoa(value, outbuffer, xpc_statcast(size_t, maxsize));
      else
         result = ntoa(value, outbuffer, radix);      /* generic unsigned int */
   }
   return result;
}

/******************************************************************************
 * xtoa()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to a hexadecimal string.
 *
 *    xtoa() converts an integer to a hex string.
 *
 *    The algorithm is very similar to itoa().  It differs in two
 *    ways:
 *
 *      -  The input value is forced to unsigned int.
 *      -  No minus signs are ever output.
 *
 *    If you want more flexibility and safety, at the cost of speed and
 *    convenience, use sprintf() to convert the hexadecimal number to a
 *    string.
 *
 * \bit64
 *    This site has some interesting information on 64-bit values:
 *
 * http://www.ccr.jussieu.fr/ccr/Documentation/Calcul/vac/html/en_US/doc/compiler/ref/rucl64mg.htm
 *
 *    Here are the longest string representations you will find:
 *
 *      -  +18,446,744,073,709,551,616   (decimal, 27 characters)
 *      -  0xFFFFFFFFFFFFFFFFUL          (hexadecimal, 20 characters)
 *
 *    We won't yet worry about "long long" until 128-bit machines are
 *    common.
 *
 * \param ui
 *    The unsigned integer to be converted to a string.  It is typed as a
 *    plain integer in order to allow the function to accept normal integers
 *    without warning.o
 *
 * \param outbuffer
 *    Provides the outputbuffer that will receive the string.  This buffer
 *    must be as least 32 characters in size, including space for the null
 *    pointer.
 *
 * \param n
 *    The size of the destination buffer.  This makes the function a bit
 *    safer to use.
 *
 * \return
 *    Returns a pointer to the original string buffer.  If a null pointer is
 *    returned, the buffer should not be used.  Otherwise, the string will
 *    be a valid, non-empty, null-terminated string.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 * \todo
 *    Make the xtoa() function air-tight in regard to 64-bit versus 32-bit,
 *    signed versus unsigned, and long versus int; this will not be easy.
 *
 *//*-------------------------------------------------------------------------*/

char *
xtoa (unsigned ui, char * outbuffer, size_t n)
{
   char * result = nullptr;
   if (not_nullptr(outbuffer))
   {
      static const size_t maxsize = 33;
      char temp[36];
      char * bptr = &temp[0];
      int currentsize = 0;
      unsigned value = xpc_statcast(unsigned, ui);    /* cast to unsigned     */
      char * outp;                                    /* used further below   */
      if (n > maxsize)
         n = maxsize;

      while (++currentsize < xpc_statcast(int, n))    /* (too much casting)   */
      {
         unsigned digivalue = value % 16;             /* get rightmost digit  */
         int digit = xpc_statcast(int, (digivalue))+'0'; /* convert to ascii  */
         if (digit > '9')
            digit = 'a' + (xpc_statcast(int, digivalue) - 10);

         *bptr++ = xpc_statcast(char, digit);         /* put in temp buffer   */
         value >>= 4;                                 /* "shift" rightward    */
         if (value == 0)                              /* nothing left         */
            break;
      }

      /* 'bptr' points after the last digit. Copy it backwards to outbuffer.  */

      outp = outbuffer;
      while (currentsize-- > 0)
         *outp++ = *(--bptr);

      *outp = 0;                                      /* terminate with null  */
      result = outbuffer;                             /* point to the output  */
   }
   return result;
}

/******************************************************************************
 * Method:        combinations()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the number of combinations of a set of values.
 *
 *    The number of combination C(n,r) of n things taken r at a time is
 *
\verbatim
                          n!
           C(n,r) = -----------
                     r! (n-r)!
\endverbatim
 *
 *    where n > 0 and r <= n.  Here, the assumption is that we have only
 *    one of each item, so that we cannot grab the same item and pair it
 *    with another instance of itself.
 *
 *    We return -1 if the conditions that n > 0 and r <= n don't hold.  If
 *    they do hold, but r == n, we simply return a 1.  If r < n, then we
 *    first simplify the equation by noting that n!/(n-r)! = (n-r+1) x
 *    (n-r+2) x ...  x n.  Since r is usually small (2 or 3), both this and
 *    r! can be calculated pretty quickly.
 *
 * \todo
 *    Note that we can optimize further by combining the two loops below.
 *
 * \param n
 *    The total number of items in the set.
 *
 * \param r
 *    The size of each group of these items.
 *
 * \return
 *    Returns the number of combination C(n,r) of n things taken r at a
 *    time.  If the parameters are invalid, then -1 is returned.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

int
combinations (int n, int r)
{
   int c = ((n > 0) && (r <= n)) ? 1 : -1;       /* the necessary conditions  */
   if (c != -1)                                  /* -1 means they don't hold  */
   {
      if (r < n)                                 /* bother to calculate?      */
      {
         int rfact = 1;                          /* yes                       */
         int ri, ni;
         for (ri = 2; ri <= r; ri++)
            rfact *= ri;

         for (ni = n - r + 1; ni <= n; ni++)
            c *= ni;

         c /= rfact;
      }
   }
   else
      xpc_warnprint_func(_("cannot calculate combination"));

   return c;
}

/******************************************************************************
 * pairing_to_index()
 *------------------------------------------------------------------------*//**
 *
 *    Converts from a pair value to an index value.
 *
 *    Given the discussion of combinations(), let us see what we can do to
 *    monitor the presentation of a set of pairings of n items, where each
 *    item cannot be paired with itself.
 *
 *    For example, we have 4 items taken 2 at a time, and we want to be sure
 *    that each item is used the same number of times.  To do this, we have
 *    to make an array of size C(4, 2).  Then, each combination number c = 0
 *    to C(4, 2)-1 corresponds to a given pairing (j, k).  Given n
 *    [constant] and c [from 0 to C(4, 2)-1], we want to find the j and k
 *    values by counting.
 *
 *    For example, given 4 values (0, 1, 2, and 3) to be paired, the indices
 *    should match up with the selections as follows, for a total of
 *    C(4, 2) = 6 pairings:
 *
\verbatim
    n=4: index    j      k
    -----------------------
           0      0      1
           1      0      2
           2      0      3
           3      1      2
           4      1      3
           5      2      3
\endverbatim
 *
 *    We have to assume below that c is a legal value, but know that the
 *    results can never violate 0 <= j < n and 0 <= k < n.  So we check, and
 *    set them both to -1 if a match was never found.
 *
 *    Note that the brute-force counting solution below is probably
 *    replaceable by a faster formula.  Right now, the slowness is assumed
 *    to be "no problem".
 *
 * \param n
 *    The number of items in the set.
 *
 * \param j
 *    The number of the first item in the pair.
 *
 * \param k
 *    The number of the second item in the pair.
 *
 * \return
 *    The index value (see the example table above) is returned.  If an
 *    error occurs (such as a negative parameter), then a -1 is returned.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

int
pairing_to_index (int n, int j, int k)
{
   int result = -1;
   cbool_t ok = (n > 1) && (j != k);
   if (ok)
   {
      cbool_t finished = false;
      int c = 0;
      int firstitem;
      int seconditem;
      for (firstitem = 0; firstitem < n-1; firstitem++)
      {
         for (seconditem = firstitem+1; seconditem < n; seconditem++)
         {
            if ((firstitem == j) && (seconditem == k))
            {
               finished = true;
               result = c;
               break;
            }
            else if (firstitem != seconditem)   /* ca 2010-04-24              */
               c++;                             /* heh heh heh                */
         }
         if (finished)
            break;
      }
   }
   return result;
}

/******************************************************************************
 * index_to_pairing()
 *------------------------------------------------------------------------*//**
 *
 *    Converts from an index value to a pair value.
 *
 *    See the inverse of this function, pairing_to_index() for a discussion.
 *    Again, a brute-force counting method is used.
 *
 * \sideeffect
 *    The pair is returned in the pointer parameters \a j and \a k.
 *
 * \param comboindex
 *    The index value for which a pair is desired.
 *
 * \param n
 *    The number of items in the set that are to be considered paired.
 *
 * \param j [out]
 *    Pointer for the first item of the pair.
 *
 * \param k [out]
 *    Pointer for the second item of the pair.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

void
index_to_pairing (int comboindex, int n, int * j, int * k)
{
   if (not_nullptr_2(j, k))
   {
      int c = 0;
      int firstitem = 0;
      int seconditem = 0;
      if (n >= 2)
      {
         cbool_t finished = false;
         for (firstitem = 0; firstitem < n-1; firstitem++)
         {
            for (seconditem = firstitem+1; seconditem < n; seconditem++)
            {
               if (c == comboindex)
               {
                  finished = true;
                  break;
               }
               else
                  c++;
            }
            if (finished)
               break;
         }
      }
      *j = firstitem;
      *k = seconditem;
   }
}

/******************************************************************************
 * random_number()
 *------------------------------------------------------------------------*//**
 *
 *    A simple routine to augment Microsoft's random-number generator.
 *
 *    This routine generates integers from 0 to rangemax-1, where rangemax <=
 *    RAND_MAX.  Thus, rangemax different integer values are generated.
 *
 *    RAND_MAX is only 32767, which is horrible for all but the simplest forms
 *    of randomness.
 *
 *    There are some more random-number functions in the template file
 *    randtmpl.h in the (upcoming) POSIX C++ Wrapper library; these should
 *    be more effective, but they work on existiing lists of items.
 *
 *    Also see methods in "Numerical Recipes in C", and in books by Donald
 *    Knuth.
 *
 *    Note that this random-number routine is slightly biased.  For
 *    example, if rangemax = 3, then the divisor is 32768/3 = 10922.  If
 *    rand() == 32767, the result value is 32767/10922 = 3.00009, or 3; if
 *    rand() == 32766, it is 3.0000, or 3.  So, these two occurrences
 *    [having a total probability of 2/32767 = 0.0061%] have to be handled
 *    by forcibly changing the value to rangemax-1.  The probabilities for
 *    each of the 3 results are:
 *
 *         0: rand()=0 to 10921 --> 10922/32767 = 0.3333231
 *         1: rand()=10922-21843 --> 10922/32767 = 0.3333231
 *         2: rand()=21844-32767 --> 10923/32767 = 0.3333537
 *
 * \note
 *    Currently, there is a much better generator than rand() in the xpccut
 *    library.  See the function xpccut_random().
 *
 * \param rangemax
 *    The range of the random numbers, from 0 to rangemax-1.
 *
 * \param seedfirst
 *    Seed this function the first time it is called.
 *
 * \param seedsame
 *    Use the same seed each time.
 *
 * \return
 *    -	numeric_test_01_01()
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

int
random_number (int rangemax, cbool_t seedfirst, cbool_t seedsame)
{
   unsigned divisor;
   unsigned rvalue;
   if (seedfirst)
   {
      unsigned seedvalue = seedsame ? 1 : (unsigned) time(NULL) ;
      srand(seedvalue);
   }
   divisor = ((unsigned) (RAND_MAX)+1) / (unsigned) rangemax;
   rvalue = ((unsigned) rand()) / divisor;
   if (rvalue >= (unsigned) rangemax)
      rvalue = rangemax - 1;                   /* fix the occasional odd case */

   return (int) rvalue;
}

#ifdef XPC_DEFINE_RANDOMIZE

/******************************************************************************
 * randomize()
 *------------------------------------------------------------------------*//**
 *
 *    Seeds the random number generator using srand().
 *
 *    The seed argument for srand() is currently "time(NULL)".  We have a
 *    lot of work to do to see if this is adequate.  Random numbers are too
 *    important to be left to chance.
 *
 * \unittests
 *    None.
 *
 *//*-------------------------------------------------------------------------*/

void
randomize (void)
{
   srand((unsigned) time(NULL));
}

#endif                              /* XPC_DEFINE_RANDOMIZE                   */

/******************************************************************************
 * seedrandom()
 *------------------------------------------------------------------------*//**
 *
 *    A wrapper for randomize().
 *
 * \unittests
 *    None.
 *
 *//*-------------------------------------------------------------------------*/

void
seedrandom (void)
{
   randomize();
}

/******************************************************************************
 * random_integers()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a random array of integers with numbers ranging from 0 to
 *    siz-1.
 *
 *    Each value appears just once in the array.
 *
 *    This function is meant for generating random trials, where each item
 *    in the trial is to be presented just once, but in a random order.
 *
 *    This function firsts allocates, and then fills an array with numbers
 *    ranging from 0 to siz-1, in order.  Then the array is randomized by
 *    random exchanges.  <b>We need to document the exact method and
 *    rigorously verify that it is correct.</b>
 *
 * \warning
 *    Be sure to free up the array [using free()]) when finished with it!
 *
 * \return
 *    Returns a null pointer if a problem occurred. Otherwise, a pointer to
 *    the allocated integer array is returned.  This pointer must later be
 *    freed by the caller.
 *
 * \param siz
 *    Size of the set of integers to store in an array.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

int *
random_integers (int siz)
{
   static cbool_t inited_random_integers = false;
   int * rptr = nullptr;
   if (siz > 1)                               /* if <= 1, getrandom() errs    */
   {
      if (! inited_random_integers)           /* initialize the generator     */
      {
         randomize();
         inited_random_integers = true;
      }
      rptr = malloc(siz * sizeof(int));
      if (not_nullptr(rptr))
      {
         int * iptr = malloc(siz * sizeof(int));
         if (not_nullptr(iptr))
         {
            int i, j;
            for (i = 0; i < siz; i++)
               iptr[i] = i;

            for (j = 0; j < siz; j++)
            {
               int temp = rand() % (siz-j);   /* 0 <= temp < (siz-i)         */
               rptr[j] = iptr[temp];
               iptr[temp] = iptr[siz-j-1];
            }
            free(iptr);
         }
         else
            xpc_errprint_func(_("failed"));
      }
      else
         xpc_errprint_func(_("failed"));
   }
   return rptr;
}

/******************************************************************************
 * Internal parameters for random-number generation
 *------------------------------------------------------------------------*//**
 *
 *    Controls random-number generation.
 *
 *    IM1 and many other parameters control the random-number generation of
 *    the function ran2().
 *
 * \private
 *    These constants control the characteristics of the random-number
 *    generator.  Here are the parameter values.  Tinker with them at your
 *    own risk!
 *
\verbatim
         const long IM1            = 2147483563;
         const long IM2            = 2147483399;
         const double AM           = 1.0 / IM1;
         const long IMM1           = IM1 - 1;
         const int IA1             = 40014;
         const int IA2             = 40692;
         const int IQ1             = 53668;
         const int IQ2             = 52774;
         const int IR1             = 12211;
         const int IR2             =  3791;
         const int NTAB            =    32;
         const long NDIV           = 1 + IMM1/NTAB;
         const double EPSILON      = 1.2e-7;
         const double RAN2MAX      = 1.0 - EPSILON;
         cbool_t inited_ran2       = false;
\endverbatim
 *
 *    We also have to add a few defines, since plain C doesn't like
 *    variables in initializer statements.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_IM1                  2147483563
#define XPC_NTAB                 32
#define XPC_RAN_EPSILON          1.2e-7

static const long IM1            = XPC_IM1;
static const long IM2            = 2147483399;
static const double AM           = 1.0 / XPC_IM1;
static const long IMM1           = XPC_IM1 - 1;
static const int IA1             = 40014;
static const int IA2             = 40692;
static const int IQ1             = 53668;
static const int IQ2             = 52774;
static const int IR1             = 12211;
static const int IR2             =  3791;
static const int NTAB            = XPC_NTAB;
static const long NDIV           = 1 + (XPC_IM1-1)/XPC_NTAB;
static const double EPSILON      = XPC_RAN_EPSILON;
static const double RAN2MAX      = 1.0 - XPC_RAN_EPSILON;
static cbool_t inited_ran2       = false;

/******************************************************************************
 * ran2()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a less dangerous random-number generator than do most
 *    compiler libraries.
 *
 *    This routine is presumably the ran2() described in "Numerical Recipes
 *    in C" book, but it looks only a little like the one in the original
 *    Pascal version of the book.  At any rate, here is the description of
 *    this routine.
 *
 *    For many purposes we do care that there be no sequential
 *    correlations, but we don't care that the discreteness of the random
 *    values be as fine as is allowed by all significant bits of the word
 *    size.  In this case, one might want the gain in speed that comes from
 *    using only one linear congruential generator, shuffling as was done
 *    in their ran0() function.
 *
 *    The period of ran2() is effectively infinite.  Its principal
 *    limitation is that it returns one of only 714025 possible values,
 *    equally spaced as a comb in the interval [0, 1).  Note the value of
 *    EPSILON below, which defines the closeness of the high part of the
 *    interval.
 *
 *    Set initvalue to any negative value to initialize the sequence.  As a
 *    convenience for C++ users, this value is defaulted to one, so that
 *    only the first call in the sequence needs to have a parameter.
 *
 *   (C) Copr. 1986-92 Numerical Recipes Software +139.
 *
 * \param initvalue
 *    The starting value of the random sequence.  Also known as the "seed".
 *
 * \return
 *    Returns a uniform deviate between 0.0 and 1.0.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

double
ran2 (long initvalue)
{
   int j;
   long k;
   static long seedvalue      = 0;
   static long dummy2         = 123456789;
   static long iy             = 0;
   static long iv[XPC_NTAB];
   double temp;
   if ((initvalue <= 0) || ! inited_ran2)             /* initialize           */
   {
      if (initvalue == 0)
         seedvalue = 1;
      else if (initvalue < 0)
         seedvalue = -initvalue;
      else
         seedvalue = initvalue;

      dummy2 = seedvalue;
      for (j = NTAB + 7; j >= 0; j--)
      {
         k = seedvalue / IQ1;
         seedvalue = IA1 * (seedvalue - k*IQ1) - k*IR1;
         if (seedvalue < 0)
            seedvalue += IM1;

         if (j < NTAB)
            iv[j] = seedvalue;
      }
      iy = iv[0];
      inited_ran2 = true;                             /* done                 */
   }
   k = seedvalue / IQ1;
   seedvalue = IA1 * (seedvalue - k*IQ1) - k*IR1;
   if (seedvalue < 0)
      seedvalue += IM1;

   k = dummy2 / IQ2;
   dummy2 = IA2 * (dummy2 - k*IQ2) - k*IR2;
   if (dummy2 < 0)
      dummy2 += IM2;

   j = iy / NDIV;
   iy = iv[j] - dummy2;
   iv[j] = seedvalue;
   if (iy < 1)
      iy += IMM1;
   if ((temp = AM*iy) > RAN2MAX)
      return RAN2MAX;
   else
      return temp;
}

/******************************************************************************
 * ran2list()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a more robust list of random numbers.
 *
 *    Similar to random_integers() in that it yields a random array of integers
 *    with numbers ranging from 0 to siz-1.  However, it uses the ran2()
 *    function to generate the random numbers, accepts a seed value (so that
 *    the sequence can be reproducable), and requires that the user provide an
 *    integer array to fill in.
 *
 *    Returns false if a problem occurred.
 *
 *
 *    The first and third options only work if neither has been opted for
 *    previous during the run of the enclosing application.
 *
 *    We've added a feature to allow this random sequence to be re-initialized,
 *    so that we can reproduce the same sequence of random numbers at any time.
 *    To do this, use the call
 *
 *         ran2list(0, 0);
 *
 *    when you will be passing a seedvalue greater than zero later, to start
 *    the sequence where you want.
 *
 * \todo
 *    Check the pointer values and add xpc_errprint_func() calls as necessary.
 *
 * \param userbuffer
 *    The destination for the random array.
 *
 * \param siz
 *    Number of elements to be in the random array.  The provided buffer
 *    should have room for at least this number of elements.
 *
 * \param seedvalue
 *    Optional seed value.  The special values -1 are described for the
 *    xpc_seedings_t enumeration.  A value of seedvalue > 0 means to  seed
 *    just once, but use the provided seed.
 *
 * \param inplaceshuffle
 *    Randomize the list or not.  If inplaceshuffle is false, the temporary
 *    array iptr[] is initialized with integers from 0 to siz-1, so that the
 *    userbuffer ends up with a randomly-organized collection of values from
 *    0 to siz-1.  If inplaceshuffle is true, the userbuffer's values are
 *    copied, so that the userbuffer ends up with its original values, but
 *    in a different order.
 *
 * \return
 *    Returns 'true' if the list was properly generated.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
ran2list (int * userbuffer, int siz, int seedvalue, cbool_t inplaceshuffle)
{
   static cbool_t inited_ran2list = false;
   cbool_t result = false;
   if (siz > 1)
   {
      int * rptr = userbuffer;
      cbool_t doseed = false;
      if (! inited_ran2list && (seedvalue != (int) XPC_SEEDING_DONE))
      {
         inited_ran2list = true;
         if (seedvalue == (int) XPC_SEEDING_NORMAL)
         {
            randomize();
            while (seedvalue == 0)
               seedvalue = rand();
         }
         doseed = true;
      }
      if (doseed)
         (void) ran2((long) seedvalue);         /* prime it & throw it away   */

      if (not_nullptr(rptr))
      {
         int * iptr = malloc(siz * sizeof(int));
         if (! is_nullptr(iptr))                /* tricky, avoid two messages */
         {
            int ji;
            if (inplaceshuffle)                 /* shuffle existing values    */
            {
               int i;
               for (i = 0; i < siz; i++)
                  iptr[i] = rptr[i];            /* copy each initial element  */
            }
            else                                /* shuffle 0 to siz-1         */
            {
               int i;
               for (i = 0; i < siz; i++)
                  iptr[i] = i;
            }
            for (ji = 0; ji < siz; ji++)
            {
               double x = ran2(1);              /* use in already-seeded way  */
               int temp = (int) (x * (siz-ji)); /* [0,1) --> [0, siz-ji)      */
               rptr[ji] = iptr[temp];
               iptr[temp] = iptr[siz-ji-1];
            }
            free(iptr);
            result = true;
         }
         else
            xpc_errprint_func(_("failed"));
      }
   }
   else
   {
      inited_ran2list = false;
      inited_ran2 = false;
   }
   return result;
}

/******************************************************************************
 * ran2shuffle()
 *------------------------------------------------------------------------*//**
 *
 *    Produces a shuffled array of random values.
 *
 *    ran2shuffle() is a minor variation on ran2list(), and it in fact uses
 *    ran2list() to perform an in-place shuffle of an array, for those cases
 *    where the array values are /not/ a[i] = i for all i.
 *
 * \param userbuffer
 *    The destination for the random array.
 *
 * \param siz
 *    Number of elements to be in random array.
 *
 * \param seedvalue
 *    Optional seed value.
 *
 * \return
 *    -	numeric_test_01_01()
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
ran2shuffle (int * userbuffer, int siz, int seedvalue)
{
   return ran2list(userbuffer, siz, seedvalue, true);
}

/******************************************************************************
 * Floating-Point Section
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * cumulative_average()
 *------------------------------------------------------------------------*//**
 *
 *    Accepts a data point and a count, and returns the current cumulative
 *    average.
 *
 *    See the Cumulative Moving Average section in the
 *    http://en.wikipedia.org/wiki/Moving_average page.
 *
 *    Our formula is a little different:
 *
\verbatim
               (n-1) C    + x
                      n-1    n
         C  = --------------------, n = 1, 2, ...
          n             n
\endverbatim
 *
 *    The first term in the numerator simply reconstitutes the series sum by
 *    multiplication, then adds the new data point, then calculates the new
 *    average.
 *
 *    An alternative implementation ought to be a bit faster, as it trades a
 *    multiplication for an addition:
 *
\verbatim
               S    + x
                n-1    n
         C  = --------------, n = 1, 2, ...
          n       n
\endverbatim
 *
 *    Here, S[n-1] is the current sum, which is updated before the division
 *    by adding x[n].
 *
 *    Both implementations suffer from the defect that the sum could
 *    overflow as time went on.  This defect can be fixed with this
 *    implementation:
 *
\verbatim
               (n-1)          1
         C  =  ----- C     + --- x
          n      n     n-1    n   n
\endverbatim
 *
 *    But then underflow could occur with a large enough n.
 *
 * \param cumavg
 *    Contains the cumulative average, which is updated this function as a
 *    side-effect.
 *
 * \param xn
 *    The current datapoint in the series, where n ranges from 1 and upward.
 *
 * \param n
 *    The index, re 1, of the current datapoint.
 *
 * \return
 *    Returns 'true' if all of the parameters were valid.
 *
 * \unittests
 *    -	numeric_test_01_01()
 *
 * \todo
 *    -  Reconstruct this function as an inline function.  -Or-
 *    -  Make it robust over the full range of values.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
cumulative_average (double * cumavg, double xn, unsigned n)
{
   cbool_t result = not_nullptr(cumavg);
   if (result)
   {
      result = n > 0;
      if (result)
      {
         *cumavg = ((double) (n - 1) * (*cumavg) + xn) / (double) n;
      }
      else
         xpc_errprint_func(_("division by zero"));
   }
   else
      xpc_errprint_func(_("null pointer"));

   return result;
}

#ifdef __CYGWIN__
#define NAN 0           /* TODO */
#endif

/******************************************************************************
 * xpc_nan()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a platform-independent "not-a-number" value.
 *
 *    xpc_nan() is provided because one cannot guarantee that a NAN
 *    constant is initialized before being used in client code.
 *
 * \note
 *    NAN is actually a union between a double value and a pair of integers.
 *    The double cast seems to cause the double field to be returned.
 *
 * \gnu
 *    As per C99, there are two NaNs:
 *
 *       -  Quiet NaN.  Does not raise exceptions when used in arithmetic.
 *       -  Signalling NaN.  The opposite of a quiet NaN.
 *
 * \return
 *    A value of "NaN" suitable for any platform.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_nan (void)
{
   return NAN;
}

/******************************************************************************
 * xpc_infinite()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a value of "infinite" suitable for any platform.
 *
 *    xpc_infinite() is provided because one cannot guarantee that the
 *    "infinite" constant is initialized before it is used in client code.
 *
 *    Luckily, C++ allows using the results of global functions to
 *    initialize constant values when the program starts.  Unluckily, we've
 *    converted this code to C.  Hence the need for a function instead of a
 *    constant.
 *
 * \return
 *    A value of "infinite" suitable for any platform.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

double
xpc_infinite (void)
{
   return INFINITY;                                         /* not HUGE_VAL   */
}

/******************************************************************************
 * xpc_is_zero()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against zero.
 *
 *    It uses the xpc_is_eq() function to make the comparison, which assumes
 *    some lack of accuracy in any floating-point result that might be zero.
 *
 * \param x
 *    The floating-point value to be tested against zero.
 *
 * \return
 *    'true' (1) if the value x is within NAN_INF_EPSILON (0.000001) of 0.0,
 *    and 'false' (0) otherwise.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_zero (double x)
{
   return xpc_is_eq(x, 0.0);        /* versus (x > -0.00001) && (x < 0.00001) */
}

/******************************************************************************
 * xpc_is_infinite()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against infinity.
 *
 * \win32
 * \win64
 *    It uses the isinf() private function to make the comparison.
 *
 * \gnu
 *    It uses the glibc isinf() function to make the comparison.
 *
 * \param x
 *    The floating-point value to be tested against infinity.
 *
 * \return
 *    'true' (1) if the value x is 'infinite'.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_infinite (double x)
{
   return isinf(x);                             /* POSIX */
}

/******************************************************************************
 * ieee_is_infinite()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against infinity.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    An infinity has an exponent of 255 (shift left 23 positions) and
 *    a zero mantissa. There are two infinities -- positive and negative.
 *
 * \param x
 *    The floating-point value to be tested against infinity.
 *
 * \return
 *    Returns 'true' (1) if the value x is 'infinite'.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
ieee_is_infinite (float x)
{
#ifdef __GNUC__
   return ! isfinite(x);
#else
   static const int inf_as_int = 0x7F800000;
   if ( ( *(int *) &x & 0x7FFFFFFF ) == inf_as_int)      // type-punning
      return true;
   else
   return false;
#endif
}

/******************************************************************************
 * xpc_is_minus_infinite()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against -infinity.
 *
 *    It uses the isinf() private function to make the comparison.
 *
 * \param x
 *    The floating-point value to be tested against minus infinity.
 *
 * \return
 *    'true' (1) if the value x is 'minus-infinite', and 'false' (0)
 *    otherwise.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_minus_infinite (double x)
{
   return isinf(x) == -1;                       /* POSIX */
}

/******************************************************************************
 * xpc_is_huge()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against the huge value.
 *
 *    It uses the isinf() private function to make the comparison.  At some
 *    point there may be a need to distinguish between "infinite" and
 *    "huge".
 *
 * \param x
 *    The floating-point value to be tested against "huge value".
 *
 * \return
 *    Returns 'true' if isinf(x) is 1.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_huge (double x)
{
   return isinf(x) == 1;                        /* POSIX */
}

/******************************************************************************
 * xpc_is_minus_huge()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against the negative huge value.
 *
 *    It uses the isinf() private function to make the comparison.
 *
 * \param x
 *    The floating-point value to be tested against minus "huge value".
 *
 * \return
 *       Returns 'true' if isinf(x) is -1.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_minus_huge (double x)
{
   return isinf(x) == -1;                       /* POSIX */
}

/******************************************************************************
 * xpc_is_normal()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number for normalness (non-infinite and
 *    non-Nan).
 *
 *    It uses the isinf() and isnan() private functions to make the
 *    comparison.
 *
 * \param x
 *    The floating-point value to be tested as a normal number.
 *
 * \return
 *    Returns 'true' if the number is not plus or minus infinite, and is also
 *    not a NaN.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_normal (double x)
{
   return isinf(x) == 0 && isnan(x) == 0 ;      /* POSIX */
}

/******************************************************************************
 * xpc_is_nan()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against NaN.
 *
 *    It uses the isnan() private function (or Windows macro) to make the
 *    comparison.
 *
 * \param x
 *    The floating-point value to be tested against "not a number".
 *
 * \return
 *    Returns 'true' if the parameter matches the NaN value.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_nan (double x)
{
   return isnan(x) != 0;                        /* POSIX */
}

/******************************************************************************
 * ieee_is_nan()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a floating-point number against NaN assuming the format is the
 *    IEEE floating-point format.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *
 *    A NAN has an exponent of 255 (shifted left 23 positions) and
 *    a non-zero mantissa.
 *
 * \param x
 *    The floating-point value to be tested against "not a number".
 *
 * \return
 *    Returns 'true' if the parameter matches the NaN value.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
ieee_is_nan (float x)
{
#ifdef __GNUC__
   return isnan(x);
#else
   int exp = *(int *) &x & 0x7F800000;                // type-punning
   int mantissa = * (int *) &x & 0x007FFFFF;          // ditto
   if (exp == 0x7F800000 && mantissa != 0)
      return true;

   return false;
#endif
}

/******************************************************************************
 * ieee_sign()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the sign of an IEEE floating-point number.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *
 *    The sign bit of a number is the high bit.
 *
 * \param x
 *    The number whose sign is to be checked.
 *
 * \return
 *    Returns the value of the sign.  0 means the value is positive or 0.0.
 *    A negative value (-2147483648) means the value is negative (or -0.0).
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 * \gcc
 *    warning: dereferencing type-punned pointer will break strict-aliasing
 *    rules.
 *
 *//*-------------------------------------------------------------------------*/

int
ieee_sign (float x)
{
   return *((int *) &x) & 0x80000000;                 // type-punning
}

/******************************************************************************
 * ieee_almost_equal()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a thorough equality check using IEEE definitions.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    However, we do all the checks, rather than make them compile only
 *    conditionally.  We want completeness.
 *
 *    These checks are:
 *
 *       -  Infinity check.
 *          If A or B are infinity (positive or negative) then only return
 *          true if they are exactly equal to each other - that is, if they
 *          are both infinities of the same sign.  This check is only needed
 *          if you will be generating infinities and you don't want them
 *          'close' to numbers near FLT_MAX.
 *       -  NAN check.
 *          If A or B are a NAN, return false. NANs are equal to nothing,
 *          not even themselves.  This check is only needed if you will be
 *          generating NANs and you use a maxUlps greater than 4 million or
 *          you want to ensure that a NAN does not equal itself.
 *       -  Sign check.
 *          After adjusting floats so their representations are
 *          lexicographically ordered as twos-complement integers a very
 *          small positive number will compare as 'close' to a very small
 *          negative number. If this is not desireable, and if you are on a
 *          platform that supports subnormals (which is the only place the
 *          problem can show up) then you need this check.  The check for A
 *          == B is because zero and negative zero have different signs but
 *          are equal to each other.
 *       -  Final check.  Convert the values to integers via their
 *          addresses.  Then make them lexicographically ordered as two's
 *          complement integers.  Then see how far apart they are.
 *
 *    The sign bit of a number is the high bit.
 *
 * \param x
 *    The floating-point value to be compared against.
 *
 * \param y
 *    The floating-point value to be compared.
 *
 * \param maxdiff
 *    The maximum number of values by which \a x and \a y can differ.  This
 *    value determines differences based on converting the values to their
 *    equivalent integer format.
 *
 * \return
 *    Returns 'true' if the numbers \a x and \a y are as close as the
 *    desired parameter value.  Here are all the conditions in which 'true'
 *    is returned:
 *
 *       -  Both are of the same kind of infinity (plus or minus).
 *       -  Both are very close together, with a difference of \a maxdiff or
 *          less if treated as integer representations.
 *
 *    'False' is returned if:
 *
 *       -  Either item is "not a number".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 * \gcc
 *    warning: dereferencing type-punned pointer will break strict-aliasing
 *    rules.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
ieee_almost_equal (float x, float y, int maxdiff)
{
   if (ieee_is_infinite(x) || ieee_is_infinite(y))    // infinity check
      return x == y;

   if (ieee_is_nan(x) || ieee_is_nan(y))              // NAN check
      return false;

   if (ieee_sign(x) != ieee_sign(y))                  // sign check
      return x == y;                                  // ????
   else                                               // final check
   {
      int xint = *(int *) &x;                         // type-punning
      int yint = *(int *) &y;                         // ditto
      int intdiff;
      if (xint < 0)
         xint = 0x80000000 - xint;                    // lex ordered

      if (yint < 0)
         yint = 0x80000000 - yint;                    // lex ordered

      intdiff = abs(xint - yint);
      if (intdiff <= maxdiff)
         return true;

      return false;
   }
}

/******************************************************************************
 * ieee_minus_zero()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the IEEE value representing minus zero.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *
 * \return
 *    Returns the value of "-0".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 * \gcc
 *    warning: dereferencing type-punned pointer will break strict-aliasing
 *    rules.
 *
 *//*-------------------------------------------------------------------------*/

float
ieee_minus_zero ()
{
   float result = (float) (*(int *) &result = 0x80000000);  // type-punning
   return result;
}

/******************************************************************************
 * ieee_nan()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the IEEE value representing "not a number" (NaN).
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    It returns the square root of -1.
 *
 * \return
 *    Returns the value of "sqrt(-1)".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

float
ieee_nan ()
{
   float result;
   result = (float) sqrt(-1.0f);
   return result;
}

/******************************************************************************
 * ieee_nan_2()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the IEEE value representing "not a number" (NaN), in a
 *    different way.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    It returns the result of "0/0".  It should give the same NaN as
 *    ieee_nan() on Intel chips.
 *
 * \return
 *    Returns the value of "0 divided by 0".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

float
ieee_nan_2 ()
{
   float zero1 = 0.0f, zero2 = 0.0f;
   float result;
   result = zero1 / zero2;
   return result;
}

/******************************************************************************
 * ieee_nan_3()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the IEEE value representing "not a number" (NaN), in a
 *    \e third way.
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    It returns the result of subtracting two infinities.  It should give
 *    the same NaN as ieee_nan() on Intel chips.
 *
 * \return
 *    Returns the value of "inf - inf".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

float
ieee_nan_3 ()
{
   float inf = ieee_infinite();
   float result;
   result = inf - inf;
   return result;
}

/******************************************************************************
 * ieee_infinite
 *------------------------------------------------------------------------*//**
 *
 *    Provides the IEEE value representing "infinity".
 *
 *    This is the IEEE version adapted from the "Comparing Floats" paper.
 *    It returns the result of "1/0".
 *
 * \return
 *    Returns the value of "1.0 / 0.0".
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

float
ieee_infinite ()
{
   float zero1 = 0.0f;
   float result;
   result = 1.0f / zero1;
   return result;
}

/******************************************************************************
 *
 *       cbool_t xpc_is_eq (double a, double b, double e)
 *       {
 *          cbool_t result;
 *          if ((a == 0.0) && (abs(b) < e))
 *             result = true;
 *          else if ((b == 0.0) && (abs(a) < e))
 *             result = true;
 *          else
 *             result = (abs((a-b)) < (e * abs(a)));
 *          return result;
 *       }
 *
 *//*-------------------------------------------------------------------------*/

/******************************************************************************
 * xpc_is_neq()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers to see if they differ enough to be
 *    considered unequal.
 *
 *    The usual epsilon is 1e-6.  The xpc_is_neq_epsilon() function is called
 *    with this value, which is provided by the macro NAN_INF_EPSILON.
 *
 *    See the arguments made in http:/.ctips.hyperformix.com/cpptips/fp_eq.
 *
 * \param a
 *    The first number in the inequality comparison.
 *
 * \param b
 *    The second number in the inequality comparison.
 *
 * \todo
 *    Update to handle infinite and minus-infinite values.
 *
 * \return
 *    Returns 'true' if the numbers differ sufficiently.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_neq (double a, double b)
{
   return xpc_is_neq_epsilon(a, b, NAN_INF_EPSILON);
}

/******************************************************************************
 * xpc_is_neq_epsilon()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers to see if they differ by the provided
 *    epsilon value or more.
 *
 *    This function check the absolute values against zero.  If necessary,
 *    it then checked for NaN using the xpc_is_nan() function.  If these tests
 *    fail, then the difference is compared to the epsilon value.
 *
 * \param a
 *    The first number in the inequality comparison.
 *
 * \param b
 *    The second number in the inequality comparison.
 *
 * \param e
 *    The epsilon value used by the comparison.
 *
 * \return
 *    Returns 'true' if the values differ sufficiently.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_neq_epsilon (double a, double b, double e)
{
   cbool_t result;
   if ((a == 0.0) && (fabs(b) >= e))
      result = true;
   else if ((b == 0.0) && (fabs(a) >= e))
      result = true;
   else if (xpc_is_nan(a) && ! xpc_is_nan(b))
      result = true;
   else if (xpc_is_nan(b) && ! xpc_is_nan(a))
      result = true;
   else
   {
      double d = fabs(a-b);
      result = d > e;
   }
   return result;
}

/******************************************************************************
 * xpc_is_eq()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers to see if they are close enough to be
 *    considered equal.
 *
 *    The usual epsilon is 1e-6.  The xpc_is_neq_epsilon() function is called
 *    with this value, which is provided by the macro NAN_INF_EPSILON.
 *
 * \param a
 *    The first number in the equality comparison.
 *
 * \param b
 *    The second number in the equality comparison.
 *
 * \return
 *    Returns 'true' if the first two parameters differ by less than
 *    NAN_INF_EPSILON.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_eq (double a, double b)
{
   return ! xpc_is_neq(a, b);
}

/******************************************************************************
 * xpc_is_eq_epsilon()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers to see if they differ by less than a
 *    value provided in the parameter list.
 *
 *    This function is called by xpc_is_eq() with a hardwired epsilon value.
 *    In turn, this function uses xpc_is_neq_epsilon() for the sake of
 *    maintenance ease.
 *
 * \param a
 *    The first number in the equality comparison.
 *
 * \param b
 *    The second number in the equality comparison.
 *
 * \param e
 *    The epsilon value used by the comparison.
 *
 * \return
 *    Returns 'true' if the first two parameters differ by less than the third
 *    parameter.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_eq_epsilon (double a, double b, double e)
{
   return ! xpc_is_neq_epsilon(a, b, e);
}

/******************************************************************************
 * xpc_is_lt_or_eq()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers like a "<=" operator.
 *
 *    The xpc_is_eq() function is called first, and then the C "<" operator.
 *    This order is important to avoid eliminating the use of the private
 *    epsilon value.
 *
 * \todo
 *    We probably want to re-order to avoid attempting to compare NANs.
 *
 * \param a
 *    The first number in the less-than/equality comparison.
 *
 * \param b
 *    The second number in the less-than/equality comparison.
 *
 * \return
 *    If the two parameters are within NAN_INF_EPSILON of each other, 'true'
 *    is returned.  If not, but the first parameter is less than the second,
 *    'true' is again returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_lt_or_eq (double a, double b)
{
   return xpc_is_eq(a, b) || (a < b);
}

/******************************************************************************
 * xpc_is_lt_or_eq_epsilon()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers like a "<=" operator, using the
 *    provided epsilon value.
 *
 *    The xpc_is_eq_epsilon() function is called first, and then the C "<"
 *    operator.  This order is important to avoid eliminating the use of the
 *    epsilon value.
 *
 * \param a
 *    The first number in the less-than/equality comparison.
 *
 * \param b
 *    The second number in the less-than/equality comparison.
 *
 * \param e
 *    The epsilon value used by the comparison.
 *
 * \return
 *    If the two parameters are within the provided epsilon of each other,
 *    'true' is returned.  If not, but the first parameter is less than the
 *    second, 'true' is again returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_lt_or_eq_epsilon (double a, double b, double e)
{
   return xpc_is_eq_epsilon(a, b, e) || (a < b);
}

/******************************************************************************
 * xpc_is_gt_or_eq()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers like a ">=" operator.
 *
 *    The xpc_is_eq() function is called first, and then the C ">" operator.
 *    This order is important to avoid eliminating the use of the private
 *    epsilon value.
 *
 * \todo
 *    We probable want re-order the checks to avoid attempting to compare
 *    NANs.  We need a consistent strategy for comparing NANs.
 *
 * \param a
 *    The first number in the greater-than/equality comparison.
 *
 * \param b
 *    The second number in the greater-than/equality comparison.
 *
 * \return
 *    Returns true if the first parameter is greater than or equal to the
 *    second parameter.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_gt_or_eq (double a, double b)
{
   return xpc_is_eq(a, b) || (a > b);
}

/******************************************************************************
 * xpc_is_gt_or_eq_epsilon()
 *------------------------------------------------------------------------*//**
 *
 *    Tests two floating point numbers like a ">=" operator, using the
 *    provided epsilon value.
 *
 *    The xpc_is_eq_epsilon() function is called first, and then the C ">"
 *    operator.  This order is important to avoid eliminating the use of the
 *    epsilon value.
 *
 * \param a
 *    The first number in the greater-than/equality comparison.
 *
 * \param b
 *    The second number in the greater-than/equality comparison.
 *
 * \param e
 *    The epsilon value used by the comparison.
 *
 * \return
 *    If the two parameters are within NAN_INF_EPSILON of each other, 'true'
 *    is returned.  If not, but the first parameter is greater than the
 *    second, 'true' is again returned.  Otherwise, 'false' is returned.
 *
 * \unittests
 *    -	numeric_test_01_02()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_gt_or_eq_epsilon (double a, double b, double e)
{
   return xpc_is_eq_epsilon(a, b, e) || (a > b);
}

/******************************************************************************
 * numerics.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
