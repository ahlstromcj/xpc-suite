/******************************************************************************
 * errorlog_macros.h
 *------------------------------------------------------------------------*//**
 *
 * \file          errorlog_macros.h
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-09-11 to 2011-01-03
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module extracts the macros from errorlogging.c so that we can
 *    re-use them in a C++ error-logging class.
 *
 *//*-------------------------------------------------------------------------*/

/******************************************************************************
 * Strings for the errorlogging options
 *------------------------------------------------------------------------*//**
 *
 *    This section defines the names of error-logging options (in English)
 *    and a few short tokens.
 *
 *    Might as well make the options more easily modifiable.  Also, a
 *    couple of additional unadvertised spellings are included to avoid puzzles.
 *
 *    Please note that only the first symbol is shown in this Doxygen
 *    documentation.  There are many more symbols:
 *
\verbatim
         _HELP             "help"
         _STDOUT           "stdout"
         _STDERR           "stderr"
         _UNBUFFERED       "unbuffered"
         _BUFFERED         "buffered"
         _UNBUFFER         "unbuffer"
         _BUFFER           "buffer"
         _LOG              "log"
         _DAEMON           "daemon"
         _QUIET            "quiet"
         _SILENT           "silent"
         _ERROR            "error"
         _ERRORS           "errors"
         _WARN             "warn"
         _WARNINGS         "warnings"
         _SDEBUG           "debug"
         _INFO             "info"
         _INFORMATION      "information"
         _ALL              "all"
         _VERBOSE          "verbose"
         _COLOR            "color"
         _NOCOLOR          "nocolor"
         _NO_COLOR         "no-color"
         _MONO             "mono"
         _TIMESTAMPS       "timestamps"
         _TIME_STAMPS      "time-stamps"
         _NO_TIMESTAMPS    "no-timestamps"
         _NO_TIME_STAMPS   "no-time-stamps"
         _SYNCH            "synch"
         _VERSION          "version"
         _NOT_APPLICABLE   "N/A"
\endverbatim
 *
 *    All of these symbols are defined in the C module, and so are not
 *    visible outside of the errorlogging.c module.
 *
 * \todo
 *    Is there any way to detect when redirecting stdout or stderr to a
 *    file, so that we can automatically set the --no-color option to avoid
 *    getting those darned escape sequences in the output?  I saw someone do
 *    that, but forget where!
 *
 *//*-------------------------------------------------------------------------*/

#define _HELP                       "help"
#define _STDOUT                     "stdout"
#define _STDERR                     "stderr"
#define _UNBUFFERED                 "unbuffered"
#define _BUFFERED                   "buffered"
#define _UNBUFFER                   "unbuffer"
#define _BUFFER                     "buffer"
#define _LOG                        "log"
#define _APPEND                     "append"
#define _SYSLOG                     "syslog"
#define _NO_SYSLOG                  "no-syslog"
#define _DAEMON                     "daemon"
#define _QUIET                      "quiet"
#define _SILENT                     "silent"
#define _ERROR                      "error"
#define _ERRORS                     "errors"
#define _WARN                       "warn"
#define _WARNINGS                   "warnings"
#define _SDEBUG                     "debug"
#define _INFO                       "info"
#define _INFORMATION                "information"
#define _ALL                        "all"
#define _VERBOSE                    "verbose"
#define _COLOR                      "color"
#define _NOCOLOR                    "nocolor"
#define _NO_COLOR                   "no-color"
#define _NC                         "-nc"
#define _MONO                       "mono"
#define _TIMESTAMPS                 "timestamps"
#define _TIME_STAMPS                "time-stamps"
#define _NO_TIMESTAMPS              "no-timestamps"
#define _NO_TIME_STAMPS             "no-time-stamps"
#define _SYNCH                      "synch"
#define _NO_SYNCH                   "no-synch"
#define _VERSION                    "version"
#define _NOT_APPLICABLE             "N/A"
#define CMD(x)                      "--" x

/******************************************************************************
 * ERRL_FMT_BASIC_MESSAGE
 *------------------------------------------------------------------------*//**
 *
 *    These strings are used in private string formatting.
 *
 * \private
 *    This string is private, and cannot be seen outside the errorlogging.c
 *    module.
 *
 *//*-------------------------------------------------------------------------*/

#define ERRL_FMT_BASIC_MESSAGE            "%s %s\n"

/******************************************************************************
 * ERRL_FMT_TIMESTAMP_MESSAGE
 *------------------------------------------------------------------------*//**
 *
 *    These strings are used in private string formatting.
 *
 * \private
 *    This string is private, and cannot be seen outside the errorlogging.c
 *    module.
 *
 *//*-------------------------------------------------------------------------*/

#define ERRL_FMT_TIMESTAMP_MESSAGE        "%s [%d.%06d] %s\n"

/******************************************************************************
 * ERRL_FMT_EXT_MESSAGE
 *------------------------------------------------------------------------*//**
 *
 *    Used in private string formatting.
 *
 * \private
 *    This string is private, and cannot be seen outside the errorlogging.c
 *    module.
 *
 *//*-------------------------------------------------------------------------*/

#define ERRL_FMT_EXT_MESSAGE              "%s %s: --%s\n"

/******************************************************************************
 * Marker characters
 *------------------------------------------------------------------------*//**
 *
 *    Provides a marker character for debugging information; there are
 *    others, but xxx_marker functions are provided for external usage.
 *
 *    This section provides easily visible characters to allow the user to
 *    see which of the error-logging functions wrote the output string.
 *
 *    Truthfully, though, it is easier for the user to remember these
 *    characters and put them in as is.  Therefore, never changes the values
 *    defined below!
 *
 *    These functions provided a way for the caller to use the same kind of
 *    colored or non-colored markers as used privately.
 *
 *    Note that a user string, "-" is provided so that the user can mark
 *    text with similar conventions, yet know it did not come from the
 *    library routines.  This string should not be used by library code,
 *    generally.  The cut_base module (POSIX C++ Wrapper library) is an
 *    exception.
 *
\verbatim
         ERRL_STR_DEBUG              >
         ERRL_STR_INFO               *
         ERRL_STR_WARN               !
         ERRL_STR_ERROR              ?
         ERRL_STR_USER               -
         ERRL_STR_PRINT              +
         ERRL_STR_SEPARATOR         " "
         ERRL_STR_EXTRA_SEPARATOR    :
         ERRL_STR_EXTRA             ": "

         COLOR_STR_DEBUG            \x1B[01;35m>\x1B[00m    magenta
         COLOR_STR_INFO             \x1B[01;34m*\x1B[00m    blue
         COLOR_STR_WARN             \x1B[01;32m!\x1B[00m    green
         COLOR_STR_ERROR            \x1B[01;31m?\x1B[00m    red
         COLOR_STR_USER             \x1B[01;36m-\x1B[00m    cyan
         COLOR_STR_PRINT            \x1B[01;37m+\x1B[00m    white
         COLOR_STR_LABEL_START      \x1B[01;36m             cyan
         COLOR_STR_END              \x1B[00m
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

#define ERRL_STR_DEBUG              ">"            /* debug info  */
#define ERRL_STR_INFO               "*"            /* information */
#define ERRL_STR_WARN               "!"            /* warning     */
#define ERRL_STR_ERROR              "?"            /* error       */
#define ERRL_STR_USER               "-"            /* user        */
#define ERRL_STR_PRINT              "+"            /* print()     */
#define ERRL_STR_SEPARATOR          " "            /* separator   */
#define ERRL_STR_EXTRA_SEPARATOR    ":"            /* separator   */
#define ERRL_STR_EXTRA              ": "           /* separator   */

#ifdef WIN32

#define COLOR_STR_DEBUG             "\x1B[01;35m>\x1B[0m"
#define COLOR_STR_INFO              "\x1B[01;34m*\x1B[0m"
#define COLOR_STR_WARN              "\x1B[01;32m!\x1B[0m"
#define COLOR_STR_ERROR             "\x1B[01;31m?\x1B[0m"
#define COLOR_STR_USER              "\x1B[01;36m-\x1B[0m"
#define COLOR_STR_PRINT             "\x1B[01;37m+\x1B[0m"
#define COLOR_STR_LABEL_START       "\x1B[01;36m"
#define COLOR_STR_END               "\x1B[0m"

#else

#define COLOR_STR_DEBUG             "\x1B[01;35m>\x1B[00m"
#define COLOR_STR_INFO              "\x1B[01;34m*\x1B[00m"
#define COLOR_STR_WARN              "\x1B[01;32m!\x1B[00m"
#define COLOR_STR_ERROR             "\x1B[01;31m?\x1B[00m"
#define COLOR_STR_USER              "\x1B[01;36m-\x1B[00m"
#define COLOR_STR_PRINT             "\x1B[01;37m+\x1B[00m"
#define COLOR_STR_LABEL_START       "\x1B[01;36m"
#define COLOR_STR_END               "\x1B[00m"

#endif

/******************************************************************************
 * XPC_STRERROR_BUFLEN
 *------------------------------------------------------------------------*//**
 *
 *    Provides a hard-wired buffer size.
 *
 * \hardwired
 *    XPC_STRERROR_BUFLEN is useful in putting a system error message into an
 *    array.  Visual C will not allow using a const int as an array size.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_STRERROR_BUFLEN      1024           /* VC won't handle const int  */

/******************************************************************************
 * errorlog_macros.h
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
