/******************************************************************************
 * infolist.cpp
 *------------------------------------------------------------------------*//**
 *
 * @file    infolist.cpp
 *
 *    This class provides a way to test a potential template issue.
 *
 * @author  Chris Ahlstrom
 * @date    2011-09-19 to 2011-09-24
 * @version $Revision$
 * @license $XPC_INTELLECTUAL_PROPERTY_LICENSE$
 *
 *    Also see the xpc::InfoList class reference and the infolist.h module.
 *
 *//*-------------------------------------------------------------------------*/

#include <algorithm>                   // std::find() or binary_search()
#include <xpc/errorlogging.h>          // xpc_errprint() family of functions.
// #include <xpc/errorlog.h>           // xpc::errorlog()
// #include <xpc/gettext_support.h>    // the _() macro

#include "infolist.hpp"                // xpc::InfoList

namespace xpc
{

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 *    Provides the default constructor for InfoList.
 *
 *//*-------------------------------------------------------------------------*/

InfoList::InfoList ()
 :
   m_Info_by_Code    (),
   m_is_sorted       (false)
{
   (void) create_default_table();
}

/******************************************************************************
 * Principal constructor
 *------------------------------------------------------------------------*//**
 *
 *    Provides the principal constructor for InfoList.
 *
 *//*-------------------------------------------------------------------------*/

InfoList::InfoList (const Info * appinfolist)
 :
   m_Info_by_Code    (),
   m_is_sorted       (false)
{
   bool ok = add(appinfolist);
   if (ok)
      (void) sort();
}

/******************************************************************************
 * add()
 *------------------------------------------------------------------------*//**
 *
 *    Inserts a whole Info array into the containers.
 *
 * @param appinfolist
 *    The array of items to be inserted into the containers.
 *
 * @return
 *    Returns 'true' if the operation succeeded in all facets.  If an item
 *    fails to be inserted, then the remaining elements are not inserted.
 *    If there were no elements to be inserted, then 'false' is also
 *    returned.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::add (const Info * appinfolist)
{
   bool result = not_nullptr(appinfolist);
   if (result)
   {
      const Info * ap = appinfolist;
      result = false;
      while (ap->code() != 99999)
      {
         result = add(*ap);
         if (! result)
            break;
      }
   }
   else
      xpc_errprint_func("null pointer");

   return result;
}

/******************************************************************************
 * add()
 *------------------------------------------------------------------------*//**
 *
 *    Inserts a whole InfoList into the containers.
 *
 *    The insertion is actually an std::vector::push_back() call.
 *
 *    The insertion is checked by verifying that the size of the container
 *    increased.  A more expensive way to verify the insertion would be to
 *    test that std::vector::back() is identical to the item just inserted.
 *
 * @param field
 *    The Info item to be inserted into the containers.
 *
 * @return
 *    Returns 'true' if the operation succeeded in all facets.  If an item
 *    fails to be inserted, then the remaining elements are not inserted.
 *    If there were no elements to be inserted, then 'false' is also
 *    returned.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::add (const Info & field)
{
   bool result;
   Container::size_type s = m_Info_by_Code.size();
   m_Info_by_Code.push_back(field);
   result = m_Info_by_Code.size() == s + 1;
   if (result)
      m_is_sorted = false;
   else
      xpc_errprint_func("push-back failed");

   return result;
}

/******************************************************************************
 * add()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a temporary Info object and adds it to the list.
 *
 * @param code
 *    Provides the application code for the application.
 *
 * @param tag
 *    Provides the short name of the application.
 *
 * @param windows_class
 *    Provides the Windows class name for the application.
 *
 * @param exe_base_name
 *    Provides the base part of the executable name for the application.
 *
 * @param must_close
 *    Indicates that the Application monitor should close the application.
 *
 * @return
 *    Returns 'true' if the insertion succeeded.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::add
(
   int code,
   const std::string & tag
)
{
   Info field(code, tag);
   return add(field);
}

/******************************************************************************
 * remove()
 *------------------------------------------------------------------------*//**
 *
 *    Removes an Info from the containers, based on its value.
 *
 *    We don't believe that removal corrupts the sort order.
 *
 * @note
 *    This function currently uses the std::find() algorithm.  But, if we
 *    can guarantee that the vectors remain sorted, we could use
 *    std::binary_search() instead, except that function just returns a
 *    boolean.  Fap.
 *
 * @param key
 *    The desired Info object to remove from the container.
 *
 * @return
 *    Returns 'true' if the operation succeeded in all facets, that is, if
 *    something got removed.  If there was nothing in either container to
 *    remove, then 'false' is returned.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::remove (const Info & field)
{
   bool result = ! m_Info_by_Code.empty();
   if (result)
   {
      Container::iterator i = std::find
      (
         m_Info_by_Code.begin(), m_Info_by_Code.end(), field
      );
      if (i != m_Info_by_Code.end())
      {
         Container::size_type s = m_Info_by_Code.size();
         (void) m_Info_by_Code.erase(i);
         result = m_Info_by_Code.size() == s - 1;
      }
      if (! result)
         xpc_errprint_func("erase failed");
   }
   return result;
}

/******************************************************************************
 * sort()
 *------------------------------------------------------------------------*//**
 *
 *    Sorts the containers, one by application code, and the other by window
 *    class.
 *
 * @return
 *    Returns 'true' if the operation succeeded, that is, if there was
 *    something to sort.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::sort ()
{
   bool result = ! m_Info_by_Code.empty();
   if (result)
   {
      /*
       * Sort the Class container by the default comparison function,
       * operator <().
       */

      std::sort(m_Info_by_Code.begin(), m_Info_by_Code.end());
      m_is_sorted = true;
   }
   return result;
}

/******************************************************************************
 * create_default_table()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a de facto list of all the info items.
 *
 *//*-------------------------------------------------------------------------*/

bool InfoList::create_default_table ()
{
   bool result = clear();
   if (result)
   {
      result = add(0, "Tag 0");
      if (result)
         result = add(1, "Tag 1");

      if (result)
         result = add(2, "Tag 2");
   }
   if (result)
      result = sort();                    // safety feature

   return result;
}

}           // namespace xpc

/*****************************************************************************
 * End of infolist.cpp
 * --------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *---------------------------------------------------------------------------*/
