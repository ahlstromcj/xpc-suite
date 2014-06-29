#if ! defined XPC_INFO_HPP
#define XPC_INFO_HPP

/******************************************************************************
 * info.hpp
 *------------------------------------------------------------------------*//**
 *
 * @file    info.hpp
 *
 *    This class provides a simple test class.
 *
 * @author  Chris Ahlstrom
 * @date    2011-09-19 to 2011-09-20
 * @version $Revision$
 * @license $XPC_INTELLECTUAL_PROPERTY_LICENSE$
 *
 *    Also see the xpc::Info class reference and the info.h module.
 *
 *//*-------------------------------------------------------------------------*/

#include <string>                      // std::string
#include <vector>                      // std::vector, not needed maybe?

namespace xpc
{

/******************************************************************************
 * class xpc::Info
 *------------------------------------------------------------------------*//**
 *
 *    Provides information.  Nothing more.
 *
 *//*-------------------------------------------------------------------------*/

class Info
{

private:

   int m_app_code;
   std::string m_app_tag;

public:

   Info ();
   Info (int code, const std::string & tag);
   int code () const
   {
      return m_app_code;
   }
   const std::string & tag () const
   {
      return m_app_tag;
   }
   bool operator < (const Info & rhs) const
   {
      return key() < rhs.key();
   }
   bool operator > (const Info & rhs) const
   {
      return rhs.key() < key();
   }
   bool operator >= (const Info & rhs) const
   {
      return ! operator <(rhs);
   }
   bool operator == (const Info & rhs) const
   {
      return ! ( operator <(rhs) || rhs.operator <(*this) );
   }
   bool operator <= (const Info & rhs) const
   {
      return ! operator >(rhs);
   }
   const int & key () const
   {
      return m_app_code;
   }

};

}           // namespace xpc

#endif      // XPC_INFO_HPP

/*****************************************************************************
 * End of info.hpp
 * --------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *---------------------------------------------------------------------------*/
