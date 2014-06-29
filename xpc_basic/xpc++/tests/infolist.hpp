#if ! defined XPC_INFOLIST_HPP
#define XPC_INFOLIST_HPP

/******************************************************************************
 * infolist.hpp
 *------------------------------------------------------------------------*//**
 *
 * @file    infolist.hpp
 *
 *    This class provides a way to manage numerically-indexed lists of
 *    xpc::Infos.
 *
 * @author  Chris Ahlstrom
 * @date    2011-09-19 to 2011-09-24
 * @version $Revision$
 * @license $XPC_INTELLECTUAL_PROPERTY_LICENSE$
 *
 *    Also see the xpc::InfoList class reference and the infolist.h
 *    module.
 *
 *//*-------------------------------------------------------------------------*/

#include <algorithm>                   // std::for_each()
#include <functional>                  // std::mem_fun1_ref_t
#include <vector>                      // std::vector
#include "info.hpp"                    // xpc::Info class

namespace xpc
{

/******************************************************************************
 * class xpc::InfoList
 *------------------------------------------------------------------------*//**
 *
 *    Provides an ordered container for Info objects.
 *
 *//*-------------------------------------------------------------------------*/

class InfoList
{

public:           // see below for why this is public

   typedef std::vector<Info> Container;

private:

   Container m_Info_by_Code;
   bool m_is_sorted;

public:

   InfoList ();
   InfoList (const Info * infoinfolist);

   /*
    * Let the compiler generate these functions:
    *
    * InfoList (const InfoList & source);
    * InfoList & operator = (const InfoList & source);
    * ~InfoList ();
    */

   size_t size () const
   {
      return m_Info_by_Code.size();
   }

   /*
    * VS 2010 started crashing when we tried to bind CxpcService member
    * functions in order to have InfoList iterate through the container
    * running them, so we have to expose the vector container so that we
    * can iterate through all the info.
    */

   const Container & info () const
   {
      return m_Info_by_Code;
   }

   const Info & info (int infocode)
   {
      return m_Info_by_Code[infocode];
   }

   bool add (const Info * list);
   bool add (const Info & field);
   bool add (int code, const std::string & tag);
   bool remove (const Info & field);
   bool clear ()
   {
      m_Info_by_Code.clear();
      return m_Info_by_Code.size() == 0;
   }
   bool sort ();

private:

   bool create_default_table ();

};          // class InfoList

}           // namespace xpc

#endif      // XPC_INFOLIST_HPP

/*****************************************************************************
 * End of infolist.hpp
 * --------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *---------------------------------------------------------------------------*/
