/******************************************************************************
 * info.cpp
 *------------------------------------------------------------------------*//**
 *
 * @file    info.cpp
 *
 *    This class provides a way to test a potential template issue.
 *
 * @author  Chris Ahlstrom
 * @date    2011-09-19 to 2011-09-24
 * @version $Revision$
 * @license $XPC_INTELLECTUAL_PROPERTY_LICENSE$
 *
 *    Also see the xpc::Info class reference and the info.h module.
 *
 *//*-------------------------------------------------------------------------*/

// #include <xpc/errorlogging.h>       // xpc_errprint() family of functions
// #include <xpc/errorlog.h>           // xpc::errorlog()
// #include <xpc/gettext_support.h>    // the _() macro

#include "info.hpp"                    // xpc::Info

namespace xpc
{

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 *    Provides the default constructor for Info.
 *
 *//*-------------------------------------------------------------------------*/

Info::Info ()
 :
   m_app_code  (99999),
   m_app_tag   ("")
{
   // no code
}

/******************************************************************************
 * Principal constructor
 *------------------------------------------------------------------------*//**
 *
 *    Provides the principal constructor for Info.
 *
 *//*-------------------------------------------------------------------------*/

Info::Info (int code, const std::string & tag)
 :
   m_app_code  (code),
   m_app_tag   (tag)
{
   // no code
}

}           // namespace xpc

/*****************************************************************************
 * End of info.cpp
 * --------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *---------------------------------------------------------------------------*/
