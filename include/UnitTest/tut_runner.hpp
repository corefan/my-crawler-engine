#ifndef TUT_RUNNER_H_GUARD
#define TUT_RUNNER_H_GUARD


#include "tut_exception.hpp"
#include "tut_result.hpp"

namespace tut
{

	// ----------------------------------------------------------------
	// Group操作接口
	
	struct GroupBase
	{
		virtual ~GroupBase() = 0
		{}

		// 执行测试
		virtual void Rewind() = 0;
		virtual bool RunNext(TestResult &) = 0;

		// 执行一个Test
		virtual bool RunTest(int n, TestResult &tr) = 0;
	};


	// -------------------------------------------------------
	/**
	* Test runner Callback interface.
	* Can be implemented by caller to update
	* tests results in real-time. User can implement
	* any of Callback methods, and leave unused
	* in default implementation.
	*/
	struct Callback
	{
		Callback()
		{}
		virtual ~Callback()
		{}

		// 一个新测试开始执行
		virtual void RunStart()
		{}

		// 一个测试组开始执行
		virtual void GroupStart(const tString &groupName)
		{}

		// 一个测试运行完毕
		virtual void TestCompleted(const TestResult &testResult)
		{}

		// 一个测试组运行完毕
		virtual void GroupCompleted(const tString &)
		{}

		// 所有的测试运行完毕
		virtual void RunCompleted()
		{}

		// 是否全部测试通过
		virtual bool AllOK() const
		{
			return true;
		}

	private:
		Callback(const Callback &);
		Callback &operator=(const Callback&);
	};




	typedef std::vector< tString >				GroupNames;
	typedef std::set< Callback * >				Callbacks;


	// ----------------------------------------------------------
	// class TestRunner

	// 测试运行者
	class TestRunner
	{
		typedef std::map<tString, GroupBase*>		GroupsType;
		typedef GroupsType::iterator				iterator;
		typedef GroupsType::const_iterator			const_iterator;

	private:
		GroupsType groups_;
		Callbacks callbacks_;

	public:
		TestRunner()
			: groups_()
			, callbacks_()
		{}

	private:
		TestRunner(const TestRunner &);
		TestRunner &operator=(const TestRunner &);


	public:
		void RegisterGroup(const tString& name, GroupBase* gr)
		{
			if( gr == 0 )
			{
				throw TutError(_T("GroupBase参数为空"));
			}

			if( groups_.find(name) != groups_.end())
			{
				throw TutError(_T("已经存在的Group: ") + name);
			}

			groups_.insert( std::make_pair(name, gr) );
		}

		void SetCallback(Callback *callback)
		{
			ClearCallbacks();
			InsertCallback(callback);
		}

		void InsertCallback(Callback *callback)
		{
			callbacks_.insert(callback);
		}

		void EraseCallback(Callback *callback)
		{
			callbacks_.erase(callback);
		}

		void ClearCallbacks()
		{
			callbacks_.clear();
		}

		const Callbacks &GetCallbacks() const
		{
			return callbacks_;
		}

		void SetCallbacks(const Callbacks &cb)
		{
			callbacks_ = cb;
		}

		// 获取所有测试Group名称
		template<typename ContainerT>
		void GroupsName(ContainerT &container) const
		{
			for(const_iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
				container.push_back(iter->first);
		}

		// 执行所有测试
		void RunTests() const
		{
			_RunStart();

			const_iterator i = groups_.begin();
			const_iterator e = groups_.end();
			while (i != e)
			{
				_GroupStart(i->first);
				_RunAllTests(i);
				_GroupCompleted(i->first);

				++i;
			}

			_RunCompleted();
		}

		// 执行指定名称Group测试
		void RunTests(const tString& groupName) const
		{
			_RunStart();

			const_iterator i = groups_.find(groupName);
			if( i == groups_.end())
			{
				_RunCompleted();
				throw NoSuchGroup(groupName);
			}

			_GroupStart(groupName);
			_RunAllTests(i);
			_GroupCompleted(groupName);
			_RunCompleted();
		}

		// 指定某测试Group中的一个测试
		bool RunTest(const tString& groupName, int n, TestResult &tr) const
		{
			_RunStart();

			const_iterator i = groups_.find(groupName);
			if( i == groups_.end())
			{
				_RunCompleted();
				throw NoSuchGroup(groupName);
			}

			_GroupStart(groupName);

			bool t = i->second->RunTest(n, tr);

			if(t && tr.result_ != TestResult::dummy)
			{
				_TestCompleted(tr);
			}

			_GroupCompleted(groupName);
			_RunCompleted();

			return t;
		}

	
	private:
		void _RunStart() const
		{
			for(Callbacks::const_iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
				(*i)->RunStart();
		}

		void _RunCompleted() const
		{
			for(Callbacks::const_iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
				(*i)->RunCompleted();
		}

		void _GroupStart(const tString &group_name) const
		{
			for(Callbacks::const_iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
				(*i)->GroupStart(group_name);
		}

		void _GroupCompleted(const tString &group_name) const
		{
			for(Callbacks::const_iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
				(*i)->GroupCompleted(group_name);
		}

		void _TestCompleted(const TestResult &tr) const
		{
			for(Callbacks::const_iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
				(*i)->TestCompleted(tr);
		}

		void _RunAllTests(const_iterator i) const
		{
			i->second->Rewind();

			TestResult tr;
			while( i->second->RunNext(tr) )
			{
				if(tr.result_ != TestResult::dummy)
				{
					_TestCompleted(tr);
				}

				if( tr.result_ == TestResult::ex_ctor)
				{
					// test object ctor failed, skip whole group
					break;
				}
			}
		}
	};


	// TestRunner Singleton
	inline TestRunner &RunnerInstance()
	{
		static TestRunner tr;
		return tr;
	}
}

#endif // TUT_RUNNER_H_GUARD

