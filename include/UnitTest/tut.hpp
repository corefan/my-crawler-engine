#ifndef TUT_H_GUARD
#define TUT_H_GUARD

#include "tut_config.hpp"
#include "tut_assert.hpp"
#include "tut_runner.hpp"




namespace tut
{


	namespace detail
	{
		/**
		* Exception-in-destructor-safe smart-pointer class.
		*/
		template< typename T>
		class SafeHolder
		{
		private:
			T* p_;
			bool permit_throw_in_dtor;


		public:
			SafeHolder()
				: p_(0)
				, permit_throw_in_dtor(false)
			{}

			~SafeHolder()
			{
				release();
			}

		private:
			SafeHolder(const SafeHolder&);
			SafeHolder& operator=(const SafeHolder&);


		public:
			T* operator->() const
			{
				return p_;
			}

			T* get() const
			{
				return p_;
			}

			/**
			* Tell ptr it can throw from destructor. Right way is to
			* use std::uncaught_exception(), but some compilers lack
			* correct implementation of the function.
			*/
			void permit_throw()
			{
				permit_throw_in_dtor = true;
			}

			/**
			* Specially treats exceptions in testNum_ ObjectType destructor;
			* if testNum_ itself failed, exceptions in destructor
			* are ignored; if testNum_ was successful and destructor failed,
			* Warning exception throwed.
			*/
			void release()
			{
				try
				{
					bool d = delete_obj();
					assert(d && "delete failed with SEH disabled: runtime bug?");
				}
				catch(const std::exception& ex)
				{
					if( permit_throw_in_dtor)
					{
						tString msg = _T("destructor of testNum_ ObjectType raised exception: ");
						msg += CA2T(ex.what());
						throw Warning(msg);
					}
				}
				catch( ... )
				{
					if( permit_throw_in_dtor)
					{
						throw Warning(_T("destructor of testNum_ ObjectType raised an exception"));
					}
				}
			}

			/**
			* Re-init holder to get brand new ObjectType.
			*/
			void reset()
			{
				release();
				permit_throw_in_dtor = false;
				p_ = new T();
			}

			bool delete_obj()
			{

				T* p = p_;
				p_ = 0;
				delete p;

				return true;
			}
		};
	}


	// forward declare

	template<typename TestT, int>
	class TestGroup;


	// ----------------------------------------------------------
	// class TestObject

	// ºÃ≥–◊‘≤‚ ‘¿‡£¨∑Ω±„∑√Œ ≤‚ ‘¿‡ ˝æ›

	template<typename ImplT>
	class TestObject
		: public ImplT
	{
		template<typename ImplT, int M>
		friend class TestGroup;


	private:
		bool		dummy_;
		int         currentTestID_;
		tString     currentTestName_;
		tString     currentTestGroup_;

	public:
		TestObject()
			: dummy_(false)
			, currentTestID_(0)
			, currentTestName_()
			, currentTestGroup_()
		{}
		~TestObject()
		{}

	public:
		void SetTestGroup(const tString &group)
		{
			currentTestGroup_ = group;
		}

		void SetTestID(int id)
		{
			currentTestID_ = id;
		}

		void SetTestName(const tString& name)
		{
			currentTestName_ = name;
		}

		const tString& GetTestName() const
		{
			return currentTestName_;
		}

		const tString& GetTestGroup() const
		{
			return currentTestGroup_;
		}

		int GetTestID() const
		{
			return currentTestID_;
		}

		/**
		* Default do-nothing Test.
		*/
		template< int n >
		void Test()
		{
		}
	};



	// ---------------------------------------------------------
	// struct TestRegister

	// ◊¢≤·≤‚ ‘∫Ø ˝
	template< typename TestT, typename GroupT, int n >
	struct TestsRegister
	{
		static void Register(GroupT& group)
		{
			group.Register(n, &TestT::Test<n>);
			TestsRegister<TestT, GroupT, n - 1>::Register(group);
		}
	};

	template< typename TestT, typename GroupT >
	struct TestsRegister<TestT, GroupT, 0>
	{
		static void Register(GroupT &)
		{
		}
	};


	// --------------------------------------------------------
	// class TestGroup

	// ≤‚ ‘◊È
	template<typename TestT, int MaxTestsInGroup = 50 >
	class TestGroup 
		: public GroupBase
	{
	public:
		typedef TestObject<TestT>								ObjectType;

	public:
		typedef void (ObjectType::*TestMethodFuncPtr)();
		typedef std::map<int, TestMethodFuncPtr>				TestsContainer;
		typedef typename TestsContainer::iterator				tests_iterator;
		typedef typename TestsContainer::const_iterator			tests_const_iterator;
		typedef typename TestsContainer::const_reverse_iterator	tests_const_reverse_iterator;
		typedef typename TestsContainer::size_type				size_type;


	private:
		const TCHAR* name_;
		TestsContainer tests_;
		tests_iterator curTest_;

		enum SEHResult
		{
			SEH_OK,
			SEH_DUMMY
		};

	public:
		explicit TestGroup(const TCHAR *name)
			: name_(name)
			, tests_()
			, curTest_()
		{
			// register itself
			RunnerInstance().RegisterGroup(name_, this);

			// register all TestsType
			TestsRegister<ObjectType, TestGroup, MaxTestsInGroup>::Register(*this);
		}

		TestGroup(const TCHAR* name, TestRunner &anotheRunner)
			: name_(name)
			, tests_()
			, curTest_()
		{
			anotheRunner.RegisterGroup(name_, this);

			// register all TestsType
			TestsRegister<ObjectType, TestGroup, MaxTestsInGroup>::Register(*this);
		};

	private:
		TestGroup(const TestGroup&);
		TestGroup &operator=(const TestGroup&);

	public:
		void Register(int n, TestMethodFuncPtr tm)
		{
			tests_[n] = tm;
		}

		void Rewind()
		{
			curTest_ = tests_.begin();
		}

		bool RunNext(TestResult &tr)
		{
			if( curTest_ == tests_.end())
			{
				return false;
			}

			// find next user-specialized testNum_
			detail::SafeHolder<ObjectType> obj;
			while (curTest_ != tests_.end())
			{
				tests_iterator current_test = curTest_++;

				if( _RunTest(current_test, obj, tr) && tr.result_ != TestResult::dummy )
				{
					return true;
				}
			}

			return false;
		}

		bool RunTest(int n, TestResult &tr)
		{
			if( tests_.rbegin() == tests_.rend() || tests_.rbegin()->first < n )
			{
				return false;
			}

			// withing scope; check if given testNum_ exists
			tests_iterator ti = tests_.find(n);
			if( ti == tests_.end())
			{
				return false;
			}

			detail::SafeHolder<ObjectType> obj;
			return _RunTest(ti, obj, tr);
		}

		/**
		* VC allows only one exception handling type per function,
		* so I have to split the method.
		*/
		bool _RunTest(const tests_iterator& ti, detail::SafeHolder<ObjectType>& obj, TestResult &tr)
		{
			tString curTestName;

			tr = TestResult(name_, ti->first, curTestName, TestResult::ok);

			try
			{
				switch (_RunTestSEH(ti->second, obj, curTestName, ti->first))
				{

				case SEH_DUMMY:
					tr.result_ = TestResult::dummy;
					break;

				case SEH_OK:
					// ok
					break;
				}
			}
			catch(const Rethrown& ex)
			{
				tr = ex.tr;
				tr.result_ = TestResult::rethrown;
			}
			catch(const TutError& ex)
			{
				tr.result_ = ex.Result();
				tr.exceptionTypeid_ = ex.Type();
				tr.msg_ = ex.What();
			}
			catch(const std::exception& ex)
			{
				tr.result_ = TestResult::ex;
				tr.exceptionTypeid_ = detail::TypeName(ex);
				tr.msg_ = CA2T(ex.what());
			}
			catch (...)
			{
				// testNum_ failed with unknown exception
				tr.result_ = TestResult::ex;
			}

			if( obj.get())
			{
				tr.name_ = obj->GetTestName();
			}
			else
			{
				tr.name_ = curTestName;
			}

			return true;
		}

		/**
		* Runs one under SEH if platform supports it.
		*/
		SEHResult _RunTestSEH(TestMethodFuncPtr tm, detail::SafeHolder<ObjectType>& obj, tString& curTestName, int curTestID)
		{
			if( obj.get() == 0 )
			{
				_ResetHolder(obj);
			}

			obj->dummy_ = false;

			obj.get()->SetTestID(curTestID);
			obj.get()->SetTestGroup(name_);
			(obj.get()->*tm)();

			if( obj->dummy_ )
			{
				// do not call obj.release(); reuse ObjectType
				return SEH_DUMMY;
			}

			curTestName = obj->GetTestName();
			obj.permit_throw();
			obj.release();

			return SEH_OK;
		}

		void _ResetHolder(detail::SafeHolder<ObjectType>& obj)
		{
			try
			{
				obj.reset();
			}
			catch(const std::exception& ex)
			{
				throw BadCtor((LPCTSTR)CA2T(ex.what()));
			}
			catch(...)
			{
				tOStringStream os;
				os << _T("Test: ") << name_ << _T("\tID: ") << curTest_->first << _T(" constructor has generated an exception; group execution is terminated");
				throw BadCtor(os.str());
			}
		}
	};



	namespace detail
	{
		struct Dummy
		{};
	}

	typedef TestGroup<detail::Dummy>	DefaultGroup;


}

#endif // TUT_H_GUARD

