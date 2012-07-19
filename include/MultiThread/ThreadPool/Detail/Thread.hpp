#ifndef __THREAD_POOL_THREAD_HPP
#define __THREAD_POOL_THREAD_HPP


#include <memory>

#include "../../Utility/ScopeGuard.hpp"
#include "../../Lock.hpp"
#include "../../Thread.hpp"


namespace async
{
	namespace threadpool
	{
		namespace detail
		{


			// --------------------------------------
			// 工作线程

			template < typename PoolT >
			class WorkerThread
			{
			public:
				typedef PoolT							PoolType;
				typedef thread::ThreadImplEx			ThreadType;

			private:
				PoolT *pool_;					// 指向宿主
				ThreadType thread_;				// thread which run loop

			public:
				explicit WorkerThread(PoolT *pool)
					: pool_(pool)
				{
				}
				~WorkerThread()
				{

				}

			public:
				DWORD Run()
				{
					while(pool_->ExceuteTask())
					{}
					
					pool_->Destroy();

					//std::cout << "exit" << std::endl;
					return 0;
				}

				
				void Join()
				{
					thread_.Stop();
				}

			public:
				// 静态工厂
				static std::tr1::shared_ptr<WorkerThread> Create(PoolT *pool)
				{
					std::tr1::shared_ptr<WorkerThread> worker(new WorkerThread(pool));
					
					worker->thread_.RegisterFunc(std::tr1::bind(&WorkerThread::Run, worker.get()));
					worker->thread_.Start();

					return worker;
				}
			};



		}
	}
}







#endif