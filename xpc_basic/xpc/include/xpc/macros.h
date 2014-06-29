#ifndef XPC_MACROS_H
#define XPC_MACROS_H

/******************************************************************************
 * macros.h
 *------------------------------------------------------------------------*//**
 *
 * \file          macros.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2005-11-25
 * \updates       2013-07-28
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides macros to support the special features of the XPC library.
 *
 *    This header file defines a number of macros that define how the XPC
 *    library is set up.  The macros in this module do not specify data
 *    types or operating-system features, but features of the XPC library.
 *
 *    The most important features are the EXTERN_C macros and the versioning
 *    macros.
 *
 *    However, note that libintl.h is included here.  See the notes on that
 *    file below.
 *
 * \macro
 *    The following are macro functions define here and in other XPC header
 *    files.  This list includes only the lower-case functional macros,
 *    which by convention are macros the act only like functions.  These
 *    macros also appear in contrib/c.vim, so that they can be color coded
 *    for those lucky vim users.
 *
\verbatim
         is_invalid_handle(x)             macros.h
         xpc_return(ok)                   macros.h
         bool_to_code(ok)                 macros.h *
         code_to_booL(code)               macros.h *
         not_nullptr(p)                   macros.h
         not_nullptr_2(p, q)              macros.h
         not_nullptr_3(p, q, r)           macros.h
         not_nullptr_4(p, q, r, s)        macros.h
         is_nullptr(p)                    macros.h
         is_nullptr_2(p, q)               macros.h
         is_nullptr_3(p, q, r)            macros.h
         is_nullptr_4(p, q, r, s)         macros.h
         not_nullfunc(p)                  macros.h
         is_nullfunc(p)                   macros.h
         xpc_recast(a, b)                 macros.h
         xpc_statcast(a, b)               macros.h
         is_posix_success(x)              os.h
         not_posix_error(x)               os.h
         is_posix_error(x)                os.h
         not_posix_success(x)             os.h
         is_valid_socket(s)               socket_t.h
         not_valid_socket(s)              socket_t.h
         xpc_closesocket(s)               socket_t.h
         is_valid_thread(x)               xthread.h
         is_invalid_thread(x)             xthread.h
\endverbatim
 *
 * \note
 *    To determine all of the predefined macros in GCC, use the following
 *    commands:  "touch foo.h ; cpp -dM foo.h"
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/os.h>                    /* includes 'config.h' & POSIX macro   */

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    Since this module has Win32-specific constructs included in it, and
 *    we've set Doxyfile to pre-define only POSIX, Doxygen cannot generate
 *    some of the documentation in this file unless we artificially define
 *    WIN32 while Doxygen is processing this file.
 *
 *    Also, this section defines or undefines macros in a way that covers
 *    the features of all compilers (well, only two so far).
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define WIN32                          /* define WIN32 for documentation      */
#undef __inline__                      /* A GNU keyword, but not a VC one     */
#else
#ifdef WIN32
#ifdef POSIX
#error WIN32 and POSIX are defined simultaneously.  This is very bad.
#endif
#endif
#endif

/******************************************************************************
 * Win32 header chaff
 *------------------------------------------------------------------------*//**
 *
 *    Although the library code compiles fine, the waveoutbuffering
 *    application (in another library's test directory) compiles with an
 *    error about TryEnterCriticalSection unless windows.h is included above
 *    the namespace directive, even though it is already included below,
 *    inside the namespace.
 *
 *    Also, Visual Studio .NET 2002 (at least) gripes about redefining some
 *    Winsock constructs unless the winsock2.h header is included before
 *    windows.h.
 *
 *    Finally, we have to force the user to use a version of Windows that is
 *    NT 4.0 or later.  We will not bother supporting older versions.  If
 *    you do so yourself, please share your fixups!
 *
 *    And people wonder why some programmers gripe about Microsoft code!
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32                           /* see the Win32 note above            */

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0400         /* requires Win NT 4.0+ features       */
#endif

#include <xpc/xwinsock.h>              /* fixes winsock2.h/windows.h ordering */

#endif

/******************************************************************************
 * Headers:          Common headers
 *------------------------------------------------------------------------*//**
 *
 *    Although we prefer to include header files only where needed, the
 *    following header are so universally used that it makes sense to
 *    include them here.
 *
 *    Note that xpc-config.h includes all of the headers, but we want to
 *    avoid using it, except in one or two places.
 *
 *//*-------------------------------------------------------------------------*/

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if XPC_HAVE_STRING_H
#include <string.h>
#endif

/******************************************************************************
 * XPCUT_VERSION() macro
 *------------------------------------------------------------------------*//**
 *
 *    A simple macro for a creation-date string.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_VERSION_DATE(x)      #x " (created " __DATE__ " " __TIME__ ")"

/******************************************************************************
 * XPCUT_VERSION_STRING() macro
 *------------------------------------------------------------------------*//**
 *
 *    A macro to make the tests have a more consistent format without a lot
 *    of extra work for the developer.
 *
 * \usage
 *    XPC_VERSION_STRING(1.4) ----> "1.4 date time"
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_VERSION_STRING(x)    (char *) (XPC_VERSION_DATE(x))

/******************************************************************************
 * cbool_t:  Fake boolean support
 *------------------------------------------------------------------------*//**
 *
 *    Provides a unique typedef for boolean values.
 *
 *    Please note that we do not want to use typedefs and enum for this
 *    work, we just want simple macro substitution.  But, sometimes the
 *    language forces our hand.
 *
 * \warning
 *    We had \e bool defined here, for C only.  However, it is very easy to
 *    have a different size than \e sizeof(int) for \e bool in C++, which
 *    makes it very problematic to use structures containing \e bool fields.
 *    In fact, we found such a problem in the debugger.  Therefore, the XPC
 *    library now uses a new typedef called \e cbool_t, to be used for
 *    declarations of booleans in C structures.
 *
 *    For function arguments, is int-vs-bool an issue?  Section 4.2 of
 *    \e Stroustrup indicates that the C++ 'bool' is converted to 'int' in
 *    arithmetic and logical expressions.  Presumably, \e bool is also
 *    implicitly converted to \e int when passed in place of an \e int
 *    function argument.
 *
 *    But, if we declare a function to be "void x(int y)" in C, and "void
 *    x(bool y)" in C++, the pushing of data could well be different.  In
 *    fact, we saw behavioral differences between gcc on Linux and Microsoft
 *    C on Windows.  So the \e bool define definitely had to go.
 *
 * \note
 *    We've added \e cbool_t and other items to our upgraded \e c.vim add-on
 *    in the \e contrib directory.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef def_cbool_t
typedef int cbool_t;
#define def_cbool_t
#endif

/******************************************************************************
 * ubool_t:  Unsigned boolean support
 *------------------------------------------------------------------------*//**
 *
 *    Provides an unsigned boolean typedef for use in bit fields.
 *
 *    We were getting a compiler warning in gcc-4 about assigning an \e int
 *    to a 1-bit bit-field (xpc_sock_flags_t in sock_codes.h).  This
 *    typedef prevents that warning.
 *
 * \note
 *    We've added \e ubool_t and other items to our \e c.vim add-ons in the
 *    \e contrib directory.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef def_ubool_t
typedef unsigned ubool_t;
#define def_ubool_t
#endif

#ifdef __cplusplus

#define xpc_boolcast(x)          ((x) ? true : false)

#else // __cplusplus

/******************************************************************************
 * xpc_boolcast macro
 *------------------------------------------------------------------------*//**
 *
 *    Converts a cbool_t or int value into a boolean.
 *
 *    This macro is defined to do nothing in C code.  However, for C++ code,
 *    it is meant to eliminate the Visual Studio C4800 "forcing value bool
 *    to 'true' or 'false" performance warning.  Note that it gets rid of
 *    the warning, but not the performance hit.
 *
 * \note
 *    We've added 'false' to our add-on c.vim in the contrib directory.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_boolcast(x)          (x)

/******************************************************************************
 * false macro
 *------------------------------------------------------------------------*//**
 *
 *    false is zero (0).  We are all zeroes.  We are Devo.
 *
 * \note
 *    We've added 'false' to our add-on c.vim in the contrib directory.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef false
#define false         0
#endif

/******************************************************************************
 * true macro
 *------------------------------------------------------------------------*//**
 *
 *    true is one (1).  We are all one.
 *
 * \note
 *    We've added 'true' to our upgraded c.vim in the contrib directory.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef true
#define true          1
#endif

#endif   // __cplusplus versus ! __cplusplus

/******************************************************************************
 * XPC_INFINITE_TIMEOUT
 *------------------------------------------------------------------------*//**
 *
 *    This macro is used where an infinite timeout is desired.
 *
 * \posix
 *    We will define it as an integer of value -1.
 *
 * \win32
 *    windows.h defines INFINITE as a 32-bit number, 0xFFFFFFFF.  We'll
 *    define it as an integer.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
#define XPC_INFINITE_TIMEOUT     ~((int) 0)           /* POSIX    */
#else
#ifdef WIN32
#define XPC_INFINITE_TIMEOUT     ((int) INFINITE)     /* Win32    */
#else
#define XPC_INFINITE_TIMEOUT     (-1)                 /* Other    */
#endif
#endif

/******************************************************************************
 * strcasecmp()
 *------------------------------------------------------------------------*//**
 *
 *    Additional macros to fill in some Windows differences.
 *
 *    The only one present so far is strcasecmp().
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32
#ifndef strcasecmp
#define strcasecmp      stricmp
#endif
#endif

/******************************************************************************
 * xpc_lseek_offset_t
 *------------------------------------------------------------------------*//**
 *
 *    Provides a cross-platform type for the offset parameter of the lseek()
 *    function.
 *
 *    This typedef resolves the slight differences in the type of the second
 *    parameter of the lseek() function.
 *
 * \posix
 *    The signature for lseek() is lseek(int, off_t, int).
 *
 * \win32
 *    The signature for lseek() is lseek(int, long, int).
 *
 *//*-------------------------------------------------------------------------*/

#ifdef POSIX
typedef off_t xpc_lseek_offset_t;
#else
typedef long xpc_lseek_offset_t;
#endif

/******************************************************************************
 * XPC_NO_DLL
 *------------------------------------------------------------------------*//**
 *
 *    Configuration option.  Uncomment this line if you want to use static
 *    linkage!  Since the XPC library doesn't (at present) officially
 *    support dynamic linkage, this macro is uncommented.  It will probably
 *    be a project option, anyway.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_NO_DLL

/*******************************************************************************
 * libintl.h
 *------------------------------------------------------------------------------
 *
 *    We've been having a serious problem with libintl.h errors in C++ code
 *    that uses the XPC library.  Somehow, the __THROW symbol gets
 *    translated as per C++ (it becomes 'throw'), and this cause the
 *    textdomain() and bindtextdomain() declarations to fail.
 *
 *    This include seems to fix that problem, by guaranteeing the libintl.h
 *    is encountered first.
 *
 *-----------------------------------------------------------------------------*/

#if XPC_HAVE_LIBINTL_H
#include <libintl.h>
#endif

/*******************************************************************************
 * Version information for modules
 *------------------------------------------------------------------------*//**
 *
 *    The version tag depends on the model of source-code control system
 *    being used.  The current value is suitable for rcs and MKS Source
 *    Integrity.  Possible for cvs and other systems, too, not sure at this
 *    time.
 *
 *    Use the XPC_LIB_VERSION macros just once (see version.c), and use the
 *    XPC_REVISION macro at the beginning of each module:
 *
 *          XPC_REVISION(modulename)
 *
 *    The semi-colon is no longer required.  In fact, in C modules, it
 *    generates a needless warning, and in GCC C++ 4.0 and above, it
 *    generates an error.
 *
 *    Although this macro generates a show_modulename_info() function, this
 *    function is private, and cannot be accessed by outside callers unless
 *    the XPC_REVISION_DECL() macros is used in the header file of the
 *    module.  This is done in a few modules only, at present.
 *
 *    Examples of some of the more important macros follow.
 *
 * XPC_REV_ARRAY(module)
 *
\verbatim
      _module_modinfo
\endverbatim
 *
 * XPC_SHOW_REVISION(module)
 *
\verbatim
      if (xpc_showinfo())
         fprintf(xpc_logfile(), "%s %s\n", xpc_infomark(), _module_modinfo)
\endverbatim
 *
 * XPC_SHOWINFO(module)
 *
\verbatim
      show_module_info ()
\endverbatim
 *
 * XPC_REVISION_DECL(module)
 *
\verbatim
      EXTERN_C_DEC
      extern void show_module_info ();
      EXTERN_C_END
\endverbatim
 *
 * XPC_REVISION(module)
 *
\verbatim
      static const char _module_modinfo [] =
         __FILE__ ": " "built on Mon May 15 21:33:57 EDT 2006"

      EXTERN_C_DEC
      void XPC_SHOWINFO(module)
      {
         if (xpc_showinfo())
            fprintf(xpc_logfile(), "%s %s\n",
               xpc_infomark(), _module_modinfo)
      }
      EXTERN_C_END
\endverbatim
 *
 * XPC_LIB_VERSION(libname)
 *
\verbatim
      static const char _libname_version[] =
         __FILE__ "\n"
         "Library version: 0.2.0 built on Mon May 15 21:33:57 EDT 2006";
\endverbatim
 *
 * XPC_VERSION_TAG(libname)
 *
\verbatim
      _libname_version
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_VERSION
#define XPC_VERSION           XPC_VERSION
#endif

#ifndef XPC_PACKAGE
#define XPC_PACKAGE           XPC_PACKAGE
#endif

#define XPC_FULL_REV_INFO     "built on "  __DATE__ " " __TIME__

#define XPC_REV_ARRAY(module) _ ## module ## _modinfo

#define XPC_SHOW_REVISION(module)   \
   fprintf(xpc_logfile(), "%s\n", XPC_REV_ARRAY(module))

#define XPC_SHOW_WITH_LIB_VERSION(module)   \
   fprintf(xpc_logfile(), "%s, version %s\n", XPC_REV_ARRAY(module), XPC_VERSION)

#define XPC_SHOW_VERSION(module, v) \
   fprintf(xpc_logfile(), "%s, %s\n", XPC_REV_ARRAY(module), v)

#define XPC_SHOWINFO(module) show_ ## module ## _info ()

#define XPC_REVISION_DECL(module)   \
EXTERN_C_DEC                        \
extern void XPC_SHOWINFO(module);   \
EXTERN_C_END

#define XPC_REVISION(module)                 \
static const char XPC_REV_ARRAY(module) [] = \
   __FILE__ ": " XPC_FULL_REV_INFO;          \
void XPC_SHOWINFO(module)                    \
   { XPC_SHOW_REVISION(module); }

#define XPC_FULL_VERSION_INFO                \
   "Library version: " XPC_VERSION " built on " __DATE__ " " __TIME__

#define XPC_VERSION_TAG(libname)    _ ## libname ## _version
#define XPC_LIB_VERSION(libname)                 \
static const char XPC_VERSION_TAG(libname) [] =    \
   __FILE__ "\n" XPC_FULL_VERSION_INFO ;

/******************************************************************************
 * XPC_INVALID_HANDLE
 *------------------------------------------------------------------------*//**
 *
 *    Provides a cross-platform value for invalid or uninitialized handles.
 *
 *    This value is kind of a sop to Win32.  Compiling for Win32 defines it
 *    as "INVALID_HANDLE_VALUE".
 *
 *//*-------------------------------------------------------------------------*/

#ifndef XPC_INVALID_HANDLE
#ifdef POSIX
#define XPC_INVALID_HANDLE       ((unsigned long int)(-1))
#else
#define XPC_INVALID_HANDLE       INVALID_HANDLE_VALUE
#endif
#endif

/******************************************************************************
 * is_valid_handle()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a cross-platform check for valid or initialized handles.
 *
 *    This macro is kind of a sop to Win32.
 *
 *//*-------------------------------------------------------------------------*/

#define is_valid_handle(x)     ((x) != XPC_INVALID_HANDLE)

/******************************************************************************
 * is_invalid_handle()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a cross-platform check for invalid or uninitialized handles.
 *
 *    This macro is kind of a sop to Win32.
 *
 *//*-------------------------------------------------------------------------*/

#define is_invalid_handle(x)     ((x) == XPC_INVALID_HANDLE)

/******************************************************************************
 * pure macro
 *------------------------------------------------------------------------*//**
 *
 *    Provides a fake C++ key word.
 *
 *    Although this may be a little shaky, we're defining this pseudo-
 *    keyword in order to make it easier to see pure virtual functions.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef pure
#define pure        = 0
#endif

/******************************************************************************
 * xpc_return() macro
 *------------------------------------------------------------------------*//**
 *
 *    Converts a boolean value to the POSIX success/error code numbers.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_return(ok)           ((ok) ? POSIX_SUCCESS : POSIX_ERROR)

/******************************************************************************
 * xpc_true_error() macro
 *------------------------------------------------------------------------*//**
 *
 *    Tests for a true error, one that fits most POSIX functions' notions of
 *    an error code.
 *
 *    Returns true if the code is not equal to POSIX_SUCCESS, and errno is
 *    not zero (0).
 *
 *    Some POSIX functions can return legal non-zero values, forcing the
 *    caller to check 'errno'.  Probably not too useful a macro; it is
 *    easier to just make that rare check explicit than to remember the
 *    existence of this macro.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_true_error(code)     (((code) != POSIX_SUCCESS) && (errno != 0))

/******************************************************************************
 * Future support:   nullptr and other nullities
 *------------------------------------------------------------------------*//**
 *
 *    This macro replaces the oft-disparaged \a NULL macro.
 *
 *    According to Sutter, Herb (2004) "The New C++, Much Ado About Nothing:
 *    A (True) Null Pointer Value for C++", C/C++ Users Journal (22.5), pp.
 *    44-47, the new token "nullptr" will replace the usage of 0 or NULL in
 *    null-pointer syntax.   Some compilers apparently already support it.
 *
 *    For now, we define it as a macro in order to make it easier to build
 *    the code under new compilers (even though 0 will still be a valid
 *    pointer value).  Once C++ supports it, we'll change the lines to:
 *
 *       # ifndef __cplusplus    <br>
 *       # define nullptr 0      <br>
 *       # endif                 <br>
 *
 *    This will let us use this symbol in C code, too.
 *
 *    POSIX and Win32 support other nullities and invalid values.  Win32, in
 *    particular, is a real mess of data types.
 *
 * <b>Handle (Win32)</b>:
 *
 *    <tt>HANDLE</tt>.
 *       This item is typedef'd as a void pointer in Windows NT.  (HWND is
 *       the same).  Yet, for example, CreateFile() returns an
 *       INVALID_HANDLE_VALUE (-1), /not/ a null.
 *
 *    <tt>SOCKET</tt>.
 *       This item is called a handle, but typedef'd as an unsigned integer
 *       (UINT_PTR, which is /not/ a pointer).  Contrary to
 *       INVALID_HANDLE_VALUE, the error value for a socket handle is
 *       xpc_invalid_socket (~0).
 *
 *    <tt>intptr_t</tt>.
 *       This standard type can store a pointer or a HANDLE on both Win32
 *       and Win64 platforms.
 *
 * <b>File pointer</b>:
 *
 *    In POSIX, a 'file pointer' is a pointer, and is returned by functions
 *    such as fopen().  An invalid file pointer is 0 (nullptr).  However,
 *    the code check \a splint will complain when comparing a FILE *
 *    variable to 'nullptr', which is treated as an integer.
 *
 *    In Win32, a file pointer is the same as in POSIX.
 *
 *    In Win32 and Win64, a pointer can also be held in an intptr_t value.
 *
 * <b>Descriptor</b>:
 *
 *    In POSIX, a 'descriptor' is an integer, and is returned by file
 *    functions such as open() or socket().  An invalid descriptor value is
 *    -1.  0 is a valid descriptor, as are other descriptor values.
 *
 *    In Win32, a descriptor is the same as in POSIX.
 *
 * <b>Error value</b>:
 *
 *    In POSIX, many functions return 0 if they succeed, and -1 if they
 *    fail.
 *
 *    Also in POSIX, many functions return -1 if the fail, and a valid
 *    descriptor if they succeed.
 *
 * <b>Thread type</b>:
 *
 *    In GNU/POSIX, a pthread_t is an unsigned long integer.  In Win32, it
 *    is a handle.
 *
 * \todo
 *    -  Replace ERRL_INVALID_FILE_HANDLE with 'nulldescriptor' (I think).
 *       See the errorlogging.h module.
 *    -  Check out the xpc_invalid_socket value.  Consider creating a
 *       'invaliddescriptor' value, if nulldescriptor isn't right.
 *
 *    Also see this URL for a definition of a nullptr class:
 *
 * http://en.wikibooks.org/wiki/More_C++_Idioms/nullptr#Solution_and_Sample_Code
 *
 *//*-------------------------------------------------------------------------*/

#ifndef nullptr                     /* nullptr define starts                  */

#ifdef __cplusplus                  /* C++                                    */

#if __cplusplus < 201103L
#define nullptr            0        /* for fopen() and generic pointers       */
#endif

#else                               /* C                                      */

#define nullptr   ((void *) 0)      /* try this on for C size                 */
#endif                              /* C/C++                                  */

#endif                              /* nullptr define ends                    */

/******************************************************************************
 * NULLptr()
 *-----------------------------------------------------------------------------
 *
 *    This macro is equated to NULL.
 *
 *    Why do we want such a thing?  Because NULL is used in so many ways
 *    that we need some symbol of our own that is
 *
 *    -# Used in a more strict manner.
 *    -# Easier to search amidst a large collection of (other project's)
 *       macros.
 *
 *    We're only going to use the NULLptr macro where functions may handle
 *    NULL pointer parameters or return NULL pointer values.  All other uses
 *    will involve nullptr.  A good example is the return value of fopen(),
 *    which can be a NULL.  The return value is <tt>FILE *</tt>, and the man
 *    page specifies the possibility of a NULL return value.
 *
 *    As function calls get nested, it may be that a value that was set to
 *    NULLptr will actually be checked in a less nested function using
 *    the nullptr macro.  And, when our projects return null pointers, they
 *    will always be called nullptr.  We will never set a value to NULLptr.
 *
 * \warning
 *    Just about everybody defines NULL.  Example:
 *    <tt>/usr/include/gif_lib.h</tt>.
 *
 *----------------------------------------------------------------------------*/

#define NULLptr                     NULL  /* a more searchable NULL           */

/******************************************************************************
 * is_NULL()
 *-----------------------------------------------------------------------------
 *
 *    This macro is analogous to is_nullptr().
 *
 *    This macro checks the argument against "NULLptr", which is an XPC
 *    synonym for NULL.
 *
 *    This macro is a little easier to search for than NULL, which just
 *    about \e everyone seem to redefine.
 *
 *----------------------------------------------------------------------------*/

#define is_NULL(x)               ((x) == NULLptr)

/******************************************************************************
 * not_NULL()
 *-----------------------------------------------------------------------------
 *
 *    This macro is analogous to not_nullptr().
 *
 *    This macro checks the argument against "NULLptr", which is an XPC
 *    synonym for NULL.
 *
 *    This macro is a little easier to search for than NULL, which just
 *    about \e everyone seem to redefine.
 *
 *----------------------------------------------------------------------------*/

#define not_NULL(x)              ((x) != NULLptr)

/******************************************************************************
 * set_nullptr()
 *-----------------------------------------------------------------------------
 *
 *    This macro sets a variable to a null pointer (see the nullptr macro).
 *
 *    The macro parameter must be an lvalue.  Unfortunately, it also looks
 *    ugly in the declaration+initialization of a pointer, even if the
 *    compiler accepts it.
 *
 *----------------------------------------------------------------------------*/

#define set_nullptr(p)           ((p) = nullptr)

/******************************************************************************
 * XPC_USE_NO_ERRL
 *-----------------------------------------------------------------------------
 *
 *    This macro allows some error-checking to be disabled via the
 *    "--enable-noerrorlogging" option of the "configure" script.
 *
 * \todo
 *    The disabling of the error-logging and error-checking in the
 *    not_nullptr() and xpc_errprint() family of macros and functions still
 *    needs to be thoroughly tested and measured for its effects on speed.
 *
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * test_nullptr() etc.
 *------------------------------------------------------------------------*//**
 *
 *    Performs a test for a nullptr.
 *
 *    Unlike the not_nullptr() macro defined below, this macro is always in
 *    force, and never shows any message if the pointer is null.  Use
 *    test_nullptr() when a null return value is a legitimate notification
 *    that something could not be done.
 *
 *----------------------------------------------------------------------------*/

#define test_nullptr(p)          ((p) != nullptr)

/******************************************************************************
 * XPC_NO_NULLPTR
 *------------------------------------------------------------------------*//**
 *
 *    The XPC_NO_NULLPTR macro is defined if the user supplied the
 *    argument "--disable-nullptr" to the "configure" script.
 *
 * \todo
 *    The disabling of the error-logging and error-checking in the
 *    not_nullptr() and xpc_errprint() family of macros and functions still
 *    needs to be thoroughly tested and measured for its effects on speed.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef XPC_NO_NULLPTR

#error "Null-pointer checking is disabled.  You fool!"

#define not_nullptr(p)              true
#define not_nullptr_2(p, q)         true
#define not_nullptr_3(p, q, r)      true
#define not_nullptr_4(p, q, r, s)   true

#define is_nullptr(p)               false
#define is_nullptr_2(p, q)          false
#define is_nullptr_3(p, q, r)       false
#define is_nullptr_4(p, q, r, s)    false

#else

/******************************************************************************
 * not_nullptr() etc.
 *------------------------------------------------------------------------*//**
 *
 *    Checks a pointer for being valid.
 *
 *    This family of macros make it easier to check for null pointers.
 *
 *    It also allows this checking to be disabled via the
 *    "--enable-noerrorlogging" option of the "configure" script.
 *
 * \usage
 *
\verbatim
      if (not_nullptr(ptr))   or   if (nullptr_2(ptr1, ptr2)) ...
      {                            {
         // do work                    // handle failure
      }                            }
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __cplusplus
#define not_nullptr(p)  \
 (xpc_not_nullptr(reinterpret_cast<const void *>(p), __func__))
#else
#define not_nullptr(p)  (xpc_not_nullptr((p), __func__))
#endif

/******************************************************************************
 * not_nullptr_2()
 *------------------------------------------------------------------------*//**
 *
 *    Checks two pointers for being valid.  They both must be valid.
 *
 *//*-------------------------------------------------------------------------*/

#define not_nullptr_2(p, q)         (not_nullptr(p) && not_nullptr(q))

/******************************************************************************
 * not_nullptr_3()
 *------------------------------------------------------------------------*//**
 *
 *    Checks three pointers for being valid.  The all must be valid.
 *
 *//*-------------------------------------------------------------------------*/

#define not_nullptr_3(p, q, r)      (not_nullptr_2(p,q) && not_nullptr(r))

/******************************************************************************
 * not_nullptr_4()
 *------------------------------------------------------------------------*//**
 *
 *    Checks four pointers for being valid.  They all must be valid.
 *
 *//*-------------------------------------------------------------------------*/

#define not_nullptr_4(p, q, r, s)   (not_nullptr_3(p,q,r) && not_nullptr(s))

/******************************************************************************
 * is_nullptr()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a pointer for being invalid.
 *
 *    Unlike not_nullptr(), this macro cannot emit an error-log message.
 *    So, if you need to check for a null pointer, use this macro, and not
 *    the not_nullptr() macro.
 *
 *    Or, if you are checking the return value of functions that return
 *    NULL, use the is_NULL() or not_NULL() macros.
 *
 *//*-------------------------------------------------------------------------*/

#define is_nullptr(p)               ((p) == nullptr)

/******************************************************************************
 * is_nullptr_2()
 *------------------------------------------------------------------------*//**
 *
 *    Checks two pointers for being invalid.  Returns 'true' if either of
 *    them are invalid.
 *
 *//*-------------------------------------------------------------------------*/

#define is_nullptr_2(p, q)          (is_nullptr(p) || is_nullptr(q))

/******************************************************************************
 * is_nullptr_3()
 *------------------------------------------------------------------------*//**
 *
 *    Checks three pointers for being invalid.  Returns 'true' if any of
 *    them are invalid.
 *
 *//*-------------------------------------------------------------------------*/

#define is_nullptr_3(p, q, r)       (is_nullptr_2(p,q) || is_nullptr(r))

/******************************************************************************
 * is_nullptr_4()
 *------------------------------------------------------------------------*//**
 *
 *    Checks four pointers for being invalid.  Returns 'true' if any of
 *    them are invalid.
 *
 *//*-------------------------------------------------------------------------*/

#define is_nullptr_4(p, q, r, s)    (is_nullptr_3(p,q,r) || is_nullptr(s))

#endif

/******************************************************************************
 * not_null_result()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a pointer for being null.
 *
 *    Do not confuse it with the not_nullptr() macro.  It differs in these
 *    ways:
 *
 *       -# It is not modifiable via a compiler configuration switch.  It is
 *          always defined.
 *       -# It does not emit an error-log message.  This macro should be
 *          used when nullptr (NULL) is a reasonable return value, one that
 *          does not imply an error.
 *
 *    The reason for this macro is merely for consistency of notation with
 *    the not_nullptr() macro.
 *
 *//*-------------------------------------------------------------------------*/

#define not_null_result(p)          ((p) != nullptr)

/******************************************************************************
 * is_null_result()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a pointer for being null.
 *
 *    This macro is identical to the is_nullptr() macro, but should be used
 *    in contexts where nullptr would be a reasonable return value.
 *
 *    Compare it to the not_null_result() macro.
 *
 *//*-------------------------------------------------------------------------*/

#define is_null_result(p)           ((p) == nullptr)

/******************************************************************************
 * xpc_good_pointer()
 *------------------------------------------------------------------------*//**
 *
 *    Checks that a pointer is not null.
 *
 *    Equivalent to not_null_result().  Use whichever version floats your
 *    boat.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_good_pointer(p)         ((p) != nullptr)

/******************************************************************************
 * is_thisptr()
 *------------------------------------------------------------------------*//**
 *
 *    Checks a pointer for being valid, assuming it is like a "this" pointer.
 *
 *    It also allows this checking to be enabled via the
 *    "--enable-thispointer" option of the "configure" script.
 *
 *    This macro should be use in only one restricted case:  where a pointer
 *    to a structure is used, in a function, as if it were a "this" pointer
 *    in C++.  More specifically, use is_thisptr() where the code might be
 *    wrapped in a class at some point, where one can then
 *    assume that the pointer is valid, and the check for a null pointer can
 *    be ignored.  The "--enable-thispointer" configuration option allows
 *    this check to be enabled for debugging.
 *
 * \usage
 *    Use this macro to determine if the pointer is safe to use.  If wrapped
 *    in a class's member function, then define XPC_NO_THISPTR.
 *
\verbatim
      void pfunction (item_t * p)
      {
         if (is_thisptr(p))
         {
            // do work using the pointer p
         }
      }
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#ifndef is_thisptr
#ifdef XPC_NO_THISPTR
#define is_thisptr(p)               true
#else
#define is_thisptr(p)               (xpc_is_thisptr((p), __func__))
#endif
#endif

/******************************************************************************
 * not_thisptr()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the opposite test of is_thisptr().
 *
 *    Unlike is_thisptr(), this macro cannot emit an error-log message.
 *
 *//*-------------------------------------------------------------------------*/

#ifndef not_thisptr
#ifdef XPC_NO_THISPTR
#define not_thisptr(p)              false
#else
#define not_thisptr(p)              ((p) == nullptr)
#endif
#endif

/******************************************************************************
 * not_nullfunc()
 *-----------------------------------------------------------------------------
 *
 *    This macro is analogous to not_nullptr().
 *
 *    This macro checks if the argument is a null function pointer.
 *
 *    This macro is a little easier to search for than NULL, which just
 *    about \e everyone seem to redefine.
 *
 *    More importantly, though, it works around a serious issue, described
 *    by this warning message that occurs when you try to :
 *
 *       <i> ISO C++ forbids casting between pointer-to-function and
 *       pointer-to-object </i>
 *
 *    The error message could hardly be clearer. There is no valid cast
 *    between pointer to function and pointer to object. You see, neither C
 *    nor C++ requires that a function pointer can be stored in a void
 *    pointer and vice versa; depending on the platform there may be no way
 *    to perform the conversion.
 *
 *    We could work around this issue by using:
 *
\verbatim
      if (not_nullptr((void *)(intptr_t) procptr)
\endverbatim
 *
 *    Instead, we just compare the pointer directly to zero (\e nullptr).
 *
 *    We don't have to get as elaborate as we did with the not_nullptr()
 *    macro.  We'll be testing only one function pointer, and we will never
 *    disable this check via conditional.
 *
 * \references
 *    -  http://www.trilithium.com/johan/2004/12/problem-with-dlsym/
 *
 * \usage
 *
\verbatim
      if (not_nullfunc(procptr))
      {
         // do work
      }
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#define not_nullfunc(p)             ((p) != nullptr)

/******************************************************************************
 * is_nullfunc()
 *-----------------------------------------------------------------------------
 *
 *    This macro is the opposite of not_nullfunc().
 *
 * \usage
 *
\verbatim
      if (is_nullfunc(procptr))
      {
         // show error message
      }
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#define is_nullfunc(p)              ((p) == nullptr)

/******************************************************************************
 * Safe member pointer deletion
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to disable and delete pointers.
 *
 *    We want to disable the pointer before we delete it, so that there is
 *    no chance that an object can be used while undergoing deletion.
 *
 *    This macro checks the pointer for nullity, creates a temporary copy of
 *    the pointer, nullifies the pointer, and deletes the object using the
 *    temporary copy.
 *
 *    It is not an error for the pointer to be null, so we use the
 *    not_null_result() test versus the not_nullptr() test.
 *
 * \param datatype
 *    Provides the fully-scoped name of the data type of the object to be
 *    deleted.
 *
 * \param ob
 *    Provides the pointer to the object to be deleted.
 *
 * \note
 *    We could implement this as a template function, but we can also make
 *    it work for C, although the C version doesn't need the data type,
 *    since free() can handle void pointers.
 *
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus                                       /* C++               */

#define safe_ptr_delete(datatype, ob) \
    if (not_null_result(ob))  \
    {                         \
       datatype * temp = ob;  \
       set_nullptr(ob);       \
       delete temp;           \
    }

#else                                                    /* C                 */

#define safe_ptr_delete(datatype, ob) \
    if (not_null_result(ob))  \
    {                         \
       datatype * temp = ob;  \
       set_nullptr(ob);       \
       free(temp);            \
    }

#endif                                                   /* C/C++             */

/******************************************************************************
 * Safe member array pointer deletion
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to disable and delete array pointers.
 *
 *    See safe_ptr_delete() for more discussion.
 *
 * \param datatype
 *    Provides the fully-scoped name of the data type of the object to be
 *    deleted.
 *
 * \param ob
 *    Provides the pointer to the object to be deleted.
 *
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus                                       /* C++               */

#define safe_array_delete(datatype, ob) \
    if (not_nullptr(ob))      \
    {                         \
       datatype * temp = ob;  \
       set_nullptr(ob);       \
       delete [] temp;        \
    }

#else                                                    /* C                 */

#define safe_array_delete(datatype, ob)   safe_ptr_delete(datatype, ob)

#endif                                                   /* C/C++             */

/******************************************************************************
 * C-style cast wrapper macros
 *-----------------------------------------------------------------------------
 *
 *    Provides C/C++ cast operators.
 *
 *    This section unifies standard C casts with C++'s reinterpret_cast<>(),
 *    without compromising the searchability of these awful casts.  The
 *    following macros are defined:
 *
 *       -  xpc_recast(a, b).    Does a basic C cast or a C++ reinterpret_cast.
 *       -  xpc_statcast(a, b).  Does a basic C cast or a C++ static_cast.
 *       -  xpc_int_cast(b).     Does an old-style or a new-style integer cast.
 *
 *    Also see the xpc_boolcast() macro.
 *
 *----------------------------------------------------------------------------*/

#ifndef XPC_CAST_MACROS          /* XPC_CAST_MACROS   */
#define XPC_CAST_MACROS

#ifdef __cplusplus               /* C++               */

#define xpc_recast(a, b)         reinterpret_cast<a>(b)
#define xpc_statcast(a, b)       static_cast<a>(b)
#define xpc_int_cast(b)          int(b)

#else                            /* C                 */

/******************************************************************************
 * xpc_recast()
 *------------------------------------------------------------------------*//**
 *
 *    Performs a standard C cast or a C++ reinterpret_cast.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_recast(a, b)         ((a) (b))

/******************************************************************************
 * xpc_statcast()
 *------------------------------------------------------------------------*//**
 *
 *    Performs a standard C cast or a C++ static_cast.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_statcast(a, b)       ((a) (b))

/******************************************************************************
 * xpc_recast()
 *------------------------------------------------------------------------*//**
 *
 *    Performs a standard C or C++ integer cast.
 *
 *    This macro is upper-case since we really only want to use it to
 *    define other macros.
 *
 *//*-------------------------------------------------------------------------*/

#define xpc_int_cast(b)          ((int) (b))

#endif                           /* C++ versus C      */

#endif                           /* XPC_CAST_MACROS   */

/******************************************************************************
 * __func__ macro
 *------------------------------------------------------------------------*//**
 *
 * __func__
 *
 *    GCC provides three magic variables which hold the name of the current
 *    function, as a string.  The first of these is `__func__', which is part
 *    of the C99 standard:
 *
 *    The identifier `__func__' is implicitly declared by the translator.  It
 *    is the name of the lexically-enclosing function.  This name is the
 *    unadorned name of the function.
 *
 *    `__FUNCTION__' is another name for `__func__'.  Older versions of GCC
 *    recognize only this name.  However, it is not standardized.  For maximum
 *    portability, use `__func__', but provide a fallback definition with
 *    the preprocessor, as done below.
 *
 *    `__PRETTY_FUNCTION__' is the decorated version of the function name.
 *    It is longer, but more informative.  It is also deprecated.  But, for
 *    now, we'll continue to use it
 *
 *    Visual Studio defines only __FUNCTION__, so a definition is provided
 *    below.
 *
 *    The xpc_errprint_func(), xpc_warnprint_func(), xpc_infoprint_func(), and
 *    xpc_dbgprint_func() macros in errorlogging.h use this macro.
 *
 *//*-------------------------------------------------------------------------*/

#define USE_PRETTY_FUNCTION   1                       /* TODO                 */
#undef USE__unused__                                  /* start deactivated    */

#ifdef POSIX                                          /* POSIX                */

#ifndef __func__
#if __STDC_VERSION__ < 199901L
#if __GNUC__ >= 2
#ifdef USE_PRETTY_FUNCTION                            /* TODO                 */
#define __func__        __PRETTY_FUNCTION__           /* adorned func names   */
#else
#define __func__        __FUNCTION__                  /* bald func names      */
#endif
#else
#define __func__        "<unknown>"
#endif
#endif
#endif

#ifndef __GNUG__                                      /* not using GNU C++?   */
#define USE__unused__                                 /* activate macros below*/
#endif

#else                                                 /* Win32                */

#ifndef __func__
#define __func__        __FUNCTION__
#endif

#endif                                                /* end of POSIX/Win32   */

/******************************************************************************
 * xpc_unused support macro
 *------------------------------------------------------------------------*//**
 *
 * __unused__
 *
 *    We have some C functions (see atomix.h) that have unused parameters.
 *    For GNU C, but not for GNU C++, we enable the macro 'xpc_unused' to
 *    stand for '__attribute__ ((unused))'.
 *
 *    GNU C++ version 4 and above can handle this attribute declaration, but
 *    earlier versions of GNU C++ cannot.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef USE__unused__
#define xpc_unused   __attribute__((unused))
#else
#define xpc_unused                                    /* disable it           */
#endif

/******************************************************************************
 * xpc_inline support macro
 *------------------------------------------------------------------------*//**
 *
 *    We want to use some of the XPC library functions in C++ classes, to
 *    promote reusabilty and to reduce the amount of coding and testing
 *    needed.  At the same time, we like to avoid the overhead of function
 *    calls if possible.  C++ provides inline functions.  As it turns out,
 *    the two main compilers used (so far), Visual C++ (.NET) and GNU gcc,
 *    both support the concept of inlined C functions.
 *
 *    Visual C/C++ uses the keywords \e inline or \e __inline, while GNU gcc
 *    uses \e inline, \e __inline, or the \e __inline__ keyword.  \e inline
 *    is a standard keyword in ISO C99, so is not safe to use as a macro.
 *    Both VC and gcc have __inline as key words.  gcc alone uses
 *    __inline__.  However, accidentally changing the meaning of that symbol
 *    can result in strange linkage errors with 'struct stat'.
 *
 *    Therefore, XPC plays it safe, and uses a totally new macro,
 *    \e xpc_inline.
 *
 *    Each compiler handles inlining in a different manner.  As discussed in
 *    the TexInfo page for gcc (<i>info gcc</i>), there are many ways to
 *    inline C functions.  After trying a couple, we settled on one method
 *    for gcc, which seems a bit less efficient than the way Visual C++
 *    seems to do it.  See the inline functions in sockbase.h for the
 *    methods used.
 *
 *    http://readlist.com/lists/gcc.gnu.org/gcc/1/9348.html
 *
 *    "The gnu_inline attribute was meant to be a modifier for the inline
 *    keyword, that would make it behave with GNU89 inline semantics. So,
 *
 *    extern __attribute__ ((gnu_inline)) inline int foo () { return 0;  }
 *
 *    in -std=gnu99 mode is exactly the same as
 *
 *    extern inline int foo () { return 0; }
 *
 *    in -std=gnu89 mode.
 *
 *    The gnu_inline attribute should e.g. allow adding:
 *
 *    # define inline inline __attribute__ ((gnu_inline))
 *
 *    getting the same inline behavior as -std=gnu89 code (unless you # undef
 *    it)."
 *
 *    For now, if the compiler isn't GNU gcc or a Microsoft compiler, the
 *    macro is set to "__inline".
 *
 * \example
 *    For a good example of inlining with gcc, see the Linux header files
 *    /usr/include/stdio.h and the file it includes if inlining is enabled,
 *    /usr/include/bits/stdio.h; an example of a function definition can be
 *    found by downloading a glibc tar-file and looking at the
 *    sysdeps/ieee754/ldbl-opt/nldbl-vprintf.c module.  To save you the
 *    trouble, here it is.  First, the \e declaration in the header file
 *    (ugly macros expanded here for readbility):
 *
\verbatim
   extern __inline int
   vprintf (const char *fmt, va_list ap)
   {
        return vfprintf (stdout, fmt, ap);
   }
\endverbatim
 *
 *    Next, the \e definition in the C file (with macros expanded):
 *
\verbatim
   int __attribute__((visibility ("hidden"))
   vprintf (const char *fmt, va_list ap)
   {
        return __nldbl_vfprintf (stdout, fmt, ap);
   }
\endverbatim
 *    where \e attribute_hidden is short for
 *
\verbatim
   __attribute__((visibility ("hidden")))
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#ifndef __inline__

#ifdef _MSC_VER

#define xpc_inline      __inline                      /* Visual C             */

#else

#ifdef __GNUC__

#ifdef __GNUC_GNU_INLINE__                            /* -fgnu89-inline       */
#define xpc_inline        inline                      /* gcc 4.2.x            */
#else
#ifdef __GNUC_STDC_INLINE__                           /* -fno-gnu89-inline    */
#define xpc_inline      __inline__                    /* gcc post-4.2         */
#else
#define xpc_inline      __inline                      /* gcc pre-4.2          */
#endif
#endif

#else

#define xpc_inline      __inline                      /* non-gcc/vc compilers */

#endif

#endif

#endif                                                /* __inline__           */

/******************************************************************************
 * XPC_INLINE_CODE
 *------------------------------------------------------------------------*//**
 *
 *    Provides a slightly less redundant way to include code for inline C
 *    functions in a header file used by gcc and Visual C.
 *
 *    This item is experimental.  It is readable only for functions with one
 *    or two lines of code.
 *
 *    Use it in place of the terminating semi-colon in an extern declaration
 *    in a header file.
 *
 *    Note the disadvantages of readability, unconventionality, and having
 *    to write the same short code snippet twice.  Also note that it depends
 *    on the compiler standards followed.
 *
 *    This macro is used for the very shortest functions, almost always the
 *    getters.  The setters usually include parameter validation, and are
 *    hence too cumbersome to inline.
 *
 * \gnu
 *    -fgnu89-inline tells GCC to use the traditional GNU semantics for
 *    inline functions when in C99 mode.  This option is roughly equivalent
 *    to adding the "gnu_inline" function attribute to all inline functions.
 *    It is accepted by GCC versions 4.1.3 and up.  In earlier gcc, C99
 *    inline semantics are not supported, and this option is then assumed to
 *    be present regardless of whether or not it is specified; specifying it
 *    explicitly only disables warnings about using inline functions in C99
 *    mode.  The option -fno-gnu89-inline is not supported in versions of
 *    GCC before 4.3.  It is supported only in C99 or gnu99 mode, not in C89
 *    or gnu89 mode.
 *
 *    See http://www.gnu.org/software/gcc/gcc-4.2/changes.html to note that
 *    __GNUC_STDC_INLINE__ and __GNUC_GNU_INLINE__ start with version 4.2 of
 *    gcc.
 *
 * \win32
 *    Is there any way to do inline functions in Visual C?
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __cplusplus                                    /* C++                  */

#define XPC_INLINE_CODE(x)    { x; }                  /* all C++ compilers    */

#else                                                 /* C                    */

#ifdef __GNUC__

#ifdef __GNUC_GNU_INLINE__                            /* -fgnu89-inline       */
#define XPC_INLINE_CODE(x)    { x; }                  /* gcc 4.2.x            */
#define C99_INLINE_SEMANTICS  0                       /* C89                  */
#else
#ifdef __GNUC_STDC_INLINE__                           /* -fno-gnu89-inline    */
#define XPC_INLINE_CODE(x)    { x; }
#define C99_INLINE_SEMANTICS  1                       /* C99                  */
#else
#define XPC_INLINE_CODE(x)    ;                       /* gcc pre-4.2          */
#endif
#endif

#else                                                 /* non-gcc compiles     */

#define XPC_INLINE_CODE(x)    ;

#endif

#endif                                                /* C++ versus C         */

/******************************************************************************
 * xpc_always_inline support macro
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to force the inlining of C functions.
 *
 *    Compilers often analyze a function that is declared to be inline, and
 *    may decide not to inline the function.  The developer can force
 *    inlining, although there are still functions that just cannot be
 *    inlined.
 *
 *    In Visual Studio, the \e __forceinline declarator can be used.  In gcc,
 *    the clumsier <i> __attribute__((always_inline)) </i> qualifier can be
 *    used.
 *
 *    Also see the xpc_inline macro.
 *
 * \deprecated
 *    Because the posititioning of this declarator/attribute differes
 *    between the compilers, we really cannot use it at this time.  Plus, we
 *    haven't yet figured out how to get it to work in gcc.  So don't use it.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __GNUC__
#define xpc_always_inline   __attribute__((always_inline))
#else
#define xpc_always_inline   __forceinline
#endif

/******************************************************************************
 * xpc_hidden support macro
 *------------------------------------------------------------------------*//**
 *
 *    Provides a way to force the hiding of C functions.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef __GNUC__
#define xpc_hidden   __attribute__((visibility("hidden")))
#else
#define xpc_hidden
#endif

#endif                                                /* XPC_MACROS_H         */

/******************************************************************************
 * macros.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
