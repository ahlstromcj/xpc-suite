/******************************************************************************
 * bad-pointers.c
 *------------------------------------------------------------------------*//**
 *
 * \file          bad-pointers.c
 * \library       xpc_suite "Bad Pointers" application
 * \author        Chris Ahlstrom
 * \date          2011-01-26
 * \update        2012-01-02
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This application provides examples of pointer usage, and what you can
 *    get away with in C code.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/xpc-config.h>
#include <xpc/tiny_macros.h>

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* declares printf(), stdout, etc.     */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* declares strcmp(), etc.             */
#endif

/******************************************************************************
 * helptext
 *------------------------------------------------------------------------*//**
 *
 *    Provides a discussion of this application.
 *
 *//*-------------------------------------------------------------------------*/

const char * const helptext =
"\n"
"bad-pointer version 1.1.0\n"
"\n"
"Usage:  bad-pointer\n"
"\n"
" --nulls      Attempt to dereference the null values.  Should cause a\n"
"              segfault.\n"
" --help       Show this banner.\n"
"\n"
"'bad-pointer' serves simply as a demonstration to developers of what\n"
"can be gotten away with via null pointers.\n"
"\n"
"Feel free to mail me at <mailto:ahlstromcj@gmail.com>.\n"
"\n"
;

/******************************************************************************
 * zero_pointer()
 *------------------------------------------------------------------------*//**
 *
 *    This function shows that code that attempts to dereference a null
 *    pointer will compile.
 *
 * \param use_it
 *
 *    If true, actually access the pointer.  Of course, this will cause a
 *    segfault, so we normally don't want to do it.
 *
 * \return
 *    Returns 'true' (non-zero) if the function succeeds.
 *
 *//*-------------------------------------------------------------------------*/

int
zero_pointer (int use_it)
{
   int * p = 0;
   printf("zero_pointer() test:\n");
   printf("  int * p = %p\n", (void *) p);   // Linux --> "int * p = (nil)"
   if (use_it)
   {
      int i = *p;
      printf("  *p = %d\n", i);
   }
   p = NULL;
   printf("  int * p = %p\n", (void *) p);   // Linux --> "int * p = (nil)"
   if (use_it)
   {
      int i = *p;
      printf("  *p = %d\n", i);
   }
   if (use_it)
   {
      int i = * (int *) 0;
      printf("  *0 = %d\n", i);
   }
   if (use_it)
   {
      int i = * (int *) NULL;
      printf("  *0 = %d\n", i);
   }
   return ! use_it;
}

/******************************************************************************
 * expressions()
 *------------------------------------------------------------------------*//**
 *
 *    This function shows that null pointers in expressions are not
 *    dereferenced -- the whole expression is what gets dereferenced.
 *
 * \gcc
 *    The result of the addition does not equal v!
 *
\verbatim
      int * z = p + (size_t) v;
\endverbatim
 *
 *    which is output as:
\verbatim
      int * z = p + (size_t) v = (nil)
\endverbatim
 *
 *    and *z there causes a segfault.  Casting carefully, as done below,
 *    does work properly.
 *
 * \param use_it
 *
 *    If true, actually access the pointer.
 *
 * \return
 *    Returns 'true' (non-zero) if the function succeeds.
 *
 *//*-------------------------------------------------------------------------*/

int
expressions (int use_it)
{
   int * p = 0;
   int value = 20;
   int * v = &value;

   // int * z = p + (size_t) v;

   int * z = (int *) ((size_t) p + (size_t) v);

   printf("expressions() test:\n");
   printf("  int * p = %p\n", (void *) p);
   printf("  int value = %d\n", value);
   printf("  int * v = &value = %p\n", (void *) v);

   // printf("  int * z = p + (size_t) v = %p\n", (void *) z);

   printf("  int * z = (int *) ((size_t) p + (size_t) v) = %p\n", (void *) z);

   printf("  *v = %d\n", *v);
   printf("  *z = %d\n", *z);   // See the gcc note in the comments
   if (use_it)
   {
      //
   }
   return ! use_it;
}

/******************************************************************************
 * sock and Foo test structures
 *------------------------------------------------------------------------*//**
 *
 *
 *//*-------------------------------------------------------------------------*/

struct Sock
{
   int blah;
   int bletch;
};

struct Foo
{
   int foo;
   struct Sock * sk;
};

/******************************************************************************
 * kernel_vulnerability()
 *------------------------------------------------------------------------*//**
 *
 *
 *//*-------------------------------------------------------------------------*/

int
kernel_vulnerability (struct Foo * tun)
{
   int result;
   struct Sock * sk;
   printf("kernel_vulnerability():\n  tun = %p\n", (void *) tun);
   if (! tun)
   {
      printf("  The tun pointer is null, will return an error\n");
      sk = tun->sk;
      printf("  sk = tun->sk = %p\n", (void *) sk);
      result = -1;
   }
   else
   {
      sk = tun->sk;
      if (! sk)
      {
         printf("  The tun->sk pointer is null, will return an error\n");
         result = -1;
      }
      else
      {
         printf("  tun->sk = %p\n", (void *) tun->sk);
         result = sk->blah;
         printf("  sk->blah = %d\n", sk->blah);
      }
   }
   return result;
}

/******************************************************************************
 * kernel_invulnerability()
 *------------------------------------------------------------------------*//**
 *
 *
 *//*-------------------------------------------------------------------------*/

int
kernel_invulnerability (struct Foo * tun)
{
   int result;
   printf("tun = %p\n", (void *) tun);
   if (! tun)
   {
      result = -1;
   }
   else
   {
      struct Sock * sk;
      tun->sk = (struct Sock *) 0;
      printf("tun->sk = %p\n", (void *) tun->sk);
      sk = tun->sk;
      result = sk->blah;
   }
   return result;
}

/******************************************************************************
 * pointer_offsets()
 *------------------------------------------------------------------------*//**
 *
 *
 *//*-------------------------------------------------------------------------*/

int
pointer_offsets (int use_it)
{
   struct Sock sock;
   struct Foo foobar;
   int result;
   sock.blah = 0;
   sock.bletch = 0;
   foobar.foo = 0;
   foobar.sk = &sock;
   printf("foobar { foo = %d; sk = %p; }\n", foobar.foo, (void *) foobar.sk);
   printf("sock   { blah = %d; bletch = %d; }\n", sock.blah, sock.bletch);
   result = kernel_vulnerability(&foobar) != -1;
   if (result)
      result = kernel_vulnerability((struct Foo *) 0) != -1;

   if (use_it)
   {
      if (result)
         result = kernel_invulnerability(&foobar) != -1;

      if (result)
         result = kernel_invulnerability((struct Foo *) 0) != -1;
   }
   return result;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the bad-pointer application.
 *
 * \return
 *    Returns POSIX_SUCCESS (0) if the function succeeds.  Other values,
 *    including possible error-codes, are returned otherwise.
 *
 *//*-------------------------------------------------------------------------*/

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   int use_nulls = 0;
   if (argc > 1)
   {
      if (strcmp(argv[1], "--help") == 0)
         printf("%s", helptext);
      else if (strcmp(argv[1], "--nulls") == 0)
         use_nulls = 1;
   }
   else
   {
      int ok;

      ok = 1;
//    ok = zero_pointer(use_nulls);
//    if (ok)
//       ok = expressions(use_nulls);

      if (ok)
         ok = pointer_offsets(use_nulls);

      if (ok)
      {
         return 0;
      }
      else
         return 1;
   }
}

/******************************************************************************
 * bad-pointers.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
