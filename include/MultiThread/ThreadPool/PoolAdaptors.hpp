#ifndef __THREAD_POOL_POOL_ADAPTORS_HPP
#define __THREAD_POOL_POOL_ADAPTORS_HPP


#include <functional>
#include <memory>

namespace async
{
	namespace threadpool
	{
		// pool->schedule(bind(&Runnable::run, task_object)).
		
		template < typename PoolT, typename RunnableT >
		inline bool Call(PoolT &pool, const std::tr1::shared_ptr<RunnableT> &obj)
		{
			return pool.Call(std::tr1::bind(&RunnableT::Run, obj));
		}

		template < typename PoolT, typename RunnableT >
		inline bool Call(PoolT &pool, const RunnableT &obj)
		{
			return pool.Call(std::tr1::bind(&RunnableT::Run, std::tr1::ref(obj)));
		}

	}
}





#endif