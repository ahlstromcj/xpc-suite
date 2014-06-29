#ifndef XPC_GETTEXT_SUPPORT_H
#define XPC_GETTEXT_SUPPORT_H

/******************************************************************************
 * gettext_support.h
 *------------------------------------------------------------------------*//**
 *
 * \file          gettext_support.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2008-03-28 to 2013-07-28
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module provides internationalization (i18n) support.
 *
 *    Internationalization is a big task, and what we have here is only the
 *    beginnings of it.
 *
 *    See the following references:
 *
 *       Loosemore, et al. (2004) "<i>GNU C Library:  Application
 *       Fundamentals</i>", Chapter 11 "<i>Message Translation</i>", GNU
 *       Press.
 *
 *       http://www.gnu.org/software/gettext/manual/gettext
 *
 *    As a programmer, the first step to bringing GNU `gettext' into your
 *    package is identifying, right in the C sources, those strings which are
 *    meant to be translatable, and those which are untranslatable.
 *    For newly written software the strings of course can and should be
 *    marked while writing it.  The `gettext' approach makes this very easy.
 *    See the macros below.
 *
 *    Doing this allows you to prepare the sources for internationalization.
 *    Later when you feel ready for the step to use the `gettext' library
 *    simply define USE_GETTEXT by enabling NLS support in your configure
 *    script.
 *
 *    Marking (using the underscore macro) has two goals. The first goal of
 *    marking is for triggering the retrieval of the translation, at run
 *    time. The second goal of the marking operation is to help xgettext at
 *    properly extracting all translatable strings when it scans a set of
 *    program sources and produces PO file templates.
 *
 * \note
 *    Many of the functions in this module can be disabled by including
 *    nullmacros.h in lieu of the present header file.  The check below
 *    catches if both versions are being included at the same time.
 *
 *    It is currently a work in progress.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* EXTERN_C_DEC and a lot more         */
XPC_REVISION_DECL(gettext_support)     /* void show_gettext_support_info()    */

#ifdef DOXYGEN                         /* expose gettext() documentation      */
#define XPC_ENABLE_NLS     1
#endif

/******************************************************************************
 * Internationalization support
 *------------------------------------------------------------------------*//**
 *
 *    This project is slowly adding support for internationalization.  At
 *    present, the "_(" macro is heavily used, but not completely used.
 *    There's still a long way to go for macro support.
 *
 *    Internationalization support is enabled by default in the XPC
 *    configure script.  This sets the variable ENABLE_NLS to 1 in the top
 *    level config.h file.  And, because we added the ax_prefix_config_h.m4
 *    macro to the m4 directory, the additional macro macro XPC_ENABLE_NLS
 *    is set to 1 in xpc-config.h.  ENABLE_NLS is used in the intl
 *    directory, and XPC_ENABLE_NLS is used in the XPC source code.
 *
 *    NLS can be disabled by running "./configure --disable-nls".
 *
 *    We need to be sure to test these combinations.
 *
 *//*-------------------------------------------------------------------------*/

/*
 * Annoyingly verbose when building the code:
 *
 * #ifdef _MSC_VER
 * #pragma message ( "No support for gettext() in Win32 at this time" )
 * #endif
 */

#if (XPC_ENABLE_NLS == 1)

#ifndef ENABLE_NLS
#define ENABLE_NLS  1
#endif

#ifndef USE_GETTEXT
#define USE_GETTEXT
#endif

#if XPC_HAVE_LIBINTL_H

/*
 * It is said to better to include the intl/gettext.h header file shipped
 * with this package.  But, if we do that, we get compiler errors.  And
 * sometimes we can get them anyway, in C++ code.  So we're hiding this GNU
 * stuff.  In general, including libintl.h by itself seems to be
 * problematic.
 *
 * EXTERN_C_DEC
 * #include <gettext.h>             // GNU internationalization in project    //
 * #include <libintl.h>             // textdomain(), etc.                     //
 * EXTERN_C_END
 */

#endif

/******************************************************************************
 * _()
 *------------------------------------------------------------------------*//**
 *
 *    Mark an in-source string for localization.
 *
 *    This macro provides a call to GNU gettext(), if the project defines
 *    the macro XPC_ENABLE_NLS.
 *
 *    It can be used where a function call is allowed.
 *
 * \gnu
 *    The _() macro is the way that GNU recommends adding support for
 *    internationalization to one's source-code modules.  The gettextize
 *    application [we believe] can scan a project's source files for
 *    instances of this macro, and use it to generate files in the intl
 *    directory that permit languages translations to be added.
 *
 *    If the usage of gettext() is enabled, this macro expands to a call to
 *    gettext().  Otherwise, it devolves to a simple parentheses operator,
 *    which does nothing.
 *
 *    According to "info gettext", the AM_GNU_GETTEXT macro determines if
 *    GNU gettext() is available, and then it sets USE_NLS to "yes", defines
 *    ENABLE_NLS as 1 in config.h (XPC_ENABLE_NLS in the XPC xpc-config.h
 *    file), and sets up to include libintl in the build.
 *
 *//*-------------------------------------------------------------------------*/

#define _(msg)             gettext (msg)

/******************************************************************************
 * gettext_noop()
 *------------------------------------------------------------------------*//**
 *
 *    Mark an in-source string for no localization.
 *
 *    See the N_() macro for more information.
 *
 *//*-------------------------------------------------------------------------*/

#define gettext_noop(msg)  msg

/******************************************************************************
 * N_()
 *------------------------------------------------------------------------*//**
 *
 *    This macro is used to mark text strings that should \a not be part of
 *    the translation process.
 *
 *    It can be used where a function call is allowed.
 *
 * \gnu
 *    It is the way that GNU recommends adding selective support for
 *    internationalization to one's source-code modules.
 *
 *    If the usage of gettext() is enabled, this macro expands to a call to
 *    gettext_noop().  Otherwise, it devolves to a null operator, which does
 *    nothing.  Either way, the string is left out of the translation
 *    process.
 *
 *//*-------------------------------------------------------------------------*/

#define N_(msg)            gettext_noop (msg)

/******************************************************************************
 * M_()
 *------------------------------------------------------------------------*//**
 *
 *    This macro is used to mark \e multi-line text strings.
 *
 *    It can be used where a function call is allowed.
 *
 *    Multi-line strings cannot be part of the translation process.
 *    However, we want to leave room in the future for handling them in some
 *    manner.
 *
 *    Note that this is an XPC convention, not a GNU convention.
 *
 *//*-------------------------------------------------------------------------*/

#define M_(msg)            msg

#else                                  /* no XPC_ENABLE_NLS defined           */

#undef ENABLE_NLS
#undef USE_GETTEXT

#define _(msg)             (msg)
#define N_(msg)             msg
#define M_(msg)             msg

/*
 * Under GCC 4, these two macro definitions cause compilation errors in
 * /usr/include/c++/4.0.2/i486-linux/bits/messages_members.h.
 *
 *    #define textdomain         (domain)
 *    #define bindtextdomain     (package, directory)
 *
 * So we converted them into a runtime warning, for now.  However, with our
 * new header-file optimizations, now defining them as xpc_warnprint(domain) and
 * xpc_warnprintex(package, directory) causes redeclaration errors.  So we
 * defined them as nothing.  But this causes a parse error.  So now we don't
 * define them at all now.  Instead, usage is wrapped in a USE_GETTEXT
 * if-def.
 *
 *    #define textdomain(domain)
 *    #define bindtextdomain(package, directory)
 *
 */

 /* Let's try again */

#define textdomain(domain)
#define bindtextdomain(package, directory)

#endif                                 /* End of XPC_ENABLE_NLS               */

/******************************************************************************
 * Portable C functions
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

#ifdef USE_GETTEXT

extern cbool_t init_gettext_support
(
   cbool_t messages,
   const char * prefix,
   const char * language,
   const char * domain
);

#endif                                 /* USE_GETTEXT                         */

extern const char * gettext_false (void);
extern const char * gettext_true (void);
extern const char * gettext_boolean (cbool_t flag);
extern const char * gettext_empty (const char * str);

EXTERN_C_END

#endif                                 /* XPC_GETTEXT_SUPPORT_H               */

/******************************************************************************
 * gettext_support.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
