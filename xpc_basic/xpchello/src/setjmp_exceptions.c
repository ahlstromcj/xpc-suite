/******************************************************************************
 * setjmp_exceptions.c
 *------------------------------------------------------------------------*//**
 *
 * \file          setjmp_exceptions.c
 * \library       xpc_suite "Set Jump" application
 * \author        Chris Ahlstrom
 * \date          2011-01-29
 * \update        2012-01-02
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides an example of using setjmp() and longjmp()
 *    to simulate the throw-catch paradigm.
 *
 *    It is derived from the code at this site:
 *
 *       http://en.wikipedia.org/wiki/Setjmp.h
 *
 *    This program's output is:
 *
\verbatim
         calling first
         calling second
         entering second
         second failed with type 3 exception; remapping to type 1.
         first failed, exception type 1
\endverbatim
 *
 *    This works as follows:
 *
 *       -# In main(), the setjmp() call sets up the current location in the
 *          code as the jump environment.  Nothing bad can happen, and setjmp()
 *          returns 0.
 *       -# The function first() is then called.  It copies the original jump
 *          environment to a local variable.  It then calls setjmp() with the
 *          original jump environment.  Again, normal operations, and setjmp()
 *          returns 0.
 *       -# first() then calls second().
 *       -# second() sets the exception type value, and then calls longjmp()
 *          with the original jump environment.  Thus, second declares an error
 *          and throws an exception.
 *       -# This exception moves the instruction pointer back to the
 *          call to setjmp() mand in first(), as if it were called again.
 *          This time, setjmp() returns a non-zero value, and so an error
 *          message is emitted.
 *       -# first() then restores the very first jump environment, and calls
 *          longjmp() to throw a second exception.
 *       -# The original setjmp() call [in main()] now repeats itself,
 *          returning a non-zero value, so that an error is handled.
 *
 *    Use a file scoped static variable for the exception stack so we can
 *    access it anywhere within this translation unit.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/xpc-config.h>
#include <xpc/tiny_macros.h>

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* declares printf(), stdout, etc.     */
#endif

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if XPC_HAVE_SETJMP_H
#include <setjmp.h>                    /* declares setjmp(), etc.             */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* declares strcmp(), etc.             */
#endif

/******************************************************************************
 * s_exception_env
 *------------------------------------------------------------------------*//**
 *
 *    Use a file scoped static variable for the exception stack so we can
 *    access it anywhere within this translation unit.
 *
 *//*-------------------------------------------------------------------------*/

static jmp_buf s_exception_env;
static int s_exception_type;

/******************************************************************************
 * second()
 *------------------------------------------------------------------------*//**
 *
 *    Use a file scoped static variable for the exception stack so we can
 *    access it anywhere within this translation unit.
 *
 *//*-------------------------------------------------------------------------*/

void
second (void)
{
   printf("entering second\n" );                         /* 3rd output        */
   s_exception_type = 3;
   longjmp(s_exception_env, s_exception_type);           /* declare failure   */
   printf("leaving second\n");                           /* not reached       */
}

void
first (void)
{
   jmp_buf my_env;
   printf("calling second\n");                           /* 2nd output        */
   memcpy(my_env, s_exception_env, sizeof(jmp_buf));
   switch (setjmp(s_exception_env))
   {
      case 3:                                               /* exception      */
         printf("second failed with type 3 exception; remapping to type 1.\n");
         s_exception_type = 1;                              /* 4th output     */

      default:                                              /* fall through   */
         memcpy(s_exception_env, my_env, sizeof(jmp_buf));  /* restore excep  */
         longjmp(s_exception_env, s_exception_type);        /* throw excep    */

      case 0:                                               /* normal ops     */
         second();
         printf("second succeeded\n");                      /* not reached    */
   }
   memcpy(s_exception_env, my_env, sizeof(jmp_buf));       /* restore excep   */
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    Set up an exception-handling situation.
 *
 *//*-------------------------------------------------------------------------*/

int
main ()
{
   volatile void * mem_buffer = NULL;
   if (setjmp(s_exception_env))                          /* true == exception */
   {                                                     /* 5th output        */
      printf("first failed, exception type %d\n", s_exception_type);
   }
   else
   {
      /*
       * This code may throw failure via longjmp().
       */

      printf("%s", "calling first\n");                   /* 1st output        */
      first();
      mem_buffer = malloc(300);                          /* allocate resource */

      /* The following won't won't happen */

      printf("%s", strcpy((char*) mem_buffer, "first succeeded!"));
   }
   if (mem_buffer)
      free((void*) mem_buffer);           /* carefully deallocate resource */

   return 0;
}

/******************************************************************************
 * setjmp_exceptions.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
