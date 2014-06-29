#ifndef XPC_FILE_MACROS_H
#define XPC_FILE_MACROS_H

/******************************************************************************
 * file_macros.h
 *------------------------------------------------------------------------*//**
 *
 * \file    file_macros.h
 *
 *    This module provides various macros and type definitions for dealing
 *    with files and file-names.
 *
 * \author  Chris Ahlstrom
 * \date    2012-06-09
 * \updates 2012-06-09
 * \version $Revision$
 *
 *    Utilities for file size, file modification time, and other functions.
 *
 *//*-------------------------------------------------------------------------*/

/******************************************************************************
 * Macros:        File macros
 *------------------------------------------------------------------------*//**
 *
 *    These macros help reduce the number of special cases that have to be
 *    coded to support Borland C++ 5.0 and Microsoft Visual C++ 5.0 in the
 *    same module, when file access and filenames are involved.
 *
 *    For Borland, _open() and _close() are considered obsolete.  The
 *    Borland replacement, _rtl_open(), is considered binary, so there is no
 *    O_BINARY flag, and anyway, Microsoft spells the flag as _O_BINARY.
 *
 *    Microsoft spells Borland's fstat struct as "_fstat".
 *
 *    In Borland C++ Builder 3.0, using a plain ftime() call can yield an
 *    error about ambiguity between ftime() and std::ftime.  So we prefix
 *    the global scope operator '::'.
 *
 *//*-------------------------------------------------------------------------*/

#if defined __BORLANDC__    // ============ BORLAND =========================

#if !defined WIN32
#pragma message("Certain constructs will be disabled in 16-bit code")
#define NO_WIN32_CONSTRUCTS
#endif

#include <dir.h>                       /* file-name info and getcwd()         */

typedef struct stat        FStatusBuf;
typedef struct timeb       TimeBuff;

#define F_MAX_DEVICE       16          /* Win32 device name size              */
#define F_ACCESS           access
#define F_CHDIR            chdir
#define F_DELETE           unlink
#define F_GETCWD           getcwd
#define F_FFBLK            struct ffblk
#define F_FINDCLOSE(x)                 /* Borland doesn't have this one       */
#define F_FINDFIRST        findfirst
#define F_FINDNEXT         findnext
#define F_FINDDATA         F_FFBLK
#define F_FTIME            ::ftime
#define F_MAX_DIR          MAXDIR
#define F_MAX_DRIVE        MAXDRIVE
#define F_MAX_EXT          MAXEXT
#define F_MAX_FNAME        MAXPATH
#define F_MAX_PATH         MAXPATH
#define F_MKDIR            mkdir
#define F_RENAME           rename
#define F_RMDIR            rmdir
#define F_STAT             ::fstat
#define F_STAT_EX          ::stat
#define F_UTIL_OPEN_FLAG   (O_RDONLY)

#define _S_IEXEC           0000100
#define _S_IFDIR           0040000

#define S_ACCESS           access
#define S_CLOSE            close
#define S_FOPEN            fopen
#define S_LSEEK            lseek
#define S_OPEN             open
#define S_STAT             stat
#define S_UNLINK           unlink

typedef struct stat stat_t;

#elif defined _MSC_VER     // ============ MICROSOFT =======================

typedef struct _stat       FStatusBuf;
typedef struct _timeb      TimeBuff;

#define F_MAX_DEVICE       16
#define F_ACCESS           _access
#define F_CHDIR            _chdir
#define F_DELETE           _unlink
#define F_GETCWD           _getcwd
#define F_FFBLK            struct _finddata_t
#define F_FINDCLOSE(x)     _findclose(x)
#define F_FINDFIRST        _findfirst
#define F_FINDNEXT         _findnext
#define F_FINDDATA         F_FFBLK
#define F_FTIME            _ftime
#define F_MAX_DIR          _MAX_DIR          /* 256 (stdlib.h)                */
#define F_MAX_DRIVE        _MAX_DRIVE        /*   3                           */
#define F_MAX_EXT          _MAX_EXT          /* 256                           */
#define F_MAX_FNAME        _MAX_FNAME        /* 256                           */
#define F_MAX_PATH         _MAX_PATH         /* 260                           */
#define F_MKDIR            _mkdir
#define F_RENAME           rename            /* an Microsoft inconsistency    */
#define F_RMDIR            _rmdir
#define F_STAT             _fstat
#define F_STAT_EX          _stat
#define F_UTIL_OPEN_FLAG   (_O_BINARY | _O_RDONLY)

#define S_ACCESS           _access_s
#define S_CLOSE            _close
#define S_FOPEN            fopen_s
#define S_LSEEK            _lseek
#define S_OPEN             _sopen_s
#define S_STAT             _stat
#define S_UNLINK           _unlink

typedef struct _stat stat_t;

#ifdef WIN32

#define F_OK         0x00              /* existence                           */
#define X_OK         0x01              /* executable (not useful on Windows)  */
#define W_OK         0x02              /* writability                         */
#define R_OK         0x04              /* readability                         */

#define O_APPEND     _O_APPEND
#define O_BINARY     _O_BINARY
#define O_CREAT      _O_CREAT
#define O_RDONLY     _O_RDONLY
#define O_RDWR       _O_RDWR
#define O_TRUNC      _O_TRUNC
#define O_WRONLY     _O_WRONLY

#endif

#else                        // ============= OTHER ==========================

typedef struct stat        FStatusBuf;
typedef struct timeb       TimeBuff;

#define F_MAX_DEVICE        6             /* DOS device name (e.g.  "COM1:")  */
#define F_ACCESS           access
#define F_CHDIR            chdir
#define F_GETCWD           getcwd
#define F_DELETE           unlink
#define F_FFBLK            ffblk
#define F_FINDCLOSE(x)     findclose(x)
#define F_FINDFIRST                       /* this will cause errors           */
#define F_FINDNEXT
#define F_FINDDATA         F_FFBLK
#define F_FTIME            ftime
#define F_MAX_DIR          256
#define F_MAX_DRIVE          3
#define F_MAX_EXT          256
#define F_MAX_FNAME        256
#define F_MAX_PATH         260
#define F_MKDIR            mkdir
#define F_RMDIR            rmdir
#define F_RENAME           rename
#define F_STAT             fstat
#define F_STAT_EX          stat
#define F_UTIL_OPEN_FLAG   (O_RDONLY)     /* O_BINARY not supported in gcc    */

#define S_ACCESS           access      /* ISO/POSIX/BSD unsafe access()       */
#define S_CLOSE            close       /* ISO/POSIX/BSD close()               */
#define S_FOPEN            fopen       /* ISO/POSIX/BSD safe sprintf()        */
#define S_LSEEK            lseek       /* ISO/POSIX/BSD lseek()               */
#define S_OPEN             open        /* ISO/POSIX/BSD safe open()           */
#define S_STAT             stat        /* ISO/POSIX/BSD stat function         */
#define S_UNLINK           remove      /* ISO/POSIX/BSD unlink()              */

typedef struct stat stat_t;            /* ISO/POSIX/BSD stat structure        */

#endif   // __BORLANDC__ versus _MSC_VER versus Other

#define F_MAX_PREFIX       (F_MAX_DRIVE + 1 + F_MAX_DIR)
#define F_MAX_FILESPEC      260        /* Use the crummy Windows limit        */
#define F_EXISTS           0x00
#define F_WRITE_PERMISSION 0x02
#define F_READ_PERMISSION  0x04
#define F_OPEN_PERMISSION  (F_WRITE_PERMISSION | F_READ_PERMISSION)

#endif      // XPC_FILE_MACROS_H

/******************************************************************************
 * End of file_macros.h
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *----------------------------------------------------------------------------*/
