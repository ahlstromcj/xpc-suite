/******************************************************************************
 * os.c
 *------------------------------------------------------------------------*//**
 *
 * \file          os.c
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \updates       03/08/2008-10/09/2009
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides macros to manage some of the differences between C support in
 *    operating systems.
 *
 * \note
 *    Links to follow up on:
 *       -  Detecting the Operating System Version (in Win32) --
 *          http://www.codeproject.com/win32/osdetect.asp
 *
 *//*-------------------------------------------------------------------------*/

#include <xpc/errorlogging.h>          /* C error-logging facility            */
#include <xpc/gettext_support.h>       /* _() internationalization macro      */
#include <xpc/file_macros.h>           /* HOST_NAME_MAX                       */
#include <xpc/integers.h>              /* int64_t, uint64_t, etc.             */
XPC_REVISION(os)

#if XPC_HAVE_UNISTD_H
#include <unistd.h>                    /* sysconf() cfg exploration function  */
#endif

#undef XPC_USE_UNAME

#if XPC_HAVE_SYS_UTSNAME_H             /* not yet supported; see UNAME(2)     */
#include <sys/utsname.h>               /* struct utsname & uname()            */
#define XPC_USE_UNAME
#endif

/******************************************************************************
 * xpc_os_name()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the name of the operating system on which the application is
 *    executing.
 *
 *    On systems that support it, this information is obtained via a call to
 *    the uname() function.
 *
 * \threadunsafe
 *    This function uses a static buffer, but it is not much of an issue for
 *    this function.
 *
 * \todo
 *    A lot more work needs to be done on the xpc_os_name() function.
 *
 * \return
 *    A pointer to the private buffer holding the name of the operating
 *    system.
 *
 * \unittests
 *    -  cpu_os_test_02_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_os_name (void)
{

#ifdef XPC_USE_UNAME                /* defined if sys/utsname.h exists        */

   static char destination[128];
   struct utsname systeminfo;
   int retcode = uname(&systeminfo);
   if (retcode == 0)
   {
      /*
       * struct utsname
       * {
       *    sysname,
       *    nodename,
       *    release,
       *    version,
       *    machine,
       *    domainname (if _GNU_SOURCE)
       * }
       *
       * All fields are character arrays, no length, null-terminated.
       */

      strcpy(destination, systeminfo.sysname);
   }
   else
   {
      xpc_errprint_func(_("failed"));
      return nullptr;
   }
   return destination;

#else

   static const char * operating_system = XPC_OS_CATEGORY;
   return operating_system;

#endif

}

/******************************************************************************
 * xpc_host_name()
 *------------------------------------------------------------------------*//**
 *
 *    Obtains the name of the computer host on which the application is
 *    executing.
 *
 *    On systems that support it, this information is obtained via a call to
 *    the gethostname() function.
 *
 * \threadunsafe
 *    This function uses a static buffer, but it is not much of an issue for
 *    this function.
 *
 * \return
 *    A pointer to the private buffer holding the name of the computer.
 *
 * \unittests
 *    -  cpu_os_test_02_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

const char *
xpc_host_name (void)
{
   static char destination[HOST_NAME_MAX];         /* a TCHAR in Win32        */
   size_t length = HOST_NAME_MAX;                  /* a DWORD in Win32        */

#ifdef WIN32

   cbool_t okay = GetComputerName((LPTSTR) destination, (DWORD *) & length);

#else

   int rcode = gethostname(destination, HOST_NAME_MAX);
   cbool_t okay = is_posix_success(rcode);

#endif

   if (okay)
      destination[length-1] = 0;
   else
   {
      xpc_strerrnoprint_func(_("failed"));
      destination[0] = 0;
   }
   return destination;
}

/******************************************************************************
 * xpc_the_sizeof_it()
 *------------------------------------------------------------------------*//**
 *
 *    Shows the sizes of a large number of data types.
 *
 * \bit64
 *    The "(long unsigned int)" cast is needed only for the -1's.
 *
 * \bit32
 *    The "(long unsigned int)" cast is needed for the -1's and all off the
 *    numeric arguments
 *
 * \todo
 *    Provide a function that shows the minimum and maximum values of the
 *    various types.
 *
 * \unittests
 *    -  cpu_os_test_02_01() ["smoke" test]
 *    -  TBD.
 *
 *//*-------------------------------------------------------------------------*/

void
xpc_the_sizeof_it (void)
{
   fprintf
   (
      stdout,
      M_(
      "%s:\n"
      "   sizeof(char)               = %u\n"
      "   sizeof(unsigned char)      = %u\n"
      "   sizeof(short)              = %u\n"
      "   sizeof(unsigned short)     = %u\n"
      "   sizeof(int16_t)            = %u\n"
      "   sizeof(uint16_t)           = %u\n"
      "   sizeof(int)                = %u\n"
      "   sizeof(unsigned int)       = %u\n"
      "   sizeof(int32_t)            = %u\n"
      "   sizeof(uint32_t)           = %u\n"
      "   sizeof(long)               = %u\n"
      "   sizeof(long long)          = %u\n"
      "   sizeof(unsigned long long) = %u\n"
      "   sizeof(int64_t)            = %u\n"
      "   sizeof(uint64_t)           = %u\n"
      "   sizeof(size_t)             = %u\n"
      "   sizeof(intptr_t)           = %u\n"
      "   sizeof(uintptr_t)          = %u\n"
      "   sizeof(void *)             = %u\n"
      ),
      _("Data-type sizes"),
      (unsigned int) sizeof(char),
      (unsigned int) sizeof(unsigned char),
      (unsigned int) sizeof(short),
      (unsigned int) sizeof(unsigned short),
      (unsigned int) sizeof(int16_t),
      (unsigned int) sizeof(uint16_t),
      (unsigned int) sizeof(int),
      (unsigned int) sizeof(unsigned int),
      (unsigned int) sizeof(int32_t),
      (unsigned int) sizeof(uint32_t),
      (unsigned int) sizeof(long),
#ifdef WIN64
      (unsigned int) sizeof(long long),
      (unsigned int) sizeof(unsigned long long),
#else
#ifdef WIN32
      (unsigned int) -1,
      (unsigned int) -1,
#else
      (unsigned int) sizeof(long long),
      (unsigned int) sizeof(unsigned long long),
#endif
#endif
      (unsigned int) sizeof(int64_t),
      (unsigned int) sizeof(uint64_t),
      (unsigned int) sizeof(size_t),
      (unsigned int) sizeof(intptr_t),
      (unsigned int) sizeof(uintptr_t),
      (unsigned int) sizeof(void *)
   );
}

/******************************************************************************
 * os.c
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
