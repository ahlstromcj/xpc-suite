/******************************************************************************
 * rowset.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          rowset.cpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-05-30 to 2010-12-10
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides two objects useful in managing database results:
 *
 *       -  xpc::row
 *       -  xpc::rowset
 *
 *    Some external functions in the xpc namespace are also provided.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>				/* XPC_REVISION macro (indirectly)		*/
#include <xpc/rowset.hpp>              /* xpc::row and xpc::rowset            */
XPC_REVISION(rowset)                   /* show_rowset_info()                  */

/******************************************************************************
 * XPC_IKEYMAX()
 *------------------------------------------------------------------------*//**
 *
 *    Defines the maximum integer-index, which allows for 5 decimal digits.
 *
 *    We don't anticipate ever needing to count so many records.  If we're
 *    wrong, change it here.
 *
 *//*-------------------------------------------------------------------------*/

#define XPC_IKEYMAX    99999

namespace xpc
{

/******************************************************************************
 * skey(int)
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to a 5-digit zero-padded string.
 *
 *    In some cases, we will want to use numeric keys to assure
 *    the ordering of otherwise unnamed results.  This function allows us
 *    to do so, at the expense of speed and of limiting the number of
 *    results to 100 thousand.
 *
 * \param k
 *    The integer that is to be converted to a string to serve as the
 *    lookup value.
 *
 * \return
 *    Returns the integer converted to a zero-padded string.  If the
 *    key is outside the legal range (0 to 99999), an empty string is
 *    returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
skey (int k)
{
   std::string result("");
   if ((k >= 0) && (k < XPC_IKEYMAX+1))
   {
      char temp[8];
      sprintf(temp, "%5.5d", k);
      result = temp;
   }
   return result;
}

/******************************************************************************
 * skey(string)
 *------------------------------------------------------------------------*//**
 *
 *    Converts a string representing an integer to a 5-digit zero-padded
 *    string.
 *
 *    This function calls ikey() to convert the string to an integer, and
 *    then calls the integer version of skey().  This is the simplest way to
 *    guarantee xpc::rowset's canonical 5-digit "PKID" format.
 *
 * \note
 *    If the string does not represent an integer, "00000" is returned.
 *    There is no detection of this erroneous case.
 *
 * \param digits
 *    The string that is meant to represent an integer.  Examples are "2"
 *    and "02", etc.
 *
 * \return
 *    Returns the string converted to a zero-padded string.  If the
 *    value represent is outside the legal range (0 to 99999), an empty
 *    string is returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
skey (const std::string & digits)
{
   return skey(ikey(digits));
}

/******************************************************************************
 * ikeymax()
 *------------------------------------------------------------------------*//**
 *
 *    Returns the maximum integer-key value allowed.
 *
 *    We restrict the index vales to the range of 0 to 99999, inclusive.
 *    This function is provided to allow us to change it seamlessly later.
 *
 * \return
 *    The maximum integer-index value is returned.
 *
 *//*-------------------------------------------------------------------------*/

int
ikeymax ()
{
   return XPC_IKEYMAX;
}

/******************************************************************************
 * ikey()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an 5-digit zero-padded string, or a normal string of digits,
 *    to an integer.
 *
 * \param s
 *    The string to convert to the integer.
 *
 * \return
 *    Returns the decoded integer.  If the number converts to greater than 5
 *    digits, or if it is negative, or the string is empty, then -1 is
 *    returned, and should be regarded as an error.
 *
 *//*-------------------------------------------------------------------------*/

int
ikey (const std::string & s)
{
   int result = -1;
   if (! s.empty())
   {
      int v = atoi(s.c_str());
      if ((v >= 0) && (v < XPC_IKEYMAX+1))
         result = v;
   }
   return result;
}

/******************************************************************************
 * pkid()
 *------------------------------------------------------------------------*//**
 *
 *    Converts an integer to a positive string.
 *
 *    This function is basically atoi() for std::string output.  Compare it
 *    to skey().  That function zero-pads the string, this one does not.  *
 * \param k
 *    The integer that is to be converted to a string to serve as the
 *    lookup value.
 *
 * \return
 *    Returns the integer converted to a string.  If the integer is
 *    negative, an empty string is returned.
 *
 * \todo
 *    This function properly belongs in the xpc::row class.
 *
 *//*-------------------------------------------------------------------------*/

std::string
pkid (int k)
{
   std::string result("");
   if (k >= 0)
   {
      char temp[32];
      sprintf(temp, "%d", k);
      result = temp;
   }
   return result;
}

/******************************************************************************
 * get_field_by_pkid() [string version]
 *------------------------------------------------------------------------*//**
 *
 *    Looks for the given PKID in the rowset, and returns the value
 *    of the specified field in the row that is found.
 *
 * \param rows
 *    The rowset to examine.
 *
 * \param spkid
 *    The desired PKID in string format (no zero-padding).  Note that this
 *    code will support PKID values are not in numeric format, though that
 *    goes against our normal convention, and our database uses only integer
 *    PKID values.
 *
 * \return
 *    Returns the field found, as a string, if the pkid value was found in
 *    the rowset.  Otherwise, an empty string is returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
rowset::get_field_by_pkid
(
   const std::string & spkid,
   const std::string & fieldname
) const
{
   std::string result;
   if (! spkid.empty())
   {
      /*
       * Find the row via its PKID.  It is in non-zero-padded string format,
       * and so we need to ensure that format when we look it up, by calling
       * the pkid() function.
       */

      row r = get_row_by_field("pkid", spkid);

      // Only needed for debugging.
      // show("row from get_field_by_pkid", r);

      result = r.value(fieldname);                 // find the field
   }
   return result;
}

/******************************************************************************
 * get_field_by_pkid() [integer version]
 *------------------------------------------------------------------------*//**
 *
 *    Looks for the given PKID in the rowset, and returns the value
 *    of the specified field in the row that is found.
 *
 * \param rows
 *    The rowset to examine.
 *
 * \param pkid
 *    The desired PKID in integer format.
 *
 * \return
 *    Returns the field found, as a string, if the pkid value was found in
 *    the rowset.  Otherwise, an empty string is returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
rowset::get_field_by_pkid
(
   int pkid,
   const std::string & fieldname
) const
{
   std::string result;
   std::string string_pkid = skey(pkid);
   if (! string_pkid.empty())
      result = get_field_by_pkid(string_pkid, fieldname);

   return result;
}

/******************************************************************************
 * get_pkid_by_field()
 *------------------------------------------------------------------------*//**
 *
 *    Given a field-name and field-value, looks for the matching value in
 *    the container, then returns the corresponding PKID string.
 *
 * \param pkid
 *    The field that is to be searched for during the lookup.  This field
 *    represents the PKID in 5-digit zero-padded format.
 *
 * \param fieldvalue
 *    The desired value to search for.
 *
 * \return
 *    Returns the PKID as a string if the desired value of the desired field
 *    was found in the container.  Otherwise, an empty string is returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
rowset::get_pkid_by_field
(
   const std::string & pkid,
   const std::string & fieldvalue
) const
{
   return get_field_by_field("pkid", pkid, fieldvalue);
}

/******************************************************************************
 * get_row_by_field()
 *------------------------------------------------------------------------*//**
 *
 *    Given a field-name and field-value, looks for the matching value in
 *    the container, then returns the corresponding PKID string.
 *
 *    This process is necessarily a linear lookup.
 *
 * \param fieldname
 *    The field that is to be examined during the lookup.
 *
 * \param fieldvalue
 *    The desired value to search for.
 *
 * \return
 *    Returns the PKID as a string if the desired value of the desired field
 *    was found in the container.  Otherwise, an empty string is returned.
 *
 *//*-------------------------------------------------------------------------*/

row
rowset::get_row_by_field
(
   const std::string & fieldname,
   const std::string & fieldvalue
) const
{
   row result;
   if (! fieldname.empty() && ! fieldvalue.empty())
   {
      xpc::rowset::const_iterator ci;
      for (ci = begin(); ci != end(); ci++)
      {
         const xpc::row & r = ci->second;          // get the next row
         std::string v = r.value(fieldname);       // get field value
         if (v == fieldvalue)
         {
            result = ci->second;
            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * get_field_by_field()
 *------------------------------------------------------------------------*//**
 *
 *    Given a field-name and field-value, looks for the matching value in
 *    the container, then returns the value of the desired field.
 *
 * \param field_to_get
 *    The field whose value we wish to get.
 *
 * \param field_to_match_on
 *    The field that is to be used to make the value match.
 *
 * \param
 *    The value that we want to match.
 *
 * \return
 *    Returns the \a field_to_get value as a string if the desired value of
 *    the desired field was found in the container.  Otherwise, an empty
 *    string is returned.
 *
 *//*-------------------------------------------------------------------------*/

std::string
rowset::get_field_by_field
(
   const std::string & field_to_get,
   const std::string & field_to_match_on,
   const std::string & value_to_match
) const
{
   std::string result("");
   if (! field_to_match_on.empty() && ! value_to_match.empty())
   {
      xpc::rowset::const_iterator ci;
      for (ci = begin(); ci != end(); ci++)
      {
         const xpc::row & r = ci->second;             // get the next row
         std::string v = r.value(field_to_match_on);  // get field value
         if (v == value_to_match)
         {
            result = r.value(field_to_get);           // get field as string
            break;
         }
      }
   }
   return result;
}

/******************************************************************************
 * show_pair()
 *------------------------------------------------------------------------*//**
 *
 *    Writes out the contents of a row pair.
 *
 *    This function exists in order to be used in an std::for_each() call.
 *
 * \param p
 *    The pair value to be shown.
 *
 *//*-------------------------------------------------------------------------*/

void
show_pair (const row::pair & p)
{
   show("first", p.first);
   show("second", p.second);
}

/******************************************************************************
 * show( row )
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string-indexed map of strings.
 *
 *    The motivation for this type is to provide an easy way to enumerate
 *    the fields found in a single database record.
 *
 *//*-------------------------------------------------------------------------*/

void
show_row (const std::string & obname, const row & container)
{
   fprintf
   (
      stdout,
      "- xpc::row '%s':\n"
      "-    Number of fields:         %d\n"
      ,
      obname.c_str(),
      int(container.size())
   );

   // Can't get the compiler to resolve show_pair() properly.  Fap!
   //
   // std::for_each(container.begin(), container.end(), show_pair);
   //
   // Let's just iterate by brute force for now.

   row::const_iterator ci;
   for (ci = container.begin(); ci != container.end(); ci++)
   {
      fprintf
      (
         stdout,
         "-    %-16.16s:        '%s'\n", ci->first.c_str(), ci->second.c_str()
      );
   }
}

/******************************************************************************
 * show(rowset)
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string-indexed map of results.
 *
 *    The motivation for this type is to provide an easy way to enumerate
 *    the matching records found in a single database query.
 *
 *//*-------------------------------------------------------------------------*/

void
show (const std::string & obname, const rowset & container)
{
   fprintf
   (
      stdout,
      "- xpc::rowset '%s':\n"
      "-    Size:                     %d\n"
      ,
      obname.c_str(),
      int(container.size())
   );
   rowset::const_iterator ci;
   for (ci = container.begin(); ci != container.end(); ci++)
   {
      std::string index = ci->first;
      const row & r = ci->second;
      fprintf
      (
         stdout,
         "- Index (key):                '%s'\n"
         ,
         index.c_str()
      );
      show(index, r);
   }
}

}        // namespace xpc

/******************************************************************************
 * rowset.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/

