#ifndef __THREADPOOL_TASK_ADAPTOR_HPP
#define __THREADPOOL_TASK_ADAPTOR_HPP

#include <functional>


namespace async
{
	namespace threadpool
	{


		// ----------------------------------------
		// ������������������Ϊvoid

		typedef std::tr1::function<void()> TaskFunction;


		// -----------------------------------------
		// �������ȼ������Խ���<�Ƚ�

		class PrioTaskFunction
		{
			size_t priority_;
			TaskFunction func_;

		public:
			typedef void ResultType;

		public:
			PrioTaskFunction(size_t priority, const TaskFunction &fun)
				: priority_(priority)
				, func_(fun)
			{}

		public:
			ResultType operator()() const
			{
				if( func_ )
					func_();
			}

			bool operator<(const PrioTaskFunction &rhs) const
			{
				return priority_ < rhs.priority_;
			}
		};


		// ------------------------------------------------
		// �ȴ���ʱѭ��ִ��

		class LoopedTaskFunction
		{
		private:
			TaskFunction func_;
			unsigned long millseconds_;
			HANDLE &exit_;

		public:
			typedef void ResultType;

		public:
			explicit LoopedTaskFunction(const TaskFunction &taskFunc, HANDLE &exit, unsigned long millseconds = 0)
				: func_(taskFunc)
				, exit_(exit)
				, millseconds_(millseconds)
			{}

		public:
			ResultType operator()() const
			{
				while( 1 )
				{
					DWORD ret = ::WaitForSingleObject(exit_, millseconds_);
					if( ret == WAIT_OBJECT_0 )
						return;
					else if( ret == WAIT_TIMEOUT && func_ )
						func_();
				}
			}
		};


		// -----------------------------------------------
		// �ȴ��¼����߳�ʱִ��

		class WaitHandleTaskFunction
		{
			TaskFunction func_;
			HANDLE &wait_;
			HANDLE &exit_;
			unsigned long millseconds_;

		public:
			typedef void ResultType;

		public:
			WaitHandleTaskFunction(const TaskFunction &task, HANDLE &wait, HANDLE &exit, unsigned long timeOut = INFINITE)
				: func_(task)
				, wait_(wait)
				, exit_(exit)
				, millseconds_(timeOut)
			{}

			ResultType operator()() const
			{
				HANDLE handle[] = { wait_, exit_ };

				while(1)
				{
					DWORD ret = ::WaitForMultipleObjects(_countof(handle), handle, FALSE, millseconds_);
					if( ret == WAIT_OBJECT_0 + 1 )
						return;
					else if( ret == WAIT_OBJECT_0 || ret == WAIT_TIMEOUT )
						func_();
					else
					{
						assert(0);
						return;
					}
				}
			}
	
		};
	}
}






#endif