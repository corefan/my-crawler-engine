#ifndef __THREADPOOL_IMPL_HPP
#define __THREADPOOL_IMPL_HPP



#include <vector>

#include "../../Lock.hpp"
#include "Thread.hpp"
#include "../TaskAdaptor.hpp"



namespace async
{
	namespace threadpool
	{
		namespace detail
		{

			// TaskT policies:		TaskFunction, PrioTaskFunc, 
			// ScheduleT policies:	FifoScheduler, LifoScheduler, PrioScheduler

			// ---------------------------------------

			template < 
				typename TaskT,
				template < typename > class ScheduleT,
				template < typename > class SizeT
			>
			class ThreadPoolImpl
			{
			public:
				typedef ThreadPoolImpl<TaskT, ScheduleT, SizeT>	PoolType;

				typedef TaskT									TaskType;
				typedef ScheduleT<TaskType>						SchedulerType;
				typedef SizeT<PoolType>							SizeControlType;

				typedef WorkerThread<PoolType>					WorkThreadType;
				typedef std::tr1::shared_ptr<WorkThreadType>	WorkThreadPtr;
				typedef std::vector<WorkThreadPtr>				WorkThreads;

			private:
				friend class WorkThreadType;

				bool isTerminateAllWorkers_;						// 是否停止所有线程					

				volatile long workers_;								// 工作线程个数
				volatile long targetWorkers_;						// 目标线程个数
				volatile long activeWorkers_;						// 活动线程个数

				SchedulerType scheduler_;							// 调度策略
				SizeControlType sizeCtl_;							// 线程数策略
				WorkThreads terminateWorkers_;						// 线程组

			private:
				typedef async::thread::AutoCriticalSection	Mutex;
				typedef async::thread::AutoLock<Mutex>		AutoLock;

				mutable Mutex								mutex_;
                mutable async::thread::SemaphoreCondition   notEmpty_;		
				//mutable async::thread::SemaphoreCondition	productor_;

			public:
				ThreadPoolImpl()
					: isTerminateAllWorkers_(false)
					, workers_(0)
					, targetWorkers_(0)
					, activeWorkers_(0)
				{
				}
				~ThreadPoolImpl()
				{
				}

			private:
				ThreadPoolImpl(const ThreadPoolImpl &);
				ThreadPoolImpl &operator=(const ThreadPoolImpl &);

			public:
				// 获取线程个数
				size_t Size() const
				{
					return workers_;
				}

				// 获取正在执行任务的线程个数
				size_t ActiveSize() const
				{
					return activeWorkers_;
				}

				// 获取Pending的线程个数
				size_t PendingSize() const
				{
					AutoLock lock(mutex_);
					return scheduler_.Size();
				}

				// 调用一次
				void Shutdown(size_t timeOut)
				{
					_Wait(timeOut);
					TerminateAllWorkers();
				}

				// 调度
				bool Schedule(const TaskType &task)
				{
					AutoLock lock(mutex_);

					// 增长策略
					if( activeWorkers_ == workers_ )
					{
						sizeCtl_.Resize(*this, workers_ + 1);
					}

					if( scheduler_.Push(task) )
					{
						notEmpty_.Signal();

						return true;
					}
					else
					{
						return false;
					}
				}

				// 清空
				void Clear()
				{
					AutoLock lock(mutex_);
					scheduler_.Clear();
				}

				// 是否为空
				bool Empty() const
				{
					AutoLock lock(mutex_);
					scheduler_.Empty();
				}
   

				// 关闭所有工作线程
				void TerminateAllWorkers()
				{
					{
						AutoLock lock(mutex_);

						isTerminateAllWorkers_ = true;
						targetWorkers_ = 0;

						//mutex_.Unlock();
						//productor_.Broadcast();
						//mutex_.Lock();

						//while( activeWorkers_ > 0 )
						//	consumer_.Wait(mutex_);
					}
						
					notEmpty_.Broadcast();
					for(WorkThreads::const_iterator iter = terminateWorkers_.begin(); iter != terminateWorkers_.end(); ++iter)
						(*iter)->Join();

					{
						AutoLock lock(mutex_);
						terminateWorkers_.clear();
					}
				}

				// 重新设置线程个数
				bool Resize(size_t workCnt)
				{
					AutoLock lock(mutex_);

					if( !isTerminateAllWorkers_ )
						targetWorkers_ = workCnt;
					else
						return false;

					if( workers_ <= targetWorkers_ )
					{
						while(workers_ < targetWorkers_)
						{
							try
							{
								WorkThreadPtr worker =  WorkThreadType::Create(this);
								terminateWorkers_.push_back(worker);

								++workers_;
								++activeWorkers_;
							}
							catch(...)
							{
								return false;
							}
						}
					}
					else
					{
						//mutex_.Unlock();
						//productor_.Broadcast();
						//mutex_.Lock();
					}

					return true;
				}

				// 从非空容器中获取任务并执行
				bool ExceuteTask()
				{
					TaskType task;

					{
						AutoLock lock(mutex_);

						if( workers_ > targetWorkers_ )
							return false;

						while(scheduler_.Empty())
						{
							if( workers_ > targetWorkers_ || isTerminateAllWorkers_ )
							{
								return false;
							}
							else
							{
								--activeWorkers_;

								notEmpty_.Wait(mutex_);

								++activeWorkers_;
							}
						}

						if( !scheduler_.Empty() )
						{
							task = scheduler_.Top();
							scheduler_.Pop();
						}
					}

					if( task )
						task();

					return true;
				}

				void Destroy()
				{
					AutoLock lock(mutex_);

					--targetWorkers_;
					--workers_;
				}


				private:
					// 等待正在工作线程
					void _Wait(size_t taskThreshold = 0)
					{
						AutoLock lock(mutex_);

						if( 0 == taskThreshold )
						{
							while( activeWorkers_ != 0 || !scheduler_.Empty() )
							{
								notEmpty_.Wait(mutex_);
							}
						}
						else
						{
							while( taskThreshold < activeWorkers_  + scheduler_.Size() )
							{
								notEmpty_.Wait(mutex_);
							}
						}
					}

					void _Wait(DWORD waitTime, size_t taskThreshold = 0) const
					{
						AutoLock lock(mutex_);

						if( 0 == taskThreshold )
						{
							while( activeWorkers_ != 0 || !scheduler_.Empty() )
							{
								if( !notEmpty_.Wait(mutex_, waitTime) )
									return false;
							}
						}
						else
						{
							while( taskThreshold < activeWorkers_  + scheduler_.Size() )
							{
								if( !notEmpty_.Wait(mutex_, waitTime) )
									return false;
							}
						}

						return true;
					}
			};
		}
	}
}






#endif