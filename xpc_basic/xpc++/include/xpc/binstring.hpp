#if ! defined XPC_BINSTRING_HPP
#define XPC_BINSTRING_HPP

/******************************************************************************
 * binstring.hpp
 *------------------------------------------------------------------------*//**
 *
 * \file          binstring.hpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2012-08-14
 * \updates       2013-07-29
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    The binstring module provides a shortcut for handing binary strings.
 *
 *    The std::string class can handle binary strings, using a special
 *    constructor with a size parameter, and the std::string::data()
 *    accessor.
 *
 *    The xpc::binstring class makes binary strings slightly easier to work
 *    with, and allows for some conveniences.
 *
 *//*-------------------------------------------------------------------------*/

#include <string>                      /* std::string                         */

namespace xpc
{

/******************************************************************************
 * binstring
 *------------------------------------------------------------------------*//**
 *
 *    Provides a limited-functionality wrapper for binary strings.
 *
 *    Inheriting from std::string is not recommend.  For this reason, and
 *    for restricting the interface to binstring, the support for strings is
 *    provided using an std::string member.
 *
 *    Only a few pass-through functions are defined for this member; there
 *    are a lot of string functions we don't care about.  The main goal for
 *    this class is to make it easy to encapsulate large amounts of binary
 *    data.
 *
 *    One specific aim is for waveforms, either one-byte mulaw values, or
 *    two-byte PCM values.
 *
 *    A side goal is to provide support for converting the binary
 *    representation to an encoded representation, or at least providing a
 *    string-generating function for that purpose.
 *
 * \warning
 *    -# We don't use std::string's size_type; we use std::size_t.  It just
 *       seems simpler.  For containers like std::string, they are the same
 *       type.  See the C++ allocator(.h) module.
 *
 *//*-------------------------------------------------------------------------*/

class binstring
{

public:

   /**
    *    Two types of binary string will ultimately be support:  binary, and
    *    encoded-binary.
    *
    * \var BINSTRING_BINARY
    *    The default, indicates that the binary data is stored as is.
    *
    * \var BINSTRING_ENCODED_BINARY
    *    Indicates that the string data is an ASCII representation of binary
    *    data.  The mostly like format will be a form of MIME (base64).
    */

   enum format
   {
      BINSTRING_BINARY,
      BINSTRING_ENCODED_BINARY
   };

private:

   /**
    *    Provides storage for the actual binary string data.
    */

   std::string m_string;

   /**
    *    Indicates the format of the binary data.  By default, it is
    *    BINSTRING_BINARY.
    */

   format m_format;

public:

   binstring ();
   binstring (const std::string & s);
   binstring (const void * bs, std::size_t sz);
   binstring
   (
      const binstring & bs,
      std::size_t pos = 0,
      std::size_t n = std::string::npos
   );
   binstring & operator = (const binstring & bs);
   binstring & assign (const binstring & bs);
   binstring & assign (const void * bs, std::size_t sz);
   binstring & assign (const binstring & bs, std::size_t pos, std::size_t n);

public:                                // accessors

   /**
    * \getter m_string.data()
    *    Note that we deliberately do not provide an accessor for
    *    m_string.c_str().
    */

   const void * data () const
   {
      return static_cast<const void *>(m_string.data());
   }

   /**
    * \getter m_string.size()
    */

   std::size_t size () const
   {
      return m_string.size();
   }

   /**
    * \getter m_string.length()
    *    This value is identical to size().
    */

   std::size_t length () const
   {
      return m_string.length();
   }

   /**
    * \getter m_string.empty()
    */

   std::size_t empty () const
   {
      return m_string.empty();
   }

   /**
    * \getter m_string
    */

   operator std::string () const
   {
      return m_string;
   }

   /**
    * @getter m_format
    */

   bool is_binary () const
   {
      return m_format == BINSTRING_BINARY;
   }

   /**
    * @getter m_format
    */

   bool is_encoded () const
   {
      return m_format == BINSTRING_ENCODED_BINARY;
   }

};

/******************************************************************************
 * Global functions
 *----------------------------------------------------------------------------*/

extern std::string string_as_hex (const std::string & s);
extern std::string string_as_hex_initializer (const std::string & s);
extern std::size_t compare_binary_exact
(
   const std::string & s1,
   const std::string & s2
);

}                 // namespace xpc

#endif            // XPC_BINSTRING_HPP

/******************************************************************************
 * binstring.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
