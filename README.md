xpc-suite 1.1

This project supports one sub-project, supporting some sub-sub-projects.
One incomplete sub-project was removed pending a radical change in
technique.

   -  xpc_basic.
      -  xpc.  Provides handy code for C programs:
         -  Macros to specify OS, CPU, and version information.
         -  Functions for logging info and error messages.
         -  Wrappers for file and string handling.
         -  Wrappers for pthreads.
         -  Functions for numeric data.
         -  A rudimentary INI partser.
         -  GNU gettext support.
         -  Other portability code.
      -  xpccut.  Provides C structures and C functions to support
         unit-testing, regression-testing, and timing.  This library is
         independent from the rest of XPC, and provides thorough tests of
         itself.
      -  xpchello.  Provides a "Hello, World" program a la GNU's version.
         Provides some examples of bad pointers and core dumps.
      -  xpc++.  Provides a wrapper for the xpc/errorlog module, and
         classes for doing averaging, handling string-maps, and systemtime.
      -  xpccut++.  Provides C++ wrappers for the xpccut project that makes
         it even easier to use.  Also tests itself.
   -  xpc_comm.  (Project removed pending change in focus).

Currently Linux-only, and built by GNU Autotools.  I get my fill of Windows
at work.

You will want to first go to the xpc_basic directory and run
"./bootstrap --help".

Chris Ahlstrom, 2014-06-29

   I am a professional programmer using C and C++, and lately, Java EE.
   My hobbies are writing code, running, and playing soccer.

# vim: ft=rc
