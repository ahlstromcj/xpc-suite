#if ! defined XPC_SYSTEMTIME_HPP
#define XPC_SYSTEMTIME_HPP

/******************************************************************************
 * systemtime.hpp
 *------------------------------------------------------------------------*//**
 *
 * \file          systemtime.hpp
 * \library       xpc++
 * \author        Chris Ahlstrom
 * \date          2010-08-21
 * \updates       2013-08-11
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module Provides the declarations for the xpc::systemtime class.
 *
 *    xpc::systemtime provides a wrapper for the xpc_get_microseconds()
 *    function in the XPC C library, which provides a time representation
 *    with a  much greater resolution than time_t (one second).
 *    xpc:systemtine also provides a way to subtract time values to get time
 *    intervals.
 *
 *    Also see the systemtime.cpp module and the systemtime.hpp module for
 *    more information.  Also see the xpc C library module 'portable' for
 *    the C functions used by this class.
 *
 *    Also consider using boost::posix_time, though it is not a "easy" as
 *    this class.
 *
 *//*-------------------------------------------------------------------------*/

#include <string>                      /* std::string                         */
#include <xpc/macros.h>                /* XPC_REVISION_DECL                   */
#include <xpc/portable.h>              /* C::xpc_get_microseconds(), etc.     */
XPC_REVISION_DECL(systemtime)

namespace xpc
{

/******************************************************************************
 * class systemtime
 *------------------------------------------------------------------------*//**
 *
 *    Provides an object to manage system times.
 *
 *    xpc::systemtime is a wrapper around the C timeval structure.
 *
 *    The operators for ==, <, >, !=, >=, and <= are provided.  We could
 *    have included the header file <code>boost/operators.hpp</code> and
 *    derived this class from boost::totally_ordered<systemtime>, but we
 *    want to keep Boost out of this particular library for now.
 *
 *    Other operators provided are =, -, -=, +, and +=.
 *
 *    Note that this class, at present, does not support the increment and
 *    decrement operators.
 *
 *//*-------------------------------------------------------------------------*/

class systemtime
{

public:

   /**
    *    A synonym for 'true', for extra visibility in making calls to the
    *    systemtime constructor.
    */

   static const bool GET_TIME = true;

   /**
    *    A synonym for 'false', for visibility in making calls to the
    *    systemtime constructor.
    */

   static const bool NO_GET_TIME = false;

private:

   /**
    *    Holds the actual time-value for this object.
    *
    *    The timeval structure is described in the gettimeofday(2) man page.
    *    It has two fields, a time_t value for seconds, and a suseconds_t
    *    value for microseconds.
    *
    *    However, the gettimeofday() function is noted as obsolete.  It is
    *    recommended to use the clock_gettime() function.  We have not done
    *    this transition yet in the xpc_get_microseconds() function.  The
    *    structure used with that function is <code>struct timespec</code>,
    *    which has a time_t value for seconds, and a long value for
    *    nanoseconds.  When we make this change, we will provide a new
    *    function called xpc_clock_microseconds(), and use <code>struct
    *    timespec</code> for this member.
    */

   struct timeval m_timeval;

   /**
    *    Indicates if the system time is valid.
    *
    *    It is normally true, but an error in obtaining the time can falsify
    *    it.
    */

   bool m_is_set;

public:

   /**
    * \defaultctor
    *
    *    This constructor, by default, creates a null system-time.
    *
    * \param getcurrenttime
    *    This parameter defaults to 'true'.  If set to 'true', then the
    *    current system time is obtained.
    */

   systemtime (bool getcurrenttime = GET_TIME)
    :
      m_timeval   (),
      m_is_set    (true)
   {
      if (getcurrenttime)
         (void) set_microseconds();
   }

   /**
    *    Provides a string constructor for system time.
    *
    *    This function is required for dealing with streaming to and from
    *    systems that don't use the serialize() function.
    *
    * \param stringvalue
    *    Provides the time in the format "seconds.fractional", as in
    *    1234567890.012345.
    */

   systemtime (const std::string & stringvalue)
    :
      m_timeval   (),
      m_is_set    (time_from_string(stringvalue))
   {
      // no other code needed
   }

   /**
    *    String-assignment operator.  This function is a counterpart to the
    *    systemtime(std::string) constructor.
    */

   systemtime & operator = (const std::string & stringvalue)
   {
      m_is_set = time_from_string(stringvalue);
      return *this;
   }

   /*
    * The following functions are either created by the compiler or are not
    * needed:
    *
    *    systemtime (const systemtime & source);
    *    systemtime & operator = (const systemtime & source);
    *    ~systemtime ();
    */

   /**
    *    Clears the clock, setting the time to 0.0.
    */

   void reset ()
   {
      m_timeval.tv_sec = 0;
      m_timeval.tv_usec = 0;
   }

   /**
    *    Tests for the clock being reset.
    *
    * \return
    *    Returns 'true' if the time is 0.0.
    */

   bool is_reset () const
   {
      return (m_timeval.tv_sec == 0) &&
         (m_timeval.tv_usec == 0);
   }

   /**
    *    Gets the current system time, storing it in the m_timeval.
    *    Calls the function xpc_get_microseconds().  Sets the m_is_set flag
    *    as per the return value, as well.
    *
    * \return
    *    Returns the value of the xpc_get_microseconds() function, which is
    *    'true' if the function succeeded.
    */

   bool set_microseconds ()
   {
      m_is_set = xpc_get_microseconds(&m_timeval) != 0;
      return m_is_set;
   }

   /*
    *    Documented in the cpp file.
    */

   bool operator < (const systemtime & rhs) const;

   /**
    *    Determines if the given time is identical to the current time.
    *    This function is written in terms of operator <.
    *
    * \param
    *    Provides the right-hand side of the comparison operator.
    *
    * \return
    *    Returns 'true' if the time is not less than or greater than the
    *    object's time.
    */

   bool operator == (const systemtime & rhs) const
   {
      return ! operator <(rhs) && ! rhs.operator <(*this);
   }

   /**
    *    Provides the ">=" operator.  This function is written in terms of
    *    operator <.
    *
    * \param
    *    Provides the right-hand side of the comparison operator.
    *
    * \return
    *    Returns 'true' if the time is not less than the current object's
    *    time.
    */

   bool operator >= (const systemtime & rhs) const
   {
      return ! operator <(rhs);
   }

   /**
    *    Provides the ">" operator.  This function is written in terms of
    *    operator <.
    *
    * \param
    *    Provides the right-hand side of the comparison operator.
    *
    * \return
    *    Returns 'true' if the time is greater than the current object's
    *    time.
    */

   bool operator > (const systemtime & rhs) const
   {
      return ! operator <(rhs) && ! operator ==(rhs);
   }

   /**
    *    Provides the "<=" operator.  This function is written in terms of
    *    operator <.
    *
    * \param
    *    Provides the right-hand side of the comparison operator.
    *
    * \return
    *    Returns 'true' if the time is less than the current (this) object's
    *    time.
    */

   bool operator <= (const systemtime & rhs) const
   {
      return operator <(rhs) || operator ==(rhs);
   }

   /**
    *    Calculates and returns "this" time minus the other time, in seconds.
    *
    * \usage
\verbatim
         xpc::systemtime current, earlier;
         unsigned long difference = current - earlier;
\endverbatim
    *
    * \param rhs
    *    The subtrahend of the equation.
    *
    * \return
    *    Returns the time difference in seconds.
    */

   double operator - (const systemtime & rhs) const
   {
      return time_difference(rhs);
   }

   /**
    *    Auto-addition operator for systemtime.
    *
    * \param rhs
    *    Provides the "right-hand side" of the operator.
    *
    * \return
    *    Returns a reference to the systemtime after the addition has been
    *    performed.
    */

   systemtime & operator += (const systemtime & rhs)
   {
      (void) xpc_time_add(&m_timeval, rhs.m_timeval);
      return *this;
   }

   /**
    *    Auto-subtraction operator for systemtime.
    *
    * \param rhs
    *    Provides the "right-hand side" of the operator.  This should
    *    normally be the \a earlier time.
    *
    * \return
    *    Returns a reference to the systemtime after the subtraction has been
    *    performed.
    */

   systemtime & operator -= (const systemtime & rhs)
   {
      systemtime earlier = rhs;
      (void) xpc_time_subtract(&earlier.m_timeval, m_timeval);
      *this = earlier;
      return *this;
   }

   /**
    *    Returns the time structure as a double value, in seconds.
    *
    * \return
    *    Returns the time as a double value, in fractional seconds.
    *
    * \todo
    *    We still need to be sure there's enough room in a double to hold
    *    the current time in microseconds from the beginning of the epoch.
    */

   double time () const
   {
      return xpc_double_time(m_timeval);
   }

   /**
    *    Returns the time in full seconds, truncated.
    *
    * \return
    *    Returns the time in full seconds.
    */

   long seconds () const
   {
      return long(m_timeval.tv_sec);
   }

   /**
    *    Returns the microseconds portion of the time.
    *
    * \return
    *    Returns the microseconds portion of the time structure.
    */

   long microseconds () const
   {
      return long(m_timeval.tv_usec);
   }

   /*
    *    Documented in the cpp file.
    */

   double duration () const;

   operator std::string () const;

   /**
    *    Obtains the time difference, in seconds, between this object
    *    and the provided object.
    *
    * \param earlier
    *    The earlier systemtime.
    */

   double time_difference (const systemtime & earlier) const
   {
      return xpc_time_difference(earlier.m_timeval, m_timeval);
   }

public:

   /**
    * @accessor m_timeval
    *    This function is used in non-intrusive serialization.  See
    *    xpc::packets::systemtime.
    */

   struct timeval & access_timeval ()
   {
      return m_timeval;
   }

   /**
    * @accessor m_is_set
    *    This function is used in non-intrusive serialization.  See
    *    xpc::packets::systemtime.
    */

   bool & access_is_set ()
   {
      return m_is_set;
   }

private:

   bool time_from_string (const std::string & stringvalue);

};             // class systemtime

}              // namespace xpc

extern const xpc::systemtime operator +
(
   const xpc::systemtime & lhs, const xpc::systemtime & rhs
);
extern const xpc::systemtime operator -
(
   const xpc::systemtime & lhs, const xpc::systemtime & rhs
);

#endif         // XPC_SYSTEMTIME_HPP

/******************************************************************************
 * systemtime.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
