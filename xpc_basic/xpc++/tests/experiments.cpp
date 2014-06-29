/******************************************************************************
 * experiments.cpp
 *------------------------------------------------------------------------*//**
 *
 * \file          experiments.cpp
 * \library       libxpc++
 * \author        Chris Ahlstrom
 * \updates       2011-09-19 to 2011-09-27
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This infolication provides a few experiments.
 *
 *//*-------------------------------------------------------------------------*/

#include <stdexcept>                   /* std::logic_error                    */
#include <iostream>                    /* std::cout and std::cerr             */
#include <xpc/cut.hpp>                 /* xpc::cut unit-test class            */
#include <xpc/errorlog.hpp>            /* xpc::errorlog class                 */

#include "infolist.hpp"                /* xpc::infolist and xpc::info classes */

namespace xpc
{

/******************************************************************************
 * Manager
 *------------------------------------------------------------------------*//**
 *
 *//*-------------------------------------------------------------------------*/

class Manager
{

private:

   // Manager (const Manager &);
   // Manager operator = (const Manager &);

public:

	Manager ();
	virtual ~Manager();

   bool check (Info a);          // (const Info & a);
   bool check_all ();

private:


   /**
    * Provides a list of all of the info items (and their Windows
    * window class names) that are to be controlled.
    */

   xpc::InfoList my_Info_List;

};

/******************************************************************************
 * Default constructor
 *------------------------------------------------------------------------*//**
 *
 *		Doesn't do much.
 *
 *//*-------------------------------------------------------------------------*/

Manager::Manager ()
 :
   my_Info_List   ()
{
   // no other ocde
}

/******************************************************************************
 * Destructor()
 *------------------------------------------------------------------------*//**
 *
 *		Doesn't do anything.
 *
 *//*-------------------------------------------------------------------------*/

Manager::~Manager ()
{
   // no other ocde
}

/******************************************************************************
 * check()
 *------------------------------------------------------------------------*//**
 *
 *		Handle one item.
 *
 *//*-------------------------------------------------------------------------*/

bool Manager::check (Info a)
{
   bool result = false;
   if (! a.tag().empty())
   {
      result = true;
   }
   return result;
}

/******************************************************************************
 * check_all()
 *------------------------------------------------------------------------*//**
 *
 *		Handle the container of items.
 *
 *//*-------------------------------------------------------------------------*/

#ifdef USE_LOOP_CODE

bool Manager::check_all ()
{
   bool result = true;
   const InfoList::Container & infolist = my_Info_List.info();
   InfoList::Container::const_iterator aci;
   InfoList::Container::const_iterator aci_end = infolist.end();
   for (aci = infolist.begin(); aci != aci_end; aci++)
   {
      result = check(*aci);
      if (! result)
         break;
   }
   return result;
}

#else    // USE_LOOP_CODE

/*
 * Seems to render VS 2010 unstable.
 */

typedef bool (Manager::* InfoFunction ) (Info);
typedef std::mem_fun1_ref_t<bool, Manager, Info> InfoFunctor;
typedef std::binder1st<InfoFunctor> InfoBinding;

bool Manager::check_all ()
{
   InfoFunction perform = &Manager::check;
   InfoFunctor memfunc = std::mem_fun_ref(perform);
   InfoBinding binding = std::bind1st(memfunc, *this);
   return true;
}


#endif   // USE_LOOP_CODE

}     // namespace xpc

/******************************************************************************
 * experiments_01_01()
 *------------------------------------------------------------------------*//**
 *
 *    Tries to duplicate a problem I saw in Visual Studio.
 *
 * \group
 *    1. Problem tests.
 *
 * \case
 *    1. Basic smoke test.
 *
 * \tests
 *    -  xpc::cutxxx()
 *
 * \param options
 *    Provides the command-line options for the unit-test infolication.
 *
 * \return
 *    Returns the unit-test status object needed by the protocol.
 *
 *//*-------------------------------------------------------------------------*/

static xpc::cut_status
experiments_01_01 (const xpc::cut_options & options)
{
   xpc::cut_status status(options, 1, 1, "XPC++", _("Basic Problem Test"));
   bool ok = status.valid();        /* note that invalidity is /not/ an error */
   if (ok)
   {
      if (! status.can_proceed())                  /* is test allowed to run? */
      {
         status.pass();                            /* no, force it to pass    */
      }
      else
      {
         xpc::cut::show(options, _("No values to show in this test"));
         if (status.next_subtest("cut_status::pass()"))
            status.pass();
      }
   }
   return status;
}

/******************************************************************************
 * main()
 *------------------------------------------------------------------------*//**
 *
 *    This is the main routine for the experiments infolication.
 *
 *//*-------------------------------------------------------------------------*/

#define EXPER_APP_NAME           "Experiments"
#define EXPER_TEST_NAME          "experiments"
#define EXPER_TEST_VERSION       1.0.5
#define DEFAULT_BASE             "../tests"
#define DEFAULT_AUTHOR           "Chris Ahlstrom"

int
main
(
   int argc,               /**< Number of command-line arguments.             */
   char * argv []          /**< The actual array of command-line arguments.   */
)
{
   static const char * const additional_help =

"\n"
EXPER_TEST_NAME "-specific options:\n"
"\n"
" --leak-check       Turn on some leak-checking code.\n"

   ;

   /*
    * Note that the XPCCUT_VERSION_STRING macro is defined in the
    * unit_test.h module of the XPCCUT unit-test C library.
    */

   xpc::cut testbattery
   (
      argc, argv,
      std::string(EXPER_TEST_NAME),
      std::string(XPCCUT_VERSION_STRING(EXPER_TEST_VERSION)),
      std::string(additional_help)
   );
   bool ok = testbattery.valid();
   if (ok)
   {
      int argcount = argc;
      char ** arglist = argv;
      cbool_t load_the_tests = true;
      if (argcount > 1)
      {
         int currentarg = 1;
         while (currentarg < argcount)
         {
            std::string arg = arglist[currentarg];
            if (arg == "--no-leak-check")
            {
               // gs_do_leak_check = false;
            }
            currentarg++;
         }

         /*
          * The xpc::mainlog() function provides a reference to a hidden
          * xpc::errorlog object that is always available [once mainlog()
          * first gets called, that is].  Here, we set the
          * main errorlog object according to the command-line arguments.
          */

         ok = log_parse(argc, argv);
      }
      if (ok && load_the_tests)
      {
         ok = testbattery.load(experiments_01_01);
         if (ok)
         {
            // (void) testbattery.load(experiments_01_02);
         }
      }
      if (ok)
         ok = testbattery.run();
      else
         xpccut_errprint(_("load of test functions failed"));
   }
   return ok ? EXIT_SUCCESS : EXIT_FAILURE ;
}

/******************************************************************************
 * experiments.cpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
