#ifndef TUT_RESULT_H_GUARD__
#define TUT_RESULT_H_GUARD__


#include "tut_config.hpp"




namespace tut
{

	namespace detail
	{
		// ----------------------------------------------
		// typeid

		template<typename T>
		inline tString TypeName(const T& t)
		{
			return tString(CA2T(typeid(t).name()));
		}
	}

	





	/**
	* ²âÊÔ½á¹û
	*
	* 
	* 
	*/
	struct TestResult
	{
		enum ResultType
		{
			ok,       //< testNum_ finished successfully
			fail,     //< testNum_ failed with ensure() or fail() methods
			ex,       //< testNum_ throwed an exceptions
			warn,     //< testNum_ finished successfully, but testNum_ destructor throwed
			term,     //< testNum_ forced testNum_ application to terminate abnormally
			ex_ctor,  //< 
			rethrown, //< 
			skipped,  //< 
			dummy     //< 
		};

		ResultType result_;


		int testNum_;
		tString groupName_;
		tString name_;
		tString msg_;
		tString exceptionTypeid_;


	public:
		TestResult()
			: groupName_()
			, testNum_(0)
			, name_()
			, result_(ok)
			, msg_()
			, exceptionTypeid_()
		{}

		TestResult(const tString& grp, int pos, const tString& test_name, ResultType res)
			: groupName_(grp)
			, testNum_(pos)
			, name_(test_name)
			, result_(res)
			, msg_()
			, exceptionTypeid_()
		{}

		TestResult(const tString& grp,int pos, const tString& test_name, ResultType res, const std::exception& ex)
			: groupName_(grp)
			, testNum_(pos)
			, name_(test_name)
			, result_(res)
			, msg_(CA2T(ex.what()))
			, exceptionTypeid_(detail::TypeName(ex))
		{}

		TestResult(const tString& grp, int pos, const tString& name, ResultType res, const tString& id, const tString& msg)
			: groupName_(grp)
			, testNum_(pos)
			, name_(name)
			, result_(res)
			, msg_(msg)
			, exceptionTypeid_(id)
		{}

		virtual ~TestResult()
		{}
	};

}

#endif
