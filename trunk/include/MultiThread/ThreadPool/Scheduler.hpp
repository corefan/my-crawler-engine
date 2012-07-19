#ifndef __THREAD_POOL_SCHEDULE_HPP
#define __THREAD_POOL_SCHEDULE_HPP


#include <deque>
#include <queue>

#include "TaskAdaptor.hpp"


namespace async
{
	namespace threadpool
	{
		// ----------------------------------------
		// 先进先出调度

		template < typename TaskT = TaskFunction >
		class FIFOScheduler
		{
		public:
			typedef TaskT TaskType;

		private:
			std::deque<TaskType> tasks_;

		public:
			bool Push(const TaskType &task)
			{
				tasks_.push_back(task);
				return true;
			}

			void Pop()
			{
				tasks_.pop_front();
			}

			const TaskType &Top() const
			{
				return tasks_.front();
			}

			size_t Size() const
			{
				return tasks_.size();
			}

			bool Empty() const
			{
				return tasks_.empty();
			}

			void Clear()
			{
				tasks_.clear();
			}
		};



		// ----------------------------------------
		// 后进先出调度

		template < typename TaskT = TaskFunction >
		class LIFOScheduler
		{
		public:
			typedef TaskT TaskType;

		private:
			std::deque<TaskType> tasks_;

		public:
			bool Push(const TaskType &task)
			{
				tasks_.push_front(task);
				return true;
			}

			void Pop()
			{
				tasks_.pop_front();
			}

			const TaskType &Top() const
			{
				return tasks_.front();
			}

			size_t Size() const
			{
				return tasks_.size();
			}

			bool Empty() const
			{
				return tasks_.empty();
			}

			void Clear()
			{
				tasks_.clear();
			}
		};


		// ----------------------------------------
		// 优先级调度

		template < typename TaskT = TaskFunction >
		class PrioScheduler
		{
		public:
			typedef TaskT TaskType;

		private:
			std::priority_queue<TaskType> tasks_;

		public:
			bool Push(const TaskType &task)
			{
				tasks_.push(task);
				return true;
			}

			void Pop()
			{
				tasks_.pop();
			}

			const TaskType &Top() const
			{
				return tasks_.front();
			}

			size_t Size() const
			{
				return tasks_.size();
			}

			bool Empty() const
			{
				return tasks_.empty();
			}

			void Clear()
			{
				while( !Empty() )
					Pop();
			}
		};
	}
}





#endif