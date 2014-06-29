/******************************************************************************
 * gettext_support.c
 *------------------------------------------------------------------------*//**
 *
 * \file          gettext_support.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2008-03-28 to 2009-11-18
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module supports the GNU internationalization infrastructure
 *    represented by the gettext library.
 *
 *    This module provides operating-specific definitions as well as the
 *    regular portability definitions.
 *
 *    This module provides internationalization (i18n) support.
 *
 *    It is currently a work in progress.
 *
 *    At this time, the code is enabled.  The _() macro defined
 *    in gettext_support.h is already used throughout this project, in
 *    preparation for turning the support on fully.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* used in the following macro call    */
#include <xpc/gettext_support.h>       /* functions, macros, and headers      */
XPC_REVISION(gettext_support)

#if XPC_HAVE_LOCALE_H
#include <locale.h>                    /* C locale declarations               */
#endif

#if XPC_HAVE_LIBINTL_H
#include <libintl.h>                   /* may be problematic on its own       */
#endif

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    This module has a function we would like to be documented, even if not
 *    enabled in normal compilation.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define USE_GETTEXT                    /* make definitions visible            */
#endif

/******************************************************************************
 * init_gettext_support()
 *------------------------------------------------------------------------*//**
 *
 *    Performs the stock setup of gettext() support.
 *
 *    This function calls setlocale(), bindtextdomain(), and textdomain().
 *
 *    PACKAGE and LOCALEDIR should be provided either by `config.h' or by
 *    the Makefile. For now consult the gettext() or 'hello' sources for
 *    more information.
 *
 * \posix
 *    The USE_GETTEXT macro must be defined for this function to be
 *    available.
 *
 * \win32
 *    Not yet available in Windows applications.
 *
 * \note
 *    The use of LC_ALL might not be appropriate. LC_ALL includes all locale
 *    categories, especially LC_CTYPE. This category is responsible for
 *    determining character classes with isalnum() etc. from `ctype.h'
 *    which could, for programs which process some kind of input language,
 *    be wrong.  This would mean that a source code using the c-cedilla
 *    character is runnable in France but not in the U.S., for example.
 *
 *    Some locales define this character according to the national
 *    conventions to '.' which is the same character used in the "C" locale
 *    to d note the decimal point.  So it is sometimes necessary to replace
 *    the LC_ALL line in the code by an alternate sequence of setlocale
 *    lines.
 *
 *    For more information, see the following:
 *
 *    -  http://www.delorie.com/gnu/docs/gettext/gettext_14.html
 *    -  http://www.gnu.org/software/gettext/manual/html_mono/gettext.html#SEC13
 *    -  http://oriya.sarovar.org/docs/gettext/node2.html
 *
 * \param messages
 *    If 'true', the LC_ALL category (locale facet) is used.  Otherwise, the
 *    LC_MESSAGES local is used.
 *
 * \param prefix
 *    The directory in which to look for files related to the installed
 *    language.  This item is similar to the --prefix item that can be
 *    passed to an application's configure script.  If this item is a null
 *    pointer, or empty, then "/usr" is used.  This item is then padded with
 *    "/share/locale" internally.
 *
 * \param language
 *    A locale name or language name (e.g. "es").  If this value is null, it
 *    is converted to "", which means that the setlocale() calls used in
 *    init_gettext_support() use a blank locale (language).  This causes the
 *    environment variable of the same name as the locale category (e.g.
 *    LC_CTYPE) to be used as the locale.  In glibc, LC_ALL is checked
 *    first, then the named category, and finally the LANG environment
 *    variable.
 *
 * \param domain
 *    The name of application or domain to be used.
 *
 *    The bindtextdomain() call will expect to find a message catalog with
 *    the following path-name, built from the parameters found or deduced in
 *    the init_gettext_support() function:
 *
\verbatim
         /prefix/share/locale/language/category/domain.mo
\endverbatim
 *
 *    For example, if we set the following values:
 *
 *       -  prefix = <b> /usr </b> (or <b>""</b> or <b>nullptr</b>)
 *       -  language = <b> es </b>
 *       -  messages = <b> true </b>
 *       -  domain = <b> zenity </b>
 *
 *    then the message catalog is found in the following file:
 *
\verbatim
         /usr/share/locale/es/LC_MESSAGES/zenity.mo
\endverbatim
 *
 * \return
 *    Returns 'true' if all of the actions completed successfully.
 *    Otherwise, 'false' is returned.
 *
 * \unittests
 *    -  gettext_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef USE_GETTEXT

cbool_t
init_gettext_support
(
   cbool_t messages,
   const char * prefix,
   const char * language,
   const char * domain
)
{
   cbool_t result = true;
   int category = LC_CTYPE;
   if (is_nullptr(prefix))
      prefix = "/usr";
   else if (strlen(prefix) == 0)
      prefix = "/usr";

   if (is_nullptr(language))
      language = "";

   if (messages)
   {
      /*
       * Localizable natural-language messages
       */

      char * locale_set = setlocale(LC_MESSAGES, language);
      if (not_nullptr(locale_set))
      {
         xpc_infoprintex(locale_set, _("messages locale set"));
      }
      else
      {
         xpc_errprint_func(_("invalid locale LC_MESSAGES"));
         result = false;
      }

#ifdef USE_LC_CTYPE

      if (result)
      {
         /*
          * C code localization, with the locale set via the environment
          * variables.
          */

         locale_set = setlocale(LC_CTYPE, language);
         if (not_nullptr(locale_set))
         {
            xpc_infoprintex(locale_set, _("C locale set"));
         }
         else
         {
            xpc_errprint_func(_("invalid locale LC_CTYPE"));
            result = false;
         }
      }

#endif

      if (result)
         category = LC_MESSAGES;
   }
   else
   {
      char * locale_set = setlocale(LC_ALL, language);
      if (not_nullptr(locale_set))
         xpc_infoprintex(locale_set, _("portable locale set"));
      else
      {
         xpc_errprint_func(_("invalid locale LC_ALL"));
         result = false;
      }
      if (result)
         category = LC_ALL;
   }
   if (result)
   {
      /*
       * Assemble the name for the location of the locale directory.
       * We make sure it is not too large.  It should be no more than about
       * 23 characters, realistically.
       *
       * Another option could be to pass a nullptr instenad of locale_dir.
       * See bindtextdomain(3) for more information about a null 'dirname'.
       */

      const char * share_dir = "/share/locale";
      if (strlen(prefix) + strlen(share_dir) < 64)
      {
         char lang_dir[64];                              /* a generous size   */
         char * base_dir;
         strcpy(lang_dir, prefix);
         strcat(lang_dir, share_dir);
         base_dir = bindtextdomain(domain, lang_dir);    /* bind app + locale */
         if (not_nullptr(base_dir))
         {
            /*
             * Show the new message base-directory.  Then set the domain for
             * future gettext() calls.
             */

            xpc_infoprintf("%s %s", _("base domain directory is"), base_dir);
            base_dir = textdomain(domain);
            if (not_nullptr(base_dir))
            {
               xpc_infoprintf("%s %s", _("current message domain is"), base_dir);
            }
            else
            {
               xpc_errprintf("textdomain(%s) %s", domain, _("failed"));
               result = false;
            }
         }
         else
         {
            xpc_errprintf
            (
               "bindtextdomain(%s, %s) %s",
               domain, lang_dir, _("failed")
            );
            result = false;
         }
      }
      else
      {
         xpc_errprint_func(_("prefix directory name is too long"));
         result = false;
      }
   }
   return result;
}

#else                                  /* ! USE_GETTEXT                       */

cbool_t
init_gettext_support
(
   cbool_t messages,
   const char * prefix,
   const char * language,
   const char * domain
)
{
   xpc_errprint_func(_("not enabled; turn on USE_GETTEXT and rebuild"));
   return false;
}

#endif                                 /* USE_GETTEXT                         */

/******************************************************************************
 * gettext_false()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a translated version of the English word "false".
 *
 *    This function also provides an instance of this word so that it can be
 *    included in the po file.
 *
 * \return
 *    Returns the translated string for "false".
 *
 * \unittests
 *    -  gettext_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
gettext_false (void)
{
   return _("false");
}

/******************************************************************************
 * gettext_true()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a translated version of the English word "true".
 *
 *    This function also provides an instance of this word so that it can be
 *    included in the po file.
 *
 * \return
 *    Returns the translated string for "true".
 *
 * \unittests
 *    -  gettext_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
gettext_true (void)
{
   return _("true");
}

/******************************************************************************
 * gettext_boolean()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a translated version of the English word "true".
 *
 *    This function also provides an instance of this word so that it can be
 *    included in the po file.
 *
 * \return
 *    Returns the translated string for "true".
 *
 * \unittests
 *    -  gettext_test_01_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
gettext_boolean
(
   cbool_t flag      /**< The value of the flag for which text is desired.    */
)
{
   return flag ? gettext_true() : gettext_false();
}

/******************************************************************************
 * gettext_empty()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the given string or the word for "<empty>".
 *
 *    NOT FINISHED.  We want to dynamically construct the string without
 *    hardwiring an array.
 *
 * \return
 *    Returns the translated string for "false".
 *
 * \unittests
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
gettext_empty (const char * str)
{
   if (strlen(str) == 0)
      str = "<empty>";

   return str;
}

/******************************************************************************
 * gettext_support.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
