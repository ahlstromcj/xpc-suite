/******************************************************************************
 * parse_ini.c
 *------------------------------------------------------------------------*//**
 *
 * \file          parse_ini.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \dates         2010-07-14
 * \updates       2012-06-02
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a way to set the application options from a Windows-style
 *    "INI" file, by parsing that configuration file and making it available
 *    as if it were a set of command-line parameters.
 *
 *    The file is in simple INI format.  Each line is one of the following:
 *
 *       -  Blank.
 *          Blank lines are ignored.
 *       -  Comment.
 *          Lines whose first non-white-space character is
 *          "#" or ";" are ignored.
 *       -  "[ Section Name ]".
 *          If present, this item is used to indicate which group of
 *          initializations are acceptable to the caller.
 *       -  <tt>optioname = optionvalue</tt>.
 *          These pairs are converted into two consecutive command-line
 *          arguments:
 *             -  <tt>argv[n]</tt> becomes "--optionname".
 *             -  <tt>argv[n+1]</tt> becomes "optionvalue".
 *       -  <tt>optioname =</tt>.
 *          This item is treated like a simple flag, since there is no value
 *          specified.
 *
 * \warning
 *    This module assumes only ASCII text.  This is a big limitation and
 *    severely harms internationalization.
 *
 *//*-------------------------------------------------------------------------*/

#include <sys/stat.h>                  /* C::_stat or C::stat structure       */
#include <ctype.h>                     /* toupper(), isalpha(), etc. macros   */
#include <stdio.h>                     /* FILE * functions                    */
#include <xpc/errorlogging.h>          /* error-reporting and XPC macros      */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/parse_ini.h>             /* the functions in this module        */
#include <xpc/portable.h>              /* xpc_current_date()                  */

#ifdef POSIX
#define _stat     stat
#endif

/******************************************************************************
 * extract_section_name() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Extracts the section name from a "[ section ]" construct, using the
 *    '[' character as a flag.
 *
 *    Ignores padding space after the '[' or before the ']'.
 *
 * \warning
 *    This function truncates the source at 80 characters.  We demand that
 *    section names be reasonably short!
 *
 * \threadunsafe
 *    For now, this function holds the results in an internal buffer, and is
 *    not threadsafe.  We'll fix this issue at some point.
 *
 * \param source
 *    The line of text to check.
 *
 * \return
 *    Returns a pointer to the section name, ready to check against and use.
 *    If there is no '[' and ']' marker, then section name is considered not
 *    to exist, and a null pointer is returned.
 *
 *//*-------------------------------------------------------------------------*/

static const char *
extract_section_name (const char * source)
{
   static char buffer[80];
   const char * result = nullptr;
   char * resultptr = &buffer[0];
   while (*source == ' ')
      source++;

   if (*source == '[')
   {
      (void) memset(buffer, 0, 80);             /* clear out old junk         */
      *resultptr++ = *source++;                 /* copy the '[' flag          */
      while (*source == ' ')
         source++;

      if (not_null_result(strchr(source, ']')))
      {
         while ((*source >= ' ') && (*source != ']'))
            *resultptr++ = *source++;

         if (resultptr[-1] == ' ')
         {
            resultptr--;
            while (*resultptr == ' ')
               *resultptr-- = 0;
         }
         if (strlen(buffer) > 1)                /* more than just '['         */
            result = &buffer[0];

         /*
          * For debugging only.
          *
          * fprintf(stdout, "extract_section_name() = %s\n", result);
          */
      }
   }
   return result;
}

/******************************************************************************
 * get_ini_option() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Returns a pointer to the first token in the provided string,
 *    terminated by a null.
 *
 *    This function accepts a line of text read from an INI-style
 *    configuration file, parses it to find the first token in the text, and
 *    then null-terminates the string.  The result is the variable name that
 *    is provided on the INI line.
 *
 *    The standard shell script comment character ('#') terminates the
 *    string, if present.
 *
 *    The standard INI script comment character (';') also terminates the
 *    string, if present.
 *
 *    If the first non-white-space character is an open-bracket ("["), then
 *    the line is assumed to contain a section group name (e.g. "[Test]"),
 *    and the result that is returned is of the form:
 *
 *          [sectionname
 *
 * \private
 *    This routine is a static, internal C function and a helper function.
 *
 * \param sourcedest
 *    Must point to a string of the form
 *
 *          optionname [ = optionvalue ]
 *
 *    where 'optionname' is a short alphanumeric token, and 'optionvalue' is
 *    a short string.  There may or may not be spaces before or after the
 *    equals sign.  They are ignored.
 *
 *    The option-name can consist of any characters except the space (or
 *    other non-printing characters, '=', ';', and '#'.  However, it is best
 *    to stick with alphanumeric characters and the underscore, in most
 *    scenarios.
 *
 * \return
 *    Returns a pointer to the first token in the \a sourcedest string.  If
 *    no token is found before a '#', ';', '=', or end-of-line is found,
 *    then a null pointer is returned.  The pointer must be deleted by the
 *    caller after usage, if it is not null.
 *
 *//*-------------------------------------------------------------------------*/

static char *
get_ini_option (char * sourcedest)
{
   char * result = nullptr;
   if (not_nullptr(sourcedest))
   {
      char * start = sourcedest;       /* start at the beginning of string    */
      cbool_t empty = false;           /* assume there is data                */
      while (*start <= ' ')            /* skip "white space"                  */
      {
         if (*start == 0)              /* found the null terminator?          */
         {
            empty = true;              /* yes, there is no option name        */
            break;
         }
         else
            (void) *start++;
      }

      /*
       * Check for either style of comment, or for a missing option-name
       * before the '='.
       */

      if
      (
         (*start == ';') || (*start == '#') ||
         (*start == '=') || (*start == '[')
      )
      {
         empty = true;                          /* there is no option name    */
      }
      if (! empty)                              /* option name exists         */
      {
         size_t count = 1;                      /* for counting characters    */
         const char * beginning = start;        /* log option name's start    */
         char * destination;                    /* to be calloc()'ed          */
         start++;                               /* we're at second character  */
         while
         (
            (*start > ' ') && (*start != '=') &&
            (*start != ';') && (*start != '#')
         )
         {
            count++;
            start++;
         }

         /*
          * Create a temporary buffer for the option name plus the null
          * pointer.
          */

         destination = calloc(count + 1, 1);    /* allocate arg buffer        */
         if (not_nullptr(destination))
         {
            (void) memcpy(destination, beginning, count);
            destination[count] = 0;
            result = destination;               /* hurray!                    */
         }
         else
            xpc_errprint_func(_("couldn't allocate option buffer"));
      }
   }
   return result;
}

/******************************************************************************
 * get_ini_value() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Returns a pointer to the value token in the string.
 *
 *    This function accepts a line of text read from a configuration file,
 *    parses it to find the value token in the text, and then
 *    null-terminates the string.
 *
 *    The standard shell script comment character, '#', terminates the
 *    string, if present.
 *
 *    The standard INI script comment character, ';', terminates the
 *    string, if present.
 *
 *    If a value starts with a double-quote, then this function allows
 *    spaces and comment characters until the next double-quote, or the end
 *    of the line.
 *
 * \private
 *    This routine is a static, internal C function and a helper function.
 *
 * \param sourcedest
 *    Must point to a string of the form
 *
 *          optionname = optionvalue
 *
 *    or
 *
 *          optionname = "optionvalue"
 *
 *    Note that there must be an equals sign and an option value.
 *
 * \param includequotes
 *    If true, then any double-quotes as used above are included in the
 *    value.  Otherwise, the quotes are dropped from both ends.
 *
 * \return
 *    Returns a pointer to the value token in the \a sourcedest string.  If
 *    no token is found before a '#', ';', '=', or end-of-line is found,
 *    then a null pointer is returned.  The pointer must be deleted by the
 *    caller after usage, if it is not null.
 *
 *//*-------------------------------------------------------------------------*/

static char *
get_ini_value (char * sourcedest, cbool_t includequotes)
{
   char * result = nullptr;
   if (not_nullptr(sourcedest))
      sourcedest = strchr(sourcedest, '=');     /* look for the '='           */

   if (xpc_good_pointer(sourcedest))            /* was it found?              */
   {
      char * start = sourcedest + 1;            /* yes, move past it          */
      cbool_t empty = false;                    /* optimistic assumption      */
      while (isspace(*start))                   /* skip main space characters */
      {
         start++;
      }
      if (*start == 0)                          /* null-terminator found?     */
         empty = true;                          /* yes, no value given        */

      if ((*start == ';') || (*start == '#'))   /* comment character found?   */
         empty = true;                          /* yes, no value given        */

      if (! empty)
      {
         size_t count = 0;                      /* for counting characters    */
         const char * beginning = start;        /* log option name's start    */
         char * destination;                    /* to be calloc()'ed          */
         cbool_t gotquote = false;              /* flags first quote gotten   */
         cbool_t inquotes = (*start == '"');    /* double-quote?              */
         if (inquotes && ! includequotes)       /* yes, but don't want 'em    */
         {
            beginning++;                        /* skip the quote             */
            start++;                            /* skip the quote             */
         }

         /*
          * Scoot past all legal value characters.  Stop at the first
          * white-space, control-characters, or INI or shell-script comment
          * character.
          */

         while (inquotes || ! isspace(*start))  /* no ctrl chars or nulls     */
         {
            if (inquotes)
            {
               if (*start == '"')
               {
                  if (gotquote)
                     inquotes = false;          /* will terminate loop        */
                  else
                     gotquote = true;           /* we have the first quote    */

                  if (includequotes)
                  {
                     count++;                   /* include the quote          */
                     start++;
                  }
               }
               else
               {
                  count++;
                  start++;                      /* next char inside ""'s      */
               }
               if (! inquotes)
                  break;                        /* don't get unquoted chars   */
            }
            else if ((*start == ';') || (*start == '#'))
               break;
            else
            {
               count++;
               start++;
            }
         }
         destination = calloc(count + 1, 1);    /* allocate arg buffer        */
         if (not_nullptr(destination))
         {
            (void) memcpy(destination, beginning, count);
            destination[count] = 0;
            result = destination;               /* hurray!                    */
         }
         if (inquotes)
            xpc_warnprint_func(_("unmatched quotes found in option value"));
      }
   }
   return result;
}

/******************************************************************************
 * append_token() [static]
 *------------------------------------------------------------------------*//**
 *
 *    Appends an option name or option value to an argv-style string buffer.
 *
 *    This function is called for each option name/value pair to build up a
 *    string of bytes of the form
 *
 *       token<null>token<null>token<null>...token<null><null>
 *
 * \param destination
 *    Provides a pointer to the string buffer that is meant to contain the
 *    string date for each of the parameters in the argument list.
 *
 *    The caller should pass the value just \e past the previous null
 *    terminator, because we want to preserve them as terminators for each
 *    argv[] string.
 *
 * \param source
 *    Provides a pointer to the source line.  This pointer points to the \a
 *    sourcedest parameter passed to the internal get_ini_option() and
 *    get_ini_value() functions.
 *
 * \param isoption
 *    The caller sets this value to 'true' if this token is an option name
 *    (as opposed to an option value).  An option name gets the standard
 *    option marker '--' prepended to it, so that it will look like an
 *    option the user provided on the command-line.
 *
 * \return
 *    Returns a pointer to just past the terminating null character if the
 *    parameters were value.  Otherwise, a null pointer is returned.
 *
 *//*-------------------------------------------------------------------------*/

static char *
append_token
(
   char * destination,
   const char * source,
   cbool_t isoption
)
{
   char * result = nullptr;
   if (not_nullptr_2(destination, source))
   {
      if (isoption)
      {
         strcpy(destination, "--");
         destination += 2;
      }
      strcat(destination, source);
      result = strchr(destination, 0) + 1;
   }
   return result;
}

/******************************************************************************
 * SET_OPT_LINE_SIZE
 *------------------------------------------------------------------------*//**
 *
 *    Provides a manifest constant for line size.  This size is used in a
 *    careful way -- the code handles excess size by truncation.
 *
 *    We do not try to implement a catch-all dynamically-allocated line
 *    buffer.  This work is more than is necessary for most files.
 *
 *    If a line of text is has more characters than this value, some will
 *    simply be lost.
 *
 *//*-------------------------------------------------------------------------*/

#define SET_OPT_LINE_SIZE     512

/******************************************************************************
 * xpc_is_INI_line()
 *------------------------------------------------------------------------*//**
 *
 *    Examines a line to see if it is useful for an INI file.
 *
 *    A line is "null" in this function if it does not start with an
 *    alphabetic character.
 *
 * \param line
 *    The line of text to be checked.
 *
 * \return
 *    This function returns true if the line started with an alphabetic
 *    character.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_is_INI_line (const char * line)
{
   cbool_t result = false;
   while (*line != 0)
   {
      if (isalpha(*line))
      {
         result = true;
         break;
      }
      line++;
   }
   return result;
}

/******************************************************************************
 * xpc_argv_from_INI()
 *------------------------------------------------------------------------*//**
 *
 *    Creates an argc/argv ensemble from a simple configuration file.
 *
 *    Each option in the file is of the following format, one and only one
 *    option per line:
 *
 *          optionname [ = optionvalue ]
 *
 *    This format is converted to
 *
 *          --optionname [ optionvalue ]
 *
 *    where the brackets indicate the optional presence of the item.
 *
 * \note
 *    Section group markers, such as the following, are ignored by this
 *    function.
 *
 *          [ Section ]
 *
 * \usage
 *    This function require a bit of setup and teardown in order to use it
 *    properly.  Here is an example of reading a configuration file called
 *    "myapp.ini", and a fictitious parsing function called
 *    "my_parse_argc_argv()".  Check out the test module for specific
 *    examples.
 *
\verbatim
      int local_argc;
      char * buffer;
      char ** local_argv = xpc_argv_from_INI("myapp.ini", &local_argc, &buffer);
      if (not_nullptr(local_argv))           // (local_argv != NULL)
      {
         my_parse_argc_argv(local_argc, local_argv, false);
         xpc_delete_argv(localargv, buffer);
      }
\endverbatim
 *
 * \note
 *    -# How much space to allocate?  The worst case is for one-character
 *       options with no values.  Each option would be 2 or 3 characters (e.g.
 *       "d\n" or "d\r\n"), and would expand to four characters (e.g.
 *       "--d<null>").  So a buffer twice the size of the file
 *       should be sufficient.
 *    -# We sanity-check the file size, to avoid problems.  We pick an
 *       arbitrary number, 256K.  Realistically, most configuration files
 *       will be under 20K.
 *    -# The fgets() function reads a string from the input stream argument
 *       and stores it.  It reads characters from the current stream
 *       position to and including the first newline character, to the end
 *       of the stream, or until the number of characters read is equal to
 *       n-1, whichever comes first. The result stored in string is appended
 *       with a null character. The newline character, if read, is included
 *       in the string.
 *
 * \param filespec
 *    Provides the name of the file from which to read the configuration
 *    information.
 *
 * \param section
 *    Provides the section group from which to get the argv values.
 *    Processing starts only when this section name is encountered, and it
 *    stops when the next section is encountered.
 *
 * \param argc_return
 *    Points to the integer value to be filled with the number of arguments
 *    that were found in the configuration file.
 *
 * \param buffer_return
 *    Points to an internally-allocated buffer holding all of the values to
 *    which the returned argv-style array will point.
 *
 * \return
 *    Returns a pointer an internally-allocated array of character pointers
 *    that is to be used like any argv-style array is used in parsing
 *    command-line arguments.
 *
 *    If there are any failures, then a null pointer is returned, and no
 *    values can be used, nor is there a need to call xpc_delete_argv().
 *
 * \todo
 *    DEAL WITH ARG 0
 *
 *//*-------------------------------------------------------------------------*/

char **
xpc_argv_from_INI
(
   const char * filespec,
   const char * section,
   int * argc_return,
   char ** buffer_return
)
{
   char ** result = nullptr;
   cbool_t ok = not_nullptr(filespec) && (strlen(filespec) > 0);
   if (ok)
      ok = not_nullptr_2(argc_return, buffer_return);

   if (ok)
   {
      struct _stat status;                         /* file info structure     */
      size_t filesize = 0;
      int rcode = _stat(filespec, &status);        /* obtain file info        */
      set_nullptr(*buffer_return);                 /* guarantee nullity       */
      *argc_return = 0;                            /* guarantee 0 arguments   */
      if (rcode == POSIX_SUCCESS)
         filesize = status.st_size;                /* now have good file size */

      if ((filesize > 0) && (filesize <= 262144))  /* light sanity check      */
      {
         FILE * fhandle = fopen(filespec, "r");
         if (not_nullptr(fhandle))
         {
            size_t fullsize = 2 * filesize;        /* see notes               */
            char * buffer = calloc(fullsize, 1);   /* allocate arg buffer     */
            if (not_nullptr(buffer))
            {
               char line[SET_OPT_LINE_SIZE];
               int argc = 0;
               char * bufferptr = buffer;
               cbool_t section_in_progress = false;
               *bufferptr = 0;                     /* terminate for strcat()  */
               bufferptr = append_token(bufferptr, filespec, false);
               argc++;
               while (not_null_result(fgets(line, SET_OPT_LINE_SIZE-1, fhandle)))
               {
                  char * nl = strchr(line, '\n');
                  char * value = nullptr;
                  char * option = nullptr;
                  const char * sectname;
                  if (not_null_result(nl))
                     *nl = 0;                      /* replace newline w/null  */

                  if (! xpc_is_INI_line(line))
                     continue;                     /* skip comment lines      */

                  sectname = extract_section_name(line);
                  if (not_null_result(sectname) && (sectname[0] == '['))
                  {
                     /*
                      * If there is no section in progress, one is becoming
                      * in progress only if it matches the desired section.
                      * In that case, we continue, to skip other parsing on
                      * that line of the INI file.  If a section was already
                      * in progress, we automatically terminate; we don't
                      * care if the next section has the same name.  Note
                      * that we have to skip the '['.
                      */

                     if (section_in_progress)
                        break;
                     else
                     {
                        section_in_progress = strcmp(sectname+1, section) == 0;
                        continue;
                     }
                  }
                  if (! section_in_progress)
                     continue;

                  nl = strchr(line, '=');          /* search for equal sign   */
                  if (not_null_result(nl))         /* if found, then...       */
                  {
                     option = get_ini_option(line);         /* get 1st token  */
                     if (xpc_good_pointer(option))
                     {
                        bufferptr = append_token(bufferptr, option, true);
                        argc++;
                        free(option);

                        /*
                         * 'false' is passed in order to not include the
                         * quotes.  If you change this to true, also change
                         * the unit test to pass with quotes in the result.
                         */

                        value = get_ini_value(line, false); /* get 2nd token  */
                        if (not_null_result(value))
                        {
                           bufferptr = append_token(bufferptr, value, false);
                           argc++;
                           free(value);
                        }
                     }
                  }
                  else
                  {
                     /*
                      * @change ca 2010-11-19
                      * Even if no '=', we'd like to include the first token
                      * in the list of command-line arguments, because it
                      * might be a boolean flag.
                      */

                     option = get_ini_option(line);         /* get 1st token  */
                     if (xpc_good_pointer(option))
                     {
                        bufferptr = append_token(bufferptr, option, true);
                        argc++;
                        free(option);
                     }
                  }
               }
               *argc_return = argc;
               if (argc > 0)
               {
                  char ** argv = calloc(argc * sizeof(char *), 1);
                  if (not_nullptr(argv))
                  {
                     int ai;
                     bufferptr = buffer;              /* start again          */
                     for (ai = 0; ai < argc; ai++)
                     {
                        argv[ai] = bufferptr;
                        bufferptr = strchr(bufferptr, 0);
                        bufferptr++;
                     }
                     result = argv;
                  }
                  else
                  {
                     xpc_errprint_func(_("couldn't allocate argv[]"));
                     free(buffer);
                     set_nullptr(buffer);
                  }
               }
               else
                  xpc_warnprint_func(_("no options and values found"));

               *buffer_return = buffer;
            }
            else
               xpc_errprint_func(_("calloc() failed"));

            (void) fclose(fhandle);
         }
         else
            xpc_errprint_func(_("could not open file for reading"));
      }
      else
         xpc_errprint_func(_("bad config file"));
   }
   else
      xpc_errprint_func(_("null pointers supplied"));

   return result;
}

/******************************************************************************
 * xpc_delete_argv()
 *------------------------------------------------------------------------*//**
 *
 * \param argv
 *    The pointer to the argv-style pointer buffer that was allocated by
 *    xpc_argv_from_INI().
 *
 * \param buffer
 *    The internal buffer.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_delete_argv (char ** argv, char * buffer)
{
   if (xpc_good_pointer(argv))
      (void) free(argv);

   if (xpc_good_pointer(buffer))
      (void) free(buffer);
}

static const char const * gs_header_fmt =

"#******************************************************************************\n"
"#* %s\n"
"#*-----------------------------------------------------------------------------\n"
"#*\n"
"#* \\file          %s\n"
"#* \\library       LIB\n"
"#* \\author        AUTHOR\n"
"#* \\updates       %s\n"
"#* \\version       $Revision$\n"
"#* \\license       $LICENSE$\n"
"#*\n"
"#*    An INI file.  Please enter your own description.\n"
"#*\n"
"#*//*-------------------------------------------------------------------------*/\n"
;

static const char * const gs_footer_fmt =

"\n"
"#******************************************************************************\n"
"#* %s\n"
"#*-----------------------------------------------------------------------------\n"
"#* Local Variables:\n"
"#* End:\n"
"#*-----------------------------------------------------------------------------\n"
"#* vim: ts=3 sw=3 et ft=dosini\n"
"#*----------------------------------------------------------------------------*/\n"
;

/******************************************************************************
 * xpc_write_INI_header()
 *------------------------------------------------------------------------*//**
 *
 *    Opens a file for output, writes a comment at the top of it, and
 *    returns the pointer for further writing.
 *
 * \param filespec
 *    The name of the file to be written.  This file, if existing already,
 *    will be completely over-written.  At some point, we may support
 *    ripping out an existing section and replacing only it.
 *
 * \param section
 *    The name of the section to be written.
 *
 * \return
 *    The opened file-handle is returned if successful.  Otherwise a null
 *    pointer is returned.  Note that this file-handle must be closed,
 *    preferably by a call to xpc_write_INI_footer().
 *
 *//*-------------------------------------------------------------------------*/

FILE *
xpc_write_INI_header
(
   const char * filespec,
   const char * section
)
{
   FILE * result = nullptr;
   cbool_t ok = not_nullptr(filespec);
   if (ok)
   {
      result = fopen(filespec, "w");
      if (not_nullptr(result))
      {
         const char * date = xpc_current_date();
         fprintf(result, gs_header_fmt, filespec, filespec, date);
         if (not_null_result(section))
            fprintf(result, "\n[ %s ]\n\n", section);
      }
      else
         xpc_errprint_func(_("could not open file for writing"));
   }
   return result;
}

/******************************************************************************
 * xpc_append_INI_header()
 *------------------------------------------------------------------------*//**
 *
 *    Given an already open file-handle, writes the given section
 *    information to the file.
 *
 *    This function is used for sections after the first section.  For the
 *    first section, always use xpc_write_INI_header().
 *
 * \param handle
 *    The file handle for the file, previous opened by xpc_write_INI_header.
 *
 * \param section
 *    The name of the section to be written.
 *
 * \return
 *    Returns 'true' if the function succeeded.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_append_INI_header
(
   FILE * fhandle,
   const char * section
)
{
   cbool_t result = not_nullptr(fhandle);
   if (result)
   {
      if (not_null_result(section))
         fprintf(fhandle, "\n[ %s ]\n\n", section);
   }
   return result;
}

/******************************************************************************
 * xpc_write_INI_item()
 *------------------------------------------------------------------------*//**
 *
 *    Writes one INI "option = value" item, to the file handle opened by
 *    xpc_write_INI_header().
 *
 * \param fhandle
 *    The file handle returned by xpc_write_INI_header().
 *
 * \param option
 *    The name of the option to be written.  It should not include the "--"
 *    marker.  If found, they are stripped.
 *
 * \param value
 *    The name of the value.  If null, then no value is written -- the
 *    option is presumed to be a flag.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the file was written
 *    to successfully.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_write_INI_item
(
   FILE * fhandle,
   const char * option,
   const char * value
)
{
   cbool_t result = not_nullptr_2(fhandle, option);
   if (result)
   {
      if (option[0] == '-' && option[1] == '-')
         option += 2;

      if (not_null_result(value))
         result = fprintf(fhandle, "%s = %s\n", option, value) > 0;
      else
         result = fprintf(fhandle, "%s =      ; a flag setting\n", option) > 0;

      if (! result)
         xpc_errprint_func(_("write failed"));
   }
   else
      xpc_errprint_func(_("invalid parameters"));

   return result;
}

/******************************************************************************
 * xpc_write_INI_footer()
 *------------------------------------------------------------------------*//**
 *
 *    Writes the footer of the INI file, and then closes the file.
 *
 * \param fhandle
 *    The file handle returned by xpc_write_INI_header().
 *
 * \param filespec
 *    The name of the file to be written.
 *
 * \return
 *    Returns 'true' if the parameters were valid and the file was closed
 *    successfully.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_write_INI_footer
(
   FILE * fhandle,
   const char * filespec
)
{
   cbool_t result = not_nullptr_2(filespec, fhandle);
   if (result)
   {
      fprintf(fhandle, gs_footer_fmt, filespec);
      result = fclose(fhandle) == 0;
   }
   else
      xpc_errprint_func(_("invalid parameters"));

   return result;
}

/******************************************************************************
 * xpc_argv_to_INI()
 *------------------------------------------------------------------------*//**
 *
 *    Writes the given command-line arguments to a well formatted INI file.
 *
 *    Doing this is a little tricky, because arguments without the "--"
 *    option marker must follow the previous argument and an equals sign.
 *
 * \param filespec
 *    The name of the file to be written.  This file, if existing already,
 *    will be completely over-written.  At some point, we may support
 *    ripping out an existing section and replacing only it.
 *
 * \param section
 *    The name of the section to be written.
 *
 * \param argc
 *    The number of arguments to be written.
 *
 * \param argv
 *    The pointer to the argv-style pointer buffer that was allocated by
 *    xpc_argv_from_INI().
 *
 * \return
 *    Returns 'true' if the file could be created.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_argv_to_INI
(
   const char * filespec,
   const char * section,
   int argc,
   char ** argv                     // const char * argv []
)
{
   cbool_t result = not_nullptr(filespec);
   if (result)
   {
      FILE * fhandle = xpc_write_INI_header(filespec, section);
      if (not_nullptr(fhandle))
      {
         result = xpc_append_argv_to_INI(fhandle, argc, argv);
         if (result)
            (void) xpc_write_INI_footer(fhandle, filespec);
      }
   }
   return result;
}

/******************************************************************************
 * xpc_append_argv_to_INI()
 *------------------------------------------------------------------------*//**
 *
 *    Writes the given command-line arguments to a well formatted INI file.
 *
 *    Doing this is a little tricky, because arguments without the "--"
 *    option marker must follow the previous argument and an equals sign.
 *
 * \param filespec
 *    The name of the file to be written.  This file, if existing already,
 *    will be completely over-written.  At some point, we may support
 *    ripping out an existing section and replacing only it.
 *
 * \param section
 *    The name of the section to be written.
 *
 * \param argc
 *    The number of arguments to be written.
 *
 * \param argv
 *    The pointer to the argv-style pointer buffer that was allocated by
 *    xpc_argv_from_INI().
 *
 * \return
 *    Returns 'true' if the file could be created.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_append_argv_to_INI
(
   FILE * fhandle,
   int argc,
   char ** argv                     // const char * argv []
)
{
   cbool_t result = not_nullptr(fhandle);
   if (result)
   {
      int argindex = 1;
      cbool_t option_active = false;
      while (argindex < argc)
      {
         const char * arg = argv[argindex++];
         if ((arg[0] == '-') && (arg[1] == '-'))
         {
            if (option_active)
               fprintf(fhandle, "   ; a flag setting\n");

            fprintf(fhandle, "%s = ", arg + 2);    // write without "--"
            option_active = true;
         }
         else
         {
            if (option_active)
            {
               fprintf(fhandle, "%s\n", arg);
               option_active = false;
            }
            else
            {
               xpc_errprint_func(_("two values in a row encountered"));
               result = false;
               break;
            }
         }
      }
   }
   return result;
}

/******************************************************************************
 * xpc_parse_boolean()
 *------------------------------------------------------------------------*//**
 *
 *    Checks for a boolean option, and parses it if present.
 *
 *    The possible boolean specifications are shown:
 *
 *       -# <tt>--flag</tt>.
 *          In this specification, the flag is the last argument, and thus
 *          has no value provided.
 *          The result is "true" for the option called "flag".
 *       -# <tt>--flag --nextoption</tt>.
 *          In this specification, the flag has no value provided.
 *          The result is "true" for the option called "flag".
 *       -# <tt>--flag true</tt>.
 *          A value for the flag is specified, and it matches "true" exactly
 *          (where "true" represents the user-language's representation of
 *          "true".
 *          The result is "true" for the option called "flag".
 *       -# <tt>--flag false</tt>.
 *          A value for the flag is specified, and it matches "false" exactly
 *          (where "false" represents the user-language's representation of
 *          "false".
 *          The result is "false" for the option called "flag".
 *       -# <tt>--flag parameters</tt>.
 *          A value for the flag is specified, and but it doesn't match one
 *          of the two representations ("true" and "false") allowed.
 *          In this specification, the flag has no value provided.
 *          The result is "true" for the option called "flag".
 *
 * \param argc
 *   main()-style number of command-line arguments.
 *
 * \param argv
 *   main()-style array of command-line arguments.
 *
 * \param argi
 *   The index (in argv[]) of the provide \e optionname command-line
 *   argument.
 *
 * \param optionname
 *    The name of the option this function is checking for.
 *
 * \param [out] boolresult
 *    Provides the actual determined value of the flag, as a pointer to the
 *    boolean result storage value.  IMPORTANT:  The caller should set this
 *    value to 'false' before passing it to the xpc_parse_boolean() call, to
 *    guarantee that the value is initialized.
 *
 * \return
 *    Returns 'true' if the optionname provide was matched by the actual
 *    argument provided.  In this case, the \e boolresult side-effect is
 *    valid and can be used.  Otherwise, it should be ignored as not
 *    applying.  Also, 'false' is returned if the \e boolresult parameter is
 *    a null pointer.
 *
 * \sideeffect
 *    The determined value of the option is returned in the \e boolresult
 *    parameter.
 *
 *//*-------------------------------------------------------------------------*/

cbool_t
xpc_parse_boolean
(
   int argc,
   char ** argv,
   int argi,
   const char * optionname,
   cbool_t * boolresult
)
{
   cbool_t result = not_nullptr_3(argv, optionname, boolresult);
   if (result)
      result = argi > 0 && argi < argc;

   if (result)
      result = not_nullptr(argv[argi]);

   if (result)
      result = strcmp(argv[argi], optionname) == 0;
   else
      xpc_errprint_func(_("null pointer"));

   if (result)
   {
      argi++;                          // get to next argument index
      if (argi >= argc)                // Spec. 1:  no more arguments
         *boolresult = true;
      else
      {
         if (argv[argi][0] == '-' && argv[argi][1] == '-')
            *boolresult = true;        // Spec. 2:  no parameter for flag
         else if (strcmp(argv[argi], _("true")) == 0)
            *boolresult = true;        // Spec. 3:  flag parameter is 'true'
         else if (strcmp(argv[argi], _("false")) == 0)
            *boolresult = false;       // Spec. 4:  flag parameter is 'false'
         else
            *boolresult = true;        // Spec. 5:  no parameter for flag
      }
   }
   return result;
}

/******************************************************************************
 * parse_ini.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
