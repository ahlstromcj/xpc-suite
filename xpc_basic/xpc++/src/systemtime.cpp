/******************************************************************************
 * systemtime.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          systemtime.cpp
 * \library       xpc++
 * \author        Chris Ahlstrom
 * \date          2010-08-21
 * \updates       2013-08-01
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides the declarations for the xpc::systemtime class.
 *
 *    This class provides a wrapper for the xpc_get_microseconds() function
 *    in the XPC C library.  We need a way to represent time in much greater
 *    resolution than time_t (one second).  We also need a way to work with
 *    subtracting the values to get time intervals.
 *
 *    Also see the xpc::systemtime class and the systemtime.hpp module
 *    for more information.
 *
 *    Also see the xpc::packets::systemtime class, the ssystemtime.cpp
 *    module, and the ssystemtime.hpp module in the XPC Comm project to see
 *    how serialization was added to this class.
 *
 *//*-------------------------------------------------------------------------*/

#include <cstdlib>                     /* std::atoi()                         */
#include <cstring>                     /* std::strchr()                       */
#include <xpc/errorlogging.h>          /* informational functions             */
#include <xpc/systemtime.hpp>          /* xpc::systemtime class               */
XPC_REVISION(systemtime)               /* show_stringmap_info()               */

#ifdef XPC_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#endif

namespace xpc
{

/******************************************************************************
 * time_from_string()
 *------------------------------------------------------------------------*//**
 *
 *    Provides common code for the string constructor and string assignment
 *    operator of the systemtime class.
 *
 *    This function is necessary for dealing with streaming to
 *    from systems that don't use the serialize() function, as in
 *    Alex's demo code that uses Qt's QTime class.
 *
 *    This function needs to handle the microseconds section (after the
 *    decimal point) properly, converting fractional values to full
 *    microsecond values.
 *
 * \param stringvalue
 *    Provides the time in the format "seconds.fractional", as in
 *    1234567890.012345.
 *
 * \return
 *    Returns 'true' if the function succeeded.  Right now, this function
 *    can't fail -- we should add basic checks to make sure the characters
 *    are numeric.
 *
 *//*-------------------------------------------------------------------------*/

bool
systemtime::time_from_string (const std::string & stringvalue)
{
   bool result = false;
   const char * digits = stringvalue.c_str();
   m_timeval.tv_sec = std::atoi(digits);
   if (m_timeval.tv_sec >= 0)
   {
      const char * decimal = std::strchr((char *) digits, '.');
      if (not_nullptr(decimal))
      {
         decimal++;
         char temp[8];
         bool nullhit = false;
         for (int count = 0; count < 6; count++)
         {
            if (decimal[count] == 0)
               nullhit = true;

            if (nullhit)
               temp[count] = '0';               // add null padding
            else
               temp[count] = decimal[count];
         }
         temp[6] = 0;
         m_timeval.tv_usec = std::atoi(temp); // get the microseconds
      }
      result = true;
   }
   return result;
}

/******************************************************************************
 * operator <()
 *------------------------------------------------------------------------*//**
 *
 *    Determines if the given time is less than the current time.
 *
 * \param
 *    Provides the right-hand side of the comparison operator.
 *
 * \return
 *    Returns 'true' if the time is less than than the current (this)
 *    object's time.
 *
 *//*-------------------------------------------------------------------------*/

bool
systemtime::operator < (const systemtime & rhs) const
{
   if (m_timeval.tv_sec == rhs.m_timeval.tv_sec)
      return m_timeval.tv_usec < rhs.m_timeval.tv_usec;
   else
      return (m_timeval.tv_sec < rhs.m_timeval.tv_sec);
}

/******************************************************************************
 * duration()
 *------------------------------------------------------------------------*//**
 *
 *    Return the difference in time from now versus when the object's time
 *    was set.
 *
 * \return
 *    The duration in seconds, in double floating-point format, is returned.
 *
 *//*-------------------------------------------------------------------------*/

double
systemtime::duration () const
{
   double result = 0.0;
   struct timeval current_time;
   if (xpc_get_microseconds(&current_time))
      result = xpc_time_difference(m_timeval, current_time);

   return result;
}

/******************************************************************************
 * operator std::string()
 *------------------------------------------------------------------------*//**
 *
 *    Converts the time portion of this class to a one-line string.
 *
 *    If the time is zero, the returned string is "0.0".  Otherwise,
 *    the returned format is "seconds.fraction", such as "1234567890.012345".
 *
 * \note
 *    Yes, I know I'm using a primitive C function instead of a type-safe
 *    C++ class, but I don't want to take the time to set up a
 *    string-stream and the correct I/O manipulator, just for one damn
 *    number.
 *
 * \return
 *    The duration in seconds, in double floating-point format, represented
 *    as a string, is returned.
 *
 *//*-------------------------------------------------------------------------*/

systemtime::operator std::string () const
{
   long seconds = m_timeval.tv_sec;
   long fractional = m_timeval.tv_usec;
   char temp[64];
   sprintf(temp, "%ld.%06ld", seconds, fractional);
   return std::string(temp);
}

}              // namespace xpc

/******************************************************************************
 * The following functions are better off in the global namespace.
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * operator + for systemtime
 *------------------------------------------------------------------------*//**
 *
 *    Adds two xpc::systemtime values together.
 *
 *    The equation for the addition is
 *
\verbatim
         result = lhs + rhs
\endverbatim
 *
 * \param lhs
 *    Provides the left-hand side of the equation.
 *
 * \param rhs
 *    Provides the right-hand side of the equation.
 *
 * \return
 *    Returns the xpc::systemtime that results from the addition.
 *
 *//*-------------------------------------------------------------------------*/

const xpc::systemtime
operator +
(
   const xpc::systemtime & lhs,
   const xpc::systemtime & rhs
)
{
   xpc::systemtime result = lhs;
   return result += rhs;
}

/******************************************************************************
 * operator - for systemtime
 *------------------------------------------------------------------------*//**
 *
 *    Subtracts two xpc::systemtime values.
 *
 *    The equation is
 *
\verbatim
         result = lhs - rhs
\endverbatim
 *
 * \param lhs
 *    Provides the left-hand side of the equation.  Usually, it is better if
 *    this is the later value in time.
 *
 * \param rhs
 *    Provides the right-hand side of the equation.
 *
 * \return
 *    Returns the xpc::systemtime that results from the addition.
 *
 *//*-------------------------------------------------------------------------*/

const xpc::systemtime
operator -
(
   const xpc::systemtime & lhs,
   const xpc::systemtime & rhs
)
{
   xpc::systemtime result = lhs;
   return result -= rhs;
}

/******************************************************************************
 * systemtime.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
