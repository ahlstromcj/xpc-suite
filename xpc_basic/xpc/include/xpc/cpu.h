#ifndef XPC_CPU_H
#define XPC_CPU_H

/******************************************************************************
 * cpu.h
 *------------------------------------------------------------------------*//**
 *
 * \file          cpu.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       03/03/2008-06/22/2008
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides CPU-specific macros and enumeration functions.
 *
 *    This header file provides the endian value (big/little) that is in
 *    force for this build of the XPC library.
 *
 *    The m4/xpc_endian.m4 file provides an AC_CC_ENDIAN m4 macro that is
 *    used in configure.ac to detect the availability of certain header
 *    files or "target_cpu" values.  The result is that the following
 *    C macros may be set up:
 *
 *       -  HAVE_ENDIAN_H
 *       -  HAVE_SYS_ISA_DEFS_H
 *       -  __BYTE_ORDER
 *       -  __BYTE_ALIGNMENT
 *
 *    If these values are not set up, then they are finalized here.
 *
 * \note
 *    -  little-endian.  Intel and VAX machines store numeric values in
 *       least significant byte first order. This ordering of bytes is known
 *       as little-endian because the data is represented
 *       "little-end-first."
 *    -  big-endian (network byte order).  Most UNIX workstation store
 *       numbers with the most significant byte first.
 *
 * \bit64
 *    For a discussion of 64-bit issues, see the file 64-bits.txt in the doc
 *    directory of this project.
 *
 *    Also see integers.h, cpu.h, cpu.c, os.h, and os.c for more
 *    information.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* cbool_t and macro definitions       */
XPC_REVISION_DECL(cpu)                 /* show_xpc_cpu_info()                 */

#if XPC_HAVE_ENDIAN_H                  /* XPC_HAVE_ENDIAN_H                   */
#include <endian.h>
#else                                  /* ! XPC_HAVE_ENDIAN_H                 */
#ifdef XPC_HAVE_SYS_ISA_DEFS_H         /* XPC_HAVE_SYS_ISA_DEFS_H             */
#include <sys/isa_defs.h>
#ifdef _LITTLE_ENDIAN
#define __BYTE_ORDER          1234
#else
#define __BYTE_ORDER          4321
#endif
#if _ALIGNMENT_REQUIRED > 0
#define __BYTE_ALIGNMENT _MAX_ALIGNMENT
#else
#define __BYTE_ALIGNMENT         1
#endif
#endif                                 /* end of XPC_HAVE_SYS_ISA_DEFS_H      */
#endif                                 /* end of XPC_HAVE_ENDIAN_H            */

#ifndef __BYTE_ALIGNMENT
#if defined(SPARC) || defined(sparc)
#if defined(__arch64__) || defined(__sparcv9)
#define __BYTE_ALIGNMENT         8
#else
#define __BYTE_ALIGNMENT         4
#endif
#endif
#endif

/******************************************************************************
 * Doxygen
 *-----------------------------------------------------------------------------
 *
 *    Turns off all of the endian definitions.
 *
 * \doxygen
 *    This module must turn off all of the definitions in order to allow the
 *    documentation to be generated.
 *
 *----------------------------------------------------------------------------*/

#ifdef DOXYGEN                         /* DOXYGEN                             */
#undef __BYTE_ORDER
#undef __BYTE_ALIGNMENT
#undef __LITTLE_ENDIAN
#undef __BIG_ENDIAN
#undef __PDP_ENDIAN
#undef __FLOAT_WORD_ORDER
#endif

/******************************************************************************
 * __BYTE_ALIGNMENT
 *------------------------------------------------------------------------*//**
 *
 *    Defines the byte-alignment requirements of the host.
 *
 *    64-bit architectures generally require a byte-alignment of 8.  32-bit
 *    architectures generally require a byte-alignment of 4.
 *
 *    Some CPUs have no byte-alignment requirements.  However, certain
 *    instructions may operate more rapidly if the operands are aligned on a
 *    32-bit or 64-bit boundary.
 *
 *    A reasonable discussion appears here:
 *
 *       http://en.wikipedia.org/wiki/Data_alignment
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __BYTE_ALIGNMENT
#define __BYTE_ALIGNMENT         1
#endif

/******************************************************************************
 * XPC_BIG_ENDIAN and XPC_LITTLE_ENDIAN
 *-----------------------------------------------------------------------------
 *
 * \undocumented
 *    These values indicate the endian of the system.  They first must be
 *    undefined for the robustness of the checks.
 *
 *----------------------------------------------------------------------------*/

#undef XPC_IS_BIG_ENDIAN
#undef XPC_IS_LITTLE_ENDIAN

/******************************************************************************
 * __LITTLE_ENDIAN
 *------------------------------------------------------------------------*//**
 *
 *    Represents the order of bytes in a little-endian system.
 *
 *    The XPC_BIG_ENDIAN and XPC_LITTLE_ENDIAN values indicate the endian of
 *    the system.  We cannot use symbols such as __BIG_ENDIAN or BIG_ENDIAN,
 *    because these are already defined in '/usr/include/endian.h'.  If they
 *    exist, we use them.  Otherwise, we try to divine them from other
 *    features.
 *
 *    If nothing matches, they are left undefined.
 *
 *    The __LITTLE_ENDIAN macro indicates the expected order of bytes as
 *    shown by the macro itself.
 *
 *    Also see the xpc_is_big_endian() and xpc_is_little_endian() functions
 *    for run-time tests of endianness.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN    1234
#endif

/******************************************************************************
 * __BIG_ENDIAN
 *------------------------------------------------------------------------*//**
 *
 *    Represents the order of bytes in a big-endian system.
 *
 *    The __BIG_ENDIAN macro indicates the expected order of bytes as
 *    shown by the macro itself.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN       4321
#endif

/******************************************************************************
 * __PDP_ENDIAN
 *------------------------------------------------------------------------*//**
 *
 *    Represents the order of bytes in a PDD-endian system (rare).
 *
 *    The __PDP_ENDIAN macro indicates the expected order of bytes as
 *    shown by the macro itself.
 *
 *    No architecture implements this type of endianness, so it is not used
 *    in the present library.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __PDP_ENDIAN
#define __PDP_ENDIAN       3412
#endif

#ifndef __BYTE_ORDER                         /* __BYTE_ORDER not defined      */

#ifdef DOXYGEN                               /* DOXYGEN                       */
#define __BYTE_ORDER       __LITTLE_ENDIAN
#else                                        /* not DOXYGEN                   */
#if defined WIN32 || defined i386 || defined __i386__ || defined _M_IX86      \
    || defined vax || defined alpha || defined __alpha || defined __CYGWIN__
#define __BYTE_ORDER       __LITTLE_ENDIAN
#else
#define __BYTE_ORDER       __BIG_ENDIAN
#endif
#endif                                       /* end of DOXYGEN                */

#endif                                       /* end of __BYTE_ORDER           */

#if __BYTE_ORDER == __LITTLE_ENDIAN          /* is little-endian or Doxygen   */

/******************************************************************************
 * XPC_IS_BIG_ENDIAN
 *------------------------------------------------------------------------*//**
 *
 *    True if the host uses big-endian order.
 *
 *    This macro is provided to improve the readability of the code.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_IS_BIG_ENDIAN           0

/******************************************************************************
 * XPC_IS_LITTLE_ENDIAN
 *------------------------------------------------------------------------*//**
 *
 *    True if the host uses big-endian order.
 *
 *    This macro is provided to improve the readability of the code.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_IS_LITTLE_ENDIAN        1

#else                                        /* is big-endian                 */

#define XPC_IS_BIG_ENDIAN           1
#define XPC_IS_LITTLE_ENDIAN        0

#endif

/******************************************************************************
 * __FLOAT_WORD_ORDER
 *------------------------------------------------------------------------*//**
 *
 *    Represents the order of bytes in a floating point number.
 *
 *    Not yet sure if this is the complete accurate documentation for this
 *    macro.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __FLOAT_WORD_ORDER
#define __FLOAT_WORD_ORDER __BYTE_ORDER
#endif

/******************************************************************************
 * External functions
 *-----------------------------------------------------------------------------
 *
 *    Provided by the C module.
 *
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern cbool_t xpc_is_big_endian (void);
extern cbool_t xpc_is_little_endian (void);
extern const char * xpc_cpu_model (void);
extern cbool_t xpc_is_16_bit (void);
extern cbool_t xpc_is_32_bit (void);
extern cbool_t xpc_is_lp32 (void);
extern cbool_t xpc_is_ilp32 (void);
extern cbool_t xpc_is_64_bit (void);
extern cbool_t xpc_is_lp64 (void);
extern cbool_t xpc_is_ilp64 (void);
extern cbool_t xpc_is_llp64 (void);

EXTERN_C_END

#endif                                       /* XPC_CPU_H                     */

/******************************************************************************
 * cpu.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
