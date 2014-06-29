/******************************************************************************
 * xpc_strings_ut.c
 *------------------------------------------------------------------------*//**
 *
 * \file          xpc_strings_ut.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2008-05-03
 * \updates       2012-06-09
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    All functions except xpc_path_slash() are covered.  That function is
 *    covered in the filenames_ut.c module.
 *
 *    See the xpc_strings.c module.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>             /* support for special XPC features       */
#include <xpc/errorlogging.h>       /* the module that is being tested here   */
#include <xpc/gettext_support.h>    /* functions, macros, and headers         */
#include <xpc/xstrings.h>           /* C::xpc_string support                  */
XPC_REVISION(xpc_strings_ut)

/******************************************************************************
 * gs_local_help
 *------------------------------------------------------------------------*//**
 *
 *    Brief description of the purpose of this test application.
 *
 *----------------------------------------------------------------------------*/

static const char * gs_local_help =

   "This application unit tests some of the string functions.\n"
   ;

/******************************************************************************
 * xpc_strings_unit_test_00()
 *------------------------------------------------------------------------*//**
 *
 *    xpc_path_slash() is already tested in xpc_filename_unit_test_01(),
 *    and will not be tested here.
 *
 *    However, we will take the opportunity to satisfy our curiousity about
 *    the error-checking in the strncpy() function.
 *
 * \linux
 *    All three variants segfault.
 *
 * \win32
 *    Not yet tested.
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_00
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("strncpy() Crash"));
   if (xpc_test_ready(trt))
   {
      const char * source = "hi";
      char destination[32];
      char * retptr = nullptr;

      /*
       * When we use 'nullptr' in this call, GCC 4.0 complains about using
       * a null argument where a non-null is required.
       *
       *    retptr = strncpy((char *) nullptr, (const char *) source, 2);
       *
       * So, we fool the compiler by using a couple of variables.
       */

      char * nullity = nullptr;
      const char * cnullity = nullptr;

      xpc_infoprint(_("subtest 1: nullptr in strncpy() destination"));
      retptr = strncpy(nullity, (const char *) source, 2);
      result = retptr == nullptr;
      if (result)
      {
         xpc_infoprint(_("subtest 2: nullptr in strncpy() source"));
         retptr = strncpy((char *) destination, cnullity, 2);
         result = retptr == destination;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: nullptr in both strncpy() parameters"));
         retptr = strncpy(nullity, cnullity, 2);
         result = retptr == nullptr;
      }
      if (result)
         xpc_warnprint_func(_("did not crash!"));
      else
         xpc_errprint_func(_("failed, and did not crash!"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_01()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that check a C string for meaningful content.
 *
 *    Deliberate failure conditions are also tested.
 *
 * \test
 *    -  xpc_string_not_void()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_01
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("xpc_string_not_void()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_not_void(nullptr)"));
      result = ! xpc_string_not_void(nullptr);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_not_void(\"\")"));
         result = ! xpc_string_not_void("");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_not_void(\"   \")"));
         result = ! xpc_string_not_void("   ");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_string_not_void(\"\\t\\r\\n)"));
         result = ! xpc_string_not_void("\t\r\n");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_string_not_void(\"a\")"));
         result = xpc_string_not_void("a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: \" xpc_string_not_void(a\")"));
         result = xpc_string_not_void(" a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_string_not_void(\"/\")"));
         result = xpc_string_not_void("/");
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_02()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that check a C string for meaningful content.
 *
 *    Deliberate failure conditions are also tested.
 *
 * \test
 *    -  xpc_string_is_void()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_02
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("xpc_string_is_void()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_is_void(nullptr)"));
      result = xpc_string_is_void(nullptr);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_is_void(\"\")"));
         result = xpc_string_is_void("");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_is_void(\"   \")"));
         result = xpc_string_is_void("   ");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_string_is_void(\"\\t\\r\\n)"));
         result = xpc_string_is_void("\t\r\n");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_string_is_void(\"a\")"));
         result = ! xpc_string_is_void("a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: xpc_string_is_void(\" a\")"));
         result = ! xpc_string_is_void(" a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_string_is_void(\"/\")"));
         result = ! xpc_string_is_void("/");
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_03()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that check compare two C strings for "semantic
 *    matching".
 *
 *    By "semantic" matching, we mean both content and meaningfulness.  By
 *    "meaningfulness", we mean printable characters versus white space.
 *    Null strings and white-space-only strings match no matter what their
 *    content is otherwise.
 *
 *    Deliberate failure conditions are also tested.
 *
 * \test
 *    -  xpc_strings_match()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_03
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("xpc_strings_match()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_strings_match(nullptr, nullptr)"));
      result = xpc_strings_match(nullptr, nullptr);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_strings_match(\"\", \"\")"));
         result = xpc_strings_match("", "");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_strings_match(\"   \", \"   \")"));
         result = xpc_strings_match("   ", "   ");
      }
      if (result)
      {
         xpc_infoprint
         (
            _("subtest 4: xpc_strings_match(\"\\t\\r\\n\", \"\\t\\r\\n\")")
         );
         result = xpc_strings_match("\t\r\n", "\t\r\n");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_strings_match(\"a\", \"a\")"));
         result = xpc_strings_match("a", "a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: xpc_strings_match(\" a\", \" a\")"));
         result = xpc_strings_match(" a", " a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_strings_match(\"/\", \"/\")"));
         result = xpc_strings_match("/", "/");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 8: xpc_strings_match(nullptr, \"\")"));
         result = xpc_strings_match(nullptr, "");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 9: xpc_strings_match(\"\", \"   \")"));
         result = xpc_strings_match("", "   ");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 10: xpc_strings_match(\"   \", nullptr)"));
         result = xpc_strings_match("   ", nullptr);
      }
      if (result)
      {
         xpc_infoprint(_("subtest 11: xpc_strings_match(\"\\t\\r\\n\", \"\")"));
         result = xpc_strings_match("\t\r\n", "");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 12: xpc_strings_match(\"abcde\", \"ABCDE\")"));
         result = ! xpc_strings_match("abcde", "ABCDE");
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_04()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that safely and robustly copy a C string.
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 * \note
 *    If you add a subtest here, add a corresponding subtest to test 05.
 *
 * \test
 *    -  xpc_string_n_copy()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_04
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_string_n_copy()"));
   if (xpc_test_ready(trt))
   {
      char destination[80];            /* won't always use all of it          */
      char * source = "hi!";

      /*
       * Removes confusion, until the unit test gets broken and fails!
       *
       * int current_errlevel = xpc_get_errlevel();
       * xpc_set_errlevel(XPC_ERROR_LEVEL_NONE);
       */

      xpc_infoprint(_("subtest 1: xpc_string_n_copy(nullptr, nullptr, 0"));
      result = ! xpc_string_n_copy(nullptr, nullptr, 0);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_n_copy(destination, nullptr, 0"));
         result = ! xpc_string_n_copy(destination, nullptr, 0);
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_n_copy(destination, source, 0"));
         result = ! xpc_string_n_copy(destination, source, 0);
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_string_n_copy(destination, nullptr, 0"));
         result = ! xpc_string_n_copy(destination, nullptr, 0);
      }
      if (result)
      {
         source = "";
         xpc_infoprint(_("subtest 5: xpc_string_n_copy(destination, \"\", 0"));
         result = ! xpc_string_n_copy(destination, source, 0);
      }
      if (result)
      {
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 6: xpc_string_n_copy(destination, \"1234567890\", 9")
         );
         result = ! xpc_string_n_copy(destination, source, 9);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, "12345678") == 0;
            if (! result)
               xpc_errprint_func(_("even partial copy was incorrect"));
         }
      }
      if (result)
      {
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 7: xpc_string_n_copy(destination, \"1234567890\", 10")
         );
         result = ! xpc_string_n_copy(destination, source, 10);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, "123456789") == 0;
            if (! result)
               xpc_errprint_func(_("even partial copy was incorrect"));
         }
      }

      /*
       * Removes confusion, until the unit test gets broken and fails!
       *
       * xpc_set_errlevel(current_errlevel);
       */

      if (result)
      {
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 8: xpc_string_n_copy(destination, \"1234567890\", 11")
         );
         result = xpc_string_n_copy(destination, source, 11);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, source) == 0;
            if (! result)
               xpc_errprint_func(_("copy succeeded, but was incorrect"));
         }
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_05()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that safely and robustly concatenate C strings.
 *
 *    This test is the same as test 04, but replaces xpc_string_n_copy()
 *    with xpc_string_n_cat().  To do so, the destination first needs a
 *    null terminator installed in destination[0], as per the requirements
 *    of the function.
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 * \note
 *    If you add a subtest here, add a corresponding subtest to test 04.
 *
 * \test
 *    -  xpc_string_n_cat()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_05
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_string_n_cat()"));
   if (xpc_test_ready(trt))
   {
      char destination[80];            /* won't always use all of it          */
      char * source = "hi!";

      /*
       * Removes confusion, until the unit test gets broken and fails!
       *
       * int current_errlevel = xpc_get_errlevel();
       * xpc_set_errlevel(XPC_ERROR_LEVEL_NONE);
       */

      destination[0] = 0;              /* prep the destination for concat.    */
      xpc_infoprint(_("subtest 1: xpc_string_n_cat(nullptr, nullptr, 0"));
      result = ! xpc_string_n_cat(nullptr, nullptr, 0);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_n_cat(destination, nullptr, 0"));
         result = ! xpc_string_n_cat(destination, nullptr, 0);
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_n_cat(destination, source, 0"));
         result = ! xpc_string_n_cat(destination, source, 0);
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_string_n_cat(destination, nullptr, 0"));
         result = ! xpc_string_n_cat(destination, nullptr, 0);
      }
      if (result)
      {
         source = "";
         xpc_infoprint(_("subtest 5: xpc_string_n_cat(destination, \"\", 0"));
         result = ! xpc_string_n_cat(destination, source, 0);
      }
      if (result)
      {
         destination[0] = 0;                    /* prep for concatenation     */
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 6: xpc_string_n_cat(destination, \"1234567890\", 9")
         );
         result = ! xpc_string_n_cat(destination, source, 9);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, "12345678") == 0;
            if (! result)
               xpc_errprint_func(_("even partial concatenation was incorrect"));
         }
      }
      if (result)
      {
         destination[0] = 0;                    /* prep for concatenation     */
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 7: xpc_string_n_cat(destination, \"1234567890\", 10")
         );
         result = ! xpc_string_n_cat(destination, source, 10);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, "123456789") == 0;
            if (! result)
               xpc_errprint_func(_("even partial concatenation was incorrect"));
         }
      }

      /*
       * Removes confusion, until the unit test gets broken and fails!
       *
       * xpc_set_errlevel(current_errlevel);
       */

      if (result)
      {
         destination[0] = 0;                    /* prep for concatenation     */
         source = "1234567890";
         xpc_infoprint
         (
            _("subtest 8: xpc_string_n_cat(destination, \"1234567890\", 11")
         );
         result = xpc_string_n_cat(destination, source, 11);
         xpc_infoprintex(_("result"), destination);
         if (result)
         {
            result = strcmp(destination, source) == 0;
            if (! result)
               xpc_errprint_func(_("concatenation succeeded, but was incorrect"));
         }
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_06()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function that converts an IPv4 address to a string.
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 *    The length of an INET address string (v. 4) is INET_ADDRSTRLEN = 16.
 *
 * \note
 *    Canned addresses (from /usr/include/netinet/in.h):
 *    -  INADDR_ANY.        (unsigned long int) 0x00000000 == 0.0.0.0
 *    -  INADDR_BROADCAST.  (unsigned long int) 0xffffffff == 255.255.255.255
 *    -  INADDR_NONE.       (unsigned long int) 0xffffffff (same as b'cast)
 *    -  INADDR_LOOPBACK.   (unsigned long int) 0x7f000001 == 127.0.0.1
 *    -  INADDR_UNSPEC_GROUP      ((in_addr_t) 0xe0000000) == 224.0.0.0
 *    -  INADDR_ALLHOSTS_GROUP   ((in_addr_t) 0xe0000001) == 224.0.0.1
 *    -  INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002) == 224.0.0.2
 *    -  INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff) == 224.0.0.255
 *
 * \test
 *    -  xpc_ip_to_string()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_06
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_ip_to_string()"));
   if (xpc_test_ready(trt))
   {
      char dest[32];
      xpc_infoprint(_("subtest 1: xpc_ip_to_string(0, nullptr, 0, false)"));
      result = is_nullptr(xpc_ip_to_string(0, nullptr, 0, false));
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_ip_to_string(0, dest, 0, false)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 0, false));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_ip_to_string(0, dest, 1, false)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 1, false));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_ip_to_string(0, dest, 6, false)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 6, false));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_ip_to_string(0, dest, 7, false)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 7, false));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: xpc_ip_to_string(0, dest, 8, false)"));
         result = not_nullptr(xpc_ip_to_string(0, dest, 8, false));
         if (result)
         {
            result = strcmp(dest, "0.0.0.0") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_ip_to_string(0, dest, 8, true)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 8, true));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 8: xpc_ip_to_string(0, dest, 9, true)"));
         result = is_nullptr(xpc_ip_to_string(0, dest, 9, true));
      }
      if (result)
      {
         xpc_infoprint(_("subtest 9: xpc_ip_to_string(0, dest, 10, true)"));
         result = not_nullptr(xpc_ip_to_string(0, dest, 10, true));
         if (result)
         {
            result = strcmp(dest, "0x0.0.0.0") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint(_("subtest 10: xpc_ip_to_string(INADDR_ANY, dest, 10, true)"));
         result = not_nullptr(xpc_ip_to_string(INADDR_ANY, dest, 10, true));
         if (result)
         {
            result = strcmp(dest, "0x0.0.0.0") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 11: xpc_ip_to_string(INADDR_BROADCAST, dest, 15, false)")
         );
         result = is_nullptr(xpc_ip_to_string(INADDR_BROADCAST, dest, 15, false));
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 12: xpc_ip_to_string(INADDR_BROADCAST, dest, 16, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_BROADCAST, dest, 16, false)
         );
         if (result)
         {
            result = strcmp(dest, "255.255.255.255") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 13: xpc_ip_to_string(INADDR_BROADCAST, dest, 18, true)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_BROADCAST, dest, 18, true)
         );
         if (result)
         {
            result = strcmp(dest, "0xff.ff.ff.ff") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 14: xpc_ip_to_string(INADDR_LOOPBACK, dest, 9, false)")
         );
         result = is_nullptr(xpc_ip_to_string(INADDR_LOOPBACK, dest, 9, false));
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 15: xpc_ip_to_string(INADDR_LOOPBACK, dest, 10, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_LOOPBACK, dest, 10, false)
         );
         if (result)
         {
            result = strcmp(dest, "127.0.0.1") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 16: xpc_ip_to_string(INADDR_LOOPBACK, dest, 12, true)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_LOOPBACK, dest, 18, true)
         );
         if (result)
         {
            result = strcmp(dest, "0x7f.0.0.1") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
       _("subtest 17: xpc_ip_to_string(htonl(INADDR_LOOPBACK), dest, 10, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(htonl(INADDR_LOOPBACK), dest, 10, false)
         );
         if (result)
         {
            xpc_infoprintex(_("result"), dest);
            result = strcmp(dest, "1.0.0.127") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 18: xpc_ip_to_string(INADDR_UNSPEC_GROUP, dest, 10, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_UNSPEC_GROUP, dest, 10, false)
         );
         if (result)
         {
            result = strcmp(dest, "224.0.0.0") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
       _("subtest 19: xpc_ip_to_string(INADDR_ALLHOSTS_GROUP, dest, 10, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_ALLHOSTS_GROUP, dest, 10, false)
         );
         if (result)
         {
            result = strcmp(dest, "224.0.0.1") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
       _("subtest 20: xpc_ip_to_string(INADDR_ALLRTRS_GROUP, dest, 10, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_ALLRTRS_GROUP, dest, 10, false)
         );
         if (result)
         {
            result = strcmp(dest, "224.0.0.2") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (result)
      {
         xpc_infoprint
         (
       _("subtest 21: xpc_ip_to_string(INADDR_MAX_LOCAL_GROUP, dest, 12, false)")
         );
         result = not_nullptr
         (
            xpc_ip_to_string(INADDR_MAX_LOCAL_GROUP, dest, 12, false)
         );
         if (result)
         {
            result = strcmp(dest, "224.0.0.255") == 0;
            if (! result)
            {
               xpc_errprint_func(_("succeeded, but string was incorrect"));
               xpc_infoprintex(_("string was"), dest);
            }
         }
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_07()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function that converts a number IP address string into an
 *    IPv4 address.
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 * \test
 *    -  xpc_string_to_ip()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_07
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_string_to_ip()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_to_ip(nullptr)"));
      result = xpc_string_to_ip(nullptr) == 0;
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_to_ip(\"\")"));
         result = xpc_string_to_ip("") == 0;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_to_ip(\"0.0.0.0\")"));
         result = xpc_string_to_ip("0.0.0.0") == 0;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 4: xpc_string_to_ip(\"0x0.0.0.0\")"));
         result = xpc_string_to_ip("0x0.0.0.0") == 0;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_string_to_ip(\"INADDR_BROADCAST\")"));
         result = xpc_string_to_ip("255.255.255.255") == INADDR_BROADCAST;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: xpc_string_to_ip(\"0xINADDR_BROADCAST\")"));
         result = xpc_string_to_ip("0xff.ff.ff.ff") == INADDR_BROADCAST;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_string_to_ip(\"INADDR_LOOPBACK\")"));
         result = xpc_string_to_ip("127.0.0.1") == INADDR_LOOPBACK;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 8: xpc_string_to_ip(\"0xINADDR_LOOPBACK\")"));
         result = xpc_string_to_ip("0x7f.0.0.1") == INADDR_LOOPBACK;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 9: xpc_string_to_ip(\"INADDR_LOOPBACK\")"));
         result = xpc_string_to_ip("127.00.000.01") == INADDR_LOOPBACK;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 10: xpc_string_to_ip(\"0xINADDR_LOOPBACK\")"));
         result = xpc_string_to_ip("0x7f.00.00.001") == INADDR_LOOPBACK;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 11: xpc_string_to_ip(\"68.58.236.1\")"));
         result = xpc_string_to_ip("68.58.236.1") == 0x443aec01;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 12: xpc_string_to_ip(\"0x44.3a.ec.01\")"));
         result = xpc_string_to_ip("0x44.3a.ec.01") == 0x443aec01;
      }
      if (result)
      {
         xpc_infoprint(_("subtest 13: xpc_string_to_ip(\"0x0.0.0.0\")"));
         result = xpc_string_to_ip("0x0.0.0.0") == 0;
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_08()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function(s) that check compare two C strings for "token
 *    matching".
 *
 *    By "token" matching, we mean both abbreviated content and
 *    meaningfulness.  By "meaningfulness", we mean printable characters
 *    versus white space.  Null strings and white-space-only strings are
 *    rejected no matter what their content is otherwise.  This policy is
 *    contrary to xpc_strings_match().
 *
 *    Deliberate failure conditions are also tested.
 *
 * \note
 *    This test is exactly like test 03, with result-adjustments for the
 *    differences in behavior.  Test 09 below adds more testing condition
 *    more suitable for checking partial matches.
 *
 * \test
 *    -  xpc_string_tokens_match()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_08
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("xpc_string_tokens_match()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_tokens_match(nullptr, nullptr)"));
      result = ! xpc_string_tokens_match(nullptr, nullptr);
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_tokens_match(\"\", \"\")"));
         result = ! xpc_string_tokens_match("", "");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 3: xpc_string_tokens_match(\"   \", \"   \")"));
         result = ! xpc_string_tokens_match("   ", "   ");
      }
      if (result)
      {
         xpc_infoprint
         (
          _("subtest 4: xpc_string_tokens_match(\"\\t\\r\\n\", \"\\t\\r\\n\")")
         );
         result = ! xpc_string_tokens_match("\t\r\n", "\t\r\n");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 5: xpc_string_tokens_match(\"a\", \"a\")"));
         result = xpc_string_tokens_match("a", "a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 6: xpc_string_tokens_match(\" a\", \" a\")"));
         result = xpc_string_tokens_match(" a", " a");

         /* TODO:  have it strip/ignore leading and trailing white-space */
      }
      if (result)
      {
         xpc_infoprint(_("subtest 7: xpc_string_tokens_match(\"/\", \"/\")"));
         result = xpc_string_tokens_match("/", "/");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 8: xpc_string_tokens_match(nullptr, \"\")"));
         result = ! xpc_string_tokens_match(nullptr, "");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 9: xpc_string_tokens_match(\"\", \"   \")"));
         result = ! xpc_string_tokens_match("", "   ");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 10: xpc_string_tokens_match(\"   \", nullptr)"));
         result = ! xpc_string_tokens_match("   ", nullptr);
      }
      if (result)
      {
         xpc_infoprint
         (
            _("subtest 11: xpc_string_tokens_match(\"\\t\\r\\n\", \"\")")
         );
         result = ! xpc_string_tokens_match("\t\r\n", "");
      }
      if (result)
      {
         xpc_infoprint
         (
            _("subtest 12: xpc_string_tokens_match(\"abcde\", \"ABCDE\")")
         );
         result = ! xpc_string_tokens_match("abcde", "ABCDE");
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

/******************************************************************************
 * xpc_strings_unit_test_09()
 *------------------------------------------------------------------------*//**
 *
 *    More tests for the functions that compare two C strings for "token
 *    matching".
 *
 *    Deliberate failure conditions are also tested.
 *
 * \note
 *    This test is an extension of test 08.
 *
 * \test
 *    -  xpc_string_tokens_match()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_09
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default */
   xpc_test_set_current_name(trt, _("xpc_string_tokens_match()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_tokens_match(\"abc\", \"abc\")"));
      result = xpc_string_tokens_match("abc", "abc");
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_tokens_match(\"abc\", \"ab\")"));
         result = xpc_string_tokens_match("abc", "ab");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_tokens_match(\"abc\", \"a\")"));
         result = xpc_string_tokens_match("abc", "a");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_tokens_match(\"ab\", \"abc\")"));
         result = ! xpc_string_tokens_match("ab", "abc");
      }
      if (result)
      {
         xpc_infoprint(_("subtest 2: xpc_string_tokens_match(\"a\", \"abc\")"));
         result = ! xpc_string_tokens_match("a", "abc");
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

#if UNIT_TEST_XPC_STRING_N_CAT_R_IMP

/******************************************************************************
 * xpc_strings_unit_test_05b()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the internal function that concatenates C strings in a
 *    threadsafe manner.
 *
 *    This test is similar to test 05, but replaces xpc_string_n_cat()
 *    with xpc_string_n_cat_r_impl().
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 * \note
 *    Keep this test at the end of the list, to keep the numbering straight.
 *
 * \test
 *    -  xpc_string_n_cat_r_impl()
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

static cbool_t
xpc_strings_unit_test_05b
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_string_n_cat_r_impl()"));
   if (xpc_test_ready(trt))
   {
      size_t ncopied;                           /* the number of chars copied */
      xpc_infoprint(_("subtest 1: xpc_string_n_cat_r_impl(nullptr, nullptr, 0"));
      ncopied = xpc_string_n_cat_r_impl(nullptr, nullptr, 0);
      result = ncopied == 0;
      if (result)
      {
         char * destination = nullptr;          /* fake a bogus pointer       */
         xpc_infoprint(_("subtest 2: xpc_string_n_cat_r_impl(&nullptr, nullptr, 0"));
         ncopied = xpc_string_n_cat_r_impl(&destination, nullptr, 0);
         result = ncopied == 0;
      }
      if (result)
      {
         /*
          * Can't do the following due to the C conventions of array
          * pointers.  Therefore, we have to create a pointer to the first
          * element of the array and pass the address of that pointer to
          * xpc_string_n_cat_r_impl().
          *
          * ncopied = xpc_string_n_cat_r_impl(&destination, nullptr, 0);
          *
          *               Will not work-------^
          */

         char destination[2];                   /* fake a valid buffer        */
         char * dptr = destination;             /* see the comment above      */
         xpc_infoprint
         (
            _("subtest 3: xpc_string_n_cat_r_impl(destination, nullptr, 0")
         );
         ncopied = xpc_string_n_cat_r_impl(&dptr, nullptr, 0);
         result = ncopied == 0;
      }
      if (result)
      {
         char destination[2];                   /* fake a valid buffer        */
         char source[2];                        /* fake a valid source        */
         char * dptr = destination;             /* see the comment above      */
         xpc_infoprint
         (
            _("subtest 4: xpc_string_n_cat_r_impl(destination, nullptr, 0")
         );
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, 0);
         result = ncopied == 0;
      }
      if (result)
      {
         char destination[8];                   /* 8 chars including null     */
         const char * source = "0123456789";    /* 11 chars including null    */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 5: xpc_string_n_cat_r_impl(8 chars, 10 chars, 8"));
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, 8);
         result = ncopied == (8-1);             /* note how null is ignored   */
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, source, ncopied) == 0;

            if (result)                         /* a side-effect sanity-check */
               result = dptr == (destination + (8-1));
         }
         else
            xpc_errprint_func(destination);
      }
      if (result)
      {
         char destination[8];                   /* 8 chars including null     */
         const char * source = "012345678";     /* 10 chars including null    */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 6: xpc_string_n_cat_r_impl(8 chars, 9 chars, 8"));
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, sizeof(destination));
         result = ncopied == (sizeof(destination) - 1);
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, "0123456", ncopied) == 0;
         }
         else
            xpc_errprint_func(destination);
      }
      if (result)
      {
         char destination[8];                   /* 8 chars including null     */
         const char * source = "01234567";      /* 9 chars including null     */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 7: xpc_string_n_cat_r_impl(8 chars, 8 chars, 8"));
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, sizeof(destination));
         result = ncopied == (sizeof(destination) - 1);
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, "0123456", ncopied) == 0;
         }
         else
            xpc_errprint_func(destination);
      }
      if (result)
      {
         char destination[8];                   /* 8 chars including null     */
         const char * source = "0123456";       /* 8 chars including null     */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 8: xpc_string_n_cat_r_impl(8 chars, 7 chars, 8"));
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, sizeof(destination));
         result = ncopied == (sizeof(destination) - 1);
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, "0123456", ncopied) == 0;
         }
         else
            xpc_errprint_func(destination);
      }
      if (result)
      {
         char destination[8];                   /* 8 chars including null     */
         const char * source = "012345";        /* 7 chars including null     */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 9: xpc_string_n_cat_r_impl(8 chars, 6 chars, 8"));
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, sizeof(destination));
         result = ncopied == strlen(source);
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, "012345", ncopied) == 0;
         }
         else
            xpc_errprint_func(destination);
      }
      if (result)
      {
         /*
          * In this subtest, we'll repeatedly add a character, checking for
          * the status of various items after each call to
          * xpc_string_n_cat_r_impl().
          */

         char destination[8];                   /* 8 chars including null     */
         const char * source;                   /* fill in on the fly         */
         char * dptr = destination;             /* see the comment above      */
         strcpy(destination, "-------");        /* fill it for a robust test  */
         destination[0] = 0;                    /* prep for concatenation     */
         xpc_infoprint(_("subtest 10a: xpc_string_n_cat_r_impl(repeated)"));
         source = "0";
         ncopied = xpc_string_n_cat_r_impl(&dptr, source, sizeof(destination));
         result = ncopied == strlen(source);
         if (result)
         {
            xpc_infoprint_func(destination);
            result = destination[ncopied] == 0; /* null right where expected  */
            if (result)
               result = strncmp(destination, "0", ncopied) == 0;
         }
         else
            xpc_errprint_func(destination);

#define SZ(x)     (sizeof(destination) - (x))

         if (result)
         {
            xpc_infoprint(_("subtest 10b:"));
            source = "1";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(1));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 2;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10c:"));
            source = "2";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(2));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 3;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10d:"));
            source = "3";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(3));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 4;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10e:"));
            source = "4";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(4));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 5;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10f:"));
            source = "5";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(5));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 6;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10g:"));
            source = "6";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(6));
            result = ncopied == strlen(source);
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 7;
               if (result)
                  result = strcmp(destination, "0123456") == 0;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10h:"));
            source = "7";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(7));
            result = ncopied == 0;
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 7;
               if (result)
                  result = strcmp(destination, "0123456") == 0;
            }
            else
               xpc_errprint_func(destination);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10i:"));
            source = "8";
            ncopied = xpc_string_n_cat_r_impl(&dptr, source, SZ(8));
            result = ncopied == 0;
            if (result)
            {
               xpc_infoprint_func(destination);
               result = strlen(destination) == 7;
               if (result)
                  result = strcmp(destination, "0123456") == 0;
            }
            else
               xpc_errprint_func(destination);
         }
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

#endif   /* UNIT_TEST_XPC_STRING_N_CAT_R_IMP */

/******************************************************************************
 * xpc_strings_unit_test_05c()
 *------------------------------------------------------------------------*//**
 *
 *    Tests the function that concatenates C strings in a threadsafe manner.
 *
 *    This test is similar to test 05b, but it tests xpc_string_n_cat_r()
 *    instead of the function that implements it, xpc_string_n_cat_r_impl().
 *
 *    Deliberate failure conditions are also tested.  Error messages are
 *    shown, unless the --quiet option is specified.  But they are expected
 *    errors, and so are not errors in the context of this unit test.
 *
 * \note
 *    Keep this test at the end of the list, to keep the numbering straight.
 *
 * \test
 *    -  xpc_string_n_cat_r()
 *    -  xpc_string_init()
 *    -  xpc_string_validate() [indirect]
 *    -  xpc_string_n_cat_r_impl() [indirect]
 *    -  xpc_string_c_str() [indirect]
 *    -  xpc_string_size() [indirect]
 *    -  xpc_string_max() [indirect]
 *
 * \return
 *    Returns 'true' if the test succeeded.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning ( disable : 4701 )
#endif

static cbool_t
xpc_strings_unit_test_05c
(
   xpc_test_row_t * trt    /**< Points to the item describing the test.       */
)
{
   cbool_t result;
   xpc_test_set_ready(trt, true);      /* Nothing to set up, true by default  */
   xpc_test_set_current_name(trt, _("xpc_string_n_cat_r()"));
   if (xpc_test_ready(trt))
   {
      xpc_infoprint(_("subtest 1: xpc_string_n_cat_r(nullptr, nullptr"));
      result = ! xpc_string_n_cat_r(nullptr, nullptr);
      if (result)
      {
         /*
          * Now for a series of tests of xpc_string_init().
          */

         xpc_string_t dest_string;
         char buffer[10];
         size_t bsize = 10;
         xpc_infoprint(_("subtest 2: xpc_string_init() and xpc_string_validate()"));
         result = ! xpc_string_init(nullptr, nullptr, 0);
         if (result)
            result = ! xpc_string_init(nullptr, buffer, 0);

         if (result)
            result = ! xpc_string_init(nullptr, &buffer[0], 0);

         if (result)
            result = ! xpc_string_validate(nullptr);

         if (result)
            result = ! xpc_string_init(&dest_string, nullptr, 0);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(&dest_string, nullptr, 0);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(&dest_string, buffer, 0);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(nullptr, nullptr, bsize);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(nullptr, buffer, bsize);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(nullptr, &buffer[0], bsize);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(&dest_string, nullptr, bsize);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = ! xpc_string_init(&dest_string, nullptr, bsize);

         if (result)
            result = ! xpc_string_validate(&dest_string);

         if (result)
            result = xpc_string_init(&dest_string, buffer, bsize);

         if (result)
            result = xpc_string_validate(&dest_string);

         if (result)
         {
            /*
             * As far as I can tell, there is no way we can get here without
             * initializing dest_string.  The problem is that Visual Studio
             * doesn't detect that this value is passed to an initialization
             * function.
             *
             * Odd, the pragma doesn't disable this warning unless it is put
             * //before// the function declaration.  Odd.
             */

            result = dest_string.str_buffer[0] == 0;
            if (result)
               result = dest_string.str_buffer == buffer;

            if (result)
               result = xpc_string_c_str(&dest_string) == buffer;

            if (result)
               result = xpc_string_max(&dest_string) == bsize;

            if (result)
               result = xpc_string_size(&dest_string) == 0;
         }
         if (! result)
            xpc_errprint_func(_("xpc_string_init()/xpc_string_validate() failed"));
      }
      if (result)
      {
          xpc_string_t dest_string;

         /*
          * One cannot, in general, use a pointer declared as follows in a
          * call to xpc_string_init().
          *
          *    char * sample = "test";
          *
          * The reason is that xpc_string_init() will try to put a null
          * terminator in sample[0].  But this array is actually in BSS [I
          * think that's the neme of it], and so is off-limits to writing by
          * the application.  Hence, a segfault results.
          */

         char sample[5];                        /* the content doesn't matter */
         xpc_infoprint(_("subtest 3: xpc_string_n_cat_r(dest, nullptr"));
         result = xpc_string_init(&dest_string, sample, sizeof(sample));
         if (result)
            result = ! xpc_string_n_cat_r(&dest_string, nullptr);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char sample[5];                        /* the content doesn't matter */
         xpc_infoprint(_("subtest 4: xpc_string_n_cat_r(nullptr, string"));
         result = xpc_string_init(&dest_string, sample, sizeof(sample));
         if (result)
            result = ! xpc_string_n_cat_r(nullptr, sample);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         char * source = "0123456789";          /* 11 chars including null    */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 5: xpc_string_n_cat_r(8 chars, 10 chars, 8"));
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = ! xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strncmp(dest, source, sizeof(dest)-1) == 0;
         }
         else
            xpc_errprint_func(dest);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         const char * source = "012345678";     /* 10 chars including null    */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 6: xpc_string_n_cat_r(8 chars, 9 chars, 8"));
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = ! xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strcmp(dest, "0123456") == 0;
            if (result)
               result = strncmp(dest, source, sizeof(dest)-1) == 0;
         }
         else
            xpc_errprint_func(dest);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         const char * source = "01234567";      /* 9 chars including null     */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 7: xpc_string_n_cat_r(8 chars, 8 chars, 8"));
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = ! xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strncmp(dest, source, sizeof(dest)-1) == 0;
         }
         else
            xpc_errprint_func(dest);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         const char * source = "0123456";       /* 8 chars including null     */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 8: xpc_string_n_cat_r(8 chars, 7 chars, 8"));
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strncmp(dest, source, sizeof(dest)-1) == 0;
         }
         else
            xpc_errprint_func(dest);
      }
      if (result)
      {
         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         const char * source = "012345";        /* 7 chars including null     */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 9: xpc_string_n_cat_r(8 chars, 6 chars, 8"));
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strncmp(dest, "012345", result) == 0;
         }
         else
            xpc_errprint_func(dest);
      }
      if (result)
      {
         /*
          * In this subtest, we'll repeatedly add a character, checking for
          * the status of various items after each call to
          * xpc_string_n_cat_r().
          */

         xpc_string_t dest_string;              /* represents our "string")   */
         char dest[8];                          /* 8 chars including null     */
         const char * source;                   /* fill in on the fly         */
         strcpy(dest, "-------");               /* fill it for a robust test  */
         xpc_infoprint(_("subtest 10a: xpc_string_n_cat_r(repeated)"));
         source = "0";
         result = xpc_string_init(&dest_string, dest, sizeof(dest));
         if (result)
            result = xpc_string_n_cat_r(&dest_string, source);

         if (result)
         {
            xpc_infoprint_func(dest);
            result = strncmp(dest, "0", result) == 0;
         }
         else
            xpc_errprint_func(dest);

         if (result)
         {
            xpc_infoprint(_("subtest 10b:"));
            source = "1";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strncmp(dest, "01", result) == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10c:"));
            source = "2";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strncmp(dest, "012", result) == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10d:"));
            source = "3";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strncmp(dest, "0123", result) == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10e:"));
            source = "4";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strncmp(dest, "01234", result) == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10f:"));
            source = "5";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strncmp(dest, "012345", result) == 0;

               if (result)
                  result = strcmp(xpc_string_c_str(&dest_string), "012345") == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10g:"));
            source = "6";
            result = xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strlen(dest) == 7;
               if (result)
                  result = strcmp(dest, "0123456") == 0;

               if (result)
                  result = strcmp(xpc_string_c_str(&dest_string), "0123456") == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10h:"));
            source = "7";
            result = ! xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strlen(dest) == 7;
               if (result)
                  result = strcmp(dest, "0123456") == 0;

               if (result)
                  result = strcmp(xpc_string_c_str(&dest_string), "0123456") == 0;
            }
            else
               xpc_errprint_func(dest);
         }
         if (result)
         {
            xpc_infoprint(_("subtest 10i:"));
            source = "8";
            result = ! xpc_string_n_cat_r(&dest_string, source);
            if (result)
            {
               xpc_infoprint_func(dest);
               result = strlen(dest) == 7;
               if (result)
                  result = strcmp(dest, "0123456") == 0;

               if (result)
                  result = strcmp(xpc_string_c_str(&dest_string), "0123456") == 0;
            }
            else
               xpc_errprint_func(dest);
         }
      }
      if (! result)
         xpc_errprint_func(_("failed"));
   }
   else
   {
      result = false;
      xpc_errprint(_("test not ready"));
   }
   xpc_test_set_result(trt, result);
   return result;
}

#ifdef _MSC_VER
#pragma warning ( 4 : 4701 )
#endif

/******************************************************************************
 * xpc_test_function_t gs_test_array []
 *------------------------------------------------------------------------*//**
 *
 *    Provide the list of tests function pointers.
 *
 *    There's no need to get fancy.  A null-terminated array is quite
 *    sufficient as a compile-time test array.
 *
 *----------------------------------------------------------------------------*/

xpc_test_function_t gs_test_array [] =
{
   xpc_strings_unit_test_01,
   xpc_strings_unit_test_02,
   xpc_strings_unit_test_03,
   xpc_strings_unit_test_04,
   xpc_strings_unit_test_05,
   xpc_strings_unit_test_06,
   xpc_strings_unit_test_07,
   xpc_strings_unit_test_08,
   xpc_strings_unit_test_09,  /* currently, this is the "latest" test         */
   xpc_strings_unit_test_05c, /* "new" test, keep it after the "latest" test  */

   /*
    * Conditionally-compiled unit tests.
    */

#if UNIT_TEST_XPC_STRING_N_CAT_R_IMP
   xpc_strings_unit_test_05b,
#endif

   nullptr,                   /* terminates the list of unit tests            */
   xpc_strings_unit_test_00   /* deliberately crashes, so we will not run it  */
};


/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    Provides the entry point into this unit-test application.
 *
 * \return
 *    Returns either EXIT_SUCCESS or EXIT_FAILURE.
 *
 *//*-------------------------------------------------------------------------*/

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   cbool_t result = false;
   const char * ver = xpc_app_version_number(1);         /* make app version  */
   xpc_test_arguments_t * arguments = xpc_test_arguments /* parse cmd-line    */
   (
      "xpc_strings_ut", ver, gs_local_help, argc, argv
   );
   if (not_nullptr(arguments))
   {
      int argi = 1;
      cbool_t done = false;
      while (argi < argc)
      {
         if (strcmp(argv[argi], "--xxxxxxxxxx") == 0)
         {
            done = true;
         }
         argi++;
      }
      if (! done)
      {
         result = xpc_test_run(gs_test_array, &arguments->tat_test_options);
      }
   }
   xpc_infoprintex("xpc_strings_ut", result ? _("passed") : _("failed"));
   return result ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/******************************************************************************
 * xpc_strings_ut.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *-----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/

