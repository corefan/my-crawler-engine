#ifndef TUT_EXCEPTION_H_GUARD__
#define TUT_EXCEPTION_H_GUARD__



#include <stdexcept>
#include "tut_result.hpp"

namespace tut
{

	// -------------------------------------------------------------------
	// class TUTError

	// The base for all TUT exceptions.

	struct TutError 
		: public std::exception
	{
		explicit TutError(const tString& msg)
			: msg_(msg)
		{}

		virtual TestResult::ResultType Result() const
		{
			return TestResult::ex;
		}

		virtual tString Type() const
		{
			return _T("tut::TutError");
		}

		const TCHAR *What() const
		{
			return msg_.c_str();
		}

		~TutError()
		{
		}

	private:
		TutError &operator=(const TutError &);

		const tString msg_;
	};


	// -------------------------------------------------------------------
	// class NoSuchGroup
	
	// Group not found exception.

	struct NoSuchGroup 
		: public TutError
	{
		explicit NoSuchGroup(const tString& grp)
			: TutError(grp)
		{}

		virtual tString Type() const
		{
			return _T("tut::NoSuchGroup");
		}

		~NoSuchGroup()
		{
		}
	};

	// -------------------------------------------------------------------
	// class NoSuchTest

	// Test not found exception.

	struct NoSuchTest 
		: public TutError
	{
		explicit NoSuchTest(const tString& grp)
			: TutError(grp)
		{}

		virtual tString Type() const
		{
			return _T("tut::NoSuchTest");
		}

		~NoSuchTest()
		{
		}
	};

	
	// -------------------------------------------------------------------
	// class BadCtor

	/* Internal exception to be throwed when
	* test constructor has failed.
	*/
	struct BadCtor 
		: public TutError
	{
		explicit BadCtor(const tString& msg)
			: TutError(msg)
		{}

		TestResult::ResultType Result() const
		{
			return TestResult::ex_ctor;
		}

		virtual tString Type() const
		{
			return _T("tut::BadCtor");
		}

		~BadCtor()
		{
		}
	};


	// -------------------------------------------------------------------
	// class Failure

	// Exception to be throwed when ensure() fails or fail() called.

	struct Failure 
		: public TutError
	{
		explicit Failure(const tString& msg)
			: TutError(msg)
		{}

		TestResult::ResultType Result() const
		{
			return TestResult::fail;
		}

		virtual tString Type() const
		{
			return _T("tut::Failure");
		}

		~Failure() throw()
		{
		}
	};

	// -------------------------------------------------------------------
	// class Warning
	
	// Exception to be throwed when test desctructor throwed an exception.

	struct Warning 
		: public TutError
	{
		explicit Warning(const tString& msg)
			: TutError(msg)
		{}

		TestResult::ResultType Result() const
		{
			return TestResult::warn;
		}

		virtual tString Type() const
		{
			return _T("tut::Warning");
		}

		~Warning()
		{
		}
	};

	
	// -------------------------------------------------------------------
	// class SEH
	
	// Exception to be throwed when test issued SEH (Win32)
	
	struct SEH : public TutError
	{
		explicit SEH(const tString& msg)
			: TutError(msg)
		{}

		virtual TestResult::ResultType Result() const
		{
			return TestResult::term;
		}

		virtual tString Type() const
		{
			return _T("tut::SEH");
		}

		~SEH()
		{
		}
	};


	// -------------------------------------------------------------------
	// class Rethrown
	
	// Exception to be throwed when child processes fail.

	struct Rethrown 
		: public Failure
	{
		explicit Rethrown(const TestResult &result)
			: Failure(result.msg_), tr(result)
		{}

		virtual TestResult::ResultType Result() const
		{
			return TestResult::rethrown;
		}

		virtual tString Type() const
		{
			return _T("tut::Rethrown");
		}

		~Rethrown()
		{
		}

		const TestResult tr;
	};

	
	// -------------------------------------------------------------------
	// class Skipped

	struct Skipped 
		: public TutError
	{
		explicit Skipped(const tString& msg)
			: TutError(msg)
		{}

		virtual TestResult::ResultType Result() const
		{
			return TestResult::skipped;
		}

		virtual tString Type() const
		{
			return _T("tut::Skipped");
		}

		~Skipped()
		{
		}
	};

}

#endif
