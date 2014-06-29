"******************************************************************************
"  cpp.vim
"------------------------------------------------------------------------------
"
"  Language:      C/C++
"  Maintainer:    Chris Ahlstrom <ahlstromcj@gmail.com>
"  Last Change:   2006-09-04 to 2013-07-19
"  Project:       XPC Basic library project
"  Usage:
"
"     This file is a Vim syntax add-on file used in addition to the installed
"     version of the cpp.vim file provided by vim.
"
"     This file is similar to c.vim, but for C++ code.  It adds keywords
"     and syntax highlighting useful to vim users.  Please note that
"     all of the keywords in c.vim also apply to C++ code, so that they
"     do not need to be repeated here.
"
"     Do a ":set runtimepath" command in vim to see what it has in it.  The
"     first entry is usually "~/.vim", so create that directory, then add an
"     "after" and "syntax" directory, so that you end up with this directory:
"
"              ~/.vim/after/syntax
"
"     Then copy the present file (cpp.vim) to 
"
"              ~/.vim/after/syntax/cpp.vim
"
"     Verify that your code now highlights the following symbols when edited.
"
"------------------------------------------------------------------------------

"------------------------------------------------------------------------------
" Our type definitions for new classes and types added by the XPC++ library
"------------------------------------------------------------------------------

syn keyword XPCC atomic_bool atomic_int autoclientthread autocritex
syn keyword XPCC autowrapclientthread averager
syn keyword XPCC client client_func_t clientlist clientthread condition critex
syn keyword XPCC inethostaddress linear_regression mutex
syn keyword XPCC ringbuffer semaphore server serverthread
syn keyword XPCC socketbase socketendpoint socketexception socketthread
syn keyword XPCC synchbase thread threadbase threadstacker
syn keyword XPCC UnitTest UnitTestFunction UnitTestOptions UnitTestStatus
syn keyword XPCC win32condition win32event

syn keyword XPCC log_parse log_shownothing log_showerrors log_showwarnings
syn keyword XPCC log_showinfo log_showall log_showdebug
syn keyword XPCC log_errprint log_errprintf log_errprintex log_warnprint
syn keyword XPCC log_warnprintf log_warnprintex log_infoprint log_infoprintf
syn keyword XPCC log_infoprintex log_strerrprint log_strerrprint log_strerrnoprint
syn keyword XPCC log_strerrnoprintex log_errprint_func log_warnprint_func
syn keyword XPCC log_infoprint_func log_strerrprint_func log_strerrnoprint_func

"------------------------------------------------------------------------------
" Our type definition for inside comments
"------------------------------------------------------------------------------

syn keyword cTodo contained cpp hpp CPP HPP krufty

"------------------------------------------------------------------------------
" Our Doxygen aliases to highlight inside of comments
"------------------------------------------------------------------------------

syn keyword cTodo contained constructor copyctor ctor
syn keyword cTodo contained defaultctor destructor dtor operator paop paoperator
syn keyword cTodo contained pure singleton virtual

"------------------------------------------------------------------------------
" Our type definitions that are basically standard C++
"------------------------------------------------------------------------------

syn keyword cType fstream ifstream ofstream istream ostream
syn keyword cType stringstream istringstream ostringstream
syn keyword cType auto_ptr const_iterator iterator map multimap set std string
syn keyword cType size_type pair type_info vector wstring
syn keyword cType bad_alloc exception
syn keyword cType first second value_type
syn keyword cType begin c_str clear empty end erase find
syn keyword cType insert length make_pair size

"------------------------------------------------------------------------------
" Operators, language constants, or manipulators
"------------------------------------------------------------------------------

syn keyword cppOperator cin cout cerr endl nothrow npos

"------------------------------------------------------------------------------
" Less common C data typedefs
"------------------------------------------------------------------------------

syn keyword cType my_data_t

"------------------------------------------------------------------------------
" Our slough of macros
"------------------------------------------------------------------------------

syn keyword cConstant xxxxxxx
syn keyword cDefine SCUZZGOZIO

"------------------------------------------------------------------------------
" cpp.vim
"------------------------------------------------------------------------------
" vim: ts=3 sw=3 et ft=vim
"------------------------------------------------------------------------------
