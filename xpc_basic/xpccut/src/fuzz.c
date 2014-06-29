/******************************************************************************
 * fuzz.c
 *------------------------------------------------------------------------*//**
 *
 * \file          fuzz.c
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2010-03-06
 * \update        2012-01-01
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a function for generating random C strings.
 *
 *    These strings normally can contain any ASCII character, including
 *    non-printing characters, and even null characters.
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/fuzz.h>                  /* fuzz functions, macros, and enums   */
#include <xpc/portable_subset.h>       /* xpc_errprint_func()                 */

#if XPC_HAVE_CTYPE_H
#include <ctype.h>                     /* for the isprint() macro             */
#endif

#if XPC_HAVE_STDIO_H
#include <stdio.h>                     /* for the fprintf() function          */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* for the memcpy() function           */
#endif

#if XPC_HAVE_TIME_H
#include <time.h>                      /* for the time() function             */
#endif

/******************************************************************************
 * XPCCUT_SRAND()
 *------------------------------------------------------------------------*//**
 *
 *    Equates to the random number generator seed function this module uses.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_SRAND(x)       xpccut_srandom(x)

/******************************************************************************
 * XPCCUT_RAND()
 *------------------------------------------------------------------------*//**
 *
 *    Equates to the random number generator function this module uses.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND()         xpccut_random()

/******************************************************************************
 * DOXYGEN
 *------------------------------------------------------------------------*//**
 *
 * \doxygen
 *    This module has a function we would like to be documented, even if not
 *    enabled in normal compilation.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef DOXYGEN
#define USE_GETTEXT                    /* make definitions visible            */
#endif

/******************************************************************************
 * XPCCUT_RAND_MAX
 *------------------------------------------------------------------------*//**
 *
 *    Provides the maximum random-integer value.
 *
 *    We do not want to use the "standard" RAND_MAX value directly, because it
 *    varies between implementations, and causes the unit-test (09.01) to fail.
 *
 * \gnu
 *    RAND_MAX is equal to 2147483647 in 64-bit Linux.  This is the same value
 *    as INT_MAX.  This value is the one we want to use.  The following
 *    environments have been shown to yield the correct results, so far:
 *
 *       -  64-bit Linux
 *       -  32-bit Windows XP
 *
 * \win32
 *    RAND_MAX is equal to 65536 in 32-bit Windows.
 *
 * \win64
 *    RAND_MAX is equal to ???? in 64-bit Windows.
 *
 *
 *//*-------------------------------------------------------------------------*/

#ifdef WIN32
#define XPCCUT_RAND_MAX       2147483647
#else
#define XPCCUT_RAND_MAX       RAND_MAX
#endif

/******************************************************************************
 * XPCCUT_RAND_FACTOR
 *------------------------------------------------------------------------*//**
 *
 *    Provides the factor part of the initial linear congruential equation.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_FACTOR    16807LL

/******************************************************************************
 * XPCCUT_RAND_FACTOR
 *------------------------------------------------------------------------*//**
 *
 *    Provides the factor part of the initial linear congruential equation.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_FACTOR    16807LL

/******************************************************************************
 * XPCCUT_RAND_ADDEND
 *------------------------------------------------------------------------*//**
 *
 *    Provides the constant part of the initial linear congruential equation.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_ADDEND    XPCCUT_RAND_MAX      /* 2147483647, or INT_MAX  */

/******************************************************************************
 * XPCCUT_RAND_DEGREES
 *------------------------------------------------------------------------*//**
 *
 *    Provides the size of ring buffer or feedback window.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_DEGREES          31

/******************************************************************************
 * XPCCUT_RAND_THROWAWAY_SIZE
 *------------------------------------------------------------------------*//**
 *
 *    Provides the size of the throw-away buffer for initial random numbers.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_THROWAWAY_SIZE  344

/******************************************************************************
 * XPCCUT_RAND_OFFSET
 *------------------------------------------------------------------------*//**
 *
 *    Provides the offset backwards to use in the feedback calculation.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_OFFSET            3

/******************************************************************************
 * XPCCUT_RAND_DEGREES_2
 *------------------------------------------------------------------------*//**
 *
 *    Combines XPCCUT_RAND_DEGREES and XPCCUT_RAND_OFFSET.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_RAND_DEGREES_2    (XPCCUT_RAND_DEGREES+XPCCUT_RAND_OFFSET)

/******************************************************************************
 * gs_r[]
 *------------------------------------------------------------------------*//**
 *
 *    The ring buffer for the xpccut_srandom() and xpccut_random()
 *    functions.
 *
 *//*-------------------------------------------------------------------------*/

static int gs_r[XPCCUT_RAND_DEGREES];

/******************************************************************************
 * gs_r_counter
 *------------------------------------------------------------------------*//**
 *
 *    The index counter for the ring buffer for the xpccut_srandom() and
 *    xpccut_random() functions.
 *
 *//*-------------------------------------------------------------------------*/

static int gs_r_counter = 0;

/******************************************************************************
 * xpccut_srandom()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a duplicate implementation of the GNU srandom() seeding
 *    function.
 *
 *    This version is provided so that the same set of random numbers can be
 *    generated on Windows as well.  Also see the xpccut_random() function.
 *
 * \gnu
 *    A description of the glibc random number generator random() is here:
 *
 *          http://www.mscs.dal.ca/~selinger/random/
 *
 *    For a given seed s, an initialization vector r[0] to r[33] is
 *    generated, using a linear congruential formula, in steps 1 to 3 below.
 *
 *    Then a sequence of pseudo-random numbers r[34], r[35], ..., r[343] is
 *    generated by a linear feedback loop, in step 4.
 *
 *    Then r[0] through r[343] are thrown away.  The i'th output o[i] of
 *    GNU's rand() is then given by step 5.  This is a 31-bit number.
 *
 *       -# r[0] = s
 *       -# r[i] = (16807 * (signed int) r[i]-1) mod 2147483647
 *                   <i> (for i = 1 to 30) </i>
 *       -# r[i] = r[i-31]
 *                   <i> (for i = 31 to 33) </i>
 *       -# r[i] = (r[i-3] + r[i-31]) mod 4294967296
 *                   <i> (for i >= 34 and i < 344) </i>
 *       -# o[i] = r[i+344] >> 1
 *
 *    Although this is said to be the formula for GNU's random(), it looks
 *    to me like rand() and random() on GNU generate the exact same
 *    sequence.
 *
 *    The code in the two functions, xpccut_srandom() and xpccut_random(),
 *    defined below, derives from the same link shown above.  It also
 *    generates the same sequence as rand() and random().
 *
 *    However, the author's implementation was rewritten to save space, by
 *    allocating a <i> temporary </i> large buffer, copying the last part of it
 *    into a ring-buffer of size 31 (XPCCUT_RAND_DEGREES), and then throwing
 *    away the original buffer.  Our implementation is tested against number
 *    the author provides in that article, as well as against GNU's random()
 *    function.  Here is how it works:
 *
 * \steps
 *    -# Allocate a buffer that is 344 + 31 integers in length.  (344 is
 *       XPCCUT_RAND_THROWAWAY_SIZE, and 31 is XPCCUT_RAND_DEGREES).
 *       -  The random integers generated into the first 344 integers will
 *          be thrown away after having been used to generate the last 31
 *          integers.
 *       -  The last 31 integers will be copied into the \a gs_r[] array,
 *          which is set up as a ring buffer.  Call this array \e r[] for
 *          short.
 *    -# Set r[0] to the value of the seed.
 *    -# Initialize the next 30 (XPCCUT_RAND_DEGREES minus 1) values to the
 *       following linear congruential formula.
\verbatim
               r[i] = (XPCCUT_RAND_FACTOR * r[i-1]) % XPCCUT_RAND_MAX;
\endverbatim
 *    -# Set the next 3 (XPCCUT_RAND_OFFSET) r[] values to the first three r[]
 *       values.
 *    -# Fill the rest of the r[] array so that
\verbatim
               r[i] = r[i-XPCCUT_RAND_DEGREES] + r[i-XPCCUT_RAND_OFFSET];
\endverbatim
 *       Unlike in the article, we calculate XPCCUT_RAND_DEGREES additional
 *       values to be stored in the ring buffer.
 *    -# Copy these last values to the ring buffer.
 *    -# Free the original buffer, throwing away 344 values.
 *
 *    See the xpccut_random() function description for what happens next.
 *
 * \note
 *    Also see
 *    -  http://en.wikipedia.org/wiki/Linear_congruential_generator
 *    -  http://en.wikipedia.org/wiki/Linear_feedback_shift_register
 *
 * \threadunsafe
 *    The ring buffer and counters are currently static.
 *
 * \param seed
 *    The value to use to start the sequence.  For our purposes, we avoid
 *    seeding with 0 and 1, but those are nonetheless valid values.
 *
 * \return
 *    Returns the value of the seed.
 *
 * \unittests
 *    See unit-tests 09.01 and 09.02 to see how this routine matches the
 *    author's numbers and GNU's random()/rand() function.
 *
 *    -  unit_unit_test_09_01()
 *    -  unit_unit_test_09_02()
 *
 *//*-------------------------------------------------------------------------*/

unsigned int
xpccut_srandom (unsigned int seed)
{
   size_t size = (XPCCUT_RAND_THROWAWAY_SIZE + XPCCUT_RAND_DEGREES) *
       sizeof(int);

   void * b = malloc(size);                                    /* Step 1      */
   if (cut_not_nullptr(b))
   {
      int i;
      int * r = (int *) b;
      r[0] = seed;                                             /* Step 2      */
      for (i = 1; i < XPCCUT_RAND_DEGREES; i++)                /* Step 3      */
      {
         r[i] = (XPCCUT_RAND_FACTOR * r[i-1]) % XPCCUT_RAND_MAX;
         if (r[i] < 0)
            r[i] += XPCCUT_RAND_MAX;
      }
      for (i = XPCCUT_RAND_DEGREES; i < XPCCUT_RAND_DEGREES_2; i++) /* Step 4 */
      {
         r[i] = r[i-XPCCUT_RAND_DEGREES];
      }
      for                                                      /* Step 5      */
      (
         i = XPCCUT_RAND_DEGREES_2;
         i < XPCCUT_RAND_THROWAWAY_SIZE+XPCCUT_RAND_DEGREES;
         i++
      )
         r[i] = r[i-XPCCUT_RAND_DEGREES] + r[i-XPCCUT_RAND_OFFSET];

      for (i = 0; i < XPCCUT_RAND_DEGREES; i++)                /* Step 6      */
         gs_r[i] = r[i + XPCCUT_RAND_THROWAWAY_SIZE];

      gs_r_counter = 0;
      free(b);                                                 /* Step 7      */
   }
   else
      xpccut_errprint_func(_("could not allocate buffer"));

   return seed;
}

/******************************************************************************
 * xpccut_random()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a duplicate implementation of the GNU random() function.
 *
 *    This version is provided so that the same set of random numbers can be
 *    generated on Windows as well.  See the xpccut_srandom() function.
 *
 *    xpccut_srandom() set up a circular buffer of 31 (XPCCUT_RAND_DEGREES)
 *    values of gs_r[].  The first value, gs_r[0], correspondes to r[344]
 *    (r[XPCCUT_RAND_THROWAWAY_SIZE]), the throw-away buffer.
 *
 * \steps
 *    -# Get the current value, gs_r[gs_r_counter], and save it.
 *    -# Right-shift it, and store it as the result of the function.
 *    -# Find the next value for this location.  It is given by
 *       gs_r[gs_r_counter] + gs_r[gs_r_counter + (31-3)], where the
 *       second index can wrap around.
 *    -# Take this value and copy it to the new location.
 *    -# Increment the ring-buffer counter, wrapping around if necessary.
 *
 * \threadunsafe
 *    The ring buffer and counters are currently static.
 *
 * \return
 *    Returns a random number, ranging from 0 to XPCCUT_RAND_MAX.
 *
 *//*-------------------------------------------------------------------------*/

unsigned int
xpccut_random (void)
{
   int current_r = gs_r[gs_r_counter];                         /* Step 1      */
   unsigned int result = ((unsigned int) current_r) >> 1;      /* Step 2      */
   int rindex = gs_r_counter +                                 /* Step 3      */
      (XPCCUT_RAND_DEGREES - XPCCUT_RAND_OFFSET);
   if (rindex >= XPCCUT_RAND_DEGREES)
      rindex -= XPCCUT_RAND_DEGREES;                           /* wrap around */

   gs_r[gs_r_counter] += gs_r[rindex];                         /* Step 4      */
   gs_r_counter++;                                             /* Step 5      */
   if (gs_r_counter == XPCCUT_RAND_DEGREES)
      gs_r_counter = 0;                                        /* wrap around */

   return result;
}

/******************************************************************************
 * xpccut_set_seed()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an external function to try to generate a random seed.
 *
 *    This function can be used internally by xpccut_fuzz(), but it is
 *    usually easier to call xpccut_set_seed() first, and just once, to set
 *    the seed, and then pass XPCCUT_SEED_SKIP to xpccut_fuzz() each time.
 *
 * \note
 *    Avoid using seed values of 0 and 1, as these are used for other
 *    purposes.
 *
 * \private
 *    This is an internal helper function only.
 *
 *//*-------------------------------------------------------------------------*/

unsigned int
xpccut_set_seed (unsigned int seed)
{
   unsigned int result = seed;
   if (result == XPCCUT_SEED_RANDOMIZE)
   {
#ifdef USE_DEV_RANDOM
#else
      seed = (unsigned int) time(NULL);
#endif
      XPCCUT_SRAND(seed);
      result = seed;
   }
   else if (result != XPCCUT_SEED_SKIP)
   {
      XPCCUT_SRAND(seed);
      result = seed;
   }
   return result;
}

/******************************************************************************
 * xpccut_rand()
 *------------------------------------------------------------------------*//**
 *
 *    A simple routine to generator random numbers in the desired range more
 *    easily.
 *
 *    This routine generates integers from 0 to rangemax-1, where rangemax <=
 *    XPCCUT_RAND_MAX.  Thus, \a rangemax different integer values are
 *    generated.
 *
 *    For efficiency (avoiding a lot of integer division), we only change the
 *    divisor when the range changes.  In most use cases, the same value of \a
 *    rangemax will be used many many times in a row.
 *
 * \param rangemax
 *    The range of the random numbers, exclusive.
 *
 * \return
 *    Returns the random number, ranging from 0 to rangemax-1.  It
 *    returns -1 if the divisor somehow came up 0.
 *
 *//*-------------------------------------------------------------------------*/

unsigned int
xpccut_rand (int rangemax)
{
   static int s_rangemax = -1;
   static unsigned divisor = 1;
   unsigned int rvalue;
   if (rangemax != s_rangemax)
   {
      s_rangemax = rangemax;
      divisor = ((unsigned) (XPCCUT_RAND_MAX)+1) / (unsigned) rangemax;
   }
   if (divisor > 0)
   {
      rvalue = ((unsigned) XPCCUT_RAND()) / divisor;
      if (rvalue >= (unsigned) rangemax)
         rvalue = rangemax - 1;                /* fix the occasional odd case */
   }
   else
   {
      xpccut_errprint_func(_("divisor is zero"));
      rvalue = (unsigned int) -1;
   }
   return rvalue;
}

/******************************************************************************
 * xpccut_exclude_characters()
 *------------------------------------------------------------------------*//**
 *
 *    Provides an easy way to remove characters from a string.
 *
 *    Without concern for efficiency, this function works by copying the
 *    source buffer, and then copying it back without the excluded
 *    characters.
 *
 * \param sourcedest
 *    Provides the string to be trimmed.  Since this is an internal helper
 *    function, this pointer is not checked.
 *
 * \param sdlength
 *    Provides the number of characters in the string.  This number will be
 *    a little bit reduced at the end.
 *
 * \param excluded_chars
 *    Provide the list of characters to remove from \a sourcedest.  This
 *    pointer is checked to make sure it is valid.  If not, it is not used,
 *    but no error value is returned.
 *
 * \return
 *    Returns the new size of the buffer.  0 is returned if an error occurs.
 *    The value of \a sdlength is returned if the \a excluded_chars array
 *    could not be used.
 *
 *//*-------------------------------------------------------------------------*/

static int
xpccut_exclude_characters
(
   char * sourcedest,
   int sdlength,
   const char * excluded_chars
)
{
   int result = 0;
   if (cut_not_nullptr(excluded_chars) && strlen(excluded_chars) > 0)
   {
      char * newcopy = malloc(sdlength);
      if (cut_not_nullptr(newcopy))
      {
         int sourceindex = 0;
         int destindex = 0;
         (void) memcpy(newcopy, sourcedest, sdlength);
         for (sourceindex = 0; sourceindex < sdlength; sourceindex++)
         {
            unsigned charvalue = (unsigned) newcopy[sourceindex];
            cbool_t exclude = cut_not_nullptr(strchr(excluded_chars, charvalue));
            if (! exclude)
               sourcedest[destindex++] = (char) charvalue;
         }
         result = destindex;
         free(newcopy);
      }
   }
   else
      result = sdlength;

   return result;
}

/******************************************************************************
 * xpccut_build_character_set()
 *------------------------------------------------------------------------*//**
 *
 *    Fills in the static array with the characters we need to generate.
 *
 * \threadunsafe
 *    Uses a static buffer, and so is unsuitable for use in more than one
 *    thread of execution.
 *
 * \param final_size_ptr
 *    Holds a pointer to an integer that is filled with the actual size of
 *    the selected character set.  This value is 0 if an error occurred.
 *
 * \param flags
 *    Provides a set of options, documented in the xpccut_fuzz_flags_t
 *    enumeration.  The following values apply to this function:
 *
 *       -  XPCCUT_FF_LETTERS_ONLY.  A shorthand for setting \a
 *          allowed_chars to the upper and lowercase ASCII letters.
 *       -  XPCCUT_FF_NUMBERS_ONLY.  A shorthand for setting \a
 *          allowed_chars to the digits and ".", "+", and "-".
 *
 *    These can be combined.  And, if \a allowed_chars is provided, that
 *    will also be added to the list.
 *
 * \param allowed_chars
 *    Provides an optional list of characters to be used in the strings.
 *    By default, all ASCII characters from 1 to 255 are included.  The null
 *    character is not included.
 *
 * \param excluded_chars
 *    Provides an optional list of characters to exclude from the strings.
 *    By default, no characters are excluded.  Note that this list is not
 *    used if the \a allowed_chars pointer is provided.  Why exclude
 *    characters that the caller has explicitly allowed.
 *
 * \return
 *    Returns the address of the static character-set buffer, filled with
 *    the desired characters.
 *
 * \sideeffect
 *    The actual size of the character set is returned in \a final_size_ptr.
 *
 *//*-------------------------------------------------------------------------*/

#define S_UPPERCASE     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define S_LOWERCASE     "abcdefghijklmnopqrstuvwxyz"
#define S_NUMBERS       "0123456789+-."

static const char *
xpccut_build_character_set
(
   int * final_size_ptr,
   xpccut_fuzz_flags_t flags,
   const char * allowed_chars,
   const char * excluded_chars
)
{
   static const char * const s_letters = S_UPPERCASE S_LOWERCASE;
   static const char * const s_numbers = S_NUMBERS;
   static char s_character_set[256];
   cbool_t ok = true;
   int final_size = 0;
   cbool_t default_chars = cut_is_nullptr(allowed_chars) ||
      strlen(allowed_chars) == 0;

   cbool_t can_allow = cut_not_nullptr(allowed_chars) &&
      strlen(allowed_chars) > 0;

   /*
    * Setting the first character to null allows us to use only the strcat()
    * function, which saves some decision making.  In fact, set the whole
    * thing to nulls.
    */

   (void) memset(s_character_set, 0, sizeof(s_character_set));
   if (flags & XPCCUT_FF_LETTERS_ONLY)
   {
      (void) strcat(s_character_set, s_letters);
      default_chars = false;
      final_size = (int) strlen(s_character_set);
   }
   if (flags & XPCCUT_FF_NUMBERS_ONLY)
   {
      (void) strcat(s_character_set, s_numbers);
      default_chars = false;
      final_size = (int) strlen(s_character_set);
   }
   if (default_chars)
   {
      int index;
      int charvalue;
      for (index = 0, charvalue = 1; index < 255; /* index++, */ charvalue++)
      {
         s_character_set[index++] = (char) charvalue;
      }
      final_size = index;

      if (index <= 1)
      {
         xpccut_errprint_func(_("too many characters excluded from default"));
         ok = false;
      }
   }
   else
   {
      if (can_allow)
      {
         /*
          * We really should not be depending on strlen().  However,
          * we exclude nulls from the explicit character sets anyway.
          * They get added randomly via an option.
          */

         size_t current_length = strlen(s_character_set);
         size_t added_length = strlen(allowed_chars);
         if (added_length > 0)
         {
            if ((current_length+added_length) < 256)
            {
               // strcat(s_character_set, allowed_chars);

               (void) memcpy
               (
                  &s_character_set[current_length],
                  allowed_chars, added_length
               );
               final_size = (int) (current_length + added_length);
            }
            else
            {
               xpccut_errprint_3_func(_("array too long"), "allowed_chars");
               ok = false;
            }
         }
         else
         {
            xpccut_errprint_3_func(_("array is empty"), "allowed_chars");
            ok = false;
         }
      }
   }
   if (ok)
   {
      final_size = xpccut_exclude_characters
      (
         s_character_set, final_size, excluded_chars
      );
   }
   else
      final_size = 0;

   if (cut_not_nullptr(final_size_ptr))
      *final_size_ptr = final_size;

   return &s_character_set[0];
}

/******************************************************************************
 * xpccut_fuzz()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string that is completely randomized, for use in testing
 *    the robustness of string handling.
 *
 * \note
 *    This function uses the C library function rand().  This function is
 *    usually a very simple function of the form
 *
\verbatim
         return ((next = next * 1103515245 + 12345) / 65536) % 32768
\endverbatim
 *
 *    While we have a better (and slower) function called ran2() in the XPC
 *    library, the one used here is almost assuredly good enough for fuzz
 *    testing.
 *
 * \threadunsafe
 *    Due to the usage of xpccut_rand().  Not critical at the moment.
 *
 * \param destination
 *    Provides the destination for the constructed string.
 *
 * \param dlength
 *    Provides the length of the string destination.
 *
 * \param number_of_bytes
 *    Provides the number of bytes to be generated.  This should be less
 *    than \a dlength, and should also be a little smaller to handle the
 *    final null, if generated.  If the \a flags parameter is set to
 *    XPCCUT_RANDOM_SIZE, then the size will range from 0 to \a
 *    number_of_bytes.
 *
 * \param seed
 *    Provides a seed value to use to seed the sequence.  Avoid using seed
 *    values of 0 and 1, as these are used for other purposes.
 *
 * \param flags
 *    Provides a set of options, documented in the xpccut_fuzz_flags_t
 *    enumeration.
 *
 * \param allowed_chars
 *    Provides an optional list of characters to be used in the strings.
 *    By default, all ASCII characters from 1 to 255 are included.
 *
 * \param excluded_chars
 *    Provides an optional list of characters to exclude from the strings.
 *    By default, no characters are excluded.
 *
 * \param prologue
 *    Provides an optional string that will be unconditionally the first
 *    part of the generated string.  This string must be a normal
 *    null-terminated ASCII string.
 *
 * \param epilogue
 *    Provides an optional string that will be unconditionally appended
 *    to the end of the generated string.  This string must be a normal
 *    null-terminated ASCII string.
 *
 * \return
 *    Returns the value of selected seed, in case the caller wants to
 *    generate the same sequence again.  If the seed returned is zero
 *    (XPCCUT_SEED_ERROR) this indicates an error of some kind.
 *
 *//*-------------------------------------------------------------------------*/

unsigned int
xpccut_fuzz
(
   char * destination,
   int dlength,
   int number_of_bytes,
   unsigned int seed,
   xpccut_fuzz_flags_t flags,
   const char * allowed_chars,
   const char * excluded_chars,
   const char * prologue,
   const char * epilogue
)
{
   /*
    * First validate the arguments that can be validated.
    */

   cbool_t ok = cut_not_nullptr(destination);
   if (ok)
   {
      ok = number_of_bytes < dlength;
      if (ok)
         ok = number_of_bytes > 0;

      if (flags & XPCCUT_FF_RANDOM_SIZE)
      {
         number_of_bytes = xpccut_rand(number_of_bytes);

         /*
          * Too much output.
          *
          * if (! xpccut_is_silent())
          * {
          *    fprintf
          *    (
          *       stdout, "%s %d\n",
          *       _("  randomized number of bytes is"), number_of_bytes
          *    );
          * }
          */
      }
      if (ok)
         (void) memset(destination, 0, dlength);   // for easy viewing
      else
         xpccut_errprint_func(_("bad sizes"));
   }
   else
      xpccut_errprint_func(_("null pointer"));

   if (ok)
   {
      int character_set_size = 0;
      const char * character_set = xpccut_build_character_set
      (
         &character_set_size, flags, allowed_chars, excluded_chars
      );

      /*
       * For testing or verification, the XPCCUT_FF_DUMP_CHARSET bit value
       * is use to pass back the created character set, rather than a fuzzed
       * string.
       */

      if (flags & XPCCUT_FF_DUMP_CHARSET)
      {
         if (seed == XPCCUT_SEED_RANDOMIZE)
            seed = 99;

         if (character_set_size > 0)
         {
            if (character_set_size < dlength)
               (void) memcpy(destination, character_set, character_set_size);
            else
            {
               if (! xpccut_is_silent())
               {
                  fprintf
                  (
                     stdout,
                     "buffer length is %d bytes, need %d for char-set\n",
                     dlength, character_set_size
                  );
               }
            }
         }
         else
            xpccut_errprint_func(_("empty character-set generated"));
      }
      else
      {
         if (character_set_size > 0)
         {
            unsigned rangemax = (unsigned) character_set_size;
            int i = 0;
            seed = xpccut_set_seed(seed);
            if (cut_not_nullptr(prologue))
            {
               i = (int) strlen(prologue);
               memcpy(destination, prologue, i);
            }
            for ( ; i < number_of_bytes; i++)
            {
               int random_index = xpccut_rand(rangemax);

               /*
                * fprintf
                * (
                *    stdout, "character[%d] = '%c' (char-set[%d])\n",
                *    i, character_set[random_index], random_index
                * );
                */

               destination[i] = character_set[random_index];
            }
            destination[i] = 0;
            if (cut_not_nullptr(epilogue))
            {
               int length = (int) strlen(epilogue);
               if (i + (int) strlen(epilogue) < number_of_bytes)
                  memcpy(&destination[i], epilogue, length);
            }
         }
         else
         {
            xpccut_errprint_func
            (
               _("empty character-set, cannot make fuzz string")
            );
            seed = 0;
         }

      }
   }
   return seed;
}

/******************************************************************************
 * xpccut_garbled_string()
 *------------------------------------------------------------------------*//**
 *
 *    Provides a string that is lightly randomized, for use in testing string
 *    handling.
 *
 *    Contrary to xpccut_fuzz(), which generates the whole string, this
 *    function accepts a string, and then modifies some of the bytes in it.
 *
 *    Also unlike xpccut_fuzz(), it is the caller's responsibility to set
 *    the seed.
 *
 *    Other than that, this function is easier to use than xpccut_fuzz(),
 *    since it has many fewer parameters.
 *
 *    To do the corruption, the following items are randomly determined:
 *
 *       -# The number of bytes to be altered in the string, up to the
 *          length of the string.
 *       -# The locations of each of the altered bytes in the string.  It
 *          may happen that some of the locations are the same, so that the
 *          number of bytes actually altered is less than given in step 1.
 *       -# The exact values of the replacement bytes.
 *
 *    The replacement bytes are taken from the complete set of ASCII
 *    characters (0 through 255).
 *
 *    However, to really stress the text-input processors of your
 *    application, also run a large number xpccut_fuzz() calls.
 *
 *    Situations to test:
 *
 *       -  Any input.
 *          -  Nulls in the wrong place.
 *          -  Non-printing characters.
 *          -  Too many characters.
 *       -  Numeric input.
 *          -  Non-numeric printable characters.
 *          -  Very large values.
 *          -  Very small values.
 *       -  Text input.
 *          -  Nonsense strings.
 *          -  Non-alphabetic printable characters.
 *
 *    The destination for the constructed string is the source.
 *
 * \param source
 *    Provides the original, uncorrupted string.  If the string is empty, it
 *    is not modified in any way.  Note that this string is \e not assumed
 *    to be null-terminated.  As a side-effect, it may be modified by this
 *    function.
 *
 * \param length
 *    Provides the length of the string source.  If you want to have
 *    some probability of altering the null terminator, increase this value
 *    by 1.  You can control which section of the string gets altered by
 *    modifying \a source and \a length.
 *
 * \return
 *    Returns the number of characters that were actually changed.  If a
 *    number less than zero is returned, an error occurred.
 *
 *//*-------------------------------------------------------------------------*/

int
xpccut_garbled_string
(
   char * source,
   int length
)
{
   int result = -1;
   if (cut_not_nullptr(source))
   {
      if (length > 0)
      {
         int altered_byte_count = xpccut_rand(length);
         int i;
         char * oldversion = malloc(length+1);
         if (cut_not_nullptr(oldversion))
         {
            (void) memcpy(oldversion, source, length+1); // includes possible null
            for (i = 0; i < altered_byte_count; i++)
            {
               int altered_byte_index = xpccut_rand(length);
               int new_byte_value = xpccut_rand(256);
               source[altered_byte_index] = (char) new_byte_value;
               if ((altered_byte_index < 0) || (altered_byte_index >= length))
               {
                  xpccut_errprint_3_func(_("function broken"), "xpccut_rand()");
                  result = -2;
               }
               if ((new_byte_value < 0) || (new_byte_value >= 256))
               {
                  xpccut_errprint_3_func(_("function broken"), "xpccut_rand()");
                  result = -3;
               }
            }
            if (result >= -1)
            {
               result = 0;
               for (i = 0; i < length+1; i++)
               {
                  if (source[i] != oldversion[i])
                     result++;
               }
            }
         }
         else
         {
            xpccut_errprint_func(_("allocation failed"));
            result = -4;
         }
      }
      else
      {
         xpccut_errprint_func(_("empty string"));
         result = -5;
      }
   }
   else
      xpccut_errprint_func(_("null pointer"));

   return result;
}

/******************************************************************************
 * xpccut_dump_string()
 *------------------------------------------------------------------------*//**
 *
 * \param source
 *    Provides the string to be dumped.  The string can include \e any
 *    ASCII characters, including nulls.
 *
 * \param source_length
 *    The full length of the string to be dumped.
 *
 *//*-------------------------------------------------------------------------*/

#define XPCCUT_DUMP_COLUMNS      6

void
xpccut_dump_string (const char * source, int source_length)
{
   cbool_t can_do = cut_not_nullptr(source) && (source_length > 0);
   if (can_do)
   {
      static const char * const s_header =
      "\n    Char        Char        Char        Char        Char        Char\n";

      /*
       * static const char * const s_format =
       *    "%3s %4s    %3s %4s    %3s %4s    %3s %4s    %3s %4s    %3s %4s\n";
       */

      static const char * const s_format_element = "%3s %4s    ";

      int columns = XPCCUT_DUMP_COLUMNS;
      int remainder = source_length % columns;
      int rows = source_length / columns;
      int row;
      if (remainder > 0)
         rows++;

      fprintf(stdout, "%s", s_header);
      for (row = 0; row < rows; row++)
      {
         int base_index = row;
         int column;
         for (column = 0; column < columns; column++)
         {
            char index_string[8];
            char characters[8];
            int index = base_index + column * rows;
            if (index >= source_length)
            {
               (void) sprintf(index_string, "   ");
               sprintf(characters, "    ");
            }
            else
            {
               unsigned ch = (unsigned) source[index];
               (void) sprintf(index_string, "%3d", index);
               if (isprint(ch))
               {
                  if (ch != ' ')
                     sprintf(characters, "  %c ", (char) ch);
                  else
                     sprintf(characters, " \" \"");
               }
               else
               {
                  /*
                   * Note that we have to mask off the extra bits that
                   * occur, since large values of ch act like negative
                   * numbers.
                   */

                  if (ch != 0)
                     sprintf(characters, "0x%02X", ch & 0xff);
                  else
                     sprintf(characters, "0x00");
               }
            }
            fprintf(stdout, s_format_element, index_string, characters);
         }
         fprintf(stdout, "\n");
      }
   }
   else
      xpccut_errprint_func(_("null pointer or empty string"));
}

/******************************************************************************
 * fuzz.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
