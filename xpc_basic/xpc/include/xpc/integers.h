#ifndef XPC_INTEGERS_H
#define XPC_INTEGERS_H

/******************************************************************************
 * integers.h
 *------------------------------------------------------------------------*//**
 *
 * \file          integers.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2005-11-26 to 2010-06-09
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides platform-independent integer definitions.
 *
 *    This header file provides a number of integers with their bit-sizes
 *    hard-coded.
 *
 * \win32
 *    For Windows, typedefs insure that they match the POSIX specification.
 *    This type and other types are defined in POSIX header files.  However,
 *    we also need these types to be defined in Win32 code.  The following
 *    types are defined:
 *
\verbatim
         typedef signed char int8_t
         typedef short int16_t
         typedef long int32_t
         typedef unsigned char uint8_t
         typedef u_short uint16_t
         typedef u_long uint32_t (a future 64-bit problem?)
\endverbatim
 *
 *    According to the Microsoft MSDN, Win-64 uses the abstract data model
 *    called LLP64 (or P64). In the LLP64 data model, only pointers expand
 *    to 64 bits; all other basic data types (integer and long) remain 32
 *    bits in length.
 *
 *    In POSIX, these typedefs are defined in /usr/include/stdint.h as being
 *    ISO C99 integer types.
 *
 *    We want to standardize on these types, and avoid similar ones (such as
 *    u_int32_t) from the inet directory.  We were using winsock2.h to
 *    define u_short and u_long, but that now seems ill-advised.
 *
 \verbatim
                                          long
   Model          char  short int   long  long  pointer  platforms

      LP32 2/4/4   8    16    16    32    --    32       Win16, Mac
     ILP32 4/4/4   8    16    32    32    --    32       Win32, Lin32, etc.
     LLP64 4/4/8   8    16    32    32    64    64       Win64
     ILP64 8/8/8   8    16    64    64    --    64       Cray, ETA
      LP64 4/8/8   8    16    32    64    --    64       UNIX64, Lin64
 \endverbatim
 *
 *    LLP64 is a 32-bit model with 64-bit addresses.  It depends on a new,
 *    non-standard data type, long long, and it requires extensive
 *    modification to existing C standards.
 *
 *    ILP64 needs an extra type, _int32, in order to provide a 32-bit data
 *    type.  ILP64 implies frequent source-code changes and non-standard
 *    data types.
 *
 *    LP64 takes the middle road -- char, short, and int remain the same as
 *    ILP32 (the most common today).  Pointers fit into longs instead of
 *    ints.  64-bit safe programs can compile and run on 32 bits without
 *    change.  The data types are natural, in that each scalar type is
 *    larger than the preceding type.
 *
 *    See http://www.unix.org/version2/whatsnew/lp64_wp.html for further
 *    discussion of why UNIX 64-bit systems use the LP64 model.
 *
 *    See http://www.unix.org/whitepapers/64bit.html for a more detailed
 *    discussion of the various models, and more on why UNIX chose LP64.
 *    It also summarize a number of data and pointer types.
 *
 *    See http://blogs.msdn.com/oldnewthing/archive/2005/01/31/363790.aspx
 *    for a brief reason why Windows 64-bit systems use the LLP64 model.
 *    Basically, they didn't want to change the layout of their "persistence
 *    formats" (data structures used to transfer information between
 *    processes).
 *
 *    Also see integers.h, cpu.h, cpu.c, os.h, and os.c for more information.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* basic features provided by OS's     */

#if XPC_HAVE_LIMITS_H
#include <limits.h>                    /* LONG_MAX and UINT_MAX               */
#endif

/******************************************************************************
 * Win32 versus POSIX integer types
 *----------------------------------------------------------------------------*/

#ifdef POSIX                           /* POSIX                               */

#ifndef XPC_STD_INTEGERS

#if XPC_HAVE_STDINT_H

#include <stdint.h>                    /* standard integer types              */

#define XPC_STD_INTEGERS

/* typedef u_int64_t uint64_t; */      /* odd... should appear in stdint.h    */

#else

#error PLEASE add definitions for int types in integers.h

#endif

#endif

#else                                  /* Win32                               */

/******************************************************************************
 * Integer typedefs [Win32]
 *-----------------------------------------------------------------------------
 *
 *    See the comment section above.  All Win32 explanations have to appear
 *    in the POSIX ifdef sections for our Doxygen setup.
 *
 *----------------------------------------------------------------------------*/

#ifdef __MINGW32__

#include <stdint.h>                    /* standard integer types              */
#else
#include <stddef.h>                    /* intptr_t and related types          */

#ifndef XPC_STD_INTEGERS
#define XPC_STD_INTEGERS

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef __int64 quad_t;
typedef unsigned __int64 u_quad_t;

#endif                                 /* XPC_STD_INTEGERS                    */
#endif                                 /* __MINGW32                           */

#endif                                 /* POSIX vs Win32                      */

/******************************************************************************
 * Bits in an integer
 *------------------------------------------------------------------------*//**
 *
 *    Provides a very primitive way to decide the size of integers in the
 *    compiler environment.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_16_BIT_INTS       1

#if UINT_MAX >> 16 == 0

#define XPC_32_BIT_INTS       1
#undef XPC_16_BIT_INTS

#elif UINT_MAX >> 32 == 0

#define XPC_64_BIT_INTS       1
#undef XPC_16_BIT_INTS

#elif UINT_MAX >> 64 == 0

#define XPC_128_BIT_INTS      1

#endif

/******************************************************************************
 * Bits in a long integer
 *------------------------------------------------------------------------*//**
 *
 *    Provides a very primitive way to decide the size of long integers in
 *    the compiler environment.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_16_BIT_LONGS      1

#if UINT_MAX >> 16 == 0

#define XPC_32_BIT_LONGS      1
#undef XPC_16_BIT_INTS

#elif UINT_MAX >> 32 == 0

#define XPC_64_BIT_LONGS      1
#undef XPC_16_BIT_INTS

#elif UINT_MAX >> 64 == 0

#define XPC_128_BIT_LONGS     1

#endif

#endif                                 /* XPC_INTEGERS_H                      */

/******************************************************************************
 * integers.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
