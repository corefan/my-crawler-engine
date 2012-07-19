#ifndef TUT_ASSERT_H_GUARD
#define TUT_ASSERT_H_GUARD


#include "tut_exception.hpp"

namespace tut
{

	namespace detail
	{

		// 编译器选择类型
		template<typename T, typename U, bool flag = sizeof(T) >= sizeof(U)>
		struct SelectType
		{
			typedef T type;
		};

		template<typename T, typename U>
		struct SelectType<T, U, false>
		{
			typedef U type;
		};



		// 增加消息前缀
		inline tOStringStream &MsgPrefix(tOStringStream &str, const tString &msg)
		{
			tOStringStream ss;
			ss << msg;

			if( !ss.str().empty() )
				str << msg << _T(": ");

			return str;
		}


		// 比较两个值是否相等
		template< typename T, typename U, typename PredT>
		void EnsureCondition(const T &actual, const U &expected, const tString &msg, const PredT &pred)
		{
			if( !pred(actual, expected) )
			{
				tOStringStream ss;
				MsgPrefix(ss, msg)
					<< _T("expected '")
					<< expected
					<< _T("' actual '")
					<< actual
					<< _T("'");

				throw Failure(ss.str());
			}
		}

		template< typename CharT, typename PredT>
		void EnsureCondition(const std::basic_string<CharT> &actual, const std::basic_string<CharT> &expected, const tString &msg, const PredT &pred)
		{
			if( !pred(actual, expected) )
			{
				tOStringStream ss;
				MsgPrefix(ss, msg)
					<< _T("expected '")
					<< expected.c_str()
					<< _T("' actual '")
					<< actual.c_str()
					<< _T("'");

				throw Failure(ss.str());
			}
		}

		//// 测试两个指针是否相等
		//template< typename LHS, typename RHS, typename M >
		//void EnsureCondition(const LHS * const actual, const RHS * const expected, const M& msg, const PredT &pred)
		//{
		//	if( expected != actual )
		//	{
		//		tOStringStream ss;
		//		detail::MsgPrefix(ss,msg)
		//			<< _T("expected `")
		//			<< (void*)expected
		//			<< _T("` actual `")
		//			<< (void*)actual
		//			<< _T("`");
		//		throw Failure(ss.str());
		//	}
		//}


		template< typename PredT >
		void EnsureCondition(const double& actual, const double& expected, const tString& msg, const PredT &pred, const double& epsilon = std::numeric_limits<double>::epsilon())
		{
			const double diff = actual - expected;

			if( !pred(actual, expected) && !((diff <= epsilon) && (diff >= -epsilon )) )
			{
				tOStringStream ss;
				MsgPrefix(ss,msg)
					<< std::scientific
					<< std::showpoint
					<< std::setprecision(16)
					<< _T("expected '")		<< expected
					<< _T("' actual '")	<< actual
					<< _T("'\t with precision '") << epsilon << _T("'");
				throw Failure(ss.str());
			}
		}

	}



	// 测试一个bool值
	inline void Ensure(bool cond, const tString &msg = _T(""))
	{
		if( !cond )
		{
			throw Failure(msg);
		}
	}


	// Fail
	inline void Fail(const tString& msg)
	{
		throw Failure(msg);
	}

	// Warning
	inline void Warn(const tString& msg)
	{
		throw Warning(msg);
	}


	// Skip
	inline void Skip(const tString& msg)
	{
		throw Skipped(msg);
	}



	// ==
	template< typename T, typename U >
	void EnsureEqual(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::equal_to<type>());
	}

	// !=
	template< typename T, typename U >
	void EnsureNotEqual(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::not2(std::equal_to<type>()));
	}

	// >
	template< typename T, typename U >
	void EnsureGreate(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::greater<type>());
	}

	// >=
	template< typename T, typename U >
	void EnsureGreateEqual(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::greater_equal<type>());
	}

	// <
	template< typename T, typename U >
	void EnsureLess(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::less<type>());
	}

	// <=
	template< typename T, typename U >
	void EnsureLessEqual(const T &actual, const U &expected, const tString &msg = _T(""))
	{
		typedef detail::SelectType<T, U>::type type;

		detail::EnsureCondition(actual, expected, msg, std::less_equal<type>());
	}



	


}

#endif

