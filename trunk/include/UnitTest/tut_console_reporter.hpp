#ifndef TUT_CONSOLE_REPORTER
#define TUT_CONSOLE_REPORTER

#include "tut.hpp"



namespace
{

	tut::tOStream& operator<<(tut::tOStream& os, const tut::TestResult& tr)
	{
		switch(tr.result_)
		{
		case tut::TestResult::ok:
			os << '.';
			break;
		case tut::TestResult::fail:
			os << '[' << tr.testNum_ << _T(": Failed]");
			break;
		case tut::TestResult::ex_ctor:
			os << '[' << tr.testNum_ << _T(": Constructor Exception]");
			break;
		case tut::TestResult::ex:
			os << '[' << tr.testNum_ << _T(": Exception]");
			break;
		case tut::TestResult::warn:
			os << '[' << tr.testNum_ << _T(": Warning]");
			break;
		case tut::TestResult::term:
			os << '[' << tr.testNum_ << _T(": Terminate]");
			break;
		case tut::TestResult::rethrown:
			os << '[' << tr.testNum_ << _T(": Rethrow]");
			break;
		case tut::TestResult::skipped:
			os << '[' << tr.testNum_ << _T(": Skipped]");
			break;
		case tut::TestResult::dummy:
			throw tut::TutError(_T("Console reporter called for dummy Result"));
		}

		return os;
	}

} // end of namespace

namespace tut
{

	/**
	* Default TUT Callback handler.
	*/
	class ConsoleReporter 
		: public tut::Callback
	{
		typedef std::vector<tut::TestResult> NotPassList;

	private:
		tString CurGroup_;
		NotPassList notPassed_;
		tOStream& os_;

		int okCount_;
		int exceptionsCount_;
		int failuresCount_;
		int terminationsCount_;
		int warningsCount_;
		int skippedCount_;

	public:
		ConsoleReporter()
			: CurGroup_(),
			notPassed_(),
#ifdef _UNICODE
			os_(std::wcout),
#else
			os_(std::cout),
#endif
			okCount_(0),
			exceptionsCount_(0),
			failuresCount_(0),
			terminationsCount_(0),
			warningsCount_(0),
			skippedCount_(0)
		{
			_Init();
		}

		explicit ConsoleReporter(tOStream& out)
			: CurGroup_(),
			notPassed_(),
			os_(out),
			okCount_(0),
			exceptionsCount_(0),
			failuresCount_(0),
			terminationsCount_(0),
			warningsCount_(0),
			skippedCount_(0)

		{
			_Init();
		}

	private:
		ConsoleReporter(const ConsoleReporter &);
		ConsoleReporter &operator=(const ConsoleReporter &);

	public:
		virtual void RunStart()
		{
			_Init();
		}

		virtual void TestCompleted(const tut::TestResult& tr)
		{
			if( tr.groupName_ != CurGroup_ )
			{
				os_ << std::endl << tr.groupName_ << _T(": ") << std::flush;
				CurGroup_ = tr.groupName_;
			}

			os_ << tr << std::flush;

			// update global statistics
			switch(tr.result_) 
			{
			case TestResult::ok:
				okCount_++;
				break;
			case TestResult::fail:
			case TestResult::rethrown:
				failuresCount_++;
				break;
			case TestResult::ex:
			case TestResult::ex_ctor:
				exceptionsCount_++;
				break;
			case TestResult::warn:
				warningsCount_++;
				break;
			case TestResult::term:
				terminationsCount_++;
				break;
			case TestResult::skipped:
				skippedCount_++;
				break;
			case tut::TestResult::dummy:
				assert( (tr.result_ != tut::TestResult::dummy) && "Should never be called");
			} // switch

			if( (tr.result_ != tut::TestResult::ok) &&
				(tr.result_ != tut::TestResult::skipped) )
			{
				notPassed_.push_back(tr);
			}
		}

		virtual void RunCompleted()
		{
			os_ << std::endl;

			if( notPassed_.size() > 0)
			{
				NotPassList::const_iterator i = notPassed_.begin();
				while (i != notPassed_.end())
				{
					tut::TestResult tr = *i;

					os_ << std::endl;

					os_ << _T("---> ") 
						<< _T("Group Name: ") 
						<< tr.groupName_
						<< _T(", Test ID: Test<") 
						<< tr.testNum_ << _T(">")
						<< (!tr.name_.empty() ? (_T(", Test Name: ") + tr.name_) : _T(""))
						<< std::endl;

					os_ << '\t' << _T(" problem: ");

					switch(tr.result_)
					{
					case TestResult::rethrown:
						os_ << _T("assertion failed in child") << std::endl;
						break;
					case TestResult::fail:
						os_ << _T("assertion failed") << std::endl;
						break;
					case TestResult::ex:
					case TestResult::ex_ctor:
						os_ << _T("unexpected exception") << std::endl;
						if( tr.exceptionTypeid_ != _T("") )
						{
							os_ << _T("     exception typeid: ")
								<< tr.exceptionTypeid_ << std::endl;
						}
						break;
					case TestResult::term:
						os_ << _T("would be terminated") << std::endl;
						break;
					case TestResult::warn:
						os_ << _T("Test passed, but cleanup code (destructor) raised an exception") << std::endl;
						break;
					default:
						break;
					}

					if( !tr.msg_.empty() )
					{
						if( tr.result_ == TestResult::fail)
						{
							os_ << '\t' << _T(" Failed assertion: '") << tr.msg_ << _T("'")
								<< std::endl;
						}
						else
						{
							os_ << '\t' << _T("Msg: '") << tr.msg_ << _T("'")
								<< std::endl;
						}
					}

					++i;
				}
			}

			os_ << std::endl;

			os_ << _T("Tests Summary:") << std::endl;
			if( terminationsCount_ > 0 )
			{
				os_ << '\t' << _T(" terminations:") << terminationsCount_ << std::endl;
			}
			if( exceptionsCount_ > 0 )
			{
				os_ << '\t' << _T(" exceptions:") << exceptionsCount_ << std::endl;
			}
			if( failuresCount_ > 0 )
			{
				os_ << '\t' << _T(" failures:") << failuresCount_ << std::endl;
			}
			if( warningsCount_ > 0 )
			{
				os_ << '\t' << _T(" warnings:") << warningsCount_ << std::endl;
			}

			os_ << '\t' << _T(" ok:") << okCount_ << std::endl;

			if( skippedCount_ > 0 )
			{
				os_ << '\t' << _T(" skipped:") << skippedCount_ << std::endl;
			}

			os_ << std::endl;
		}

		virtual bool AllOK() const
		{
			return notPassed_.empty();
		}

	private:
		void _Init()
		{
#ifdef _UNICODE
			const static std::locale loc("chs");
			os_.imbue(loc);
#endif
			okCount_ = 0;
			exceptionsCount_ = 0;
			failuresCount_ = 0;
			terminationsCount_ = 0;
			warningsCount_ = 0;
			skippedCount_ = 0;
			notPassed_.clear();
		}
	};

}

#endif
