#ifndef __BOUNDED_QUEUE_HPP
#define __BOUNDED_QUEUE_HPP


#include "../MultiThread/Lock.hpp"
#include <queue>

namespace async
{
	namespace container
	{
		template< typename T, typename A = std::allocator<T> >
		class BlockingQueue
		{
			typedef async::thread::AutoCriticalSection	Mutex;
			typedef async::thread::AutoLock<Mutex>		AutoLock;
			typedef async::thread::SemaphoreCondition	Condtion;
			typedef std::deque<T, A>					Container;

			mutable Mutex mutex_;
			Condtion notEmpty_;
			Container queue_;

		public:
			BlockingQueue()
			{} 
			explicit BlockingQueue(A &allocator)
				: queue_(allocator)
			{}

		private:
			BlockingQueue(const BlockingQueue &);
			BlockingQueue &operator=(const BlockingQueue &);

		public:
			void Put(const T &x)
			{
				{
					AutoLock lock(mutex_);
					queue_.push_back(x);
				}

				notEmpty_.Signal();
			}

			T Get()
			{
				T front;
				{
					AutoLock lock(mutex_);
					while(queue_.empty())
					{
						notEmpty_.Wait(mutex_);
					}
					assert(!queue_.empty());
					front = queue_.front();
					queue_.pop_front();
				}

				return front;
			}

			size_t Size() const
			{
				AutoLock lock(mutex_);
				return queue_.size();
			}

			bool Empty() const
			{
				AutoLock lock(mutex_);
				return queue_.empty();
			}

			template < typename FuncT >
			void for_each(const FuncT &func)
			{
				AutoLock lock(mutex_);
				std::for_each(queue_.begin(), queue_.end(), func);
			}
		};
	}


}

#endif  

