/******************************************************************************
 * binstring.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          binstring.cpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \date          2012-08-14
 * \updates       2013-07-29
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    The binstring module provides a shortcut for handing binary strings.
 *
 *//*-------------------------------------------------------------------------*/

#include <cctype>                      /* std::isprint()                      */
#include <iomanip>                     /* std::setw() and std::hex            */
#include <limits>                      /* std::numeric_limits<>               */
#include <sstream>                     /* std::ostringstream                  */
#include <xpc/binstring.hpp>           /* xpc::binstring class                */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */

namespace xpc
{

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 *    Creates an empty binary string.
 *
 *//*-------------------------------------------------------------------------*/

binstring::binstring ()
 :
   m_string    (),
   m_format    (BINSTRING_BINARY)
{
   // No code; the string is empty
}

/******************************************************************************
 * String constructor
 *------------------------------------------------------------------------*//**
 *
 *    Copies the string parameter.  Note that the parameter could well
 *    represent ASCII data.  We do not care.
 *
 *    Perhaps later we can analyze it to see if it is an encoded-binary
 *    representation.
 *
 * \param s
 *    Provides the string to copy into the binstring's m_string member.
 *
 *//*-------------------------------------------------------------------------*/

binstring::binstring (const std::string & s)
 :
   m_string    (s),
   m_format    (BINSTRING_BINARY)
{
   // No code; the string is now filled
}

/******************************************************************************
 * Data string constructor
 *------------------------------------------------------------------------*//**
 *
 *    Copies the string parameter using the size parameter.
 *    The buffer could represent ASCII data.  We do not care.
 *
 *    Perhaps later we can analyze it to see if it is an encoded-binary
 *    representation.
 *
 * \param bs
 *    Provides a pointer to the (presumably) binary data to be copies.
 *
 * \param sz
 *    The number of bytes to be copied.  Obviously, this value must be less
 *    than or equal to the actual number of bytes in \a bs.
 *
 *//*-------------------------------------------------------------------------*/

binstring::binstring (const void * bs, std::size_t sz)
 :
   m_string    (static_cast<const char *>(bs), sz),
   m_format    (BINSTRING_BINARY)
{
   // No code; the string is now filled
}

/******************************************************************************
 * Copy constructor
 *------------------------------------------------------------------------*//**
 *
 *    Copies one binstring to another.
 *
 *    Note that it closely follows the std::string copy constructor.
 *    However, there is currently no allocator parameter.  That might
 *    change later, for reasons of efficiency.
 *
 * \param bs
 *    Provides the binary string object to be copied.
 *
 * \param pos
 *    The position in the binary string at which to start copying.  Defaults
 *    to 0 (the first byte).
 *
 * \param n
 *    The number of bytes to copy.  Defaults to std::string::npos (all of
 *    the bytes).
 *
 * \todo
 *    Consider how to add an allocator parameter to the binstring copy
 *    constructor.
 *
 *//*-------------------------------------------------------------------------*/

binstring::binstring
(
   const binstring & bs,
   std::size_t pos,
   std::size_t n
) :
   m_string    (bs.m_string, pos, n),
   m_format    (bs.m_format)
{
   // No code; the string is now filled
}

/******************************************************************************
 * Principal assignment operator
 *------------------------------------------------------------------------*//**
 *
 *    Assigns one binstring to another.
 *
 *    Note that it does not support location and size selection the way the
 *    copy constructor does.  This mirrors std::string.
 *
 * \param bs
 *    Provides the binary string object to be assigned.
 *
 * \return
 *    Returns a reference to the destination object.
 *
 *//*-------------------------------------------------------------------------*/

binstring &
binstring::operator = (const binstring & bs)
{
   if (this != &bs)
   {
      m_string = bs.m_string;
      m_format = bs.m_format;
   }
   return *this;
}

/******************************************************************************
 * assign(binstring)
 *------------------------------------------------------------------------*//**
 *
 *    Assigns one binstring to another.
 *
 * \param bs
 *    Provides the binary string object to be assigned.
 *
 * \return
 *    Returns a reference to the destination object.
 *
 *//*-------------------------------------------------------------------------*/

binstring &
binstring::assign (const binstring & bs)
{
   if (this != &bs)
   {
      m_string = bs.m_string;
      m_format = bs.m_format;
   }
   return *this;
}

/******************************************************************************
 * assign(void *)
 *------------------------------------------------------------------------*//**
 *
 *    Assigns a sized void array to the object.
 *
 * \param bs
 *    Provides the (binary) data array to be assigned.
 *
 * \param sz
 *    Provides the number of bytes in the data array.
 *
 * \return
 *    Returns a reference to the destination object.
 *
 * \todo
 *    Could check for the data truly being binary.
 *
 *//*-------------------------------------------------------------------------*/

binstring &
binstring::assign (const void * bs, std::size_t sz)
{
   m_string.assign(static_cast<const char *>(bs), sz),
   m_format = BINSTRING_BINARY;
   return *this;
}

/******************************************************************************
 * assign(binstring, position, size)
 *------------------------------------------------------------------------*//**
 *
 *    Assigns a substring of one binstring to another binstring.
 *
 * \param bs
 *    Provides the binary string object to be assigned.
 *
 * \param pos
 *    The position in the source (bs) at which to start.  Can range from 0
 *    to the length of the binstring minus 1.
 *
 * \param n
 *    The number of bytes to copy.  Can range from 0 on up.  If set to
 *    std::string::npos, the whole of the string (starting from \e pos) is
 *    copied.
 *
 * \return
 *    Returns a reference to the destination object.
 *
 *//*-------------------------------------------------------------------------*/

binstring &
binstring::assign (const binstring & bs, std::size_t pos, std::size_t n)
{
   m_string.assign(bs, pos, n);
   m_format = BINSTRING_BINARY;
   return *this;
}

/******************************************************************************
 * string_as_hex()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a string showing a string as hex values.
 *
 * \param s
 *    A normal or binary string to be shown as hex characters.
 *
 * \return
 *    Returns the converted string.
 *
 *//*-------------------------------------------------------------------------*/

std::string
string_as_hex (const std::string & s)
{
   std::ostringstream output;
   std::size_t sz = s.size();
   if (sz > 0)
   {
      std::size_t index = 0;
      for (;;)
      {
         for (int i = 0; i < 8; i++)
         {
            int c = s[index];
            int c2 = c;
            if (! std::isprint(c))
               c2 = '.';

            output
               << " 0x" << std::setw(2) << std::hex << std::setfill('0') << c
               << " " << std::setw(1) << char(c2)
               ;

            index++;
            if (index == sz || i == 7)
            {
               output << std::endl;
               break;
            }
            else
               output << " ";
         }
         if (index == sz)
         {
            output << std::endl;
            break;
         }
      }
   }
   else
   {
      output << _("empty string") << std::endl;
   }
   return output.str();
}

/******************************************************************************
 * string_as_hex_initializer()
 *------------------------------------------------------------------------*//**
 *
 *    Creates a string showing a string as hex values suitable for inclusion
 *    in a C/C++ module.
 *
 * \param s
 *    A normal or binary string to be shown as hex characters.
 *
 * \return
 *    Returns the converted string.
 *
 *//*-------------------------------------------------------------------------*/

std::string
string_as_hex_initializer (const std::string & s)
{
   std::ostringstream output;
   std::size_t sz = s.size();
   if (sz > 0)
   {
      std::size_t index = 0;
      for (;;)
      {
         for (int i = 0; i < 8; i++)
         {
            int c = s[index];
            output
               << "   0x"
               << std::setw(2) << std::hex << std::setfill('0') << c
               << ","
               ;

            index++;
            if (index == sz || i == 7)
            {
               output << std::endl;
               break;
            }
         }
         if (index == sz)
         {
            output << std::endl;
            break;
         }
      }
   }
   else
   {
      output << _("empty string") << std::endl;
   }
   return output.str();
}

/******************************************************************************
 * compare_binary_exact()
 *------------------------------------------------------------------------*//**
 *
 *    Compares two strings byte-by-byte.
 *
 * \param s1
 *    The first of the strings to be compared.
 *
 * \param s2
 *    The second of the strings to be compared.
 *
 * \return
 *    Returns 0 if the strings match, otherwise returns the index of the
 *    first mismatch.  This index is less than 0 if string s1 is less than
 *    string s2.  See the strcmp(3) man page.  If the strings are of
 *    different size, then std::numeric_lists<std::size_t>::max() is
 *    returned.
 *
 *//*-------------------------------------------------------------------------*/

std::size_t
compare_binary_exact (const std::string & s1, const std::string & s2)
{
   std::size_t result = 0;
   std::size_t size1 = s1.size();
   std::size_t size2 = s2.size();
   if (size1 == size2)
   {
      std::size_t count = size1 > size2 ? size2 : size1 ;
      std::size_t i;
      for (i = 0; i != count; i++)
      {
         if (s1[i] < s2[i])
         {
            result = -count;
            break;
         }
         else if (s1[i] > s2[i])
         {
            result = count;
            break;
         }
      }
   }
   else
      result = std::numeric_limits<std::size_t>::max();

   return result;
}

}                 // namespace xpc

/******************************************************************************
 * binstring.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
