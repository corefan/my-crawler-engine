#ifndef __THREAD_POOL_HPP
#define __THREAD_POOL_HPP

#include "Detail/PoolImpl.hpp"
#include "TaskAdaptor.hpp"
#include "Scheduler.hpp"
#include "SizeControl.hpp"



namespace async
{
	namespace threadpool
	{

		// ---------------------------------------------------

		template < 
			typename TaskT									= TaskFunction,
			template < typename > class ScheduleT			= FIFOScheduler,
			template < typename > class SizeCtlT			= StaticSize
		>
		class ThreadPoolT
		{	
			typedef detail::ThreadPoolImpl<TaskT, ScheduleT, SizeCtlT> ThreadPoolImplType;
			typedef std::tr1::shared_ptr<ThreadPoolImplType>									ThreadPoolImplPtr;

		
		public:
			typedef TaskT								TaskType;
			typedef ScheduleT<TaskType>					SchedulerType;
			
		private:
			ThreadPoolImplPtr impl_;

		public:
			explicit ThreadPoolT(size_t cnt = 0)
				: impl_(new ThreadPoolImplType)
			{
				impl_->Resize(cnt);
			}
			~ThreadPoolT()
			{
				Shutdown();
			}

		private:
			ThreadPoolT(const ThreadPoolT &);
			ThreadPoolT &operator=(const ThreadPoolT &);

		public:
			size_t Size() const
			{
				return impl_->Size();
			}

			size_t ActiveSize() const
			{
				return impl_->ActiveSize();
			}

			size_t PenddingSize() const
			{
				return impl_->PendingSize();
			}

			bool Call(const TaskType &task)
			{
				return impl_->Schedule(task);
			}

			void Clear()
			{
				impl_->Clear();
			}

			bool Empty() const
			{
				return impl_->Empty();
			}

			void Shutdown(size_t timeOut = 0)
			{
				impl_->Shutdown(timeOut);
			}
		};


		typedef ThreadPoolT<TaskFunction, FIFOScheduler, StaticSize>		StaticFIFOPool;
		typedef ThreadPoolT<TaskFunction, LIFOScheduler, StaticSize>		StaticLIFOPool;
		typedef ThreadPoolT<PrioTaskFunction, PrioScheduler, StaticSize>	StaticPrioPool;

		typedef ThreadPoolT<TaskFunction, FIFOScheduler, DynamicSize>		DynamicFIFOPool;
		typedef ThreadPoolT<TaskFunction, LIFOScheduler, DynamicSize>		DynamicLIFOPool;
		typedef ThreadPoolT<PrioTaskFunction, PrioScheduler, DynamicSize>	DynamicPrioPool;

		typedef StaticFIFOPool ThreadPool; 
	}
}






#endif