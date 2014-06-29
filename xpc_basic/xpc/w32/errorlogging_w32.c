
/******************************************************************************
 * getlasterrortext()
 *------------------------------------------------------------------------*//**
 *
 *    Retrieve a human-readable error message. The message will be stored in
 *    a native (byte-based) string.  Optionally, the error code can be
 *    included in the message by setting the 'includecode' parameter to a
 *    non-zero value.  By default this is what is passed.
 *
 *    The message is obtained from the operating system based on the message
 *    ID returned by the Win32 function GetLastError().  It is formatted
 *    into a self-allocated array by FormatMessage().
 *
 *    If there is no system error in force, a null is returned, and the call
 *    should not show the message (or at least show a success message.)
 *
 *//*-------------------------------------------------------------------------*/

const char * getlasterrortext (int includecode)
{
   const char * result = nullptr;
   DWORD errorcode = GetLastError();                        /* msg ID        */
   if (errorcode != 0)
      result = getlasterrortextex(errorcode, includecode);

   return result;
}

/******************************************************************************
 * getlasterrortextex()
 *------------------------------------------------------------------------*//**
 *
 *    Creates an error message base on a system error code.
 *
 * @win32
 *    This function uses FormatMessage() to create an error message.
 *
 * @threadunsafe
 *    This function employs a static buffer.  See getlasterrortext_r() for a
 *    better function.
 *
 * @param errorcode
 *    Provides an integer representing a system error code, as returned by
 *    the Win32 function GetLastError().
 *
 * @param includecode
 *    If set to 'true' (1), the function includes the integer value of the
 *    error code in the message's output.
 *
 * @return
 *    A pointer to the message buffer is returned.
 *
 *----------------------------------------------------------------------------*/

const char * getlasterrortextex (int errorcode, int includecode)
{
   const size_t textbuffersize = 1024;
   static char textbuffer[1024];
   LPTSTR lpsztemp = nullptr;
   DWORD dresult = FormatMessage
   (
      FORMAT_MESSAGE_ALLOCATE_BUFFER   |
      FORMAT_MESSAGE_FROM_SYSTEM       |
      FORMAT_MESSAGE_ARGUMENT_ARRAY,
      0,                                                 /* no msg source    */
      errorcode,                                         /* msg ID           */
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),         /* language ID      */
      (LPTSTR) &lpsztemp,                                /* msg buffer       */
      0,                                                 /* no max size      */
      0                                                  /* no insert args   */
   );
   if ((dresult == 0) || (textbuffersize < dresult+14))  /* why 14?          */
      strcpy(textbuffer, _("Text buffer too small"));    /* tell programmer  */
   else if (not_nullptr(lpsztemp))
   {
      if (includecode)
      {
         sprintf                                         /* get msg & code   */
         (
            textbuffer, _("Error %lu: %s"),
            (unsigned long) errorcode, lpsztemp
         );
      }
      else
         sprintf(textbuffer, _("Error: %s"), lpsztemp);  /* "copy" the msg   */

      (void) LocalFree(lpsztemp);                        /* free up!         */
   }
   else
      strcpy(textbuffer, _("Bad message buffer allocation"));

   return &textbuffer[0];
}

/******************************************************************************
 * getlasterrortext_r()
 *------------------------------------------------------------------------*//**
 *
 *    Creates an error message base on a system error code.
 *
 * @win32
 *    This function uses FormatMessage() to create an error message.
 *
 * @threadunsafe
 *    This function employs a static buffer.  See getlasterrortext_r() for a
 *    better function.
 *
 * @param errorcode
 *    Provides an integer representing a system error code, as returned by
 *    the Win32 function GetLastError().
 *
 * @param includecode
 *    If set to 'true' (1), the function includes the integer value of the
 *    error code in the message's output.
 *
 * @return
 *    A pointer to the message buffer is returned.
 *
 *----------------------------------------------------------------------------*/

const char * getlasterrortext_r (char * textbuffer, size_t textbuffersize)
{
   LPTSTR lpsztemp = nullptr;
   DWORD errorcode = GetLastError();                     /* the latest FUBAR  */
   DWORD dresult = FormatMessage
   (
      FORMAT_MESSAGE_ALLOCATE_BUFFER   |
      FORMAT_MESSAGE_FROM_SYSTEM       |
      FORMAT_MESSAGE_ARGUMENT_ARRAY,
      0,                                                 /* no msg source     */
      errorcode,                                         /* Win32 msg ID      */
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),         /* language ID       */
      (LPTSTR) &lpsztemp,                                /* msg buffer        */
      1024,                                              /* allocation size   */
      0                                                  /* no insert args    */
   );
   if ((dresult == 0) || (textbuffersize < dresult+14))  /* why 14?           */
      strcpy(textbuffer, _("Text buffer too small"));    /* tell programmer   */
   else if (not_nullptr(lpsztemp))
   {
      lpsztemp[_tcsclen(lpsztemp)-2] = 0;                /* remove CR/LF      */
      sprintf                                            /* get msg & code    */
      (
         textbuffer, _("Error %lu: %s"),
         (unsigned long) errorcode, lpsztemp
      );

      (void) LocalFree(lpsztemp);                        /* free it up!       */
   }
   else
      strcpy(textbuffer, _("Bad message buffer allocation"));

   return &textbuffer[0];
}

