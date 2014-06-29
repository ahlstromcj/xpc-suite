/******************************************************************************
 * hello-main.c
 *------------------------------------------------------------------------*//**
 *
 * \file          hello-main.c
 * \library       xpc_suite "Hello" application
 * \author        Chris Ahlstrom
 * \date          2008-05-09
 * \updates       2012-01-01
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides a simple hello-world application.
 *    Here is a description of <b>GNU</b> <i>gettext</i>.
 *
 *    -  http://www.gnu.org/software/gettext/manual/gettext.html
 *    -  Routines:
 *       -  gettext:       GNU gettext utilities.
 *       -  autopoint:     Copy gettext infrastructure.
 *       -  gettextize:    Prepare a package for gettext.
 *       -  msgattrib:     Select part of a PO file.
 *       -  msgcat:        Combine several PO files.
 *       -  msgcmp:        Compare a PO file and template.
 *       -  msgcomm:       Match two PO files.
 *       -  msgconv:       Convert PO file to encoding.
 *       -  msgen:         Create an English PO file.
 *       -  msgexec:       Process a PO file.
 *       -  msgfilter:     Pipe a PO file through a filter.
 *       -  msgfmt:        Make MO files out of PO files.
 *       -  msggrep:       Select part of a PO file.
 *       -  msginit:       Create a fresh PO file.
 *       -  msgmerge:      Update a PO file from template.
 *       -  msgunfmt:      Uncompile MO file into PO file.
 *       -  msguniq:       Unify duplicates for PO file.
 *       -  xgettext:      Extract strings into a PO file.
 *    -  http://www.heiner-eichmann.de/autotools/gettext_overview.html
 *
 * \usage
 *    -# First run this program as is on your system.  The "Hello, world!"
 *       should be displayed in English (or your main language).
 *    -# Then make this setting:
\verbatim
            export LC_ALL=
\endverbatim
 *    -# Then make one of the following settings.  Both should cause
 *       "!Hola mundo!" to be displayed, given that those locales are
 *       installed on your system.
\verbatim
            export LANG=es_ES
            export LANG=es_ES.ISO-8859-1
\endverbatim
 *       If an error occurs, see what is in the /etc/locale.alias file and
 *       try another Spanish setting.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/xpc-config.h>
#include <xpc/tiny_macros.h>

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* declares printf(), stdout, etc.     */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* declares strcmp(), etc.             */
#endif

#if XPC_HAVE_LIBINTL_H
#include <libintl.h>                   /* declares gettext()                  */
#endif

#if XPC_HAVE_LOCALE_H
#include <locale.h>                    /* declares setlocale()                */
#endif

#if XPC_ENABLE_NLS
#define USE_GETTEXT
#endif

/******************************************************************************
 * error_print_func()
 *------------------------------------------------------------------------*//**
 *
 *    This macro supplies the name of the current C function to the
 *    error_print_ex() function.
 *
 * \unittests
 *    -  None.
 *
 *//*-------------------------------------------------------------------------*/

#define error_print_func(x)      error_print_ex(x, __func__)

/******************************************************************************
 * error_print_ex()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a function similar to the xpc_errprint_ex() function in the
 *    main XPC library.
 *
 *    This function is provided so that there are no other dependencies of
 *    the XPC 'hello' application.
 *
 * \unittests
 *    -  None.
 *
 *//*-------------------------------------------------------------------------*/

static void
error_print_ex
(
   const char * message,   /**< The main part of the message to be printed.   */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   fprintf(stdout, "? %s: %s\n", extra, message);
}

/******************************************************************************
 * info_print_ex()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a function similar to the xpc_infoprint_ex() function in the
 *    main XPC library.
 *
 *    This function is provided so that there are no other dependencies of
 *    the XPC 'hello' application.
 *
 * \unittests
 *    -  None.
 *
 *//*-------------------------------------------------------------------------*/

static void
info_print_ex
(
   const char * message,   /**< The main part of the message to be printed.   */
   const char * extra      /**< The tag for the message to be printed.        */
)
{
   fprintf(stdout, "* %s: %s\n", extra, message);
}

/******************************************************************************
 * text_domain()
 *------------------------------------------------------------------------*//**
 *
 *    Sets up the gettext() support in a very rudimentary way.
 *
 *    There's a lot of checks and things that could be done to make this
 *    setup robust, but they are not done here.
 *
 *    Sometimes an application needs to replace the
 *    <tt>setlocale(LC_ALL, "")</tt> call with
 *
\verbatim
      setlocale(LC_CTYPE, "");
      setlocale(LC_MESSAGES, "");
      . . .
\endverbatim
 *
 *    See http://www.gnu.org/software/gettext/manual/gettext.html#Triggering
 *    for reasons why this might be necessary.
 *
 *    The available locales are:
 *
 *       -  LC_ALL
 *       -  LC_CTYPE
 *       -  LC_MESSAGES (GNU systems)
 *       -  LC_COLLATE
 *       -  LC_MONETARY
 *       -  LC_NUMERIC
 *       -  LC_TIME
 *
 * \win32
 *    Not yet available in Windows applications.
 *
 * \return
 *    Returns the result of the locale setup: 'true' if it succeeded, and
 *    'false' otherwise.
 *
 * \unittests
 *    -  None.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef USE_GETTEXT         /* turned off by "./configure --disable-gettext"   */

int
text_domain (void)
{
   int result = false;
   char * locale_set = setlocale(LC_MESSAGES, "");    /* LC_ALL is invalid */
   if (locale_set != nullptr)
   {
      info_print_ex(locale_set, _("LC_MESSAGES locale setting"));
      locale_set = bindtextdomain(XPC_PACKAGE, LOCALEDIR);
      if (locale_set != nullptr)
      {
         locale_set = textdomain(XPC_PACKAGE);
         if (locale_set != nullptr)
         {
            info_print_ex(locale_set, _("current text domain"));
            result = true;
         }
         else
            error_print_ex(_("failed"), "textdomain()");
      }
      else
         error_print_ex(_("failed"), "bindtextdomain()");
   }
   else
      error_print_ex(_("invalid locale"), "LC_MESSAGES");

   return result;
}

#else

int
text_domain (void)
{
   error_print_func("gettext not available; turn on USE_GETTEXT and rebuild");
   return false;
}

#endif

/******************************************************************************
 * helptext
 *------------------------------------------------------------------------*//**
 *
 *    Provides a discussion of this application.
 *
 *//*-------------------------------------------------------------------------*/

const char * const helptext =
   "\n"
   "hello version 1.1.0\n"
   "\n"
   "Usage:  hello [arbitrary strings] [--help]\n"
   "\n"
   "Without options, this program prints a translated version of the famous\n"
   "'Hello, world!' message.\n"
   "\n"
   "Otherwise, it translates each argument, if possible, one by one.\n"
   "\n"
   "'hello' serves simply as a demonstration to developers of\n"
   "how to do two things:\n"
   "\n"
   "   1. Build an application that is internationalized.\n"
   "   2. Link an application to one of the 'XPC' libraries we\n"
   "      have created.\n"
   "\n"
   "If you typed 'make' from the root of this project, then you\n"
   "will be able to navigate the documentation of these issues by\n"
   "directing your browser to $top/doc/dox/html/index.html, and\n"
   "you can also peruse the usual READMEs and INSTALLs you might\n"
   "find in this project.\n"
   "\n"
   "Feel free to mail me at <mailto:ahlstromcj@gmail.com>.\n"
   "\n"
   "I apologize that I don't have a translation for this help text.\n"
   "\n"
;

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the hello application.
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
   int result = 0;
   if (argc > 1)
   {
      if (strcmp(argv[1], "--help") == 0)
         printf(helptext);
      else if (text_domain())
      {
         int arg = 1;
         while (arg < argc)
         {
            fprintf(stdout, "   %s\n", _(argv[arg++]));
         }
      }
      else
         result = 1;
   }
   else
   {
#ifdef USE_GETTEXT
      printf
      (
         "\n"
         "Welcome to the '%s' domain!\n\n"
         "Running with partial internationalization, with localization files\n"
         "from directory %s.\n"
         "\n"
         ,
         XPC_PACKAGE, LOCALEDIR
      );
#else
      printf
      (
         "\n"
         "Welcome to the '%s' domain!\n\n"
         "Running with no internationalization enabled.\n"
         "\n"
         ,
         XPC_PACKAGE
      );
#endif

      if (text_domain())
      {
         printf("\n%s\n\n", _("Hello, world!"));
         result = 0;
      }
      else
         result = 1;
   }
   return result;
}

/******************************************************************************
 * hello-main.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
