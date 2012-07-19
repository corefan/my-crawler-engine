#ifndef TUT_MAIN_H
#define TUT_MAIN_H

#include "tut.hpp"
#include "tut_console_reporter.hpp"
#include "tut_cppunit_reporter.hpp"


namespace tut
{

	/** Helper function to make test binaries simpler.
	*
	* Example of basic usage follows.
	*
	* @code
	*  namespace tut { test_runner_singleton runner; }
	*
	*  int main(int argc, char **argv)
	*  {
	*      if( TutMain(argc, argv) )
	*          return 0;
	*      else
	*          return -1;
	*  }
	* @endcode
	*
	* It is also possible to do some generic initialization before
	* running any tests and cleanup before exiting application.
	* Note that TutMain can throw tut::no_such_group or tut::NoSuchTest.
	*
	* @code
	*  namespace tut { test_runner_singleton runner; }
	*
	*  int main(int argc, char **argv)
	*  {
	*      tut::xml_reporter reporter;
	*      tut::runner.get().insert_callback(&reporter);
	*
	*      MyInit();
	*      try
	*      {
	*          TutMain(argc, argv);
	*      }
	*      catch(const tut::tut_error &ex)
	*      {
	*          std::cerr << "TUT error: " << ex.what() << std::endl;
	*      }
	*      MyCleanup();
	*  }
	* @endcode
	*/
#ifdef _UNICODE
	inline bool TutMain(int argc, const TCHAR * const * const argv, tOStream &os = std::wcerr)
#else
	inline bool TutMain(int argc, const TCHAR * const * const argv, tOStream &os = std::cerr)
#endif
	{
		tStringStream usage;
		usage << _T("Usage: ") << argv[0] << _T(" [group] [testcase]") << std::endl;

		GroupNames gr;
		RunnerInstance().GroupsName(gr);
		usage << _T("Available test groups:") << std::endl;

		for(GroupNames::const_iterator i = gr.begin(); i != gr.end(); ++i)
		{
			usage << "    " << *i << std::endl;
		}

		if( argc > 1 )
		{
			if( tString(argv[1]) == _T("-h") ||
				tString(argv[1]) == _T("--help") ||
				tString(argv[1]) == _T("/?") ||
				argc > 3 )
			{
				os << usage.rdbuf();
				return false;
			}
		}

		// Check command line options.
		switch(argc)
		{
		case 1:
			RunnerInstance().RunTests();
			break;

		case 2:
			RunnerInstance().RunTests(argv[1]);
			break;

		case 3:
			{
				TCHAR *end = 0;
				int t = _tcstol(argv[2], &end, 10);
				if(end != argv[2] + _tcslen(argv[2]))
				{
					throw NoSuchTest(_T("`") + tString(argv[2]) + _T("` should be a number"));
				}

				TestResult tr;
				if(!RunnerInstance().RunTest(argv[1], t, tr) || tr.result_ == TestResult::dummy)
				{
					throw NoSuchTest(_T("No testcase `") + tString(argv[2]) + _T("` in group `") + argv[1] + _T("`"));
				}
			}
			break;
		}

		return true;
	} // TutMain()

}

#endif
