#if !defined XPC_AVERAGER_HPP
#define XPC_AVERAGER_HPP

/*******************************************************************************
 * averager.hpp
 *-------------------------------------------------------------------------*//**
 *
 * \file          averager.hpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-12-27 to 2010-12-27
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This class provides a calculator-like averager functionality packaged
 *    in a class.
 *
 *    Also see the xpc::averager class description and the averager.cpp
 *    module for more information.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/macros.h>                /* XPC_REVISION macros                 */
XPC_REVISION_DECL(averager)            /* void show_averager_info()           */

namespace xpc
{

/*******************************************************************************
 * averager
 *------------------------------------------------------------------------*//**
 *
 *    Implements simple descriptive statistics:  mean and standard deviation.
 *
 *-----------------------------------------------------------------------------*/

class averager
{

private:

   /**
    *    Provides a base value that is subtracted from incoming values.
    *    The purpose of this value is to avoid accumulating very large
    *    numbers, and losing precision.
    *
    *    The default value of this member is 0.
    */

   double m_BaseValue;

   /**
    *    Indicates if the accumulators have changed in value since the last
    *    queries for statistical results.  Provides for lazy evaluation of
    *    statistics.
    */

   mutable bool m_IsDirty;

   /**
    *    Holds the sum of the incoming values.
    */

   mutable double m_SumX;

   /**
    *    Holds the sum-of-squares of the incoming values.
    */

   mutable double m_Sum2;

   /**
    *    Holds the N (number of data points) of the incoming values.
    */

   mutable int m_SumN;

   /**
    *    Holds the current mean of the incoming values.
    */

   mutable double m_SumMean;

   /**
    *    Holds the most recent incoming value.
    */

   mutable double m_Current;

   /**
    *    Holds the current standard error of the mean of the incoming
    *    values.
    */

   mutable double m_SumSEM;

   /**
    *    Holds the current standard deviation of the incoming values.
    */

   mutable double m_SumStdDev;

public:

   averager (const double & basevalue = 0.0f);
   averager (const averager & source);
   averager & operator = (const averager & source);
   averager & operator += (double x);
   averager & operator -= (double x);
   bool operator == (const averager & comparison);

public:

   void clear ();                         /* clears out the accumulators      */
   double mean () const;                  /* mean value                       */
   double sem () const;                   /* std error of mean                */
   double stddev () const;                /* std deviation                    */
   int n () const;                        /* N (count of items)               */
   int accumulate (double y);             /* accumulate univariate statistics */
   int deaccumulate (double y);           /* remove a number                  */
   double sum () const;                   /* sum                              */
   double sum_of_squares () const;        /* sum of squares                   */

   /**
    * \getter m_Current
    */

   double x () const
   {
      return m_Current;
   }

private:                                  /* logically const helper functions */

   void calculate () const;
   double calculate_mean () const;
   double calculate_sem () const;
   double calculate_stddev () const;

};             /* class averager    */

}              /* namespace xpc     */

#endif         /* XPC_AVERAGER_HPP  */

/******************************************************************************
 * averager.hpp
 *----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *//*-------------------------------------------------------------------------*/
