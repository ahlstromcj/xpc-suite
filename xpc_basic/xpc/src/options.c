/******************************************************************************
 * options.c
 *------------------------------------------------------------------------*//**
 *
 * \file          options.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       2011-07-13 to 2011-07-19
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides functions to help in the parsing of command-line arguments.
 *
 *    Although the GNU getopt() and getopt_long() functions are nice, they
 *    have the limitation that they error out on options that aren't present
 *    in the list(s) passed to them.  But we like to distribute the options
 *    to the subsystems we're using (e.g. error-logging options versus
 *    options that are peculiar to a particular application).
 *
 *    We also like to use a brute-force if-then-else sequence to parse the
 *    options.  The if statements are not much more verbose than the lists
 *    of structures you have to create for getopt_long().
 *
 *    However, it is desirable to provide some helper functions to make it
 *    easier on the callers.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* C error-logging facility            */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/options.h>               /* This module's functions             */
XPC_REVISION(options)

/******************************************************************************
 * xpc_parse_flag()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a command-line argument against a target value.
 *
 *    Both the command-line argument and the target must be a string that
 *    starts with either a single dash or two dashes.  Each of the following
 *    is a valid command-line argument or target:
 *
 *       -  -flag
 *       -  --flag
 *       -  -
 *       -  --
 *
 * \param argcp
 *    Provides the current argument index into the argument list.  If the
 *    tokens match, then this value is automatically incremented.  This
 *    value is provided as a pointer so that it can be automatically
 *    incremented as a side-effect.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 * \param target
 *    The desired token to be checked against.  This token must include the
 *    single or double dashes needed by the argument.
 *
 * \return
 *    Returns 'true' if the given token matched the target token, and
 *    'false' otherwise, including if the parameters were bad.
 *
 * \sideeffect
 *    The \a argcp parameter is incremented, to save the caller the trouble.
 *
 * \unittests
 *    -  options_test_01_01() [smoke test only]
 *    -  options_test_02_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_flag
(
   int * argcp,
   const char * const argv [],
   const char * target
)
{
   /*
    * Consider doing an assert() on this test.  After all, this is a
    * command-line thing and the user can see the result.  Would be good to
    * figure out a way to save trouble by checking argcp and argv only once.
    * Also note that the argument and the target *must* start with "-" or
    * "--".  This check also traps empty strings.
    */

   cbool_t result = not_nullptr_3(argcp, argv, target);
   if (result)
   {
      const char * currentarg = argv[*argcp];
      result = *target == '-';
      if (result)
      {
         result = *currentarg == '-';
         if (! result)
         {
            xpc_errprintex
            (
               _("command-line option requires dash(es)"), currentarg
            );
         }
      }
      else
      {
         xpc_errprintex
         (
            _("command-line option target requires dash(es)"), target
         );
      }
      if (result)
      {
         result = strcmp(currentarg, target) == 0;
         if (result)
            (*argcp)++;
      }
   }
   return result;
}

/******************************************************************************
 * xpc_parse_string()
 *------------------------------------------------------------------------*//**
 *
 *    Tests a command-line argument against a target value, then determines
 *    if a good string argument, to return, has been specified.
 *
 *    Each of the following is a valid command-line string argument pair:
 *
 *       -  -string hello
 *       -  --string hello
 *
 *       -  -string=hello
 *       -  --string=hello
 *       -  -string:hello
 *       -  --string:hello
 *
 *    Note that the "hello" string can be a quoted value with spaces, in
 *    which case main() strips off the quotes and leaves the spaces in that
 *    \e single argument.  [We'll have a lot of permutations to test!]
 *
 * \param value
 *    If the function succeeds, this parameter points to the string
 *    argument.  Otherwise, it points to an empty string.
 *
 * \param argcp
 *    Provides the current argument index into the argument list.  If the
 *    tokens match, then this value is automatically incremented.  This
 *    value is provided as a pointer so that it can be automatically
 *    incremented as a side-effect.
 *
 * \param argc
 *    The total number of command-line arguments, used as a check to see if
 *    the value argument was provided.
 *
 * \param argv
 *    The main()-style array of command-line arguments.
 *
 * \param target
 *    The desired token to be checked against.  This token must include the
 *    single or double dashes needed by the argument.
 *
 * \return
 *    Returns 'true' if the given token matched the target token, and
 *    'false' otherwise, including if the parameters were bad.
 *
 * \sideeffect
 *    The \a argcp parameter is incremented, to save the caller the trouble.
 *
 * \unittests
 *    -  options_test_01_01() [smoke test only]
 *    -  options_test_02_01()
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_string
(
   const char ** value,
   int * argcp,
   int argc,
   const char * const argv [],
   const char * target
)
{
   /*
    * Consider doing an assert() on this test.  After all, this is a
    * command-line thing and the user can see the result.  Would be good to
    * figure out a way to save trouble by checking argcp and argv only once.
    */

   cbool_t result = xpc_parse_flag(argcp, argv, target);
   if (result)
      result = not_nullptr(value);

   if (result)
   {
      *value = "";
      result = *argcp < argc;       /* make sure there's 1 argument left      */
      if (result)
      {
         const char * currentarg;
         currentarg = argv[*argcp];
         result = *currentarg != '-';
         if (result)
         {
            *value = currentarg;
            (*argcp)++;
         }
         else
         {
            xpc_errprintex
            (
               _("command-line string value cannot start with dash(es)"),
               currentarg
            );
         }
      }
   }
   return result;
}

/******************************************************************************
 * options.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
