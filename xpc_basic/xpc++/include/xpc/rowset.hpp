#if ! defined XPC_ROWSET_HPP
#define XPC_ROWSET_HPP

/******************************************************************************
 * rowset.hpp
 *------------------------------------------------------------------------*//**
 *
 * \file          rowset.hpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-05-30 to 2010-12-07
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides xpc::row to represent the columns in a row of values in a
 *    database result, ordered by column name, and xpc::rowset to represent
 *    a set of xpc::row objects.
 *
 *//*-------------------------------------------------------------------------*/

#include "stringmap.hpp"               /* xpc::stringmap<VALUETYPE>           */

namespace xpc
{

/******************************************************************************
 * row
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string-indexed map of strings.
 *
 *    The motivation for this type is to provide an easy way to enumerate
 *    the fields found in a single database record.
 *
 *    Danged if I couldn't make either of these classes (row and rowset)
 *    work as simple typedefs.  The show_pair(stringmap::pair) functions
 *    would not resolve for the row and rowset typedefs.
 *
 * \todo
 *    -  Add a way to generate a reordered version of the row, for display
 *       purposes;
 *    -  Add support for a "name" of the row.
 *
 *//*-------------------------------------------------------------------------*/

class row : public stringmap<std::string>
{

public:

   row () : stringmap<std::string>()
   {
      //
   }

};

/******************************************************************************
 * Global functions in the xpc namespace
 *-----------------------------------------------------------------------------
 *
 *    Each is documented in the cpp file.
 *
 *----------------------------------------------------------------------------*/

class rowset;                          /* forward reference                   */

extern std::string skey (int k);
extern std::string skey (const std::string & s);
extern int ikey (const std::string & s);
extern int ikeymax ();
extern std::string pkid (int k);
extern void show (const std::string & obname, const row & container);
extern void show (const std::string & obname, const rowset & container);

/******************************************************************************
 * rowset
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string-indexed map of results.
 *
 *    The motivation for this type is to provide an easy way to enumerate
 *    the matching records found in a single database query.
 *
 * \note
 *    Although any kind of string can be used as a key value, in most cases
 *    we will want the rows to be ordered in the same manner as they were
 *    obtained in the database.  So, conventionally, the keys will be
 *    numeric strings as created by the skey() function.
 *
 * \todo
 *    -  Add support for a "name" of the rowset.
 *
 *//*-------------------------------------------------------------------------*/

class rowset : public stringmap<row>
{

public:

   rowset () : stringmap<row>()
   {
      //
   }

   /**
    *    Converts an integer key to a 5-digit zero-padded string key,
    *    then calls the std::string version of insert().
    *
    *    This function is useful when wanting to ensure the ordering
    *    of results (e.g. from an ORDER BY database lookup).
    *
    * \param key
    *    The integer that is to be converted to a string to serve as the
    *    lookup value for the inserted object.
    *
    * \param value
    *    The value object to be added to the container.
    *
    * \return
    *    The size of the container after insertion is returned.  If
    *    important, the caller should check that the size is one larger.
    *    Zero is returned if the key is invalid (outside the range of 0 to
    *    99999), so that insertion did not occur.
    */

   int insert (int key, const row & value)
   {
      int result = 0;
      std::string stringkey = skey(key);
      if (! stringkey.empty())
         result = stringmap<row>::insert(stringkey, value);

      return result;
   }

   /**
    *    Same as the insert() function that manufactures a new key.
    *
    *    The only difference is that the new string-key is returned.
    *
    * \param value
    *    The value object to be added to the container.
    *
    * \return
    *    The new key that was used; this is size of the container after
    *    insertion is returned, converted to the zero-padded string format.
    *    If the insertion failed, this string is empty.
    */

   std::string append (const row & value)
   {
      std::string stringkey = skey(size() +1);
      int original_size = size();
      int new_size = stringmap<row>::insert(stringkey, value);
      if (new_size == original_size + 1)
         return stringkey;
      else
         return std::string("");
   }

   /**
    *    Provides a way to look up a string key and return a row.
    *
    * \param key
    *    Provides the string key to be looked up.
    *
    * \return
    *    Returns the row found.  If it was not found, then a
    *    default-constructor row is returned.
    */

   row value (const std::string & key) const
   {
      return stringmap<row>::value(key);
   }

   /**
    *    Provides a way to look up an integer key and return a value.
    *
    *    This function converts an integer key to a 5-digit zero-padded
    *    string key, then calls the std::string version of value().
    *
    * \param key
    *    The integer that is to be converted to a string to serve as the
    *    lookup value.
    *
    * \return
    *    Returns the row found.  If it was not found, then a
    *    default-constructor row is returned.
    */

   row value (int key)
   {
      std::string stringkey = skey(key);
      return stringkey.empty() ? row() : stringmap<row>::value(stringkey) ;
   }

   /**
    *    Provides an integer version of lookup.
    *
    * \return
    *    Returns an iterator for the found element of the container, if any.
    *    Otherwise end() is returned.
    */

   iterator find (int key)
   {
      // return iterator(find(skey(key)));

      return iterator(stringmap<row>::find(skey(key)));
   }

   /**
    * \accessor stringmap<row>::find() const
    *    Provides an integer version of const lookup.
    *
    * \return
    *    Returns a const iterator for the found element of the container, if
    *    any.  Otherwise end() is returned.
    */

   const_iterator find (int key) const
   {
      return const_iterator(stringmap<row>::find(skey(key)));
   }

   std::string get_field_by_pkid
   (
      const std::string & pkid,
      const std::string & fieldname
   ) const;
   std::string get_field_by_pkid
   (
      int pkid,
      const std::string & fieldname
   ) const;
   std::string get_pkid_by_field
   (
      const std::string & fieldname,
      const std::string & fieldvalue
   ) const;
   row get_row_by_field
   (
      const std::string & fieldname,
      const std::string & fieldvalue
   ) const;
   std::string get_field_by_field
   (
      const std::string & field_to_get,
      const std::string & field_to_match_on,
      const std::string & value_to_match
   ) const;

};

}                 // namespace xpc

#endif            // XPC_ROWSET_HPP

/******************************************************************************
 * rowset.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
