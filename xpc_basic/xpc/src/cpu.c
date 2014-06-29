/******************************************************************************
 * cpu.c
 *------------------------------------------------------------------------*//**
 *
 * \file          cpu.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2008-05-03 to 2010-04-02
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides macros and functions to manage some of the differences
 *    in C support between various CPUs.
 *
 *    This module provides CPU-specific macros and enumeration functions.
 *
 *    These items are rudimentary.  For a more thorough library/API, see the
 *    following project, available for various UNIX implementations:
 *
 *       http://gwenole.beauchesne.info/en/projects/cpuinfo
 *
 * \warning
 *    This module actually tests the compiler environment, rather than the
 *    CPU on which the code is executing.
 *
 * \todo
 *    Functions to write:
 *    -  xpc_is_16_bit()
 *    -  xpc_is_32_bit()
 *    -  xpc_path_max()
 *    -  xpc_open_file_max()
 *
 * Functions written:
 *
 *    -  xpc_is_lp32()  [Windows 3.1 and Macintosh]
 *    -  xpc_is_ilp32() [UNIX 32]
 *    -  xpc_is_lp64()  [UNIX 64]
 *    -  xpc_is_llp64() [Win64 a.k.a. Win32 for 64-bit Windows]
 *
 *    Also see the contrib/config.guess script for more ideas.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* C error-logging facility            */
#include <xpc/gettext_support.h>       /* the _() internationalization macro  */
#include <xpc/cpu.h>                   /* CPU-detection items                 */
#include <xpc/integers.h>              /* int16_t, etc.                       */

XPC_REVISION(cpu)

/******************************************************************************
 * xpc_is_big_endian()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a big-endian machine.
 *
 *    This link provides a brief discussion of "endianness" or "byte order":
 *
 *       http://unixpapa.com/incnote/byteorder.html
 *
 *    In brief:
 *
 *       -# Big-Endian systems save the most significant byte first. Sun and
 *          Motorola processors, IBM-370s and PDP-10s are big-endian. JPEG,
 *          Photoshop, Sun raster, and SGI images contains big-endian values.
 *          Network-byte order is big-endian.
 *       -# Little-Endian systems save the least significant byte first. The
 *          entire Intel x86 family, Vaxes, Alphas and PDP-11s are
 *          little-endian. BMP and GIF images contains little-endian values.
 *          FLI and Quicktime are little-endian.
 *       -# Middle-Endian or PDP-Endian systems save the most significant
 *          word first, with each word having the least significant byte
 *          first.  For developers of new software, it is not only perfectly
 *          reasonable, but strongly recommended, to ignore this possiblity.
 *
 *    The C libraries of most systems provide __BYTE_ORDER as a compile-time
 *    way to determine the systems endianness.  Usually there is an endian.h
 *    header to define this value properly for the system.
 *
 *    If not, the following run-time test is available (the link above has
 *    an alternative):
 *
\verbatim
         int am_big_endian()
         {
            long one= 1;
            return !(*((char *)(&one)));
         }
\endverbatim
 *
 *    Here, the value stored is 0x1 == 0 0 0 1, and the test checks if the
 *    first byte is 0.  If so, then the CPU is big-endian.
 *
 *    Here's another link, if you like:
 *
 *       http://www.cs.umass.edu/~verts/cs32/endian.html
 *
 *    It also talks about the endian values used in the storage of a number
 *    of graphics formats.
 *
 * \todo
 *    We have a number of other functions we ought to write.  See the C
 *    module itself for the list.
 *
 * \return
 *    Returns 'true' if the system is big-endian.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_big_endian (void)
{
   long one = 1;
   cbool_t result = *((char *)(&one)) == 0;

#if XPC_IS_BIG_ENDIAN
   if (! result)
      xpc_errprint_func(_("compiler configuration does not match"));
#endif

   return result;
}

/******************************************************************************
 * xpc_is_little_endian()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a little-endian machine.
 *
 *    Uses the xpc_is_big_endian() function to do the work.
 *
 *    An alternate implementation is
 *
\verbatim
         unsigned long singlebit = 1L;
         char * pc = (char *) &singlebit;
         return *pc == 0x01;
\endverbatim
 *
 * \return
 *    Returns 'true' if the system is little-endian.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_little_endian (void)
{
   return ! xpc_is_big_endian();
}

/******************************************************************************
 * xpc_cpu_model()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates which model the CPU is.
 *
 *    This function is not yet written; this note serves as a reminder to
 *    work it out.
 *
 * \posix
 *    TBD.
 *
 * \win32
 *    Under Windows, one can call GetSystemInfo() and
 *    IsProcessorFeaturePresent() to get data on the CPU.  One can test for
 *    SSE, 3DNOW, and so on.  Here's an assembler method on an x86 machine:
 *
 *       http://www.gamedev.net/community/forums/topic.asp?
 *          topic_id=293192&whichpage=1&#1853082
 *
 * \return
 *    Returns the name of the CPU in English.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_cpu_model (void)
{
   return _("CPU model to be determined");
}

/******************************************************************************
 * xpc_is_16_bit()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 16-bit CPU.
 *
 * \return
 *    Returns 'true' if the system is running on a 16-bit CPU.
 *
 * \todo
 *    Needs to be written, returns false for now (grin), and so is not
 *    exposed.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_16_bit (void)
{
   return false;
}

/******************************************************************************
 * xpc_is_32_bit()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 32-bit CPU.
 *
 *    It doesn't really test the CPU per se, just the compiler model, but it
 *    should be enough.
 *
 * \return
 *    Returns 'true' if the system is running on a 32-bit CPU.
 *
 * \todo
 *    Needs to be written, returns true for now (grin), and so is not
 *    exposed.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_32_bit (void)
{
   return xpc_is_lp32() || xpc_is_ilp32();
}

/******************************************************************************
 * xpc_is_lp32()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 16-bit CPU with the LP32 model.
 *
 *    This model is used by Windows 3.1 and older, non-OSX Macintosh systems.
 *
 * \return
 *    Returns 'true' if the system is running on a 32-bit CPU with 32-bit
 *    longs and pointers, and 16-bit integers.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_lp32 (void)
{
   return
   (
      (sizeof(int) == sizeof(int16_t)) &&
      (sizeof(long) == sizeof(int32_t)) &&
      (sizeof(long *) == sizeof(int32_t))
   );
}

/******************************************************************************
 * xpc_is_ilp32()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 32-bit CPU with the ILP32 model.
 *
 *    This model is used by most modern 32-bit UNIX and Linux systems.
 *
 * \return
 *    Returns 'true' if the system is running on a 32-bit CPU with 32-bit
 *    integers, longs, and pointers.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_ilp32 (void)
{
   return
   (
      (sizeof(int) == sizeof(int32_t)) &&
      (sizeof(long) == sizeof(int32_t)) &&
      (sizeof(long *) == sizeof(int32_t))
   );
}

/******************************************************************************
 * xpc_is_64_bit()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 64-bit CPU.
 *
 *    It doesn't really test the CPU per se, just the compiler model, but it
 *    should be enough.
 *
 * \return
 *    Returns 'true' if the system is running on a 64-bit CPU.
 *
 * \todo
 *    Needs to be written, returns true for now (grin), and so is not
 *    exposed.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_64_bit (void)
{
   return xpc_is_lp64() || xpc_is_ilp64() || xpc_is_llp64();
}

/******************************************************************************
 * xpc_is_lp64()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 64-bit CPU with the LP64 model.
 *
 *    This model is used by 64-bit versions of UNIX.
 *
 * \return
 *    Returns 'true' if the system is running on a 64-bit CPU with 32-bit
 *    integers, 64-bit longs, 64-bit long longs (?), and 64-bit pointers.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_lp64 (void)
{
   return
   (
      (sizeof(int) == sizeof(int32_t)) &&
      (sizeof(long) == sizeof(int64_t)) &&
      (sizeof(long *) == sizeof(int64_t))
#ifndef _MSC_VER     // NEED A BETTER CHECK THAN THIS, THOUGH
      &&
      (sizeof(long long) == sizeof(int64_t))
#endif
   );
}

/******************************************************************************
 * xpc_is_ilp64()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 64-bit CPU with the ILP64 model.
 *
 *    This model has been used by some systems, though it is not stated
 *    which at this (old) source:
 *
 *       http://www.unix.org/whitepapers/64bit.html
 *
 * \return
 *    Returns 'true' if the system is running on a 64-bit CPU with 64-bit
 *    integers, 64-bit longs, 64-bit long longs, and 64-bit pointers.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_ilp64 (void)
{
   return
   (
      (sizeof(int) == sizeof(int64_t)) &&
      (sizeof(long) == sizeof(int64_t)) &&
      (sizeof(long *) == sizeof(int64_t))
#ifndef _MSC_VER        // NEED A BETTER CHECK THOUGH
      &&
         (sizeof(long long) == sizeof(int64_t))
#endif
   );
}

/******************************************************************************
 * xpc_is_llp64()
 *------------------------------------------------------------------------*//**
 *
 *    Indicates if the CPU is a 64-bit CPU with the LLP64 model.
 *
 *    This model is used by 64-bit versions of Windows.
 *
 * \return
 *    Returns 'true' if the system is running on a 64-bit CPU with 32-bit
 *    integers, 32-bit longs, 64-bit long longs, and 64-bit pointers.
 *
 * \unittests
 *    -  cpu_os_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_llp64 (void)
{
   return
   (
      (sizeof(int) == sizeof(int32_t)) &&
      (sizeof(long) == sizeof(int32_t)) &&
      (sizeof(long *) == sizeof(int64_t))
#ifndef _MSC_VER     // NEED A BETTER CHECK THAN THIS, THOUGH
      &&
         (sizeof(long long) == sizeof(int64_t))
#endif
   );
}

/******************************************************************************
 * cpu.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
