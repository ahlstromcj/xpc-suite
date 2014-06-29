/******************************************************************************
 * averager.cpp
 *-----------------------------------------------------------------------------
 *
 * \file          averager.cpp
 * \library       xpc
 * \author        Chris Ahlstrom
 * \updates       2010-12-27 to 2010-12-27
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This class provides a calculator-like averager functionality packaged
 *    in a class.
 *
 *    The averager class provides a way to accumulate sums and
 *    sums-of-squares to be used in calculating the mean and standard
 *    deviation of an input data "vector".
 *
 *    Also see the xpc::averager class description and the averager.hpp
 *    module for more information.
 *
 *//*-------------------------------------------------------------------------*/

#include <math.h>                      /* sqrt()                              */
#include <xpc/portable.h>              /* portability issues                  */
#include <xpc/errorlogging.h>          /* C::xpc_showinfo(), etc.             */
#include <xpc/averager.hpp>            /* xpc::averager                       */
XPC_REVISION(averager)

namespace xpc
{

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 *    The default constructor provides a zeroed, cleared, and ready-to-start
 *    statistical buffer.
 *
 * \param basevalue
 *    Provides an optional value to be deducted from every incoming value.
 *
 *//*-------------------------------------------------------------------------*/

averager::averager (const double & basevalue)
 :
   m_BaseValue    (basevalue),
   m_IsDirty      (false),
   m_SumX         (0.0),
   m_Sum2         (0.0),
   m_SumN         (0),
   m_SumMean      (0.0),
   m_Current      (0.0),
   m_SumSEM       (0.0),
   m_SumStdDev    (0.0)
{
   /* No other functionality */
}

/******************************************************************************
 * Copy constructor
 *------------------------------------------------------------------------*//**
 *
 *    The copy constructor might be useful in branching off one set of data
 *    from another to make a comparison.  In any case, it is easy to
 *    provide, and it doesn't hurt to make it explicit.
 *
 * \param source
 *    The averager object to be copied.
 *
 *//*-------------------------------------------------------------------------*/

averager::averager (const averager & source)
 :
   m_BaseValue    (source.m_BaseValue),
   m_IsDirty      (source.m_IsDirty),
   m_SumX         (source.m_SumX),
   m_Sum2         (source.m_Sum2),
   m_SumN         (source.m_SumN),
   m_SumMean      (source.m_SumMean),
   m_Current      (source.m_Current),
   m_SumSEM       (source.m_SumSEM),
   m_SumStdDev    (source.m_SumStdDev)
{
   /* No other functionality */
}

/******************************************************************************
 * Principal assignment operator
 *------------------------------------------------------------------------*//**
 *
 *    The principal assignment operator is easy to provide, and it doesn't
 *    hurt to make it explicit.
 *
 * \param source
 *    The averager object to be assigned from.
 *
 *//*-------------------------------------------------------------------------*/

averager &
averager::operator = (const averager & source)
{
   if (this != &source)
   {
      /* no resources to free at this time */

      m_BaseValue  = source.m_BaseValue;
      m_IsDirty    = source.m_IsDirty;
      m_SumX       = source.m_SumX;
      m_Sum2       = source.m_Sum2;
      m_SumN       = source.m_SumN;
      m_SumMean    = source.m_SumMean;
      m_Current    = source.m_Current;
      m_SumSEM     = source.m_SumSEM;
      m_SumStdDev  = source.m_SumStdDev;
   }
   return *this;
}

/******************************************************************************
 * Accumulate, operator +=()
 *------------------------------------------------------------------------*//**
 *
 *    Performs the same function as the sigma-plus key on a statistical
 *    calculator.
 *
 * \param x
 *    The incoming current value of the dependent variable.
 *
 * \return
 *    Returns a reference to this object, as expected by normal
 *    implementations of this operator.
 *
 *//*-------------------------------------------------------------------------*/

averager &
averager::operator += (double x)
{
   (void) accumulate(x);
   return *this;
}

/******************************************************************************
 * De-accumulate, operator -=()
 *------------------------------------------------------------------------*//**
 *
 *    Performs the same function as the sigma-minus key on a statistical
 *    calculator.
 *
 *//*-------------------------------------------------------------------------*/

averager &
averager::operator -= (double x)
{
   (void) deaccumulate(x);
   return *this;
}

/******************************************************************************
 * Equivalence, operator ==()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a definition of equivalence between two averagers.
 *
 *    If the count, the sum, and the sum of squares are all the same, then
 *    we consider the two averagers to be the same.  However, even if they
 *    are the same, the input sequences can indeed be different.  At least,
 *    though, all of the accessor functions of averager will yield the same
 *    values.
 *
 * \param comp
 *    The averager to be compared to this averager.
 *
 * \return
 *    Returns 'true' if the two averagers have accumulated the same
 *    statistics.
 *
 * \todo
 *    Precision ranges need to be incorporated, and the concept of
 *    equivalence may be useless anyway.
 *
 *//*-------------------------------------------------------------------------*/

bool
averager::operator == (const averager & comp)
{
   return
      (m_BaseValue == comp.m_BaseValue) &&
      (m_SumN == comp.m_SumN) &&
      (m_SumX == comp.m_SumX) &&
      (m_Sum2 == comp.m_Sum2);
}

/******************************************************************************
 * clear()
 *------------------------------------------------------------------------*//**
 *
 *    This function sets all of the accumulators back to null values.
 *
 *//*-------------------------------------------------------------------------*/

void
averager::clear ()
{
   m_IsDirty      = false;
   m_SumX         = 0.0;
   m_Sum2         = 0.0;
   m_SumN         = 0;
   m_SumMean      = 0.0;
   m_Current      = 0.0;
   m_SumSEM       = 0.0;
   m_SumStdDev    = 0.0;
}

/******************************************************************************
 * accumulate()
 *------------------------------------------------------------------------*//**
 *
 *    This function serves as the encapsulation of the accumulation of
 *    variable values.
 *
 *    This implementation of the function adds the given level to the sum and
 *    sum-of-squares buffers.  For convenience, it also returns the
 *    count of accumulations.
 *
 * \param x
 *    The value to be accumulated.  If m_BaseValue is not equal to 0.0, then
 *    it is deducted from this value before the accumulation.
 *
 * \return
 *    Returns the number of data points accumulated to this point.
 *
 *//*-------------------------------------------------------------------------*/

int
averager::accumulate (double x)
{
   m_IsDirty = true;                 /* "be sure to recalculate" */
   if (m_BaseValue != 0.0)
      x -= m_BaseValue;

   m_SumX += x;
   m_Sum2 += x * x;
   m_SumN++;
   m_Current = x;
   return n();
}

/******************************************************************************
 * deaccumulate()
 *------------------------------------------------------------------------*//**
 *
 *    Deducts the value from the sum buffers.  Otherwise, acts like
 *    accumulate().
 *
 * \param x
 *    The value to be de-accumulated.  If m_BaseValue is not equal to 0.0,
 *    then it is deducted from this value before the de-accumulation.
 *
 * \return
 *    Returns the number of data points accumulated now that this data point
 *    has been deleted.
 *
 *//*-------------------------------------------------------------------------*/

int
averager::deaccumulate (double x)
{
   if (n() > 0)
   {
      m_IsDirty = true;                /* "be sure to recalculate"            */
      if (m_BaseValue != 0.0)
         x -= m_BaseValue;

      m_SumX -= x;
      m_Sum2 -= x * x;
      m_SumN--;
      m_Current = 0.0;                 /* use an UNKNOWN value of some kind   */
   }
   return n();
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *
 *    These functions give the caller access to some private statistical
 *    members.  See the inline definitions in the class for the simpler
 *    functions.
 *
 *    We must always make the calculation, even though this might slow
 *    things down a little.  The reason, as I see it, is that the
 *    accumulate() and deaccumulate() functions return the mean, via mean(),
 *    and this clears the m_IsDirty flag, so that sem() never would make any
 *    calculations.
 *
 *    Actually, I now prefer to let mean() calculate the SEM also, so that
 *    the sem() function just needs to return that, or make the calculation
 *    if the object is dirty (new data has been added, or data has been
 *    deleted).  The concept of dirtiness has been absorbed into the
 *    calculate() function.
 *
 *    Note that all of these functions are logically constant, meaning that
 *    the caller does not realize that it is modifying the object when it
 *    calls these functions.  But that modifying is okay, because, as far as
 *    the caller is concerned, the object is just returning the value of a
 *    member.
 *
 *    Please read more about the ineffable coolness of accessor functions in
 *    Scott Meyer's first book, "Effective C++".  Accessor functions are
 *    your friend!
 *
 *//*-------------------------------------------------------------------------*/

int
averager::n () const
{
   return m_SumN;                             /* N (count of items) */
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *    Access the xxx statistical value.
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::sum () const
{
   return m_SumX;
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *    Access the xxx statistical value.
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::sum_of_squares () const
{
   return m_Sum2;
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *    Access the xxx statistical value.
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::mean () const
{
   calculate();
   return m_SumMean;
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *    Access the xxx statistical value.
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::sem () const
{
   calculate();
   return m_SumSEM;
}

/******************************************************************************
 * xxx()
 *------------------------------------------------------------------------*//**
 *
 *    Access the xxx statistical value.
 *
 * \return
 *    Returns the value of xxx at this point in time.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::stddev () const
{
   calculate();
   return m_SumStdDev;
}

/******************************************************************************
 * calculate()
 *------------------------------------------------------------------------*//**
 *
 *    Evaluates all of the statistical values, if the current state of the
 *    averager is "dirty".
 *
 *    This helper function is used by "const" functions, so it must be const
 *    itself.  The members it modifies must be declared as mutable.
 *
 *    Note that it has its own helper functions, which must also be be treated
 *    as logically constant.  This const stuff is cool, but can be a real pain
 *    sometimes.
 *
 *//*-------------------------------------------------------------------------*/

void
averager::calculate () const
{
   if (m_IsDirty)
   {
      m_SumMean    = calculate_mean();
      m_SumSEM     = calculate_sem();
      m_SumStdDev  = calculate_stddev();
      m_IsDirty    = false;
   }
}

/******************************************************************************
 * calculate_mean()
 *------------------------------------------------------------------------*//**
 *
 *    Another helper function used in lazy evaluation.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::calculate_mean () const
{
   double mean;
   if (m_SumN > 0)
      mean = m_SumX / m_SumN;
   else
      mean = 0.0;

   return mean;
}

/******************************************************************************
 * calculate_stddev()
 *------------------------------------------------------------------------*//**
 *
 *      Let s be the standard deviation.  Then
 *
\verbatim
                                          2
                     N          [  N     ]
                    ---         [ ---    ]
                    \    2      [ \      ]
                  N /   x   -   [ /   x  ]
                    ---  i      [ ---  i ]
            2         i=1       [ i=1    ]
           s  =  -----------------------------------------
                             N(N-1)
\endverbatim
 *
 * or, in terms of member fields of this class,
 *
\verbatim
                               2
            2      mSumN * mSum2 - mSumX
           s  =  -------------------------
                   mSumN * (mSumN-1)
\endverbatim
 *
 * \return
 *    Returns the calculated standard deviation, or a value less than zero
 *    if an error was encountered.
 *
 *//*-------------------------------------------------------------------------*/

double
averager::calculate_stddev () const
{
   double stdev;
   if (m_SumN > 1)
   {
      double temp = double(m_SumN) * m_Sum2 - m_SumX * m_SumX;
      if (temp >= 0.0)
         stdev = sqrt(temp / (double(m_SumN) * (double(m_SumN) - 1.0)));
      else
         stdev = -9.0;                    /* an impossible result */
   }
   else
      stdev = -1.0;                       /* an impossible result */

   return stdev;
}

/******************************************************************************
 * calculate_sem()
 *------------------------------------------------------------------------*//**
 *
 *    Let m be the standard error of the mean, and let s be the as described in
 *    the calculateStdDev() function.  Then, in terms of the standard
 *    deviation,
 *
\verbatim
              m = s/sqrt(N)
\endverbatim
 *
 *    or, in terms of member fields of this class,
 *
\verbatim
                        2
            2      mSum2 - mSumX /mSumN
           m  =  ------------------------
                   mSumN * (mSumN-1)
\endverbatim
 *
 *//*-------------------------------------------------------------------------*/

double
averager::calculate_sem () const
{
   double sem = calculate_stddev();
   if (sem > 0.0)
      sem /= sqrt(double(m_SumN));

   return sem;
}

}          /* namespace xpc */

/******************************************************************************
 * averager.cpp
 *----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *//*-------------------------------------------------------------------------*/

