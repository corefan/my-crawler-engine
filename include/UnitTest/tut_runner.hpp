#ifndef TUT_RUNNER_H_GUARD
#define TUT_RUNNER_H_GUARD


#include "tut_exception.hpp"
#include "tut_result.hpp"

namespace tut
{

	// ----------------------------------------------------------------
	// Group�����ӿ�
	
	struct GroupBase
	{
		virtual ~GroupBase() = 0
		{}

		// ִ�в���
		virtual void Rewind() = 0;
		virtual bool RunNext(TestResult &) = 0;

		// ִ��һ��Test
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

		// һ���²��Կ�ʼִ��
		virtual void RunStart()
		{}

		// һ�������鿪ʼִ��
		virtual void GroupStart(const tString &groupName)
		{}

		// һ�������������
		virtual void TestCompleted(const TestResult &testResult)
		{}

		// һ���������������
		virtual void GroupCompleted(const tString &)
		{}

		// ���еĲ����������
		virtual void RunCompleted()
		{}

		// �Ƿ�ȫ������ͨ��
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

	// ����������
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
				throw TutError(_T("GroupBase����Ϊ��"));
			}

			if( groups_.find(name) != groups_.end())
			{
				throw TutError(_T("�Ѿ����ڵ�Group: ") + name);
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

		// ��ȡ���в���Group����
		template<typename ContainerT>
		void GroupsName(ContainerT &container) const
		{
			for(const_iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
				container.push_back(iter->first);
		}

		// ִ�����в���
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

		// ִ��ָ������Group����
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

		// ָ��ĳ����Group�е�һ������
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

